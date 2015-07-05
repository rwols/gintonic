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
		HFONT hFont;
		HDC hdc;
		std::cout << "Initializing font " << key() << '\n';
		const auto filename = key().string();
		if (AddFontResourceEx(filename.c_str(), FR_PRIVATE, 0) == 0)
		{
			BOOST_THROW_EXCEPTION(font_not_found() << errinfo_path(key()));
		}
		hFont = CreateFont(48,0,0,0,FW_DONTCARE,FALSE,TRUE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH,TEXT("Impact"));
		SelectObject(hdc, hFont);
		std::cout << "Success!\n";
		#endif
	}

	font::~font()
	{

	}

} // namespace gintonic