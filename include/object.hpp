#ifndef gintonic_object_hpp
#define gintonic_object_hpp

#include "flyweights.hpp"
#include <boost/serialization/access.hpp>

namespace gintonic 
{

/*****************************************************************************
 * gintonic::object                                                          *
 ****************************************************************************/

template <class Derived, class KeyType> class object
{
public:
	typedef KeyType key_type;

	inline const key_type& key() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_key;
	}

	inline bool operator == (const object& other) 
		const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return key() == other.key();
	}
	inline bool operator != (const object& other) 
		const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !operator==(other);
	}

	static std::size_t use_count() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return s_num_objects;
	}

protected:

	inline virtual ~object() BOOST_NOEXCEPT_OR_NOTHROW
	{
		--s_num_objects;
	}

	object()
	{
		++s_num_objects;
	}

	object(const object& other) : m_key(other.m_key)
	{
		++s_num_objects;
	}

	object& operator=(const object& other)
	{
		m_key = other.m_key;
		++s_num_objects;
		return *this;
	}

	#ifdef BOOST_MSVC
	
	inline object(object&& other) BOOST_NOEXCEPT_OR_NOTHROW
	: m_key(std::move(other.m_key))
	{
		/* Empty on purpose. */
	}
	inline object& operator=(object&& other) BOOST_NOEXCEPT_OR_NOTHROW
	{
		m_key = std::move(other.m_key);
		return *this;
	}

	inline object(object&&) BOOST_NOEXCEPT_OR_NOTHROW = default;
	inline object& operator=(object&&) BOOST_NOEXCEPT_OR_NOTHROW = default;
	
	#endif

	object(const key_type& key) 
	: m_key(key)
	{
		++s_num_objects;
	}

	object(key_type&& key) BOOST_NOEXCEPT_OR_NOTHROW 
	: m_key(std::move(key))
	{
		/* Empty on purpose. */
	}

	struct key_extractor
	{
		inline const key_type& operator()(const Derived& d) 
			const BOOST_NOEXCEPT_OR_NOTHROW
		{
			return d.key();
		}
	};

	virtual void construct_from_key() = 0;

private:

	key_type m_key;

	static std::size_t s_num_objects;

public:
	
	typedef boost::flyweights::flyweight
	<
		boost::flyweights::key_value
		<
			key_type, 
			Derived, 
			key_extractor
		>
	> flyweight;
};

template <class Derived, class KeyType> 
std::size_t object<Derived, KeyType>::s_num_objects = 0;

} // namespace gintonic

#endif