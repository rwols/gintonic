#include "EntityVisitor.hpp"

#include "Entity.hpp"

namespace gintonic {

EntityVisitor::EntityVisitor(std::shared_ptr<Entity> root)
: root(root)
{
	/* Empty on purpose. */
}

void EntityVisitor::execute()
{
	mDepth = 0;
	mContinue = static_cast<bool>(root) && onStart();
	if (mContinue) visit(root);
	onFinish();
}

void EntityVisitor::visit(std::shared_ptr<Entity> entity)
{
	mContinue = onVisit(entity);
	++mDepth;
	for (auto lChild : *entity)
	{
		if (mContinue) visit(lChild);
		else break;
	}
	--mDepth;
}

} // namespace gintonic