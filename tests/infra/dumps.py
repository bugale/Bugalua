import os
import tempfile
import subprocess
from typing import Any
import logging

from cffi import FFI
from minidump.utils import createminidump

logger = logging.getLogger('tests')


def take_dump(lua: Any, ffi: FFI, lua_state: Any, code: str) -> str:
    """ Runs the given lua code, exposing a `dump()` global function. Returns the name of the dump file generated """
    with tempfile.NamedTemporaryFile(suffix='.dmp', delete=False) as dump_file:
        filename = dump_file.name
    dumps_taken = 0

    def dump(_: Any) -> int:
        nonlocal dumps_taken
        logger.debug('Taking a dump')
        try:
            if dumps_taken == 0:
                createminidump.create_dump(pid=os.getpid(), output_filename=filename, mindumptype=(
                    createminidump.MINIDUMP_TYPE.MiniDumpWithFullMemory |
                    createminidump.MINIDUMP_TYPE.MiniDumpIgnoreInaccessibleMemory |
                    createminidump.MINIDUMP_TYPE.MiniDumpWithUnloadedModules |
                    createminidump.MINIDUMP_TYPE.MiniDumpWithProcessThreadData |
                    createminidump.MINIDUMP_TYPE.MiniDumpWithFullMemoryInfo |
                    createminidump.MINIDUMP_TYPE.MiniDumpWithThreadInfo |
                    createminidump.MINIDUMP_TYPE.MiniDumpWithHandleData |
                    createminidump.MINIDUMP_TYPE.MiniDumpWithTokenInformation
                ))
            dumps_taken += 1
        except Exception:
            logger.exception('Failed taking dump')
        return 0

    callback = ffi.callback('lua_CFunction', dump)
    lua.lua_pushstring(lua_state, b'dump')
    lua.lua_pushcclosure(lua_state, callback, 0)
    lua.lua_settable(lua_state, lua.LUA_GLOBALSINDEX)
    lua.luaL_loadstring(lua_state, code.encode('utf-8'))
    if lua.lua_pcall(lua_state, 0, 0, 0) != 0:
        error_message = ffi.string(lua.lua_tolstring(lua_state, -1, ffi.NULL)).decode('utf-8')
        lua.lua_remove(lua_state, -1)
        raise Exception(f'Error running Lua code {error_message}')
    assert dumps_taken == 1, f'Expected exactly one dump to be requested, {dumps_taken} were requested'
    assert os.path.isfile(filename), 'No dump was created'
    logger.debug('Successfully took dump at: %s', filename)
    return filename


def run_windbg_commands(dump: str, commands: str) -> str:
    """ Opens the given dump file in WinDbg, runs the given commands, and returns the output """
    token = '***WINDBGTOKEN***'
    with tempfile.NamedTemporaryFile(mode='w', encoding='utf-8', delete=False) as commands_file:
        commands_file.write(f'''
!lua
.echo "{token}"
{commands}
.echo "{token}"
qq
''')
    logger.debug('Running windbg commands: %s', commands)
    proc = subprocess.run([r'C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe', '-z', dump, '-c', f'$<{commands_file.name}'],
                          check=False, capture_output=True, encoding='utf-8')
    # Token appears 4 times (twice inside the command echo, and twice as its output)
    output = proc.stdout.split(token)[2].strip()  # Take the middle part
    output = '\n'.join(output.split('\n')[:-1])  # Remove the last line (which is the beginning of the token echo command)
    logger.debug('windbg commands: %s returned: %s', commands, output)
    return output
