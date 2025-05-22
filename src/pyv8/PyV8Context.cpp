#include "PyV8Context.h"
#include "V8Isolate.h"

namespace py {
void V8Context_dealloc(PyV8Context* self) {
  self->v8Context.reset();
  delete self;
}

/********************* 运行代码 ********************/
PyObject* RunCode(PyV8Context* self, PyObject* args, PyObject* kwds) {
  char* code;
  static char* kwlist[] = {const_cast<char*>("code"), nullptr};
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &code)) {
    return Py_None;
  }
  v8::Isolate* isolate = self->v8Context->GetIsolate();
  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Context> context = self->v8Context->GetContext();
  v8::Context::Scope context_scope(context);

  v8::Local<v8::Value> result = self->v8Context->RunCode(code);
  if (result->IsString()) {
    v8::String::Utf8Value utf8_value(isolate, result);
    return PyUnicode_FromString(*utf8_value);
  } else if (result->IsNumber()) {
    double number = result.As<v8::Number>()->Value();
    return PyFloat_FromDouble(number);
  } else if (result->IsBoolean()) {
    bool boolean = result.As<v8::Boolean>()->Value();
    return PyBool_FromLong(boolean);
  } else {
    v8::String::Utf8Value utf8_value(
        isolate, result->ToString(context).ToLocalChecked());
    return PyUnicode_FromString(*utf8_value);
  }
}
}  // namespace rs