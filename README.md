# MicroPython-Cython Compatibility Layer

This repository provides a compatibility header and accompanying stubs that allow Cython-generated modules (which target the CPython API) to run on MicroPython.

## Overview

[MicroPython](https://github.com/micropython/micropython) is a lightweight Python implementation for microcontrollers and other constrained systems. Unlike CPython, its API is designed for efficiency and minimal footprint; as a result, many of the functions and calling conventions available in CPython are either missing or implemented differently.

[Cython](https://github.com/cython/cython), on the other hand, is built around the CPython API. Its code generation and extension mechanism assume that a vast, mature API is available. However, adapting Cython itself to generate MicroPython-compatible code is a monumental task. The Cython codebase is huge and deeply intertwined with CPython semantics, and its core developers are experts on CPython—not on MicroPython. Expecting them to rework or even add optional support for MicroPython directly is unrealistic.

This takes a different approach: rather than modifying Cython, we implement a compatibility layer that maps the CPython API calls (as expected by Cython-generated modules) onto the corresponding functions in MicroPython’s public API.

## Problem Statement

- **API Discrepancy:** MicroPython provides only a subset of CPython’s C API. Cython-generated modules call numerous functions (e.g. `PyObject_Call`, `PyErr_SetString`, etc.) that either do not exist or behave differently on MicroPython.
- **Codebase Complexity:** Modifying Cython itself to support MicroPython directly would require an in-depth understanding of both CPython’s internals and the idiosyncrasies of MicroPython. The Cython codebase is too large and complex to easily incorporate dual-target support.
- **Developer Expertise:** Most Cython core developers are well-versed in CPython internals but have little exposure to MicroPython. Expecting them to learn and accommodate a vastly different runtime (with its own memory management, type system, and calling conventions) is not feasible.

## Proposed Solution

We solve the compatibility challenge by providing a header file that re-implements (or “stubs”) the CPython API functions in terms of MicroPython’s native functions. Key points include:

- **Mapping API Calls:** Functions such as `PyObject_Call`, `PyErr_SetString`, `PyLong_AsLongLong`, and many others are re-implemented using MicroPython’s API (e.g. `mp_call_function_n_kw`, `mp_raise_TypeError`, etc.).
- **Minimal Overhead:** The goal is not to achieve full CPython compatibility but to support enough of the API so that a wide range of Cython-generated extensions can run without modification.
- **Layered Approach:** By providing incremental stubs (with functions, macros, and even minimal GIL support), this layer allows for iterative improvements and gradual expansion of supported CPython APIs.

## Design Details

- **Function Wrappers:** We’ve implemented key functions (e.g. error reporting, memory allocation, calling protocols) in a minimal way. For example, `PyRun_SimpleString` is implemented to create a lexer, parse, compile, and then execute Python code from a C string.
- **Error and Exception Handling:** Minimal implementations for functions like `PyErr_Format`, `PyErr_Fetch`, and `PyErr_Restore` allow for basic exception propagation from Cython modules.
- **Memory Allocation:** The compatibility layer wraps MicroPython’s `m_malloc`, `m_realloc`, and `m_free` functions to provide `PyMem_Malloc` and related routines.
- **Call Protocol Support:** We supply stubs for various `PyObject_Call*` APIs to allow a Cython module to call Python functions (including those implemented purely in Python or frozen in the firmware) without needing source access.
- **Limitations:** Some complex CPython APIs are not fully implemented (or only minimally so), and further refinement may be necessary as real-world modules are tested.

## Why Not Modify Cython?

- **Codebase Size:** Cython is a large and sophisticated compiler designed for CPython. Retrofitting it to understand the nuances of MicroPython would require extensive changes.
- **Focus & Expertise:** The Cython team has historically concentrated on CPython, and adapting to MicroPython would force them to learn a fundamentally different runtime environment.
- **Maintenance Overhead:** Supporting two diverging runtimes (CPython and MicroPython) in Cython’s code generation would significantly complicate maintenance and future development.


## Contributing

Contributions are welcome! If you have patches, additional API stubs, or improvements to thr compatibility layer, please submit pull requests. We especially welcome input from developers familiar with CPython’s C API and the internal workings of MicroPython.

## License

This project is licensed under the GNU Lesser General Public License v2.1.
