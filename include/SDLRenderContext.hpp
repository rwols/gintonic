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

    void clear() override final;

    void setClearColor(const vec4f& color) override final;

    void setVirtualSynchronization(const bool yesOrNo) override final;

    void present() override final;

    void resize() override final;

    void focus() const noexcept override final;

    const char* getName() const noexcept override final;

    const char* getVersion() const noexcept override final;

  private:
    void* mHandle;
};

} // gintonic
