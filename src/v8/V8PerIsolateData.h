#pragma once
#include "include/v8.h"

namespace rs {

class V8PerIsolateData {
 public:
  enum : uint32_t { kEmbedderNative };

  V8PerIsolateData(v8::Isolate* isolate);
  ~V8PerIsolateData();

 private:
  v8::Isolate* m_isolate;
  std::map<std::string, >
};

}  // namespace rs
