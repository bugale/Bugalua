'use strict';

const console = { log: (...args) => host.diagnostics.debugLog(...args, '\n') };

function getLuaState(luaAddr) {
  // #region General Types
  const ptrsize = host.currentSession.Attributes.Machine.PointerSize;
  const voidType = { size: 0, name: 'void', from(addr) { return addr; } };

  function int(size, name, signed = false) {
    return {
      size,
      name,
      from(addr) { return parseInt(host.memory.readMemoryValues(addr, 1, size, signed)[0]); },
    };
  }
  const [uint8, uint32] = [int(1, 'uint8'), int(4, 'uint32')];
  const sizeT = int(ptrsize, 'size_t');

  function ptr(type) {
    return {
      size: ptrsize,
      name: `${type.name}*`,
      from(addr) {
        return {
          addr: sizeT.from(addr),
          type: type.name,
          get value() { return type.from(sizeT.from(addr)); },
          toString() { return `[${type.name}* at 0x${addr.toString(16)}]`; },
        };
      },
    };
  }
  function ptr32(type) {
    return {
      size: 4,
      name: `${type.name}*`,
      from(addr) {
        return {
          addr: uint32.from(addr),
          type: type.name,
          get value() { return type.from(uint32.from(addr)); },
          toString() { return `[${type.name}* at 0x${addr.toString(16)}]`; },
        };
      },
    };
  }

  function array(type, length) {
    class Array {
      constructor(addr) {
        Object.defineProperties(this, {
          length: { value: length, writable: false },
          [Symbol.iterator]: {
            value: () => {
              let index = 0;

              return {
                next: () => ({
                  done: index >= this.length,
                  value: type.from(addr + (index++) * type.size),
                }),
              };
            },
          },
        });
      }
    }
    return { size: length * type.size, name: (`${type.name}[${length}]`), from(addr) { return new Array(addr); } };
  }

  function struct(name, fieldsArr) {
    let offset = 0;
    const fields = [];
    fieldsArr.forEach(([type, fieldName]) => {
      fields.push({ type, fieldName, offset });
      offset += type.size;
    });
    class Struct {
      constructor(addr) {
        fields.forEach((field) => {
          Object.defineProperty(this, field.fieldName, {
            get() {
              return field.type.from(addr + field.offset);
            },
          });
        });
        this.toString = () => `[struct ${name} at 0x${addr.toString(16)}]`;
      }
    }
    return { size: offset, name, from(addr) { return new Struct(addr); } };
  }

  function defer(fn, name) {
    return { get size() { return fn().size; }, name, get from() { return fn().from; } };
  }
  // #endregion General Types

  // #region LuaJIT 2.0.5 Types
  const MSize = uint32;
  const MRef = ptr32;
  const GCRef = ptr32;

  const luaJit205GCState = struct('GC_State', [
    [MSize, 'total'],
    [MSize, 'threshold'],
    [uint8, 'currentwhite'],
    [uint8, 'state'],
    [uint8, 'nocdatafin'],
    [uint8, 'unused2'],
    [MSize, 'sweepstr'],
    [GCRef(voidType), 'root'],
    [MRef(voidType), 'sweep'],
    [GCRef(voidType), 'gray'],
    [GCRef(voidType), 'grayagain'],
    [GCRef(voidType), 'weak'],
    [GCRef(voidType), 'mmudata'],
    [MSize, 'stepmul'],
    [MSize, 'debt'],
    [MSize, 'estimate'],
    [MSize, 'pause'],
  ]);

  const luaJit205GlobalState = struct('global_State', [
    [ptr(GCRef(voidType)), 'strhash'],
    [MSize, 'strmask'],
    [MSize, 'strnum'],
    [ptr(voidType), 'allocf'],
    [ptr(voidType), 'allocd'],
    [luaJit205GCState, 'gc'],
    /* Incomplete */
  ]);

  const luaJit205LuaState = struct('lua_State', [
    [GCRef(voidType), 'nextgc'],
    [uint8, 'marked'],
    [uint8, 'gct'],
    [uint8, 'dummy_ffid'],
    [uint8, 'status'],
    [MRef(luaJit205GlobalState), 'glref'],
    /* Incomplete */
  ]);
  // #endregion LuaJIT 2.0.5 Types

  // #region Frontend
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

  class LuaState {
    constructor(addr) {
      this.L = luaJit205LuaState.from(addr);
    }

    get address() {
      return this.L.addr;
    }

    get MemoryUsage() {
      return new SizeBytes(this.L.glref.value.gc.total);
    }
  }
  // #endregion Frontend

  return new LuaState(luaAddr);
}

function initializeScript() {
  return [
    new host.functionAlias((addr) => getLuaState(addr), 'lua'),
  ];
}

const exports = [
  console,
  initializeScript,
];
