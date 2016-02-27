/**
 * @file mat4fstack.hpp
 * @brief Defines a stack of four by four matrices.
 * @author Raoul Wols
 */

#ifndef gintonic_mat4fstack_hpp
#define gintonic_mat4fstack_hpp

#include "mat4f.hpp"
#include "allocator.hpp"
#include <stack>
#include <deque>

namespace gintonic {

/**
 * @brief A stack of 4x4 matrices, but with some benefits.
 */
class mat4fstack
{
public:

	/**
	 * @brief Default constructor pushes the identity matrix on the stack.
	 */
	inline mat4fstack()
	{
		m_stack.push(mat4f(1.0f));
	}

	/**
	 * @brief Constructor that pushes the given matrix onto the stack.
	 * @param m Some matrix to start out with.
	 */
	inline mat4fstack(const mat4f& m)
	{
		m_stack.push(m);
	}

	/**
	 * @brief Constructor that pushes the given matrix onto the stack.
	 * @param m Some matrix to start out with.
	 */
	inline mat4fstack(mat4f&& m)
	{
		m_stack.push(std::move(m));
	}

	/// Copy constructor.
	mat4fstack(const mat4fstack&) = default;

	/// Copy assignment operator.
	mat4fstack& operator = (const mat4fstack&) = default;

	/// Move constructor.
	inline mat4fstack(mat4fstack&& other) 
	: m_stack(std::move(other.m_stack))
	{
		/* Empty on purpose. */
	}

	/// Move assignment operator.
	inline mat4fstack& operator = (mat4fstack&& other)
	{
		m_stack = std::move(other.m_stack);
		return *this;
	}

	/**
	 * @brief Access the top element of the stack.
	 * @return A reference to the top element of the stack.
	 */
	inline mat4f& top() 
	{
		return m_stack.top();
	}

	/**
	 * @brief Access the top element of the stack.
	 * @return A const reference to the top element of the stack.
	 */
	inline const mat4f& top() const
	{
		return m_stack.top();
	}

	/**
	 * @brief Make a copy of the top element and push it onto the stack.
	 */
	inline void push() 
	{
		m_stack.push(m_stack.top());
	}

	/**
	 * @brief Multiply the matrix with the top element and push the result
	 * onto the stack.
	 * @param m The matrix to multiply the top matrix with.
	 */
	inline void push(const mat4f& m)
	{
		m_stack.push(m_stack.top() * m);
	}

	/**
	 * @brief Multiply the matrix with the top element and push the result
	 * onto the stack.
	 * @param m The matrix to multiply the top matrix with.
	 */
	inline void push(mat4f&& m)
	{
		m_stack.push(m_stack.top() * std::move(m));
	}

	/**
	 * @brief Multiply a matrix with the top element and push the result
	 * onto the stack.
	 * @tparam Args Variable template count.
	 * @param args Arguments for any constructor of mat4f. The arguments
	 * are perfectly forwarded.
	 */
	template <class ...Args> inline void emplace(Args&& ...args)
	{
		m_stack.emplace(m_stack.top() * mat4f(std::forward<Args>(args)...));
	}

	/**
	 * @brief Pop the top of the stack.
	 */
	inline void pop()
	{
		m_stack.pop();
	}

	/**
	 * @brief Get the size of the stack.
	 * @return The size of the stack.
	 */
	inline std::size_t size() const noexcept
	{
		return m_stack.size();
	}

	/**
	 * @brief Swap this stack with another stack.
	 * @param other The stack to swap with.
	 */
	inline void swap(mat4fstack& other)
	{
		m_stack.swap(other.m_stack);
	}

private:
	std::stack<mat4f,std::deque<mat4f,allocator<mat4f>>> m_stack;
};

} // namespace gintonic

#endif