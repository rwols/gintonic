/**
 * @file Console.hpp
 * @brief Defines a Console.
 * @author Raoul Wols
 */

#pragma once

#include "Graphics/Font.hpp"
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

	Console();

	Console(lua_State* luaState);

	// std::string activeString;
	std::size_t cursorPosition = 0;

	bool insertCharAtCursorPosition(const char c);

	bool removeCharAtCursorPosition();

	bool removeCharBeforeCursorPosition();

	bool appendChar(const char c);

	bool removeCharAtEnd();

	bool appendChunk(const std::string& chunk);

	bool appendChunk(const char* start, const std::size_t length);

	bool appendChunk(const char* begin, const char* end);

	bool appendChunk(const char* chunk);

	bool compileActiveString();

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
		return mBuffer[index];
	}
	inline const std::string& operator [] (const std::size_t index) const noexcept
	{
		return mBuffer[index];
	}
	inline std::size_t size() const noexcept { return mBuffer.size(); }

	inline const std::string& getActiveString() const noexcept
	{
		return mActiveString;
	}

	lua_State* getLuaState() noexcept { return mLuaState.get(); }
	const lua_State* getLuaState() const noexcept { return mLuaState.get(); }

private:
	std::unique_ptr<lua_State, decltype(&lua_close)> mLuaState;
	std::vector<std::string> mBuffer;
	std::string mLastError;
	std::string mActiveString;

	static int printCallback(lua_State*);
	void subscribeToRendererEvents();
};

} // namespace gintonic