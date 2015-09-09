/**
 * @file SQTstack.hpp
 * @brief Defines a stack of SQTs.
 * @author Raoul Wols
 */

#ifndef gintonic_SQTstack_hpp
#define gintonic_SQTstack_hpp

#include "SQT.hpp"
#include "allocator.hpp"
#include <stack>
#include <deque>

namespace gintonic {

/**
 * @brief An SQT stack is a stack of SQTs, but with some benefits.
 */
class SQTstack
{
public:

	/**
	 * @brief Default constructor.
	 * @details The default constructor pushes a default-constructed
	 * SQT onto the stack.
	 */
	SQTstack()
	{
		m_stack.push(SQT());
	}

	/**
	 * @brief Initializes the stack with an SQT.
	 * 
	 * @param sqt The SQT to initialize the stack with.
	 */
	inline SQTstack(const SQT& sqt)
	{
		m_stack.push(sqt);
	}

	/**
	 * @brief Initializes the stack with an SQT.
	 * 
	 * @param sqt The SQT to initialize the stack with.
	 */
	inline SQTstack(SQT&& sqt)
	{
		m_stack.push(std::move(sqt));
	}

	/**
	 * @brief Default copy constructor.
	 */
	SQTstack(const SQTstack&) = default;

	/**
	 * @brief Default copy assignment operator.
	 */
	SQTstack& operator = (const SQTstack&) = default;

	/**
	 * @brief Default move constructor.
	 */
	inline SQTstack(SQTstack&& other) 
	: m_stack(std::move(other.m_stack))
	{
		/* Empty on purpose. */
	}

	/**
	 * @brief Default move assignment operator.
	 */
	inline SQTstack& operator = (SQTstack&& other)
	{
		m_stack = std::move(other.m_stack);
		return *this;
	}

	/**
	 * @brief Initialize an SQTstack with a scaling, a rotation and a translation.
	 * 
	 * @param u0 The scaling. This gets forwarded to the constructor of vec3f.
	 * @param u1 The rotation. This gets forwarded to the constructor of quatf.
	 * @param u2 The translation. This gets forwarded to the constructor of vec3f.
	 */
	template <class U0, class U1, class U2>
	SQTstack(U0&& u0, U1&& u1, U2&& u2)
	{
		m_stack.emplace(std::forward<U0>(u0), std::forward<U1>(u1), std::forward<U2>(u2));
	}

	/**
	 * @brief Access the top element of the stack.
	 * @return A reference to the top element of the stack.
	 */
	inline SQT& top() 
	{
		return m_stack.top();
	}

	/**
	 * @brief Access the top element of the stack.
	 * @return A constant reference to the top element of the stack.
	 */
	inline const SQT& top() const
	{
		return m_stack.top();
	}

	/**
	 * @brief Push a default-constructed SQT onto the stack.
	 */
	inline void push() 
	{
		m_stack.push(m_stack.top());
	}

	/**
	 * @brief Add the SQT with the top of the stack and push the result onto the stack.
	 * 
	 * @param sqt The SQT.
	 */
	inline void push(const SQT& sqt)
	{
		m_stack.push(m_stack.top() % sqt);
	}

	/**
	 * @brief Add the SQT with the top of the stack and push the result onto the stack.
	 * 
	 * @param sqt The SQT.
	 */
	inline void push(SQT&& sqt)
	{
		m_stack.push(m_stack.top() % std::move(sqt));
	}

	/**
	 * @brief Add the SQT constructed with the top of the stack and push the result onto the stack.
	 * 
	 * @param scale The scaling.
	 * @param rotation The rotation quaternion.
	 * @param translation The translation from the origin.
	 */
	inline void push(const vec3f& scale, const quatf& rotation, const vec3f& translation)
	{
		m_stack.push(m_stack.top() % SQT(scale, rotation, translation));
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
	inline std::size_t size() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_stack.size();
	}

	/**
	 * @brief Swap this stack with another stack.
	 * @param other Another SQTstack.
	 */
	inline void swap(SQTstack& other)
	{
		m_stack.swap(other.m_stack);
	}

private:
	std::stack<SQT,std::deque<SQT,allocator<SQT>>> m_stack;
};

} // namespace gintonic

#endif