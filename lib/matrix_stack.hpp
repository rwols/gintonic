#ifndef gintonic_matrix_stack_hpp
#define gintonic_matrix_stack_hpp

#include "renderer.hpp"

namespace gintonic {

class matrix_stack
{
public:
	template <class ...Args> void set_model_matrix(Args&& ...args)
	{
		m_matrix_VM = renderer::camera().matrix_V() * mat4f(std::forward<Args>(args)...);
		m_matrix_PVM = renderer::camera().matrix_P(renderer::width(), renderer::height()) * m_matrix_VM;
		m_matrix_N = m_matrix_VM.upper_left_33().invert().transpose();
	}

	inline const mat4f& matrix_PVM() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_matrix_PVM;
	}

	inline const mat4f& matrix_VM() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_matrix_VM;
	}

	inline const mat3f& matrix_N() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_matrix_N;
	}

private:
	mat4f m_matrix_PVM;
	mat4f m_matrix_VM;
	mat3f m_matrix_N;
};

} // namespace gintonic

#endif