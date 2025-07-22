#include "script_wrappable.h"

namespace rs {

ScriptWrappable::~ScriptWrappable() = default;

void ScriptWrappable::Wrap(v8::Local<v8::Object> object,
                           ScriptWrappable* wrappable) {
  assert(wrappable->wrapper_.IsEmpty());

  v8::Isolate* isolate = object->GetIsolate();
  object->SetAlignedPointerInInternalField(1, wrappable);
  wrappable->wrapper_.Reset(isolate, object);
}

}  // namespace svm
