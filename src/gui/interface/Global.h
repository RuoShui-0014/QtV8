#pragma once

#include <include/cppgc/garbage-collected.h>
#include <include/v8-local-handle.h>
#include <include/v8-template.h>

namespace rs::binding {

class Global : public cppgc::GarbageCollected<Global> {
 public:
  explicit Global();
  ~Global();

  static v8::Local<v8::Template> GetInterface(v8::Isolate* isolate);
  static void InstallInterface(v8::Isolate* isolate,
                               v8::Local<v8::Template> interface);
};

}  // namespace rs::binding
