#ifndef _KNET_KRANK_FEATURE_MANAGER_H_
#define _KNET_KRANK_FEATURE_MANAGER_H_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include "str_util.h"
#include "stringpiece.h"

#include "../cppml/src/transformer.h"

namespace fe {

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
  TransformedUserAction(int id_, float label_, bool is_positive_,
                        bool is_negative_, bool isvideo_)
      : id(id_),
        label(label_),
        is_positive(is_positive_),
        is_negative(is_negative_),
        isvideo(isvideo_) {}
  int id;
  float label;
  bool is_positive;
  bool is_negative;
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
  FeaturePipline(int min_count = 0, float positive_threshold = 0.49f,
                 float negative_threshold = 0.04f)
      : rowkey_indexer_(min_count, cppml::MinCountStringIndexer::MODE::COUNTER),
        processed_lines_(0),
        positive_threshold_(positive_threshold),
        negative_threshold_(negative_threshold) {}

  void feed(const std::string& line) {
    ++processed_lines_;
    if (processed_lines_ % 50000 == 0) {
      std::cerr << "[FeaturePipline] " << processed_lines_
                << " lines processed." << std::endl;
    }
    rowkey_indexer_.feed(line);
  }

  StringPiece GetRowkeyListToken(
      const std::vector<StringPiece>& features) const {
    return features[1];
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
      float label = GetLabel(isvideo, rinfo1, rinfo2);
      bool is_positive = IsPositive(isvideo, rinfo1, rinfo2, label);
      bool is_negative = IsNegative(isvideo, rinfo1, rinfo2, label);

      TransformedUserAction action(id, label, is_positive, is_negative,
                                   isvideo);

      transformed_feature.actions.push_back(action);
    }
    return transformed_feature;
  }

  float GetLabel(bool isvideo, float rinfo1, float rinfo2) const {
    float label = 0.0;
    if (isvideo) {
      // video effective play
      // bool o1 = (rinfo1 < 30 && rinfo2 > rinfo1 * 0.8);
      // bool o2 = (rinfo1 >= 30 && rinfo2 > rinfo1 * 0.5);
      // label = o1 || o2;
      float biases = 5.0;  // add a biases for video duration
      if (rinfo1 > 6.0) {
        label = std::min(1.0f, rinfo2 / (rinfo1 + biases));
      }
    } else {
      // article effective reading
      // label = (rinfo1 > 0.9 || rinfo2 > 40);
      label = rinfo1;
    }
    // use label^2 as the final label, because x^2 is sharper than x when x is
    // close to 1.0
    return label * label;
  }

  bool IsPositive(bool isvideo, float rinfo1, float rinfo2, float label) const {
    if (label > positive_threshold_) {
      return true;
    }
    return false;
  }

  bool IsNegative(bool isvideo, float rinfo1, float rinfo2, float label) const {
    if (label < negative_threshold_ || rinfo2 < 5) {
      return true;
    }
    return false;
  }

  TransformedFeature transform(const RawFeature& feature) const {
    TransformedFeature transformed_feature;
    for (auto& action : feature.actions) {
      int id = rowkey_indexer_.transform(action.rowkey).as_integer();
      float label = GetLabel(action.isvideo, action.rinfo1, action.rinfo2);
      bool is_positive =
          IsPositive(action.isvideo, action.rinfo1, action.rinfo2, label);
      bool is_negative =
          IsNegative(action.isvideo, action.rinfo1, action.rinfo2, label);
      transformed_feature.actions.push_back(
          {id, label, is_positive, is_negative, action.isvideo});
    }
    return transformed_feature;
  }

  int getRowkeyId(const std::string& rowkey) const {
    return rowkey_indexer_.transform(rowkey).as_integer();
  }

  FeaturePipline& setPositiveThreshold(float thr) {
    this->positive_threshold_ = thr;
    return *this;
  }

  FeaturePipline& setNegativeThreshold(float thr) {
    this->negative_threshold_ = thr;
    return *this;
  }

  void save(std::ofstream& out) const {
    rowkey_indexer_.save(out);
    out.write((char*)&positive_threshold_, sizeof(float));
    out.write((char*)&negative_threshold_, sizeof(float));
  }
  void load(std::ifstream& in) {
    rowkey_indexer_.load(in);
    in.read((char*)&positive_threshold_, sizeof(float));
    in.read((char*)&negative_threshold_, sizeof(float));
    std::cout << "[FeaturePipline] positive_threhold = " << positive_threshold_
              << std::endl;
    std::cout << "[FeaturePipline] negative_threhold = " << negative_threshold_
              << std::endl;
  }

  void dump_rowkeys(const std::string& filename) const {
    rowkey_indexer_.dump(filename);
  }

 private:
  cppml::MinCountStringIndexer rowkey_indexer_;
  int64_t processed_lines_;
  float positive_threshold_;
  float negative_threshold_;
};

class FeatureManager {
 public:
  FeatureManager(int min_count = 1, float positive_threshold = 0.49f,
                 float negative_threshold = 0.04f)
      : feature_pipline_(min_count, positive_threshold, negative_threshold) {}

  void ReadFromFiles(const std::string& rowkey_count_file) {
    std::ifstream ifs(rowkey_count_file);
    if (!ifs.is_open()) {
      std::cerr << "Open " << rowkey_count_file << " failed." << std::endl;
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
      std::cerr << "Save FeatureManager failed (open file '" << filename
                << "' failed)." << std::endl;
      exit(-1);
    }
    feature_pipline_.save(ofs);
  }
  void load(const std::string& filename) {
    std::ifstream ifs(filename, std::ios::in & std::ios::binary);
    if (!ifs.is_open()) {
      std::cerr << "Load FeatureManager failed (open file failed)."
                << std::endl;
      exit(-1);
    }
    feature_pipline_.load(ifs);
  }

  void dump_rowkeys(const std::string& filename) const {
    feature_pipline_.dump_rowkeys(filename);
  }

  TransformedFeature transform(const std::string& line) const {
    return feature_pipline_.transform(line);
  }

  TransformedFeature transform(const RawFeature& feature) const {
    return feature_pipline_.transform(feature);
  }

  int getRowkeyId(const std::string& rowkey) const {
    return feature_pipline_.getRowkeyId(rowkey);
  }

 private:
  FeaturePipline feature_pipline_;
};

}  // namespace fe

#endif