#pragma once

#include <v8.h>

#include "src/v8/V8Context.h"

namespace rs {
/* init and end v8 engine */
class V8Initializer {
 public:
  static V8Initializer& GetInstance();

  ~V8Initializer();

  void Init();

  static void SetV8Flag(const char* flag);

  [[nodiscard]] bool Inited() const;

  [[nodiscard]] v8::Platform* GetPlatform() const;

  std::shared_ptr<v8::TaskRunner> GetTaskRunner(v8::Isolate* isolate);

 private:
  V8Initializer();

  bool m_inited{false};
  std::unique_ptr<v8::Platform> m_platform;
};

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
  v8::Isolate::CreateParams m_create_params;
  v8::Isolate* m_isolate{nullptr};
  std::unique_ptr<Scope> m_v8isolate_scope;
  std::shared_ptr<V8Context> m_default_context;
  std::unique_ptr<v8::CppHeap> m_cpp_heap;
};
}  // namespace rs
