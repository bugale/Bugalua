/* option for multiple returns in `lua_pcall' and `lua_call' */
#define LUA_MULTRET	-1


/*
** pseudo-indices
*/
#define LUA_REGISTRYINDEX	-10000
#define LUA_ENVIRONINDEX	-10001
#define LUA_GLOBALSINDEX	-10002


/* thread status; 0 is OK */
#define LUA_YIELD	1
#define LUA_ERRRUN	2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM	4
#define LUA_ERRERR	5


typedef struct lua_State lua_State;

typedef int (*lua_CFunction) (lua_State *L);


/*
** functions that read/write blocks when loading/dumping Lua chunks
*/
typedef const char * (*lua_Reader) (lua_State *L, void *ud, size_t *sz);

typedef int (*lua_Writer) (lua_State *L, const void* p, size_t sz, void* ud);


/*
** prototype for memory-allocation functions
*/
typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t nsize);


/*
** basic types
*/
#define LUA_TNONE		-1

#define LUA_TNIL		0
#define LUA_TBOOLEAN		1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER		3
#define LUA_TSTRING		4
#define LUA_TTABLE		5
#define LUA_TFUNCTION		6
#define LUA_TUSERDATA		7
#define LUA_TTHREAD		8



/* minimum Lua stack available to a C function */
#define LUA_MINSTACK	20


/* type of numbers in Lua */
typedef double lua_Number;


/* type for integer functions */
typedef size_t lua_Integer;



/*
** state manipulation
*/
extern lua_State *(lua_newstate) (lua_Alloc f, void *ud);
extern void       (lua_close) (lua_State *L);
extern lua_State *(lua_newthread) (lua_State *L);

extern lua_CFunction (lua_atpanic) (lua_State *L, lua_CFunction panicf);


/*
** basic stack manipulation
*/
extern int   (lua_gettop) (lua_State *L);
extern void  (lua_settop) (lua_State *L, int idx);
extern void  (lua_pushvalue) (lua_State *L, int idx);
extern void  (lua_remove) (lua_State *L, int idx);
extern void  (lua_insert) (lua_State *L, int idx);
extern void  (lua_replace) (lua_State *L, int idx);
extern int   (lua_checkstack) (lua_State *L, int sz);

extern void  (lua_xmove) (lua_State *from, lua_State *to, int n);


/*
** access functions (stack -> C)
*/

extern int             (lua_isnumber) (lua_State *L, int idx);
extern int             (lua_isstring) (lua_State *L, int idx);
extern int             (lua_iscfunction) (lua_State *L, int idx);
extern int             (lua_isuserdata) (lua_State *L, int idx);
extern int             (lua_type) (lua_State *L, int idx);
extern const char     *(lua_typename) (lua_State *L, int tp);

extern int            (lua_equal) (lua_State *L, int idx1, int idx2);
extern int            (lua_rawequal) (lua_State *L, int idx1, int idx2);
extern int            (lua_lessthan) (lua_State *L, int idx1, int idx2);

extern lua_Number      (lua_tonumber) (lua_State *L, int idx);
extern lua_Integer     (lua_tointeger) (lua_State *L, int idx);
extern int             (lua_toboolean) (lua_State *L, int idx);
extern const char     *(lua_tolstring) (lua_State *L, int idx, size_t *len);
extern size_t          (lua_objlen) (lua_State *L, int idx);
extern lua_CFunction   (lua_tocfunction) (lua_State *L, int idx);
extern void	       *(lua_touserdata) (lua_State *L, int idx);
extern lua_State      *(lua_tothread) (lua_State *L, int idx);
extern const void     *(lua_topointer) (lua_State *L, int idx);


/*
** push functions (C -> stack)
*/
extern void  (lua_pushnil) (lua_State *L);
extern void  (lua_pushnumber) (lua_State *L, lua_Number n);
extern void  (lua_pushinteger) (lua_State *L, lua_Integer n);
extern void  (lua_pushlstring) (lua_State *L, const char *s, size_t l);
extern void  (lua_pushstring) (lua_State *L, const char *s);
extern const char *(lua_pushfstring) (lua_State *L, const char *fmt, ...);
extern void  (lua_pushcclosure) (lua_State *L, lua_CFunction fn, int n);
extern void  (lua_pushboolean) (lua_State *L, int b);
extern void  (lua_pushlightuserdata) (lua_State *L, void *p);
extern int   (lua_pushthread) (lua_State *L);


/*
** get functions (Lua -> stack)
*/
extern void  (lua_gettable) (lua_State *L, int idx);
extern void  (lua_getfield) (lua_State *L, int idx, const char *k);
extern void  (lua_rawget) (lua_State *L, int idx);
extern void  (lua_rawgeti) (lua_State *L, int idx, int n);
extern void  (lua_createtable) (lua_State *L, int narr, int nrec);
extern void *(lua_newuserdata) (lua_State *L, size_t sz);
extern int   (lua_getmetatable) (lua_State *L, int objindex);
extern void  (lua_getfenv) (lua_State *L, int idx);


/*
** set functions (stack -> Lua)
*/
extern void  (lua_settable) (lua_State *L, int idx);
extern void  (lua_setfield) (lua_State *L, int idx, const char *k);
extern void  (lua_rawset) (lua_State *L, int idx);
extern void  (lua_rawseti) (lua_State *L, int idx, int n);
extern int   (lua_setmetatable) (lua_State *L, int objindex);
extern int   (lua_setfenv) (lua_State *L, int idx);


/*
** `load' and `call' functions (load and run Lua code)
*/
extern void  (lua_call) (lua_State *L, int nargs, int nresults);
extern int   (lua_pcall) (lua_State *L, int nargs, int nresults, int errfunc);
extern int   (lua_cpcall) (lua_State *L, lua_CFunction func, void *ud);
extern int   (lua_load) (lua_State *L, lua_Reader reader, void *dt,
                                        const char *chunkname);

extern int (lua_dump) (lua_State *L, lua_Writer writer, void *data);


/*
** coroutine functions
*/
extern int  (lua_yield) (lua_State *L, int nresults);
extern int  (lua_resume) (lua_State *L, int narg);
extern int  (lua_status) (lua_State *L);

/*
** garbage-collection function and options
*/

#define LUA_GCSTOP		0
#define LUA_GCRESTART		1
#define LUA_GCCOLLECT		2
#define LUA_GCCOUNT		3
#define LUA_GCCOUNTB		4
#define LUA_GCSTEP		5
#define LUA_GCSETPAUSE		6
#define LUA_GCSETSTEPMUL	7

extern int (lua_gc) (lua_State *L, int what, int data);


/*
** miscellaneous functions
*/

extern int   (lua_error) (lua_State *L);

extern int   (lua_next) (lua_State *L, int idx);

extern void  (lua_concat) (lua_State *L, int n);

extern lua_Alloc (lua_getallocf) (lua_State *L, void **ud);
extern void lua_setallocf (lua_State *L, lua_Alloc f, void *ud);


/*
** auxillary functions
*/

extern lua_State *(luaL_newstate) (void);
extern void luaL_openlibs(lua_State *L);
extern int luaL_loadstring(lua_State *L, const char *s);
