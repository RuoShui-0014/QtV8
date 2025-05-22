//
// Created by Administrator on 24-12-2.
//
#pragma once

#include "src/v8/V8Isolate.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

namespace py {
/********************* 设置v8引擎初始化参数 ********************/
PyObject* SetV8Flag(PyObject* self, PyObject* args, PyObject* kwargs);

struct PyV8Context;

// 定义 Py_V8Isolate 对象的结构体
struct PyV8Isolate {
  PyV8Isolate() = default;
  ~PyV8Isolate() {
    v8Isolate.reset();
  }
  PyObject ob_base{0, nullptr};

  std::shared_ptr<rs::V8Isolate> v8Isolate;
  PyV8Context* default_context{nullptr};
};

PyObject* V8Isolate_new(PyTypeObject* type_object, PyObject*, PyObject*);
int V8Isolate_init(PyV8Isolate* self, PyObject* args, PyObject* kwds);
void V8Isolate_dealloc(PyV8Isolate* self);
int V8Isolate_traverse(PyV8Isolate *self, visitproc visit, void *arg);
int V8Isolate_clear(PyV8Isolate *self);
void V8Isolate_free(PyV8Isolate *self);

/********************* 触发垃圾回收 ********************/
PyObject* NotifyGC(PyV8Isolate* self, PyObject* args, PyObject* kwds);

// 定义方法列表
static PyMethodDef V8Isolate_methods[] = {
    {"notify_gc", reinterpret_cast<PyCFunction>(NotifyGC), METH_NOARGS,
     "Notify v8 engine gc."},
    {nullptr}  // Sentinel
};
// 定义成员（属性）列表
static PyGetSetDef V8Isolate_getset[] = {{nullptr}};
// 定义 Point 类型
static PyTypeObject PyTypeV8Isolate = {
    .ob_base = {{{1}, (nullptr)}, (0)},
    .tp_name = "V8Isolate",               // 类的名字
    .tp_basicsize = sizeof(PyV8Isolate),  // 类的大小
    .tp_itemsize{},
    .tp_dealloc = reinterpret_cast<destructor>(V8Isolate_dealloc),  // 析构函数
    .tp_vectorcall_offset{},
    .tp_getattr{},
    .tp_setattr{},
    .tp_as_async{},
    .tp_repr{},
    .tp_as_number{},
    .tp_as_sequence{},
    .tp_as_mapping{},
    .tp_hash{},
    .tp_call{},
    .tp_str{},
    .tp_getattro{},
    .tp_setattro{},
    .tp_as_buffer{},
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_VERSION_TAG | Py_TPFLAGS_HAVE_GC,
    .tp_doc = "V8Isolate class",
    // .tp_traverse{},
    // .tp_clear{},
    .tp_traverse = reinterpret_cast<traverseproc>(V8Isolate_traverse),
    .tp_clear = reinterpret_cast<inquiry>(V8Isolate_clear),
    .tp_richcompare{},
    .tp_weaklistoffset{},
    .tp_iter{},
    .tp_iternext{},
    .tp_methods = V8Isolate_methods,
    .tp_getset = V8Isolate_getset,
    .tp_base{},
    .tp_dict{},
    .tp_descr_get{},
    .tp_descr_set{},
    .tp_dictoffset{},
    .tp_init = reinterpret_cast<initproc>(V8Isolate_init),
    .tp_alloc{},
    .tp_new = reinterpret_cast<newfunc>(V8Isolate_new),
    .tp_free = reinterpret_cast<freefunc>(V8Isolate_free),
    .tp_is_gc{},
    .tp_bases{},
    .tp_mro{},
    .tp_cache{},
    .tp_subclasses{},
    .tp_weaklist{},
    .tp_del{},
    .tp_version_tag{},
    .tp_finalize{},
    .tp_vectorcall{},
    .tp_watched{},
};
}  // namespace py
