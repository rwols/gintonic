#include "Base.hpp"

#include "../Renderer.hpp"

namespace gintonic {
namespace GUI {

Base::Base()
: Base(box2f(vec2f(0.0f, 0.0f), vec2f(0.0f, 0.0f)), vec4f(1.0f, 1.0f, 1.0f, 0.0f))
{
	/* Empty on purpose. */
}

Base::Base(const box2f& box)
: Base(box, vec4f(1.0f, 1.0f, 1.0f, 0.0f))
{
	/* Empty on purpose. */
}

Base::Base(const box2f& box, const vec4f& color)
: box(box)
, color(color)
{
	Renderer::onMouseMove.connect(std::bind(&Base::mouseMoveHandler, this, std::placeholders::_1));
}

void Base::draw() const noexcept
{
	drawImplementation();
	for (const auto& lChild : mChildren)
	{
		lChild->draw();
	}
}

void Base::addChild(Base* child)
{
	mChildren.emplace_back(child);
}

void Base::mouseMoveHandler(const vec2f& mousePosition) noexcept
{
	if (mMouseIsInBox)
	{
		if (false == box.contains(mousePosition))
		{
			mMouseIsInBox = false;
			onMouseLeave();
		}
	}
	else // mouse was not in the box (WHATS IN THE BOOOOOXXX?)
	{
		if (true == box.contains(mousePosition))
		{
			mMouseIsInBox = true;
			onMouseEnter();
		}
	}
}

} // namespace GUI
} // namespace gintonic 