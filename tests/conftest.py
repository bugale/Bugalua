from typing import Any

from cffi import FFI
import pytest


from infra import lua_loaders


@pytest.fixture
def _ffi_and_lua() -> tuple[FFI, Any]:
    return lua_loaders.luajit_205()


@pytest.fixture
def ffi(_ffi_and_lua: tuple[FFI, Any]) -> FFI:
    return _ffi_and_lua[0]


@pytest.fixture
def lua(_ffi_and_lua: tuple[FFI, Any]) -> Any:
    return _ffi_and_lua[1]


@pytest.fixture
def lua_state(lua: Any) -> Any:
    state = lua.luaL_newstate()
    lua.luaL_openlibs(state)
    yield state
    lua.lua_close(state)


@pytest.fixture
def lua_state_addr(ffi: FFI, lua_state: Any) -> int:
    return int(ffi.cast('size_t', lua_state))
