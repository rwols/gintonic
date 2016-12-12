#include "Graphics/GUI/ConsoleView.hpp"

#include "Graphics/Renderer.hpp"
#include "Graphics/ShaderPrograms.hpp"

#include <chrono>

namespace gintonic {
namespace GUI {

ConsoleView::ConsoleView()
: mTimer(Timer<float>::Type::kLoop, 0.25f)
{
	mTimer.onFire.connect( [this] ()
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
	
	const auto  lMaxHorizontalOffset = (box.maxCorner.x - box.minCorner.x);

	for (const auto& lLine : *console)
	{
		lStartPos = font->draw(lLine.c_str(), lLine.length(), lStartPos, lScale, lMaxHorizontalOffset);
		lStartPos.x = box.minCorner.x;
		lStartPos.y -= font->getPointSize() * lScale.y;
	}

	const auto lOriginalStartPos = lStartPos;

	// Draw the "input thingy"
	lStartPos = font->draw("> ", 2, lStartPos, lScale, lMaxHorizontalOffset);

	// Draw the active string
	font->draw(console->getActiveString().c_str(), console->getActiveString().length(), lStartPos, lScale, lMaxHorizontalOffset);

	// Draw the cursor
	// mTimer.update(Renderer::deltaTime());
	if (mDrawCursor)
	{
		// The +2 is because we also draw the "input thingy"
		std::string lFakeString(console->cursorPosition + 2, ' ');
		lFakeString.append(1, '_');
		font->draw(lFakeString.c_str(), lFakeString.length(), lOriginalStartPos, lScale, lMaxHorizontalOffset);
	}

}

} // namespace GUI
} // namespace gintonic