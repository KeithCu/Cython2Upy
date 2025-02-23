#ifndef CPYTHON_TO_MICROPYTHON_H
#define CPYTHON_TO_MICROPYTHON_H

#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include <string.h>  // for strlen()

// Define basic types to match CPython's expectations
typedef mp_obj_t PyObject;
typedef int Py_ssize_t;

// Define NULL if not defined
#ifndef NULL
#define NULL ((void*)0)
#endif

// Map CPython object constants to MicroPython ones
#define Py_None mp_const_none
#define Py_True mp_const_true
#define Py_False mp_const_false
#define Py_NotImplemented mp_const_notimplemented
#define Py_Ellipsis mp_const_ellipsis

// Disable CPython-specific refcounting (MicroPython manages memory differently)
#define Py_INCREF(obj) ((void)0)
#define Py_DECREF(obj) ((void)0)
#define Py_XINCREF(obj) ((void)0)
#define Py_XDECREF(obj) ((void)0)
#define __Pyx_NewRef(obj) (obj)
#define __Pyx_GOTREF(obj) ((void)0)
#define __Pyx_GIVEREF(obj) ((void)0)

// ---------------------
// List Creation/Manipulation
// ---------------------
static inline PyObject* PyList_New(Py_ssize_t size) {
    return mp_obj_new_list((size_t)size, NULL);
}

static inline int PyList_SET_ITEM(PyObject* list, Py_ssize_t i, PyObject* item) {
    mp_obj_list_store(list, (size_t)i, item);
    return 0;
}

#define __Pyx_PyList_SET_ITEM(list, i, item) PyList_SET_ITEM(list, i, item)

// ---------------------
// Tuple Creation
// ---------------------
static inline PyObject* PyTuple_New(Py_ssize_t size) {
    return mp_obj_new_tuple((size_t)size, NULL);
}

static inline int PyTuple_SET_ITEM(PyObject* tuple, Py_ssize_t i, PyObject* item) {
    mp_obj_t* items;
    mp_obj_get_array_fixed_n(tuple, (size_t)i + 1, &items);
    items[i] = item;
    return 0;
}

// Note: Tuples are immutable in MicroPython, but for compatibility with CPython,
// we allow setting items assuming the tuple is freshly created and not yet used.

// ---------------------
// Dictionary Creation
// ---------------------
static inline PyObject* PyDict_New() {
    return mp_obj_new_dict(0);
}

// ---------------------
// Set Creation
// ---------------------
static inline PyObject* PySet_New(PyObject* iterable) {
    if (iterable == NULL) {
        return mp_obj_new_set(0, NULL);
    } else {
        PyObject* set = mp_obj_new_set(0, NULL);
        PyObject* iter = mp_getiter(iterable, NULL);
        PyObject* item;
        while ((item = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
            mp_obj_set_store(set, item);
        }
        return set;
    }
}

// ---------------------
// Type Checking
// ---------------------
#define PyLong_Check(obj) mp_obj_is_int(obj)
#define PyFloat_Check(obj) mp_obj_is_float(obj)
#define PyUnicode_Check(obj) mp_obj_is_str(obj)
#define PyList_Check(obj) mp_obj_is_type(obj, &mp_type_list)
#define PyTuple_Check(obj) mp_obj_is_type(obj, &mp_type_tuple)
#define PyDict_Check(obj) mp_obj_is_type(obj, &mp_type_dict)
#define PySet_Check(obj) mp_obj_is_type(obj, &mp_type_set)

// ---------------------
// Type Conversion
// ---------------------
static inline long PyLong_AsLong(PyObject* obj) {
    if (mp_obj_is_int(obj)) {
        return mp_obj_get_int(obj);
    }
    mp_raise_TypeError(MP_ERROR_TEXT("expected int"));
    return 0;
}

static inline double PyFloat_AsDouble(PyObject* obj) {
    if (mp_obj_is_float(obj)) {
        return mp_obj_get_float(obj);
    }
    mp_raise_TypeError(MP_ERROR_TEXT("expected float"));
    return 0.0;
}

static inline const char* PyUnicode_AsUTF8(PyObject* obj) {
    if (mp_obj_is_str(obj)) {
        return mp_obj_str_get_str(obj);
    }
    mp_raise_TypeError(MP_ERROR_TEXT("expected str"));
    return NULL;
}

#define PyInt_FromLong(x) mp_obj_new_int(x)
static PyObject* __pyx_int_1 = MP_OBJ_NEW_SMALL_INT(1);
static PyObject* __pyx_int_2 = MP_OBJ_NEW_SMALL_INT(2);
static PyObject* __pyx_int_3 = MP_OBJ_NEW_SMALL_INT(3);

static inline PyObject* __Pyx_PyUnicode_FromString(const char* s) {
    return mp_obj_new_str(s, strlen(s));
}

// ---------------------
// Sequence Operations
// ---------------------
static inline PyObject* PySequence_GetItem(PyObject* seq, Py_ssize_t i) {
    return mp_obj_subscr(seq, mp_obj_new_int(i), MP_OBJ_SENTINEL);
}

static inline int PySequence_SetItem(PyObject* seq, Py_ssize_t i, PyObject* item) {
    mp_obj_subscr(seq, mp_obj_new_int(i), item);
    return 0;
}

static inline Py_ssize_t PySequence_Size(PyObject* seq) {
    mp_obj_t len_obj = mp_obj_len_maybe(seq);
    if (len_obj == MP_OBJ_NULL) {
        PyErr_SetString(PyExc_TypeError, "object has no len()");
        return -1;
    }
    return mp_obj_get_int(len_obj);
}

// ---------------------
// General Subscription Operations
// ---------------------
static inline PyObject* PyObject_GetItem(PyObject* obj, PyObject* key) {
    return mp_obj_subscr(obj, key, MP_OBJ_SENTINEL);
}

static inline int PyObject_SetItem(PyObject* obj, PyObject* key, PyObject* value) {
    mp_obj_subscr(obj, key, value);
    return 0;
}

static inline int PyObject_DelItem(PyObject* obj, PyObject* key) {
    mp_obj_subscr(obj, key, MP_OBJ_NULL);
    return 0;
}

static inline int PyDict_SetItem(PyObject* dict, PyObject* key, PyObject* value) {
    mp_obj_dict_store(dict, key, value);
    return 0;
}

static inline int PyDict_SetItemString(PyObject* dict, const char* key, PyObject* value) {
    mp_obj_dict_store(dict, mp_obj_new_str(key, strlen(key)), value);
    return 0;
}

// ---------------------
// Iteration
// ---------------------
static inline PyObject* PyObject_GetIter(PyObject* obj) {
    return mp_getiter(obj, NULL);
}

static inline PyObject* PyIter_Next(PyObject* iter) {
    mp_obj_t item = mp_iternext(iter);
    if (item == MP_OBJ_STOP_ITERATION) {
        return NULL;
    }
    return item;
}

// ---------------------
// Function Calls
// ---------------------
static inline PyObject* PyObject_Call(PyObject* callable, PyObject* args, PyObject* kwargs) {
    size_t n_args = mp_obj_len(args);
    size_t n_kw = (kwargs == NULL) ? 0 : mp_obj_dict_get_len(kwargs);
    mp_obj_t* call_args = m_new(mp_obj_t, n_args + 2 * n_kw);
    // Copy positional arguments
    for (size_t i = 0; i < n_args; i++) {
        call_args[i] = mp_obj_subscr(args, mp_obj_new_int(i), MP_OBJ_SENTINEL);
    }
    // Copy keyword arguments
    if (n_kw > 0) {
        mp_map_t* kw_map = mp_obj_dict_get_map(kwargs);
        for (size_t i = 0; i < kw_map->used; i++) {
            call_args[n_args + 2 * i] = kw_map->table[i].key;
            call_args[n_args + 2 * i + 1] = kw_map->table[i].value;
        }
    }
    PyObject* result = mp_call_function_n_kw(callable, n_args, n_kw, call_args);
    m_del(mp_obj_t, call_args, n_args + 2 * n_kw);
    return result;
}

static inline PyObject* __Pyx_PyObject_CallOneArg(PyObject* func, PyObject* arg) {
    mp_obj_t args[1] = { arg };
    return mp_call_function_n_kw(func, 1, 0, args);
}

// ---------------------
// Attribute Access
// ---------------------
static inline int PyObject_SetAttr(PyObject* obj, PyObject* attr, PyObject* value) {
    mp_store_attr(obj, attr, value);
    return 0;
}

#define __Pyx_PyObject_GetAttrStr(obj, attr) mp_load_attr(obj, mp_obj_new_str(attr, strlen(attr)))

static inline PyObject* __Pyx_GetBuiltinName(const char* name) {
    return mp_load_name(mp_obj_new_str(name, strlen(name)));
}

PyObject* __pyx_builtin_print = NULL;

// ---------------------
// Module Creation (already defined in the draft)
// ---------------------
typedef struct {
    const char *m_name;
    const char *m_doc;
    Py_ssize_t m_size;
    PyObject *m_dict;
} PyModuleDef;

static inline PyObject* PyModule_Create(PyModuleDef* def) {
    PyObject* module = mp_obj_new_module(def->m_name);
    if (def->m_doc != NULL) {
        mp_obj_dict_store(mp_obj_module_get_globals(module),
                          mp_obj_new_str("__doc__", strlen("__doc__")),
                          mp_obj_new_str(def->m_doc, strlen(def->m_doc)));
    }
    def->m_dict = mp_obj_module_get_globals(module);
    extern PyObject* __Pyx_GetBuiltinName(const char* name);
    extern PyObject* __pyx_builtin_print;
    if (__pyx_builtin_print == NULL) {
        __pyx_builtin_print = __Pyx_GetBuiltinName("print");
    }
    return module;
}

#define PyModule_GetDict(module) mp_obj_module_get_globals(module)

// ---------------------
// Module Importing
// ---------------------
static inline PyObject* PyImport_ImportModule(const char* name) {
    mp_obj_t import_func = mp_load_global(MP_QSTR___import__);
    mp_obj_t module_name = mp_obj_new_str(name, strlen(name));
    return mp_call_function_1(import_func, module_name);
}

// ---------------------
// Error Handling
// ---------------------
static inline void PyErr_SetObject(PyObject* exc, PyObject* value) {
    if (mp_obj_is_type(exc, &mp_type_type)) {
        mp_obj_t exc_instance = mp_obj_new_exception_arg1(exc, value);
        mp_raise_obj(exc_instance);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("exception needs to be a type"));
    }
}

#define PyErr_SetString(exc, msg) mp_raise_msg(&mp_type_##exc, MP_ERROR_TEXT(msg))
#define PyErr_Occurred() (mp_obj_is_exception_type(mp_err_get_raised()) ? mp_err_get_raised() : NULL)
#define __Pyx_PyErr_SetNone(exc) mp_raise_type(&mp_type_##exc)

// ---------------------
// String Representations
// ---------------------
static inline PyObject* PyObject_Str(PyObject* obj) {
    mp_obj_t str_func = mp_load_global(MP_QSTR_str);
    return mp_call_function_1(str_func, obj);
}

static inline PyObject* PyObject_Repr(PyObject* obj) {
    mp_obj_t repr_func = mp_load_global(MP_QSTR_repr);
    return mp_call_function_1(repr_func, obj);
}

static inline const char* PyObject_AsString(PyObject* obj) {
    return mp_obj_str_get_str(obj);
}

// ---------------------
// Module Initialization Support (Stubs)
// ---------------------
#define __pyx_mstate_global ((void*)0)
#define __pyx_d NULL

// Disable unused CPython features
#define CYTHON_USE_TYPE_SLOTS 0
#define CYTHON_FAST_THREAD_STATE 0
#define CYTHON_FAST_PYCALL 0
#define CYTHON_VECTORCALL 0

#endif /* CPYTHON_TO_MICROPYTHON_H */