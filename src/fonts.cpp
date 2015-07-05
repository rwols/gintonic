#include "fonts.hpp"

namespace gintonic {

	font::font(const key_type& filename) : object<font, key_type>(filename)
	{
		init_class();
	}

	font::font(key_type&& filename) : object<font, key_type>(std::move(filename))
	{
		init_class();
	}

	void font::init_class()
	{
		#ifdef BOOST_MSVC
		std::cout << "User tried to initialize font " << key() << '\n';
		#endif
	}

	font::~font()
	{

	}

} // namespace gintonic