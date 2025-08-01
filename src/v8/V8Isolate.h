#pragma once

#include <v8.h>

namespace rs {

class PerIsolateData;
class V8Context;

class V8Isolate {
 public:
  class Scope {
   public:
    explicit Scope(v8::Isolate* isolate)
        : m_isolateScope{isolate}, m_handle{isolate} {}
    ~Scope() = default;

   private:
    v8::Isolate::Scope m_isolateScope;
    v8::HandleScope m_handle;
  };

  V8Isolate();
  ~V8Isolate();

  void InitCppgc();
  v8::Isolate* GetIsolate();

  void SetDefaultContext(v8::Local<v8::ObjectTemplate> global_template);
  std::shared_ptr<V8Context> GetDefaultV8Context();

  [[nodiscard]] v8::Local<v8::Value> RunCode(const char* code) const;
  void NotifyGC() const { m_isolate->LowMemoryNotification(); }

 private:
  std::unique_ptr<v8::CppHeap> m_cpp_heap;
  std::unique_ptr<v8::Isolate::CreateParams> m_create_params;
  v8::Isolate* m_isolate;
  std::unique_ptr<Scope> m_v8isolate_scope;
  std::unique_ptr<PerIsolateData> m_per_isolate_data;
  std::shared_ptr<V8Context> m_default_context;
};
}  // namespace rs
