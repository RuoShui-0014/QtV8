#pragma once

#include "src/v8/V8Context.h"
#include <Python.h>

namespace py {
struct PyV8Context {
  PyObject ob_base{0, nullptr};
  std::shared_ptr<rs::V8Context> v8Context;
};

void V8Context_dealloc(PyV8Context* self);

/********************* 运行代码 ********************/
PyObject* RunCode(PyV8Context* self, PyObject* args, PyObject* kwds);

// 定义方法列表
static PyMethodDef V8Context_methods[] = {
    {"run_code", reinterpret_cast<PyCFunction>(RunCode),
     METH_VARARGS | METH_KEYWORDS, "Notify v8 engine gc."},
    {nullptr}  // Sentinel
};
// 定义成员（属性）列表
static PyGetSetDef V8Context_getset[] = {{nullptr}};
// 定义 Point 类型
static PyTypeObject PyTypeV8Context = {
    .ob_base = {{{1}, (nullptr)}, (0)},
    .tp_name = "V8Context",          // 类的名字
    .tp_basicsize = sizeof(PyV8Context),  // 类的大小
    .tp_itemsize{},
    .tp_dealloc = reinterpret_cast<destructor>(V8Context_dealloc),  // 析构函数
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
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "V8Context class",
    .tp_traverse{},
    .tp_clear{},
    .tp_richcompare{},
    .tp_weaklistoffset{},
    .tp_iter{},
    .tp_iternext{},
    .tp_methods = V8Context_methods,
    .tp_getset = V8Context_getset,
    .tp_base{},
    .tp_dict{},
    .tp_descr_get{},
    .tp_descr_set{},
    .tp_dictoffset{},
    .tp_init{},
    .tp_alloc{},
    .tp_new{},
    .tp_free{},
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
