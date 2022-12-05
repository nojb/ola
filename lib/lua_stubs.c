#include <lua.h>
#include <lauxlib.h>

#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/callback.h>
#include <caml/fail.h>

#include <assert.h>

#define State_val(v) ((lua_State *)(Nativeint_val(v)))

value lua__newstate(value v_unit)
{
  return caml_copy_nativeint((intnat)luaL_newstate());
}

#define LUA_PUSH(type,Val) \
  value lua__push##type(value v_state, value v) \
  { \
    lua_push##type(State_val(v_state), Val(v)); \
    return Val_unit; \
  }

LUA_PUSH(boolean, Bool_val);
LUA_PUSH(number, Double_val);
LUA_PUSH(integer, Long_val);

value lua__pushnil(value v_state)
{
  lua_pushnil(State_val(v_state));
  return Val_unit;
}

value lua__pushstring(value v_state, value v_string)
{
  lua_pushlstring(State_val(v_state), String_val(v_string), caml_string_length(v_string));
  return Val_unit;
}

static int release_ml(lua_State *L)
{
  value *v_f = lua_touserdata(L, 1);
  caml_remove_generational_global_root(v_f);
  return 0;
}

static int call_ml(lua_State *L)
{
  value *v_f = lua_touserdata(L, lua_upvalueindex(1));
  value v_res = caml_callback_exn(*v_f, caml_copy_nativeint((intnat)L));
  if (Is_exception_result(v_res)) {
    lua_pushstring(L, "callback error");
    lua_error(L);
  }
  return Long_val(v_res);
}

value lua__pushfunction(value v_state, value v_f)
{
  lua_State *L = State_val(v_state);
  value *f = lua_newuserdata(L, sizeof(value));
  *f = v_f;
  caml_register_generational_global_root(f);
  lua_newtable(L);
  lua_pushcfunction(L, &release_ml);
  lua_setfield(L, -2, "__gc");
  lua_setmetatable(L, -2);
  lua_pushcclosure(L, &call_ml, 1);
  return Val_unit;
}

value lua__checkstack(value v_state, value v_sz)
{
  return Val_bool(lua_checkstack(State_val(v_state), Long_val(v_sz)));
}

#define LUA_IS(type) \
  value lua__is##type(value v_state, value v_index) { \
    return Val_bool(lua_is##type(State_val(v_state), Long_val(v_index))); \
  }

LUA_IS(number);
LUA_IS(string);

#define LUA_TO(type, Val) \
  value lua__to##type(value v_state, value v_index) \
  { \
    return Val(lua_to##type(State_val(v_state), Long_val(v_index))); \
  }

LUA_TO(boolean, Val_bool);
LUA_TO(number, caml_copy_double);
LUA_TO(integer, Val_long);

value lua__tostring(value v_state, value v_index)
{
  lua_State *L = State_val(v_state);
  size_t n;
  const char *p = lua_tolstring(L, Long_val(v_index), &n);
  return caml_alloc_initialized_string(n, p);
}

value lua__pcall(value v_state, value v_nargs)
{
  lua_State *L = State_val(v_state);
  int r = lua_pcall(L, Long_val(v_nargs), LUA_MULTRET, 0);
  switch (r) {
    case LUA_ERRRUN:
      caml_failwith(lua_tostring(L, -1));
    case LUA_ERRMEM:
      caml_failwith(lua_tostring(L, -1));
    case LUA_ERRERR:
      caml_failwith(lua_tostring(L, -1));
    case LUA_OK:
      break;
    default:
      assert(0);
  }
  return Val_unit;
}

value lua__pop(value v_state, value v_n)
{
  lua_pop(State_val(v_state), Long_val(v_n));
  return Val_unit;
}

value lua__setglobal(value v_state, value v_name)
{
  lua_setglobal(State_val(v_state), String_val(v_name));
  return Val_unit;
}

value lua__getglobal(value v_state, value v_name)
{
  lua_getglobal(State_val(v_state), String_val(v_name));
  return Val_unit;
}

value lua__close(value v_state)
{
  lua_close(State_val(v_state));
  return Val_unit;
}

value lua__loadstring(value v_state, value v_s)
{
  int r = luaL_loadstring(State_val(v_state), String_val(v_s));
  if (r != LUA_OK) {
    caml_failwith("ERR");
  }
  return Val_unit;
}
