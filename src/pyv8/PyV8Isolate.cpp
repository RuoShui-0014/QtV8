//
// Created by Administrator on 24-12-2.
//

#include "PyV8Isolate.h"

#include "PyV8Context.h"

namespace py {

static PyThread_type_lock gc_lock = NULL;
/********************* 设置v8引擎初始化参数 ********************/
PyObject* SetV8Flag(PyObject* self, PyObject* args, PyObject* kwargs) {
  const char* flag_str;
  static char* kwlist[] = {const_cast<char*>("flag"),
                           nullptr};  // 关键字参数的名称
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|s", kwlist, &flag_str)) {
    return nullptr;  // 如果解析失败，返回 NULL
  }
  rs::V8Initializer::SetV8Flag(flag_str);
  return Py_None;
}

PyObject* V8Isolate_new(PyTypeObject* type_object, PyObject*, PyObject*) {
  PyV8Isolate* self = new PyV8Isolate;
  self->ob_base.ob_refcnt = 0;
  self->ob_base.ob_type = type_object;
  Py_INCREF(self);

  return reinterpret_cast<PyObject*>(self);
}

// 初始化函数 (__init__)
int V8Isolate_init(PyV8Isolate* self, PyObject* args, PyObject* kwds) {
  self->v8Isolate = std::make_shared<rs::V8Isolate>();
  return 0;
}

// 析构函数
void V8Isolate_dealloc(PyV8Isolate* self) {
  if (PyObject_IS_GC(reinterpret_cast<PyObject*>(self))) {
    PyObject_GC_UnTrack(self);
  }

  V8Isolate_clear(self);
  self->v8Isolate.reset();
  if (self->default_context) {
    self->default_context->ob_base.ob_refcnt--;
  }
  // delete self;
}

int V8Isolate_traverse(PyV8Isolate *self, visitproc visit, void *arg) {
  Py_VISIT(self->default_context);
  return 0;
}

int V8Isolate_clear(PyV8Isolate *self) {
  Py_CLEAR(self->default_context);
  return 0;
}

void V8Isolate_free(PyV8Isolate *self) {
  delete self;
}


/********************* 触发垃圾回收 ********************/
PyObject* NotifyGC(PyV8Isolate* self, PyObject* args, PyObject* kwds) {
  self->v8Isolate->NotifyGC();
  return Py_None;
}
}  // namespace py
