'use strict';

const console = { log: (...args) => host.diagnostics.debugLog(...args, '\n') };

class SizeBytes {
  constructor(bytes) {
    this.bytes = bytes;
  }

  toString() {
    if (this.bytes <= 512) {
      return `${this.bytes.toLocaleString()} B`;
    }

    const unitsArr = ['KiB', 'MiB', 'GiB', 'TiB', 'PiB', 'EiB', 'ZiB', 'YiB'];
    let number = this.bytes / 1024;
    let units = 0;
    while (number > 512) {
      number /= 1024;
      units += 1;
    }
    return `${number.toFixed(2)} ${unitsArr[units]} (${this.bytes.toLocaleString()} B)`;
  }
}

class LuaStateVisualizer {
  get MemoryUsage() {
    const glref = host.evaluateExpression(`(global_State*)${this.glref.ptr32}`);
    return new SizeBytes(glref.gc.total);
  }
}

function bangLua(addr) {
  try {
    host.evaluateExpression('(lua_State*)0');
  } catch (err) {
    console.log('No Lua symbols found. Please load the Lua symbols first.');
    return null;
  }
  if (typeof addr === 'undefined') {
    console.log('Usage: !lua <lua_state_address>');
    return null;
  }
  return host.evaluateExpression(`(lua_State*)${addr}`);
}

function initializeScript() {
  return [
    new host.functionAlias((addr) => bangLua(addr), 'lua'),
    new host.typeSignatureRegistration(LuaStateVisualizer, 'lua_State'),
  ];
}

const exports = [
  initializeScript,
];
