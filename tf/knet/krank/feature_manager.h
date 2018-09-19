#ifndef _KNET_KRANK_FEATURE_MANAGER_H_
#define _KNET_KRANK_FEATURE_MANAGER_H_

#include <fstream>
#include <iostream>
#include <string>

#include "str_util.h"
#include "stringpiece.h"

#include "../../../cppml/src/transformer.h"

namespace krank {

struct UserAction {
  UserAction(float rinfo1_, float rinfo2_, const std::string& rowkey_,
             bool isvideo_)
      : rinfo1(rinfo1_), rinfo2(rinfo2_), rowkey(rowkey_), isvideo(isvideo_) {}

  float rinfo1;
  float rinfo2;
  std::string rowkey;
  bool isvideo;
};

struct TransformedUserAction {
  TransformedUserAction(float rinfo1_, float rinfo2_, int id_, bool isvideo_)
      : rinfo1(rinfo1_), rinfo2(rinfo2_), id(id_), isvideo(isvideo_) {}

  float rinfo1;
  float rinfo2;
  int id;
  bool isvideo;
};

struct RawFeature {
  std::vector<UserAction> actions;
};

struct TransformedFeature {
  std::vector<TransformedUserAction> actions;
};

class FeaturePipline {
 public:
  FeaturePipline(int min_count = 0) : rowkey_indexer_(min_count) {}

  void feed(const std::string& line) {
    std::vector<StringPiece> features = Split(line, '\t');
    // features[0]: uin
    // features[1]: rowkey list

    if (features.size() != 2) {
      std::cerr << "line format error." << std::endl;
      exit(-1);
    }
    std::vector<StringPiece> h = Split(GetRowkeyListToken(features), ' ');
    ParseRowkeyList(h);
  }

  StringPiece GetRowkeyListToken(
      const std::vector<StringPiece>& features) const {
    return features[1];
  }

  void ParseRowkeyList(const std::vector<StringPiece>& h) {
    for (auto& s : h) {
      std::vector<StringPiece> tokens = Split(s, ':');
      // tokens: rowkey, isvideo, duration(ratio), watch_time(stay_time)
      if (tokens.size() != 4) {
        std::cerr << "rowkey list format error." << std::endl;
        exit(-1);
      }
      rowkey_indexer_.feed(std::string(tokens[0]));
    }
  }
  void feed_end() { rowkey_indexer_.feed_end(); }

  TransformedFeature transform(const std::string& line) const {
    std::vector<StringPiece> pieces = Split(line, '\t');
    std::vector<StringPiece> h = Split(GetRowkeyListToken(pieces), ' ');
    TransformedFeature transformed_feature;
    for (auto& s : h) {
      // tokens: rowkey, isvideo, duration(ratio), watch_time(stay_time)
      std::vector<StringPiece> tokens = Split(s, ':');

      // TODO(zhezhaoxu) Validate data
      int id = rowkey_indexer_.transform(std::string(tokens[0])).as_integer();
      bool isvideo = (tokens[1][0] == '1');
      float rinfo1 = std::stof(std::string(tokens[2]));
      float rinfo2 = std::stof(std::string(tokens[3]));
      TransformedUserAction action(rinfo1, rinfo2, id, isvideo);
      transformed_feature.actions.push_back(action);
    }
    return transformed_feature;
  }

  TransformedFeature transform(const RawFeature& feature) const {
    TransformedFeature transformed_feature;
    for (auto& action : feature.actions) {
      int id = rowkey_indexer_.transform(action.rowkey).as_integer();
      transformed_feature.actions.push_back(
          {action.rinfo1, action.rinfo2, id, action.isvideo});
    }
    return transformed_feature;
  }

  void save(std::ofstream& out) const {
    rowkey_indexer_.save(out);
  }
  void load(std::ifstream& in) {
    rowkey_indexer_.load(in);
  }

  void dump_rowkeys(const std::string& filename) const {
    rowkey_indexer_.dump(filename);
  }

 private:
  cppml::MinCountStringIndexer rowkey_indexer_;
};

class FeatureManager {
 public:
  FeatureManager(int min_count = 0) : feature_pipline_(min_count) {}

  void ReadFromFile(const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
      std::cerr << "Open " << filename << " failed." << std::endl;
      exit(-1);
    }

    std::string line;
    while (!ifs.eof()) {
      std::getline(ifs, line);
      if (line.empty()) {
        continue;
      }
      feature_pipline_.feed(line);
    }
    feature_pipline_.feed_end();
  }

  void save(const std::string& filename) const {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
      std::cerr<< "Save FeatureManager failed (open file failed)." << std::endl;
      exit(-1);
    }
    feature_pipline_.save(ofs);
  }
  void load(const std::string& filename) {
    std::ifstream ifs(filename, std::ios::in & std::ios::binary);
    if (!ifs.is_open()) {
      std::cerr<< "Load FeatureManager failed (open file failed)." << std::endl;
      exit(-1);
    }
    feature_pipline_.load(ifs);
  }

  void dump_rowkeys(const std::string& filename) const {
    feature_pipline_.dump_rowkeys(filename);
  }

 private:
  FeaturePipline feature_pipline_;
};

}  // namespace krank

#endif