#include "Graphics/GUI/StringPointerView.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/ShaderPrograms.hpp"

namespace gintonic {
namespace GUI {

void StringPointerView::drawImplementation() const noexcept
{
	if (!font || !pointerToString) return;
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

	font->draw(pointerToString->c_str(), pointerToString->length(), lTopLeft, lScale, lMaxHorizontalOffset);
}

} // namespace GUI
} // namespace gintonic