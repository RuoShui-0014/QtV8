#pragma once
#include "include/v8.h"

namespace rs {

class V8PerIsolateData {
 public:
  enum : uint32_t { kEmbedderNative };

  explicit V8PerIsolateData(v8::Isolate* isolate);
  ~V8PerIsolateData();

 private:
  v8::Isolate* m_isolate;
};

}  // namespace rs
