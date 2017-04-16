#pragma once

namespace gintonic
{

class RenderContext;

class RenderStrategy
{
  public:
    RenderStrategy(RenderContext& context);
    virtual ~RenderStrategy() = default;
    virtual void drawFrame();
    RenderContext& context;
};

} // gintonic
