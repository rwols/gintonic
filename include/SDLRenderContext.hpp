#pragma once

#include "RenderContext.hpp"

namespace gintonic
{

class SDLWindow;

class SDLRenderContext : public RenderContext
{
  public:
    SDLRenderContext(SDLWindow& owner, const int major, const int minor);
    ~SDLRenderContext() override;

    void present() override final;

    void resize() override final;

    void focus() const noexcept override final;

    const char* getName() const noexcept override final;

    const char* getVersion() const noexcept override final;

  private:
    void* mHandle;
};

} // gintonic
