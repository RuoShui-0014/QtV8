//
// Created by ruoshui on 25-6-20.
//

#include "V8PerIsolateData.h"

namespace rs {
V8PerIsolateData::V8PerIsolateData(v8::Isolate* isolate) : m_isolate(isolate) {
  m_isolate->SetData(kEmbedderNative, this);
}
V8PerIsolateData::~V8PerIsolateData() = default;
}  // namespace rs
