#include <gintonic/EntityVisitor.hpp>

#include <gintonic/Entity.hpp>

using namespace gintonic;

EntityVisitor::EntityVisitor(Entity& root) : root(root)
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

void EntityVisitor::visit(Entity& entity)
{
    mContinue = onVisit(entity);
    ++mDepth;
    for (auto& lChild : *entity)
    {
        if (mContinue)
            visit(lChild);
        else
            break;
    }
    --mDepth;
}
