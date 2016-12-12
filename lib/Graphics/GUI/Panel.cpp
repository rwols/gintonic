#include "Graphics/GUI/Panel.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/ShaderPrograms.hpp"
#include "Graphics/Mesh.hpp"

namespace gintonic {
namespace GUI {

Panel::Panel()
: Base()
{
	/* Empty on purpose. */
}

Panel::Panel(const box2f& box) 
: Base(box)
{
	/* Empty on purpose. */
}

Panel::Panel(const box2f& box, const vec4f& color) 
: Base(box, color)
{
	/* Empty on purpose. */
}

void Panel::drawImplementation() const noexcept
{
	const auto& lProgram = GUIShaderProgram::get();
	const auto lCenterOfBox = (box.minCorner + box.maxCorner) * 0.5f;
	const auto lScale = (box.maxCorner - box.minCorner) * 0.5f;
	lProgram.setPosition(lCenterOfBox);
	lProgram.setScale(lScale);
	lProgram.setColor4(this->color);
	lProgram.setWithoutColorAlpha(0);
	if (texture)
	{
		lProgram.setHasGUITexture(1);
		lProgram.setGUITexture(0);
		texture->bind(0);
	}
	else
	{
		lProgram.setHasGUITexture(0);
	}
	Renderer::drawPackedUnitQuad();
}

} // namespace GUI
} // namespace gintonic