#pragma once

namespace gintonic
{

class Window;
union vec4f;

class RenderContext
{
  public:
    virtual ~RenderContext() = default;

    /// \brief Clear the screen (colors, depth and stencil).
    virtual void clear() = 0;

    /// \brief Set the clear color when calling \c RenderContext::clear.
    virtual void setClearColor(const vec4f& color) = 0;

    /// \brief Wether to synchronize the framerate to the monitor.
    virtual void setVirtualSynchronization(const bool yesOrNo) = 0;

    /// \brief Swap the front and back buffers.
    virtual void present() = 0;

    /// \brief Resize the context.
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
