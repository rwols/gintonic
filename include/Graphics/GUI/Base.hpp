#pragma once

#include "../../Math/box2f.hpp"
#include "../../Math/vec4f.hpp"

#include <boost/signals2.hpp>

#include <memory>
#include <list>

namespace gintonic {
namespace GUI {

class Base
{
public:

	using ChildrenDataStructure = std::list<std::unique_ptr<Base>>;
	using iterator = ChildrenDataStructure::iterator;
	using const_iterator = ChildrenDataStructure::const_iterator;

	box2f box;
	vec4f color;

	boost::signals2::signal<void()> onMouseEnter;
	boost::signals2::signal<void()> onMouseLeave;
	boost::signals2::signal<void()> onMouseLeftClick;
	boost::signals2::signal<void()> onMouseRightClick;

	Base();
	Base(const box2f& box);
	Base(const box2f& box, const vec4f& color);
	
	virtual ~Base() noexcept = default;

	void draw() const noexcept;

	void addChild(Base* child);

	inline iterator begin() noexcept { return mChildren.begin(); }
	inline iterator end() noexcept { return mChildren.end(); }
	inline const_iterator begin() const noexcept { return mChildren.begin(); }
	inline const_iterator end() const noexcept { return mChildren.end(); }
	inline const_iterator cbegin() const noexcept { return mChildren.cbegin(); }
	inline const_iterator cend() const noexcept { return mChildren.cend(); }

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

private:

	virtual void drawImplementation() const noexcept = 0;

	void mouseMoveHandler(const vec2f&) noexcept;

	std::list<std::unique_ptr<Base>> mChildren;
	bool mMouseIsInBox = false;

};

} // namespace GUI
} // namespace gintonic