/**
 * @file Console.hpp
 * @brief Defines a Console.
 * @author Raoul Wols
 */

#pragma once

#include "../Graphics/Font.hpp"
#include <lua.hpp>

namespace gintonic {

class Console
{
public:

	class LuaStatePointerIsNullException : public std::exception
	{
	public:
		inline LuaStatePointerIsNullException() noexcept {}
		virtual ~LuaStatePointerIsNullException() noexcept = default;
		inline const char* what() const noexcept { return "LuaStatePointerIsNullException"; }
	};

	Console(lua_State* luaState) : mLuaState(luaState, &lua_close)
	{
		if (!luaState) throw LuaStatePointerIsNullException();
	}

	Console() : mLuaState(lua_open(), &lua_close)
	{
		luaL_openlibs(mLuaState);
	}

	

	bool append(std::string newLine)
	{
		const auto error = luaL_loadbuffer(mLuaState, newLine.c_str(), newLine.length(), "line")
		mBuffer.emplace_back(std::move(newLine));
		if (error)
		{
			mLastError = lua_tostring(mLuaState, -1);
			lua_pop(mLuaState, 1);  /* pop error message from the stack */
			return false;
		}
		else
		{
			error = lua_pcall(mLuaState, 0, 0, 0);
			if (error)
			{
				mLastError = lua_tostring(mLuaState, -1);
				lua_pop(mLuaState, 1); /* pop error message from the stack */
				return false;
			}
			else
			{
				return true;
			}
		}
	}

	typedef std::vector<std::string>::iterator iterator;
	typedef std::vector<std::string>::const_iterator const_iterator;

	inline iterator begin() noexcept { return mBuffer.begin(); }
	inline iterator end() noexcept { return mBuffer.end(); }
	inline const_iterator begin() const noexcept { return mBuffer.begin(); }
	inline const_iterator end() const noexcept { return mBuffer.end(); }
	inline const_iterator cbegin() const noexcept { return mBuffer.cbegin(); }
	inline const_iterator cend() const noexcept { return mBuffer.cend(); }
	inline std::string& operator [] (const std::size_t index) noexcept
	{
		return mBuffer[i];
	}
	inline const std::string& operator [] (const std::size_t index) const noexcept
	{
		return mBuffer[i];
	}
	inline std::size_t size() const noexcept { return mBuffer.size(); }

	lua_State* getLuaState() noexcept { return mLuaState.get(); }
	const lua_State* getLuaState() const noexcept { return mLuaState.get(); }

private:
	std::unique_ptr<lua_State, decltype(&lua_close)> mLuaState;
	std::vector<std::string> mBuffer;
	std::string mLastError;
};

} // namespace gintonic