# MicroPython-Cython Compatibility Layer

![Cython2Upy](https://github.com/user-attachments/assets/a635b8ad-a53c-45ff-8a63-679599729726)

This repository provides a compatibility header and stubs to enable Cython-generated modules (targeting the CPython API) to run on MicroPython.

## Overview

[MicroPython](https://github.com/micropython/micropython) is a lightweight Python implementation for microcontrollers and other constrained systems. Unlike CPython, its API is designed for efficiency and minimal footprint; as a result, many of the functions and calling conventions available in CPython are either missing or implemented differently.

[Cython](https://github.com/cython/cython), on the other hand, is built around the CPython API. Its code generation and extension mechanism assume that a vast, mature API is available. However, adapting Cython itself to generate MicroPython-compatible code is a monumental task. The Cython codebase is huge and deeply intertwined with CPython semantics, and its core developers are experts on CPython—not on MicroPython.

This takes a different approach: rather than modifying Cython, we implement a compatibility layer that maps the CPython API calls (as expected by Cython-generated modules) onto the corresponding functions in MicroPython’s public API.

## Problem Statement

- **API Mismatch:** MicroPython’s C API is a subset of CPython’s, lacking or altering functions like `PyObject_Call` and `PyErr_SetString` that Cython relies on.
- **Cython Complexity:** Modifying Cython for MicroPython demands deep knowledge of both runtimes, a daunting task given Cython’s scale and CPython focus.
- **Expertise Gap:** Cython developers specialize in CPython, not MicroPython’s unique memory, type, and calling systems.

## Proposed Solution

This compatibility layer re-implements key CPython API functions using MicroPython’s API, including:

- **API Mapping:** Functions like `PyObject_Call` and `PyLong_AsLongLong` are rewritten with MicroPython calls (e.g., `mp_call_function_n_kw`).
- **Lightweight Design:** Supports essential APIs for Cython modules with minimal overhead, not full CPython emulation.
- **Incremental Stubs:** Provides functions, macros, and basic GIL support, expandable over time.

## Design Details

- **Function Wrappers:** Implements essentials like `PyRun_SimpleString` (via parsing and execution) and error-handling routines.
- **Exceptions:** Stubs for `PyErr_Format`, `PyErr_Fetch`, etc., enable basic exception propagation.
- **Memory:** Wraps `m_malloc`, `m_realloc`, and `m_free` for `PyMem_Malloc` compatibility.
- **Calling:** Supports `PyObject_Call*` APIs for invoking Python functions, including firmware-frozen ones.
- **Limitations:** Some CPython APIs are minimally implemented, requiring refinement as use cases emerge.

## Contributing

We welcome pull requests for patches, new stubs, or enhancements—especially from those familiar with CPython’s C API or MicroPython internals.

## License

Licensed under the GNU Lesser General Public License v2.1.
