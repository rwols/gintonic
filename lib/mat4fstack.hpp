#ifndef gintonic_mat4fstack_hpp
#define gintonic_mat4fstack_hpp

#include "mat4f.hpp"
#include "allocator.hpp"
#include <stack>
#include <deque>

namespace gintonic {

class mat4fstack
{
public:

	inline mat4fstack()
	{
		m_stack.push(mat4f(1.0f));
	}

	inline mat4fstack(const mat4f& m)
	{
		m_stack.push(m);
	}

	inline mat4fstack(mat4f&& m)
	{
		m_stack.push(std::move(m));
	}

	mat4fstack(const mat4fstack&) = default;
	mat4fstack& operator = (const mat4fstack&) = default;

	inline mat4fstack(mat4fstack&& other) 
	: m_stack(std::move(other.m_stack))
	{
		/* Empty on purpose. */
	}

	inline mat4fstack& operator = (mat4fstack&& other)
	{
		m_stack = std::move(other.m_stack);
		return *this;
	}

	// mat4fstack(const vec3f& scale, const quatf& rotation, const vec3f& translation)
	// {
	// 	m_stack.emplace(scale, rotation, translation);
	// }

	inline mat4f& top() 
	{
		return m_stack.top();
	}

	inline const mat4f& top() const
	{
		return m_stack.top();
	}

	inline void push() 
	{
		m_stack.push(m_stack.top());
	}

	inline void push(const mat4f& m)
	{
		m_stack.push(m_stack.top() * m);
	}

	inline void push(mat4f&& m)
	{
		m_stack.push(m_stack.top() * std::move(m));
	}

	template <class ...Args> inline void emplace(Args&& ...args)
	{
		m_stack.emplace(m_stack.top() * mat4f(std::forward<Args>(args)...));
	}

	inline void pop()
	{
		m_stack.pop();
	}

	inline std::size_t size() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_stack.size();
	}

	inline void swap(mat4fstack& other)
	{
		m_stack.swap(other.m_stack);
	}

private:
	std::stack<mat4f,std::deque<mat4f,allocator<mat4f>>> m_stack;
};

} // namespace gintonic

#endif