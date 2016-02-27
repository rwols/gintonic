#include "luabindings.hpp"
#include "lua-5.3.1/src/lua.hpp"
#include <iostream>
#include <cassert>
#include "filesystem.hpp"
#include "vec2f.hpp"

namespace {

static void register_vec2f(lua_State* L)
{
	// lua_pushcclosure(L, &vec2f::vec2f);
}

static void register_vec3f(lua_State* L)
{

}

} // anonymous namespace

namespace gintonic {

void luatest()
{

}

} // namespace gintonic