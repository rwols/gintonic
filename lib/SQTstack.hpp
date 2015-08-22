#ifndef gintonic_matrix_stack_hpp
#define gintonic_matrix_stack_hpp

#include "SQT.hpp"
#include "allocator.hpp"
#include <stack>
#include <deque>

namespace gintonic {

class SQTstack
{
public:

	SQTstack() = default;

	inline SQTstack(const SQT& sqt)
	{
		m_stack.push(sqt);
	}

	inline SQTstack(SQT&& sqt)
	{
		m_stack.push(std::move(sqt));
	}

	SQTstack(const SQTstack&) = default;
	SQTstack& operator = (const SQTstack&) = default;

	inline SQTstack(SQTstack&& other) 
	: m_stack(std::move(other.m_stack))
	{
		/* Empty on purpose. */
	}

	inline SQTstack& operator = (SQTstack&& other)
	{
		m_stack = std::move(other.m_stack);
		return *this;
	}

	SQTstack(const vec3f& scale, const quatf& rotation, const vec3f& translation)
	{
		m_stack.emplace(scale, rotation, translation);
	}

	inline SQT& top() 
	{
		return m_stack.top();
	}

	inline const SQT& top() const
	{
		return m_stack.top();
	}

	inline void push() 
	{
		m_stack.push(m_stack.top());
	}

	inline void push(const SQT& sqt)
	{
		m_stack.push(m_stack.top() % sqt);
	}

	inline void push(SQT&& sqt)
	{
		m_stack.push(m_stack.top() % std::move(sqt));
	}

	inline void push(const vec3f& scale, const quatf& rotation, const vec3f& translation)
	{
		m_stack.push(m_stack.top() % SQT(scale, rotation, translation));
	}

	inline void pop()
	{
		m_stack.pop();
	}

	inline std::size_t size() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_stack.size();
	}

	inline void swap(SQTstack& other)
	{
		m_stack.swap(other.m_stack);
	}

private:
	std::stack<SQT,std::deque<SQT,allocator<SQT>>> m_stack;
};

} // namespace gintonic

#endif