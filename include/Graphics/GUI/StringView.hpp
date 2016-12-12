#pragma once

#include "Base.hpp"

#include "../Font.hpp"

namespace gintonic {
namespace GUI {

class StringView : public Base
{
public:
	
	std::shared_ptr<Font> font;
	std::string text;

	StringView() = default;

	virtual ~StringView() noexcept = default;

private:
	virtual void drawImplementation() const noexcept;
};

} // namespace GUI
} // namespace gintonic