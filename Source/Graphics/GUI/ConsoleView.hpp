#pragma once

#include "Base.hpp"

#include "../../Foundation/Console.hpp"
#include "../../Foundation/timers.hpp"

#include "../Font.hpp"


namespace gintonic {
namespace GUI {

class ConsoleView : public Base
{
public:
	
	std::shared_ptr<Font> font;
	Console* console = nullptr;

	ConsoleView();

	virtual ~ConsoleView() noexcept = default;

private:
	mutable bool mDrawCursor = true;
	mutable loop_timer timer;
	virtual void drawImplementation() const noexcept;
};

} // namespace GUI
} // namespace gintonic