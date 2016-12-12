#include "Foundation/Console.hpp"
#include "Foundation/scancodes.hpp"
#include "Graphics/Renderer.hpp"

namespace gintonic {

int Console::printCallback(lua_State* L)
{
	lua_pushstring(L, "Console"); // push key
	lua_gettable(L, LUA_REGISTRYINDEX); // get value from registry[key]
	Console* console = (Console*) lua_touserdata(L, -1);
	lua_pop(L, 1); // pop the value, we don't need it anymore
	const int nargs = lua_gettop(L);
	for (int i = 1; i <= nargs; ++i)
	{
		const char* str = lua_tostring(L, i);
		if (str) console->mBuffer.emplace_back(str);
	}
	return 0;
}

Console::Console() : mLuaState(luaL_newstate(), &lua_close)
{
	static const struct luaL_Reg printlib [] = 
	{
		{"print", Console::printCallback},
		{nullptr, nullptr} /* end of array */
	};
	luaL_openlibs(mLuaState.get());
	lua_getglobal(mLuaState.get(), "_G");
	luaL_setfuncs(mLuaState.get(), printlib, 0);
	lua_pop(mLuaState.get(), 1);
	lua_pushstring(mLuaState.get(), "Console");  // push key
	lua_pushlightuserdata(mLuaState.get(), this); // push value
	lua_settable(mLuaState.get(), LUA_REGISTRYINDEX); // registry[key] = value
	subscribeToRendererEvents();
}

Console::Console(lua_State* luaState) : mLuaState(luaState, &lua_close)
{
	if (!luaState) throw LuaStatePointerIsNullException();
	subscribeToRendererEvents();
}

void Console::subscribeToRendererEvents()
{
	Renderer::onTextInput.connect( [this] (const char text[32]) -> void
	{
		const std::size_t length = std::strlen(text);
		mActiveString.append(text, length);
		cursorPosition += length;
	});
	Renderer::onTextEdit.connect( [this] (const char edit[32], const int start, const int length) -> void
	{
		std::cout << "onTextEdit(" << edit << ", " << start << ", " << length << ")\n";
	});
	Renderer::onKeyPress.connect( [this] (const int scancode, const int keycode, const int mod) -> void
	{
		switch (scancode)
		{
			case SDL_SCANCODE_RETURN:
				this->compileActiveString();
				break;
			case SDL_SCANCODE_BACKSPACE:
				if (cursorPosition == 0) break;
				mActiveString.erase(
					mActiveString.begin() + cursorPosition - 1, 
					mActiveString.begin() + cursorPosition);
				--cursorPosition;
				break;
			case SDL_SCANCODE_DELETE:
				if (cursorPosition == mActiveString.length() - 1) break;
				mActiveString.erase(
					mActiveString.begin() + cursorPosition, 
					mActiveString.begin() + cursorPosition + 1);
				break;
			case SDL_SCANCODE_LEFT:
				this->cursorPosition = std::max(0, (int)this->cursorPosition - 1);
				break;
			case SDL_SCANCODE_RIGHT:
				this->cursorPosition = std::min(this->mActiveString.length(), this->cursorPosition + 1);
				break;
			default:
				break;
		}
	});
}

bool Console::insertCharAtCursorPosition(const char c)
{
	if (c == '\n') // enter key
	{
		return compileActiveString();
	}
	else if (c == '\b') // backspace key
	{
		if (cursorPosition == 0) return false;
		mActiveString.erase(mActiveString.begin() + cursorPosition - 1, mActiveString.begin() + cursorPosition);
		--cursorPosition;
		return true;
	}
	else if (c == 127) // delete key
	{
		if (cursorPosition == mActiveString.length() - 1) return false;
		mActiveString.erase(mActiveString.begin() + cursorPosition, mActiveString.begin() + cursorPosition + 1);
		return true;
	}
	else
	{
		mActiveString.insert(cursorPosition, 1, c);
		++cursorPosition;
		return true;
	}
}

bool Console::appendChar(const char c)
{
	if (c == '\n') return compileActiveString();  // enter key
	else if (c == '\b') return removeCharAtEnd(); // backspace key
	else mActiveString.push_back(c);
	return true;
}

bool Console::removeCharAtEnd()
{
	if (mActiveString.empty() == false)
	{
		mActiveString.erase(std::prev(mActiveString.end()));
		return true;
	}
	else
	{
		return false;
	}
}

bool Console::appendChunk(const std::string& chunk)
{
	mActiveString.append(chunk);
	return true;
}

bool Console::appendChunk(const char* start, const std::size_t length)
{
	mActiveString.append(start, start + length);
	return true;
}

bool Console::appendChunk(const char* begin, const char* end)
{
	mActiveString.append(begin, end);
	return true;
}

bool Console::appendChunk(const char* chunk)
{
	mActiveString.append(chunk);
	return true;
}

bool Console::compileActiveString()
{
	auto lError = luaL_loadbuffer(mLuaState.get(), mActiveString.c_str(), mActiveString.length(), "line");
	mBuffer.emplace_back(std::move(mActiveString));
	mActiveString.clear();
	cursorPosition = 0;
	if (lError)
	{
		mLastError = lua_tostring(mLuaState.get(), -1);
		lua_pop(mLuaState.get(), 1);  /* pop error message from the stack */
		mBuffer.push_back(mLastError);
		return false;
	}
	else
	{
		lError = lua_pcall(mLuaState.get(), 0, 0, 0);
		if (lError)
		{
			mLastError = lua_tostring(mLuaState.get(), -1);
			lua_pop(mLuaState.get(), 1); /* pop error message from the stack */
			mBuffer.push_back(mLastError);
			return false;
		}
		else
		{
			return true;
		}
	}
}

} // namespace gintonic