
#pragma once

namespace gintonic {

template <class ExpressionType> 
struct VectorExpression
{
	typedef ExpressionType expression_type;
	typedef typename expression_type::scalar_type scalar_type;
	
	constexpr VectorExpression() = default;

	inline constexpr static std::size_t size() noexcept { return expression_type::size(); }

	inline constexpr scalar_type& operator[](const std::size_t index)       noexcept { return static_cast<      expression_type&>(*this)[index]; }
	inline constexpr scalar_type  operator[](const std::size_t index) const noexcept { return static_cast<const expression_type&>(*this)[index]; }

	inline constexpr operator       expression_type&() noexcept { return static_cast<      expression_type&>(*this); }
	inline constexpr operator const expression_type&() noexcept { return static_cast<const expression_type&>(*this); }

};

template <class E1, class E2>
class VectorSum : public VectorExpression<VectorSum<E1, E2>>
{
private:

    E1 mLeftHandSide;
    E2 mRightHandSide;

public:

	typedef typename std::common_type<typename E1::scalar_type, typename E2::scalar_type>::type scalar_type;

    constexpr VectorSum(
    	VectorExpression<E1> lhs, 
    	VectorExpression<E2> rhs)
    : VectorExpression<VectorSum<E1, E2>>()
    , mLeftHandSide(std::move(lhs))
    , mRightHandSide(std::move(rhs))
    {
    	static_assert(E1::size() == E2::size(), "Dimensions must be the same.");
    }

    inline constexpr std::size_t size() noexcept { return E1::size(); }

    inline constexpr scalar_type operator[](const std::size_t index) const noexcept { return mLeftHandSide[index] + mRightHandSide[index]; }
};
  
template <class E1, class E2>
constexpr decltype(auto) operator + (const VectorExpression<E1>& lhs, const VectorExpression<E2>& rhs)
{
	return VectorSum<E1, E2>(lhs, rhs);
}

template <std::size_t N, typename ScalarType>
class Vector : public VectorExpression<Vector<N, ScalarType>>
{
public:

	typedef ScalarType scalar_type;

private:

	scalar_type mData[N];

public:

	constexpr Vector() = default;

	inline static constexpr std::size_t size() noexcept { return N; }

	constexpr Vector(std::array<scalar_type, N> arr) : mData(std::move(arr)) {}

	constexpr Vector(std::initializer_list<scalar_type> init) : mData()
	{
		auto j = init.begin();
		for (std::size_t i = 0; i < N; ++i)
		{
			mData[i] = *j;
			++j;
		}
	}

	template <class ExpressionType>
	constexpr Vector(const VectorExpression<ExpressionType> expression) : mData()
	{
		for (std::size_t i = 0; i < N; ++i)
		{
			mData[i] = expression[i];
		}
	}

	inline constexpr scalar_type& operator[](const std::size_t index)       noexcept { return mData[index]; }
	inline constexpr scalar_type  operator[](const std::size_t index) const noexcept { return mData[index]; }

	friend std::ostream& operator << (std::ostream& os, const Vector& vector)
	{
		os << "[ ";
		for (std::size_t i = 0; i < N; ++i) os << vector[i] << ' ';
		return os << ']';
	}


};

} // namespace gintonic
