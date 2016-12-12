#pragma once

#include "Base.hpp"

#include "../Texture2D.hpp"

namespace gintonic {
namespace GUI {

class Panel : public Base
{
public:

	std::shared_ptr<Texture2D> texture;

	Panel();
	Panel(const box2f& box);
	Panel(const box2f& box, const vec4f& color);

	virtual ~Panel() noexcept = default;

private:
	virtual void drawImplementation() const noexcept;
};

} // namespace GUI
} // namespace gintonic