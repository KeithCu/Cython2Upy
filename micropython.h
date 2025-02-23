#ifndef CPYTHON_TO_MICROPYTHON_H
#define CPYTHON_TO_MICROPYTHON_H

// ============================================================
// Includes and Basic Definitions
// ============================================================
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include <string.h>  // for strlen()

// ------------------------------------------------------------
// Basic Types and Constants
// ------------------------------------------------------------
typedef mp_obj_t PyObject;
typedef int Py_ssize_t;

#ifndef NULL
#define NULL ((void*)0)
#endif

// ------------------------------------------------------------
// CPython Object Constants Mapping
// ------------------------------------------------------------
#define Py_None mp_const_none
#define Py_True mp_const_true
#define Py_False mp_const_false
#define Py_NotImplemented mp_const_notimplemented
#define Py_Ellipsis mp_const_ellipsis

// ------------------------------------------------------------
// Disable CPython-Specific Reference Counting
// ------------------------------------------------------------
#define Py_INCREF(obj) ((void)0)
#define Py_DECREF(obj) ((void)0)
#define Py_XINCREF(obj) ((void)0)
#define Py_XDECREF(obj) ((void)0)
#define __Pyx_NewRef(obj) (obj)
#define __Pyx_GOTREF(obj) ((void)0)
#define __Pyx_GIVEREF(obj) ((void)0)

// ============================================================
// Container Creation Functions
// ============================================================

// ---------------------
// List Creation and Manipulation
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
// Tuple Creation and Manipulation
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

// ============================================================
// Type Checking and Conversion
// ============================================================

// ---------------------
// Type Checking Macros
// ---------------------
#define PyLong_Check(obj) mp_obj_is_int(obj)
#define PyFloat_Check(obj) mp_obj_is_float(obj)
#define PyUnicode_Check(obj) mp_obj_is_str(obj)
#define PyList_Check(obj) mp_obj_is_type(obj, &mp_type_list)
#define PyTuple_Check(obj) mp_obj_is_type(obj, &mp_type_tuple)
#define PyDict_Check(obj) mp_obj_is_type(obj, &mp_type_dict)
#define PySet_Check(obj) mp_obj_is_type(obj, &mp_type_set)

// ---------------------
// Type Conversion Functions
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

// ============================================================
// Sequence Operations
// ============================================================

// ---------------------
// Basic Sequence Operations
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
// General Subscription Operations (Mapping Protocol)
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
// Iteration Functions
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

// ============================================================
// Function Calling
// ============================================================
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

// ============================================================
// Attribute Access and Object Introspection
// ============================================================
static inline int PyObject_SetAttr(PyObject* obj, PyObject* attr, PyObject* value) {
    mp_store_attr(obj, attr, value);
    return 0;
}

#define __Pyx_PyObject_GetAttrStr(obj, attr) mp_load_attr(obj, mp_obj_new_str(attr, strlen(attr)))

static inline PyObject* __Pyx_GetBuiltinName(const char* name) {
    return mp_load_name(mp_obj_new_str(name, strlen(name)));
}

PyObject* __pyx_builtin_print = NULL;

static inline PyObject* PyObject_Type(PyObject* obj) {
    return (PyObject*)mp_obj_get_type(obj);
}

static inline int PyObject_IsTrue(PyObject* obj) {
    return mp_obj_is_true(obj);
}

static inline int PyObject_HasAttr(PyObject* obj, PyObject* attr) {
    return mp_obj_attr_exists(obj, attr);
}

static inline PyObject* PyObject_GetAttr(PyObject* obj, PyObject* attr) {
    return mp_load_attr(obj, attr);
}

static inline int PyObject_DelAttr(PyObject* obj, PyObject* attr) {
    mp_store_attr(obj, attr, MP_OBJ_NULL);
    return 0;
}

static inline Py_ssize_t PyObject_Length(PyObject* obj) {
    mp_obj_t len_obj = mp_obj_len_maybe(obj);
    if (len_obj == MP_OBJ_NULL) {
        mp_raise_TypeError(MP_ERROR_TEXT("object has no len()"));
        return -1;
    }
    return mp_obj_get_int(len_obj);
}

// ============================================================
// Module Management
// ============================================================

// ---------------------
// Module Creation and Importing
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

static inline PyObject* PyImport_ImportModule(const char* name) {
    mp_obj_t import_func = mp_load_global(MP_QSTR___import__);
    mp_obj_t module_name = mp_obj_new_str(name, strlen(name));
    return mp_call_function_1(import_func, module_name);
}

static inline PyObject* PyModule_GetName(PyObject* module) {
    if (mp_obj_is_type(module, &mp_type_module)) {
        return mp_obj_module_get_name(module);
    }
    mp_raise_TypeError(MP_ERROR_TEXT("expected module"));
    return NULL;
}

static inline int PyModule_AddObject(PyObject* module, const char* name, PyObject* value) {
    mp_obj_dict_store(mp_obj_module_get_globals(module), mp_obj_new_str(name, strlen(name)), value);
    return 0;
}

// ============================================================
// Error Handling
// ============================================================
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

static inline PyObject* PyErr_NewException(const char* name, PyObject* base, PyObject* dict) {
    mp_obj_t exc_type = mp_obj_new_type(mp_obj_new_str(name, strlen(name)), base ? base : (PyObject*)&mp_type_Exception, dict);
    return exc_type;
}

static inline void PyErr_Clear(void) {
    mp_err_clear();
}

static inline PyObject* PyErr_Occurred(void) {
    return mp_obj_is_exception_type(mp_err_get_raised()) ? mp_err_get_raised() : NULL;
}

// ============================================================
// String and Byte Operations
// ============================================================

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
// Byte String Operations
// ---------------------
static inline PyObject* PyBytes_FromString(const char* s) {
    return mp_obj_new_bytes((const byte*)s, strlen(s));
}

static inline PyObject* PyBytes_FromStringAndSize(const char* s, Py_ssize_t len) {
    if (s == NULL) {
        return mp_obj_new_bytes(NULL, 0);
    }
    return mp_obj_new_bytes((const byte*)s, (size_t)len);
}

static inline char* PyBytes_AsString(PyObject* obj) {
    if (mp_obj_is_type(obj, &mp_type_bytes)) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(obj, &bufinfo, MP_BUFFER_READ);
        return (char*)bufinfo.buf;
    }
    mp_raise_TypeError(MP_ERROR_TEXT("expected bytes"));
    return NULL;
}

static inline Py_ssize_t PyBytes_Size(PyObject* obj) {
    if (mp_obj_is_type(obj, &mp_type_bytes)) {
        return mp_obj_bytes_get_len(obj);
    }
    mp_raise_TypeError(MP_ERROR_TEXT("expected bytes"));
    return -1;
}

#define PyBytes_GET_SIZE(obj) PyBytes_Size(obj)
#define PyBytes_AS_STRING(obj) PyBytes_AsString(obj)

// ---------------------
// Bytearray Operations
// ---------------------
static inline PyObject* PyByteArray_FromObject(PyObject* obj) {
    mp_obj_t bytearray_func = mp_load_global(MP_QSTR_bytearray);
    return mp_call_function_1(bytearray_func, obj);
}

static inline PyObject* PyByteArray_FromStringAndSize(const char* s, Py_ssize_t len) {
    PyObject* bytes = PyBytes_FromStringAndSize(s, len);
    return PyByteArray_FromObject(bytes);
}

static inline char* PyByteArray_AsString(PyObject* obj) {
    if (mp_obj_is_type(obj, &mp_type_bytearray)) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(obj, &bufinfo, MP_BUFFER_READ);
        return (char*)bufinfo.buf;
    }
    mp_raise_TypeError(MP_ERROR_TEXT("expected bytearray"));
    return NULL;
}

static inline Py_ssize_t PyByteArray_Size(PyObject* obj) {
    if (mp_obj_is_type(obj, &mp_type_bytearray)) {
        return mp_obj_len(obj);
    }
    mp_raise_TypeError(MP_ERROR_TEXT("expected bytearray"));
    return -1;
}

// ---------------------
// Additional Type Checking for Bytes
// ---------------------
#define PyBytes_Check(obj) mp_obj_is_type(obj, &mp_type_bytes)
#define PyByteArray_Check(obj) mp_obj_is_type(obj, &mp_type_bytearray)
#define PyMemoryView_Check(obj) mp_obj_is_type(obj, &mp_type_memoryview)

// ============================================================
// Numeric Operations
// ============================================================
static inline PyObject* PyNumber_Add(PyObject* a, PyObject* b) {
    return mp_binary_op(MP_BINARY_OP_ADD, a, b);
}

static inline PyObject* PyNumber_Subtract(PyObject* a, PyObject* b) {
    return mp_binary_op(MP_BINARY_OP_SUBTRACT, a, b);
}

static inline PyObject* PyNumber_Multiply(PyObject* a, PyObject* b) {
    return mp_binary_op(MP_BINARY_OP_MULTIPLY, a, b);
}

static inline PyObject* PyNumber_TrueDivide(PyObject* a, PyObject* b) {
    return mp_binary_op(MP_BINARY_OP_TRUE_DIVIDE, a, b);
}

static inline PyObject* PyNumber_FloorDivide(PyObject* a, PyObject* b) {
    return mp_binary_op(MP_BINARY_OP_FLOOR_DIVIDE, a, b);
}

static inline PyObject* PyNumber_Remainder(PyObject* a, PyObject* b) {
    return mp_binary_op(MP_BINARY_OP_MODULO, a, b);
}

static inline PyObject* PyNumber_Negative(PyObject* obj) {
    return mp_unary_op(MP_UNARY_OP_NEGATIVE, obj);
}

static inline PyObject* PyNumber_Absolute(PyObject* obj) {
    return mp_unary_op(MP_UNARY_OP_ABS, obj);
}

// ============================================================
// Comparison Operations
// ============================================================
#define Py_LT 0
#define Py_LE 1
#define Py_EQ 2
#define Py_NE 3
#define Py_GT 4
#define Py_GE 5

static inline PyObject* PyObject_RichCompare(PyObject* a, PyObject* b, int op) {
    mp_binary_op_t mp_op;
    switch (op) {
        case Py_LT: mp_op = MP_BINARY_OP_LESS_THAN; break;
        case Py_LE: mp_op = MP_BINARY_OP_LESS_THAN_OR_EQUAL; break;
        case Py_EQ: mp_op = MP_BINARY_OP_EQUAL; break;
        case Py_NE: mp_op = MP_BINARY_OP_NOT_EQUAL; break;
        case Py_GT: mp_op = MP_BINARY_OP_GREATER_THAN; break;
        case Py_GE: mp_op = MP_BINARY_OP_GREATER_THAN_OR_EQUAL; break;
        default: mp_raise_ValueError(MP_ERROR_TEXT("invalid comparison operator")); return NULL;
    }
    return mp_binary_op(mp_op, a, b);
}

static inline int PyObject_RichCompareBool(PyObject* a, PyObject* b, int op) {
    PyObject* result = PyObject_RichCompare(a, b, op);
    return mp_obj_is_true(result);
}

// ============================================================
// Buffer Protocol
// ============================================================
typedef struct {
    void* buf;
    Py_ssize_t len;
    int readonly;
    char* format;
    int ndim;
    Py_ssize_t* shape;
    Py_ssize_t* strides;
    Py_ssize_t* suboffsets;
    Py_ssize_t itemsize;
    void* internal;
} Py_buffer;

static inline int PyObject_AsReadBuffer(PyObject* obj, const void** buf, Py_ssize_t* len) {
    mp_buffer_info_t bufinfo;
    if (mp_get_buffer(obj, &bufinfo, MP_BUFFER_READ)) {
        *buf = bufinfo.buf;
        *len = bufinfo.len;
        return 0;
    }
    mp_raise_TypeError(MP_ERROR_TEXT("object does not support buffer protocol"));
    return -1;
}

static inline int PyObject_AsWriteBuffer(PyObject* obj, void** buf, Py_ssize_t* len) {
    mp_buffer_info_t bufinfo;
    if (mp_get_buffer(obj, &bufinfo, MP_BUFFER_RW)) {
        *buf = bufinfo.buf;
        *len = bufinfo.len;
        return 0;
    }
    mp_raise_TypeError(MP_ERROR_TEXT("object does not support writable buffer"));
    return -1;
}

static inline int PyObject_AsCharBuffer(PyObject* obj, const char** buf, Py_ssize_t* len) {
    return PyObject_AsReadBuffer(obj, (const void**)buf, len);
}

static inline PyObject* PyMemoryView_FromObject(PyObject* obj) {
    mp_obj_t args[1] = { obj };
    return mp_call_function_n_kw(mp_load_global(MP_QSTR_memoryview), 1, 0, args);
}

#define PyMemoryView_GET_BUFFER(obj, buf) \
    do { \
        mp_buffer_info_t bufinfo; \
        mp_get_buffer_raise(obj, &bufinfo, MP_BUFFER_READ); \
        (buf)->buf = bufinfo.buf; \
        (buf)->len = bufinfo.len; \
        (buf)->readonly = 1; \
        (buf)->format = NULL; \
        (buf)->ndim = 1; \
        (buf)->shape = &(buf)->len; \
        (buf)->strides = NULL; \
        (buf)->suboffsets = NULL; \
        (buf)->itemsize = 1; \
        (buf)->internal = NULL; \
    } while (0)

// ============================================================
// Argument Parsing
// ============================================================
static int PyArg_ParseTuple(PyObject* tuple, const char* format, ...) {
    if (!mp_obj_is_type(tuple, &mp_type_tuple)) {
        mp_raise_TypeError(MP_ERROR_TEXT("expected tuple"));
        return 0;
    }
    va_list args;
    va_start(args, format);
    size_t tuple_len = mp_obj_tuple_get_len(tuple);
    const char* fmt = format;
    size_t arg_index = 0;
    while (*fmt && arg_index < tuple_len) {
        PyObject* item = mp_obj_tuple_get_item(tuple, arg_index);
        switch (*fmt) {
            case 'i': { 
                int* ptr = va_arg(args, int*);
                if (!mp_obj_is_int(item)) { 
                    mp_raise_TypeError(MP_ERROR_TEXT("expected int")); 
                    va_end(args); 
                    return 0; 
                }
                *ptr = mp_obj_get_int(item); 
                break; 
            }
            case 's': { 
                const char** ptr = va_arg(args, const char**);
                if (!mp_obj_is_str(item)) { 
                    mp_raise_TypeError(MP_ERROR_TEXT("expected str")); 
                    va_end(args); 
                    return 0; 
                }
                *ptr = mp_obj_str_get_str(item); 
                break; 
            }
            case 'O': { 
                PyObject** ptr = va_arg(args, PyObject**);
                *ptr = item; 
                break; 
            }
            default: 
                mp_raise_ValueError(MP_ERROR_TEXT("unsupported format specifier")); 
                va_end(args); 
                return 0;
        }
        fmt++;
        arg_index++;
    }
    if (*fmt != '\0' || arg_index < tuple_len) { 
        mp_raise_ValueError(MP_ERROR_TEXT("argument mismatch")); 
        va_end(args); 
        return 0; 
    }
    va_end(args);
    return 1;
}

// ============================================================
// Enhanced Container Operations
// ============================================================

// ---------------------
// Enhanced Sequence Operations
// ---------------------
static inline Py_ssize_t PyTuple_Size(PyObject* tuple) {
    if (mp_obj_is_type(tuple, &mp_type_tuple)) {
        return mp_obj_tuple_get_len(tuple);
    }
    mp_raise_TypeError(MP_ERROR_TEXT("expected tuple"));
    return -1;
}

static inline PyObject* PyTuple_GetItem(PyObject* tuple, Py_ssize_t i) {
    if (mp_obj_is_type(tuple, &mp_type_tuple)) {
        return mp_obj_tuple_get_item(tuple, (size_t)i);
    }
    mp_raise_TypeError(MP_ERROR_TEXT("expected tuple"));
    return NULL;
}

static inline Py_ssize_t PyList_Size(PyObject* list) {
    if (mp_obj_is_type(list, &mp_type_list)) {
        return mp_obj_list_get_len(list);
    }
    mp_raise_TypeError(MP_ERROR_TEXT("expected list"));
    return -1;
}

static inline PyObject* PyList_GetItem(PyObject* list, Py_ssize_t i) {
    if (mp_obj_is_type(list, &mp_type_list)) {
        return mp_obj_list_get_item(list, (size_t)i);
    }
    mp_raise_TypeError(MP_ERROR_TEXT("expected list"));
    return NULL;
}

static inline PyObject* PySequence_Fast(PyObject* seq, const char* msg) {
    if (mp_obj_is_type(seq, &mp_type_list) || mp_obj_is_type(seq, &mp_type_tuple)) {
        return seq;
    }
    mp_raise_TypeError(MP_ERROR_TEXT(msg));
    return NULL;
}

// ---------------------
// Enhanced Dictionary Operations
// ---------------------
static inline Py_ssize_t PyDict_Size(PyObject* dict) {
    if (mp_obj_is_type(dict, &mp_type_dict)) {
        return mp_obj_dict_get_len(dict);
    }
    mp_raise_TypeError(MP_ERROR_TEXT("expected dict"));
    return -1;
}

static inline PyObject* PyDict_GetItem(PyObject* dict, PyObject* key) {
    if (mp_obj_is_type(dict, &mp_type_dict)) {
        mp_map_t* map = mp_obj_dict_get_map(dict);
        mp_map_elem_t* elem = mp_map_lookup(map, key, MP_MAP_LOOKUP);
        return elem ? elem->value : NULL;
    }
    mp_raise_TypeError(MP_ERROR_TEXT("expected dict"));
    return NULL;
}

static inline int PyDict_DelItem(PyObject* dict, PyObject* key) {
    if (mp_obj_is_type(dict, &mp_type_dict)) {
        mp_obj_dict_delete(dict, key);
        return 0;
    }
    mp_raise_TypeError(MP_ERROR_TEXT("expected dict"));
    return -1;
}

static inline PyObject* PyDict_Keys(PyObject* dict) {
    if (mp_obj_is_type(dict, &mp_type_dict)) {
        return mp_obj_dict_keys(dict);
    }
    mp_raise_TypeError(MP_ERROR_TEXT("expected dict"));
    return NULL;
}

static inline PyObject* PyDict_Values(PyObject* dict) {
    if (mp_obj_is_type(dict, &mp_type_dict)) {
        return mp_obj_dict_values(dict);
    }
    mp_raise_TypeError(MP_ERROR_TEXT("expected dict"));
    return NULL;
}

static inline PyObject* PyDict_Items(PyObject* dict) {
    if (mp_obj_is_type(dict, &mp_type_dict)) {
        return mp_obj_dict_items(dict);
    }
    mp_raise_TypeError(MP_ERROR_TEXT("expected dict"));
    return NULL;
}

// ============================================================
// Miscellaneous Functions
// ============================================================
static inline PyObject* Py_BuildValue(const char* format, ...) {
    if (format[0] == '\0') {
        return Py_None;
    }
    va_list args;
    va_start(args, format);
    PyObject* result = NULL;
    if (strcmp(format, "i") == 0) {
        result = mp_obj_new_int(va_arg(args, int));
    } else if (strcmp(format, "s") == 0) {
        const char* s = va_arg(args, const char*);
        result = mp_obj_new_str(s, strlen(s));
    } else if (strcmp(format, "O") == 0) {
        result = va_arg(args, PyObject*);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("unsupported format in Py_BuildValue"));
    }
    va_end(args);
    return result;
}

static inline int PyCallable_Check(PyObject* obj) {
    return mp_obj_is_callable(obj);
}

static inline PyObject* PyObject_CallObject(PyObject* callable, PyObject* args) {
    if (args == NULL) {
        return mp_call_function_0(callable);
    }
    return PyObject_Call(callable, args, NULL);
}

// ============================================================
// Module Initialization Support and CPython Feature Disables
// ============================================================
#define __pyx_mstate_global ((void*)0)
#define __pyx_d NULL

#define CYTHON_USE_TYPE_SLOTS 0
#define CYTHON_FAST_THREAD_STATE 0
#define CYTHON_FAST_PYCALL 0
#define CYTHON_VECTORCALL 0

#endif /* CPYTHON_TO_MICROPYTHON_H */
