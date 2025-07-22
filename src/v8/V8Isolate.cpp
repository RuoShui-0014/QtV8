#include "V8Isolate.h"

#include "V8Context.h"
#include "V8Initializator.h"
#include "include/libplatform/libplatform.h"
#include "include/v8-cppgc.h"
#include "../module/per_isolate_data.h"

namespace rs {

V8Isolate::V8Isolate() {
  /* 初始化v8引擎 */
  if (!V8Initializer::GetInstance().Inited()) {
    V8Initializer::GetInstance().Init();
  }

  InitCppgc();

  m_create_params = std::make_unique<v8::Isolate::CreateParams>();
  m_create_params->array_buffer_allocator =
      v8::ArrayBuffer::Allocator::NewDefaultAllocator();

  m_create_params->cpp_heap = m_cpp_heap.get();
  m_create_params->embedder_wrapper_type_index = 0;
  m_create_params->embedder_wrapper_object_index = 1;


  m_isolate = v8::Isolate::Allocate();
  v8::Isolate::Initialize(m_isolate, *m_create_params);
  m_v8isolate_scope = std::make_unique<Scope>(m_isolate);

  m_per_isolate_data = std::make_unique<PerIsolateData>(m_isolate);
  m_default_context = std::make_unique<V8Context>(this);
}

V8Isolate::~V8Isolate() {
  m_default_context.reset();
  m_v8isolate_scope.reset();
  m_per_isolate_data.reset();
  m_isolate->Dispose();
  delete m_create_params->array_buffer_allocator;
}

void V8Isolate::InitCppgc() {
  m_cpp_heap = v8::CppHeap::Create(V8Initializer::GetInstance().GetPlatform(),
                                   v8::CppHeapCreateParams{{}});
}

v8::Isolate* V8Isolate::GetIsolate() {
  return m_isolate;
}
void V8Isolate::SetDefaultContext(
    v8::Local<v8::ObjectTemplate> global_template) {
  m_default_context->SetContext(global_template);
}

std::shared_ptr<V8Context> V8Isolate::GetDefaultV8Context() {
  return m_default_context;
}

v8::Local<v8::Value> V8Isolate::RunCode(const char* code) const {
  return {};
}
}  // namespace rs
