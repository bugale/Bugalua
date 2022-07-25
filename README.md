# Bugalua
`Bugalua` is an extension for WinDbg for debugging [LuaJIT](https://github.com/LuaJIT/LuaJIT)

It currently allows the user to:
* See the memory usage of a given lua_State object

`Bugalua` requires the symbols of the LuaJIT library to be loaded.

## Using Bugalua
To use Bugalua you need to firs load the file `bugalua/bugalua.js` from this repository to WinDbg.
This can be done by running:

   ```text
   0:000> .scriptload "C:\path_to_Bugalua_repo\bugalua\bugalua.js"
   JavaScript script successfully loaded from 'C:\path_to_Bugalua_repo\bugalua\bugalua.js'
   ```

Alternatively, you can install `Bugalua` so that it will be automatically loaded when you run `!lua`. See next section.

After `Bugalua` is successfully loaded, you can use it by running `!lua <address>`:

   ```text
   0:000> !lua 0x25b50378
   (lua_State*)0x25b50378                 : 0x25b50378 : [object Object] [Type: lua_State *]
       [<Raw View>]     [Type: lua_State]
       MemoryUsage      : 26.10 KiB (26,724 B)
   ```

Or, by just looking at an object of type `lua_State*`:
   ```text
   0:000> dx (lua_State*)0x25b50378
   (lua_State*)0x25b50378                 : 0x25b50378 : [object Object] [Type: lua_State *]
       [<Raw View>]     [Type: lua_State]
       MemoryUsage      : 26.10 KiB (26,724 B)
   ```

## Installing Bugalua

If you would like to have `!lua` loaded every time your debugger starts instead of loading the `Bugalua` manually follow the below steps:

1. Clone this GitHub repository,

2. If you want to install it in the default location (`C:\Program Files (x86)\Windows Kits\10\Debuggers\x64`), run the file `Insttall.ps1` in the root directory of the repository. For any other location, run `.\Install.ps1 C:\the_path_to_your_windbg\windbg.exe`. This will also work on `WinDbg Preview` (i.e. `WinDbgX.exe`).

## Compatibility
`Bugalua` is currently only officialy compatible with these Lua versions:
* [LuaJIT 2.0.5](https://github.com/LuaJIT/LuaJIT/releases/tag/v2.0.5) default build (i.e. no LJ_64)

Nevertheless, it might work with other versions as well, if their internal implementation is similar enough.
