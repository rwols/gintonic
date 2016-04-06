#include "ConsoleView.hpp"

#include "../Renderer.hpp"
#include "../ShaderPrograms.hpp"

#include <chrono>

namespace gintonic {
namespace GUI {

ConsoleView::ConsoleView()
: timer(std::chrono::milliseconds(250))
{
	timer.action.connect( [this] (const class timer*)
	{
		mDrawCursor = !mDrawCursor;
	});
}

void ConsoleView::drawImplementation() const noexcept
{
	if (!font || !console) return;

	const auto& lProgram = GUIShaderProgram::get();
	lProgram.setColor4(this->color);
	lProgram.setPosition(vec2f(0.0f, 0.0f));
	lProgram.setScale(vec2f(1.0f, 1.0f));
	lProgram.setHasGUITexture(1);
	lProgram.setGUITexture(0);
	lProgram.setWithoutColorAlpha(0);

	const vec2f lScale(2.0f / static_cast<GLfloat>(Renderer::width()), 2.0f / static_cast<GLfloat>(Renderer::height()));
	      auto  lStartPos = vec2f(box.minCorner.x, box.maxCorner.y - font->getPointSize() * lScale.y);
	const auto  lMaxHorizontalOffset = (box.maxCorner.x - box.minCorner.x) * font->getPointSize() * lScale.x;

	for (const auto& lLine : *console)
	{
		lStartPos = font->draw(lLine.c_str(), lLine.length(), lStartPos, lScale, lMaxHorizontalOffset);
		lStartPos.x = box.minCorner.x;
		lStartPos.y -= font->getPointSize() * lScale.y;
	}

	// Draw the active string
	/*lStartPos =*/ font->draw(console->getActiveString().c_str(), console->getActiveString().length(), lStartPos, lScale, lMaxHorizontalOffset);

	// Draw the cursor
	timer.update(Renderer::deltaTime());
	if (mDrawCursor)
	{
		// if (lMaxHorizontalOffset * scale.x <= (lPosition.x + 2.0f * mChar[i-32].bl * scale.x))
		// {
		// 	lPosition.x = lOriginalXPosition;
		// 	lPosition.y -= static_cast<GLfloat>(mPointSize * scale.y);
		// }
		float lCursor = static_cast<float>(console->cursorPosition) * font->getPointSize() * lScale.x * 0.5f;
		while (lMaxHorizontalOffset < lCursor)
		{
			lCursor -= lMaxHorizontalOffset;
			lStartPos.y -= font->getPointSize() * lScale.y;
		}
		lStartPos.x = box.minCorner.x + lCursor;

		font->draw("_", 1, lStartPos, lScale, lMaxHorizontalOffset);
	}

}

} // namespace GUI
} // namespace gintonic