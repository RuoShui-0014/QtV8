#include "per_isolate_data.h"

#include "wrapper_type_info.h"

namespace rs {

PerIsolateData::PerIsolateData(v8::Isolate* isolate) : isolate_(isolate) {
  if (!isolate) {
    return;
  }

  isolate->SetData(IsolateData::kPerIsolateData, this);
}

PerIsolateData::~PerIsolateData() {
  if (isolate_) {
    isolate_->SetData(IsolateData::kPerIsolateData, nullptr);
  }
}

PerIsolateData* PerIsolateData::From(v8::Isolate* isolate) {
  return static_cast<PerIsolateData*>(
      isolate->GetData(IsolateData::kPerIsolateData));
}

v8::Local<v8::Template> PerIsolateData::FindV8Template(const void* key) {
  auto result = template_map_.find(key);
  if (result != template_map_.end()) {
    return result->second.Get(isolate_);
  }
  return v8::Local<v8::Template>();
}

void PerIsolateData::AddV8Template(const void* key,
                                   v8::Local<v8::Template> value) {
  template_map_.emplace(key, v8::Eternal<v8::Template>(isolate_, value));
}

void PerIsolateData::Reset(v8::Isolate* isolate) {
  if (!isolate) {
    return;
  }

  isolate_ = isolate;
  isolate->SetData(IsolateData::kPerIsolateData, this);
}

}  // namespace rs
