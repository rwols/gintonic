#pragma once

namespace gintonic
{

class Window;

class RenderContext
{
  public:
    virtual ~RenderContext() = default;

    /// \brief Draw to the viewport.
    virtual void present() = 0;

    virtual void resize() = 0;

    /// \brief Focus this context for the calling thread.
    virtual void focus() const noexcept = 0;

    /// \brief The name of this RenderContext.
    virtual const char* getName() const noexcept = 0;

    /// \brief The version of this RenderContext.
    virtual const char* getVersion() const noexcept = 0;

    /// \brief Get the window that owns this RenderContext.
    Window& getWindow() noexcept { return mOwner; }

    /// \brief Get the window that owns this RenderContext.
    const Window& getWindow() const noexcept { return mOwner; }

  protected:
    RenderContext(Window& owner) : mOwner(owner) {}

  private:
    Window& mOwner;
};

} // namespace gintonic
