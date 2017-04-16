#include "RenderStrategy.hpp"
#include "Math/vec4f.hpp"
#include "RenderContext.hpp"

using namespace gintonic;

RenderStrategy::RenderStrategy(RenderContext& ctx) : context(ctx)
{
    context.setVirtualSynchronization(true);
    context.setClearColor(vec4f(0.0f, 0.0f, 0.0f, 1.0f));
}

void RenderStrategy::drawFrame() { context.clear(); }
