from typing import Any
import logging

from cffi import FFI
import pytest

from infra import dumps

logger = logging.getLogger('tests')


@pytest.mark.parametrize('usage', [0, 1024, 133337, 1024 * 512, 1024 * 1024 * 512])
def test_memory_usage(ffi: FFI, lua: Any, lua_state: Any, lua_state_addr: int, usage: int) -> None:
    """ Test """
    lua.lua_newuserdata(lua_state, usage)
    dump = dumps.take_dump(lua=lua, ffi=ffi, lua_state=lua_state, code='''
        gcusage = collectgarbage("count")
        dump()
    ''')
    lua.lua_pushstring(lua_state, b'gcusage')
    lua.lua_gettable(lua_state, lua.LUA_GLOBALSINDEX)
    gcusage = lua.lua_tonumber(lua_state, -1)
    lua.lua_remove(lua_state, -1)
    assert (gcusage * 1024) >= usage, 'GC usage is smaller than expected'

    output = dumps.run_windbg_commands(dump=dump, commands=f'dx @$scriptContents.getLuaState({lua_state_addr}).MemoryUsage.bytes')
    memory_used = int(output.split(':')[2].strip(), 0)
    assert memory_used == gcusage * 1024, 'Memory usage is not as expected'
