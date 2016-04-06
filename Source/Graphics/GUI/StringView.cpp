#include "StringView.hpp"
#include "../Renderer.hpp"
#include "../ShaderPrograms.hpp"

namespace gintonic {
namespace GUI {

void StringView::drawImplementation() const noexcept
{
	if (!font) return;
	const auto& lProgram = GUIShaderProgram::get();
	lProgram.setColor4(this->color);
	lProgram.setPosition(vec2f(0.0f, 0.0f));
	lProgram.setScale(vec2f(1.0f, 1.0f));
	lProgram.setHasGUITexture(1);
	lProgram.setGUITexture(0);
	lProgram.setWithoutColorAlpha(0);

	const vec2f lScale(2.0f / static_cast<GLfloat>(Renderer::width()), 2.0f / static_cast<GLfloat>(Renderer::height()));
	const auto lTopLeft = vec2f(box.minCorner.x, box.maxCorner.y - font->getPointSize() * lScale.y);
	const auto lMaxHorizontalOffset = (box.maxCorner.x - box.minCorner.x);

	font->draw(text.c_str(), text.length(), lTopLeft, lScale, lMaxHorizontalOffset);
}

} // namespace GUI
} // namespace gintonic