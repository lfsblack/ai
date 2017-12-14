// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: tensorflow/core/protobuf/master_service.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "tensorflow/core/protobuf/master_service.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace tensorflow {
namespace grpc {

namespace protobuf_tensorflow_2fcore_2fprotobuf_2fmaster_5fservice_2eproto {


namespace {


}  // namespace

PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::ParseTableField
    const TableStruct::entries[] GOOGLE_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  {0, 0, 0, ::google::protobuf::internal::kInvalidMask, 0, 0},
};

PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::AuxillaryParseTableField
    const TableStruct::aux[] GOOGLE_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  ::google::protobuf::internal::AuxillaryParseTableField(),
};
PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::ParseTable const
    TableStruct::schema[] GOOGLE_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { NULL, NULL, 0, -1, -1, -1, -1, NULL, false },
};

const ::google::protobuf::uint32 TableStruct::offsets[1] = {};
static const ::google::protobuf::internal::MigrationSchema* schemas = NULL;
static const ::google::protobuf::Message* const* file_default_instances = NULL;
namespace {

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "tensorflow/core/protobuf/master_service.proto", schemas, file_default_instances, TableStruct::offsets, factory,
      NULL, NULL, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
}

}  // namespace
void TableStruct::InitDefaultsImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::internal::InitProtobufDefaults();
  ::tensorflow::protobuf_tensorflow_2fcore_2fprotobuf_2fmaster_2eproto::InitDefaults();
}

void InitDefaults() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &TableStruct::InitDefaultsImpl);
}
namespace {
void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] GOOGLE_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
      "\n-tensorflow/core/protobuf/master_servic"
      "e.proto\022\017tensorflow.grpc\032%tensorflow/cor"
      "e/protobuf/master.proto2\274\004\n\rMasterServic"
      "e\022T\n\rCreateSession\022 .tensorflow.CreateSe"
      "ssionRequest\032!.tensorflow.CreateSessionR"
      "esponse\022T\n\rExtendSession\022 .tensorflow.Ex"
      "tendSessionRequest\032!.tensorflow.ExtendSe"
      "ssionResponse\022Z\n\017PartialRunSetup\022\".tenso"
      "rflow.PartialRunSetupRequest\032#.tensorflo"
      "w.PartialRunSetupResponse\022B\n\007RunStep\022\032.t"
      "ensorflow.RunStepRequest\032\033.tensorflow.Ru"
      "nStepResponse\022Q\n\014CloseSession\022\037.tensorfl"
      "ow.CloseSessionRequest\032 .tensorflow.Clos"
      "eSessionResponse\022N\n\013ListDevices\022\036.tensor"
      "flow.ListDevicesRequest\032\037.tensorflow.Lis"
      "tDevicesResponse\022<\n\005Reset\022\030.tensorflow.R"
      "esetRequest\032\031.tensorflow.ResetResponseB3"
      "\n\032org.tensorflow.distruntimeB\023MasterServ"
      "iceProtosP\001b\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 739);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "tensorflow/core/protobuf/master_service.proto", &protobuf_RegisterTypes);
  ::tensorflow::protobuf_tensorflow_2fcore_2fprotobuf_2fmaster_2eproto::AddDescriptors();
}
} // anonymous namespace

void AddDescriptors() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at dynamic initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;

}  // namespace protobuf_tensorflow_2fcore_2fprotobuf_2fmaster_5fservice_2eproto


// @@protoc_insertion_point(namespace_scope)

}  // namespace grpc
}  // namespace tensorflow

// @@protoc_insertion_point(global_scope)
