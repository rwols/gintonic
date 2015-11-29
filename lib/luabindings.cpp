#include "luabindings.hpp"
#include "selene/selene.h"
#include <iostream>
#include <cassert>
#include "filesystem.hpp"
#include "vec2f.hpp"

namespace {

void init_vec2f_lua_bindings(sel::State& state)
{
	using gintonic::vec2f;
	state["vec2f"].SetClass<vec2f, void>();
	state["vec2f"].SetClass<vec2f, float>("x", &vec2f::x);
	state["vec2f"].SetClass<vec2f, float>("y", &vec2f::y);
}

void init_vec3f_lua_bindings(sel::State& state)
{

}

void init_vec4f_lua_bindings(sel::State& state)
{

}

void init_mat2f_lua_bindings(sel::State& state)
{

}

void init_mat3f_lua_bindings(sel::State& state)
{

}

void init_mat4f_lua_bindings(sel::State& state)
{

}

void init_quatf_lua_bindings(sel::State& state)
{

}

void init_SQT_lua_bindings(sel::State& state)
{

}

void init_entity_lua_bindings(sel::State& state)
{

}

void init_components_lua_bindings(sel::State& state)
{

}

} // anonymous namespace

namespace gintonic {

void luatest()
{
	#ifdef BOOST_MSVC
		boost::filesystem::current_path(get_executable_path());
	#else
		boost::filesystem::current_path(get_executable_path() / "..");
	#endif
	std::cout << "Hello world!\n";
	sel::State state{true};
	state.Load("../scripts/test.lua");
	std::string bar = state["bar"][3];
	std::string baz = state["bar"]["key"];
	std::cout << bar << ' ' << baz << '\n';
}

sel::State get_lua_state()
{
	sel::State result{true};
	init_vec2f_lua_bindings(result);
	init_vec3f_lua_bindings(result);
	init_vec4f_lua_bindings(result);
	init_mat2f_lua_bindings(result);
	init_mat3f_lua_bindings(result);
	init_mat4f_lua_bindings(result);
	init_quatf_lua_bindings(result);
	init_SQT_lua_bindings(result);
	init_entity_lua_bindings(result);
	init_components_lua_bindings(result);
	return result;
}

} // namespace gintonic