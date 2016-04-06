#pragma once

#include "Base.hpp"

#include "../Font.hpp"

namespace gintonic {
namespace GUI {

class StringPointerView : public Base
{
public:
	
	std::shared_ptr<Font> font;
	const std::string* pointerToString = nullptr;

	StringPointerView() = default;

	virtual ~StringPointerView() noexcept = default;

private:
	virtual void drawImplementation() const noexcept;
};

} // namespace GUI
} // namespace gintonic