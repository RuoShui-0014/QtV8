//
// Created by ruoshui on 25-7-4.
//

#pragma once

#include <v8.h>
#include <map>

namespace rs {

class PerIsolateData {
  using V8TemplateMap = std::map<const void*, v8::Eternal<v8::Template>>;

 public:
  explicit PerIsolateData(v8::Isolate* isolate);
  ~PerIsolateData();

  static PerIsolateData* From(v8::Isolate* isolate);

  V8TemplateMap& GetV8TemplateMap() { return template_map_; }
  v8::Local<v8::Template> FindV8Template(const void* key);
  void AddV8Template(const void* key, v8::Local<v8::Template> value);
  void Reset(v8::Isolate* isolate);

 private:
  v8::Isolate* isolate_;
  V8TemplateMap template_map_;
};

}  // namespace svm
