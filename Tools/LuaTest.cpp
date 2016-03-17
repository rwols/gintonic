#include "gintonic.hpp"
#include <string>
#include <iostream>

#include "../Source/lua-5.3.1/src/lua.hpp"

int main(int argc, char** argv) 
{
	std::cout << "\n\tVery crude interactive Lua interpreter.\n\n";
	std::unique_ptr<lua_State, decltype(&lua_close)> state(luaL_newstate(), lua_close);
	luaL_openlibs(state.get());
	std::string line;
	int error;
	while (std::getline(std::cin, line))
	{
		error = luaL_loadbuffer(state.get(), line.c_str(), line.length(), "line") || lua_pcall(state.get(), 0, 0, 0);
		if (error)
		{
			std::cerr << lua_tostring(state.get(), -1) << '\n';
			lua_pop(state.get(), 1); // Pop the error msg
		}
	}
	return 0;
}