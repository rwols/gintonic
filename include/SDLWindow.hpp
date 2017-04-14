#pragma once

#include "Window.hpp"

struct SDL_Window;

namespace gintonic
{

class SDLWindow : public Window
{
  public:
    SDLWindow(const char* title);
    SDLWindow(const char* title, const int width, const int height);
    ~SDLWindow() override;

    void resize(const int newWidth, const int newHeight) override final;

    /// \brief Get the current width.
    int getWidth() const noexcept override final { return mWidth; }

    /// \brief Get the current height.
    int getHeight() const noexcept override final { return mHeight; }

    /// \brief Get the current aspect ratio.
    float getAspectRatio() const noexcept override final
    {
        return mAspectRatio;
    }

    /// \brief Get the current dimensions (the width and height).
    vec2f getDimensions() const noexcept override final
    {
        return vec2f(static_cast<float>(mWidth), static_cast<float>(mHeight));
    }

    int getID() const noexcept;

  private:
    int mWidth;
    int mHeight;
    float mAspectRatio;
    SDL_Window* mHandle;
    friend class SDLRenderContext;
};

} // gintonic
