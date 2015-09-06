/**
 * @file object.hpp
 * @author Raoul Wols
 * @deprecated We probably want to remove this altogether.
 * The only class that inherits from this class is font. But font could do
 * without this class too. So I don't really see a use. The reason this class
 * exists is because I wanted to unify the use of Boost.Flyweights.
 */

#ifndef gintonic_object_hpp
#define gintonic_object_hpp

#include "flyweights.hpp"
#include <boost/serialization/access.hpp>

namespace gintonic {

/*****************************************************************************
 * gintonic::object                                                          *
 ****************************************************************************/

/**
 * @brief An object class that encapsulates a flyweight with a key.
 * @tparam The derived class type.
 * @tparam The key type.
 */
template <class Derived, class KeyType> class object
{
public:

	/// The key type.
	typedef KeyType key_type;

	/**
	 * @brief Get the key.
	 * @return A const reference to the key.
	 */
	inline const key_type& key() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_key;
	}

	/**
	 * @brief Compare two objects.
	 * @details Two objects compare equal if their keys compare equal.
	 * @param other Another object.
	 * @return True if the two objects compare equal, false otherwise.
	 */
	inline bool operator == (const object& other) 
		const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return key() == other.key();
	}

	/**
	 * @brief Compare two objects.
	 * @details Two objects compare inequal if their keys compare inequal.
	 * @param other Another object.
	 * @return True if the two objects don't compare equal, false otherwise.
	 */
	inline bool operator != (const object& other) 
		const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !operator==(other);
	}

	/**
	 * @brief Static method to query the number of objects in use.
	 * @details For every different template instantiation, this number is
	 * different.
	 * @return The number of objects in use.
	 */
	static std::size_t use_count() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return s_num_objects;
	}

protected:

	/**
	 * @brief Destructor. Decreases the number of objects in use by one.
	 */
	inline virtual ~object() BOOST_NOEXCEPT_OR_NOTHROW
	{
		--s_num_objects;
	}

	/**
	 * @brief Default constructor. Increases the number of objects in use by
	 * one.
	 */
	object()
	{
		++s_num_objects;
	}

	/**
	 * @brief Constructor. Increases the number of objects in use by one.
	 * @param key The key to set.
	 */
	object(const key_type& key) 
	: m_key(key)
	{
		++s_num_objects;
	}

	/**
	 * @brief Constructor. Increases the number of objects in use by one.
	 * @param key The key to move.
	 */
	object(key_type&& key) BOOST_NOEXCEPT_OR_NOTHROW 
	: m_key(std::move(key))
	{
		++s_num_objects;
	}

	/**
	 * @brief Unifying structure to extract keys.
	 */
	struct key_extractor
	{
		/**
		 * @brief Functor operator.
		 * @param d A derived object.
		 * @return The key.
		 */
		inline const key_type& operator()(const Derived& d) 
			const BOOST_NOEXCEPT_OR_NOTHROW
		{
			return d.key();
		}
	};

	/**
	 * @brief Pure virtual method that should be called in a constructor
	 * that takes a key.
	 * @deprecated Don't use this.
	 */
	virtual void construct_from_key() = 0;

private:

	key_type m_key;

	static std::size_t s_num_objects;

public:
	
	/**
	 * @brief Flyweight type.
	 */
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