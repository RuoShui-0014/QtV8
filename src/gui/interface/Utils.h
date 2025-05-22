#pragma once

#include <v8.h>

#include <array>

namespace rs {

class Utils {
 public:
  static v8::Local<v8::String> V8String(const char* str);

  struct AttributeConfig {
    const char* name;
    v8::FunctionCallback get_callback;
    v8::FunctionCallback set_callback;
    v8::ConstructorBehavior behavior;
  };
  template <AttributeConfig config>
  struct AttributeConfigs {
    std::array<AttributeConfig, sizeof(config) / sizeof(AttributeConfig)>
        config_;
  };  // static void InstallAttributes(v8::Isolate* isolate,
  //                               v8::Local<v8::Template> interface,
  //                               v8::Local<v8::Signature> signature,
  //                               AttributeConfigs configs) {
  //   for (auto& config : configs.config_) {
  //     v8::Local<v8::FunctionTemplate> get_template =
  //         v8::FunctionTemplate::New(isolate, config.get_callback, {}, signature,
  //                                   0, v8::ConstructorBehavior::kThrow);
  //     interface.As<v8::ObjectTemplate>()->Set(
  //         v8::String::NewFromUtf8(isolate, std::string_view("get ") +
  //                                              std::string_view(config.name)),
  //         get_template);
  //   }
  // }

};

}  // namespace rs
