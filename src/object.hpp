#ifndef flyweight_keyvalue_hpp
#define flyweight_keyvalue_hpp

#include "filesystem.hpp"
#include "flyweights.hpp"
#include <boost/serialization/access.hpp>

namespace gintonic 
{
	template <class Derived, class KeyType> class object
	{
	public:
		typedef KeyType key_type;
	private:
		key_type m_key;
	protected:
		explicit object(const key_type& key) : m_key(key)
		{
			#ifdef WITH_MEMORY_PROFILING
			++s_num_objects;
			#endif
		}
		explicit object(key_type&& key) BOOST_NOEXCEPT_OR_NOTHROW : m_key(std::move(key)) {}
		struct key_extractor
		{
			inline const key_type& operator()(const Derived& d) const BOOST_NOEXCEPT_OR_NOTHROW
			{
				return d.key();
			}
		};
	public:
		inline virtual ~object() BOOST_NOEXCEPT_OR_NOTHROW
		{
			#ifdef WITH_MEMORY_PROFILING
			--s_num_objects;
			#endif
		}
		object() = delete;
		object(const object&) = delete;
		object& operator=(const object&) = delete;
		inline object(object&& other) : m_key(std::move(other.m_key)) {}
		inline object& operator=(object&& other) BOOST_NOEXCEPT_OR_NOTHROW
		{
			m_key = std::move(other.m_key);
			return *this;
		}
		inline const key_type& key() const BOOST_NOEXCEPT_OR_NOTHROW { return m_key; }
		inline bool operator == (const object& other) const BOOST_NOEXCEPT_OR_NOTHROW
		{
			return key() == other.key();
		}
		inline bool operator != (const object& other) const BOOST_NOEXCEPT_OR_NOTHROW
		{
			return !operator==(other);
		}
		typedef boost::flyweights::flyweight<boost::flyweights::key_value<key_type, Derived, key_extractor>> flyweight;
		#ifdef WITH_MEMORY_PROFILING
		static std::size_t number_in_use() BOOST_NOEXCEPT_OR_NOTHROW { return s_num_objects; }
		#endif
	private:
		friend class boost::serialization::access;
		template <class Archive> void serialize(Archive& ar, object& o, const unsigned int version)
		{
			ar & o.m_key;
		}

		#ifdef WITH_MEMORY_PROFILING
		static std::size_t s_num_objects;
		#endif
	};

	template <class KeyType> class polymorphic_object
	{
	public:
		typedef KeyType key_type;
		inline virtual ~polymorphic_object() BOOST_NOEXCEPT_OR_NOTHROW {}
		inline polymorphic_object(const polymorphic_object&) = delete;
		inline polymorphic_object& operator = (const polymorphic_object&) = delete;
		inline polymorphic_object(polymorphic_object&& other) : m_key(std::move(other.m_key)) {}
		inline polymorphic_object& operator = (polymorphic_object&& other)
		{
			m_key = std::move(other.m_key);
			return *this;
		}
		inline const key_type& key() const BOOST_NOEXCEPT_OR_NOTHROW { return m_key; }
	protected:
		polymorphic_object(const KeyType& key) : m_key(key) {}
		polymorphic_object(key_type&& key) : m_key(std::move(key)) {}
	private:
		key_type m_key;
	};

	#ifdef WITH_MEMORY_PROFILING
	template <class Derived, class KeyType> std::size_t object<Derived, KeyType>::s_num_objects = 0;
	#endif

} // end of namespace gintonic

#endif