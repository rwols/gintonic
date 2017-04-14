#pragma once

#include "Math/vec2f.hpp"
#include "Signal.hpp"
#include <memory>

namespace gintonic
{

class RenderContext;

class Window
{
  public:
    virtual ~Window();

    Signal<void()> onEnter;
    Signal<void()> onLeave;

    /// \brief Calls present of the current \c RenderContext.
    void present();

    /// \brief Resize the window.
    virtual void resize(const int newWidth, const int newHeight) = 0;

    /// \brief Get the current width.
    virtual int getWidth() const noexcept = 0;

    /// \brief Get the current height.
    virtual int getHeight() const noexcept = 0;

    /// \brief Get the current aspect ratio.
    virtual float getAspectRatio() const noexcept = 0;

    /// \brief Get the current dimensions (the width and height).
    virtual vec2f getDimensions() const noexcept = 0;

    template <class T> T* getRenderContextAs() noexcept
    {
        return static_cast<T*>(context.get());
    }

    template <class T> const T* getRenderContextAs() const noexcept
    {
        return static_cast<const T*>(context.get());
    }

    std::unique_ptr<RenderContext> context;

  protected:
    Window();
};

} // namespace gintonic
