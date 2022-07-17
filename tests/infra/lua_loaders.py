import os
from typing import Any

from cffi import FFI


def luajit_205() -> tuple[FFI, Any]:
    """ LuaJit 2.0.5 FFI object """
    ffi = FFI()
    directory = os.path.join(os.path.dirname(__file__), '..', 'binaries', 'luajit-2.0.5-Win-x64')
    with open(os.path.join(directory, 'header.h'), encoding='utf-8') as header:
        ffi.cdef(header.read())
    return ffi, ffi.dlopen(os.path.join(directory, 'lua51.dll'))
