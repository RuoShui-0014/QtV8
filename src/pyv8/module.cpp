#define PY_SSIZE_T_CLEAN
#include "PyV8Context.h"
#include "PyV8Isolate.h"

#ifdef __cplusplus
extern "C" {
#endif

// 定义模块的方法
static PyMethodDef module_methods[] = {
    {"set_v8flag", reinterpret_cast<PyCFunction>(rs::V8Initializer::SetV8Flag),
     METH_VARARGS | METH_KEYWORDS, "Set v8 engine init args."},
    {nullptr}  // Sentinel
};

// 定义模块
static PyModuleDef module_self = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "pyv8",               // 模块名
    .m_doc = "A rs v8 engine vm.",  //
    .m_size = -1,                   // 全局状态
    .m_methods = module_methods     // 模块的方法
};

// 模块初始化
PyMODINIT_FUNC PyInit_pyv8(void) {
  PyObject* module = PyModule_Create(&module_self);

  // 将 V8Isolate 类型添加到模块中
  PyModule_AddType(module, &py::PyTypeV8Isolate);

  // 将 V8Context 类型添加到模块中
  PyModule_AddType(module, &py::PyTypeV8Context);

  return module;
}

#ifdef __cplusplus
}
#endif
