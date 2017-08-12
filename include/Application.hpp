#pragma once
#include "Foundation/Profiler.hpp"
#include "Graphics/Renderer.hpp"
#include "gintonic_export.h"

namespace gintonic {

class GINTONIC_EXPORT Application
{
  public:
    Application(int argc, char** argv);

    virtual ~Application() noexcept = default;

    void renderUpdate();

    inline bool shouldClose() const noexcept
    {
        return gintonic::Renderer::shouldClose();
    }

    inline void close() const noexcept { gintonic::Renderer::close(); }

  protected:
    const int                         mArgCount;
    const char**                      mArgVariable;
    std::shared_ptr<gintonic::Entity> mRootEntity =
        gintonic::Entity::create("Root");

    bool mCameraRespondsToKeyboard = true;
    bool mCameraRespondsToMouse = true;

    float mMoveSpeed = 1.0f;

    double mElapsedTime = 0.0;

    double mDeltaTime = 1.0 / 60.0;

    gintonic::vec3f
    getRandomNormalizedVector3(const int resolution = 1000) const noexcept;

    gintonic::vec4f
    getRandomNormalizedVector4(const int resolution = 1000) const noexcept;

    gintonic::vec4f getRandomColor(const float wCoordinate,
                                   const int resolution = 1000) const noexcept;

    gintonic::vec4f getRandomLightAttenuation(const float wCoordinate,
                                              const int resolution = 1000) const
        noexcept;

  private:
    virtual void onRenderUpdate() = 0;

    void processCameraInput();
};

} // namespace gintonic

#define DEFINE_MAIN(DerivedApplicationName, programName, shortDescription)     \
    int main(int argc, char* argv[])                                           \
    {                                                                          \
        try                                                                    \
        {                                                                      \
            DerivedApplicationName app(argc, argv);                            \
            while (app.shouldClose() == false)                                 \
            {                                                                  \
                app.renderUpdate();                                            \
            }                                                                  \
        }                                                                      \
        catch (const std::exception& ex)                                       \
        {                                                                      \
            std::cerr << ex.what() << '\n';                                    \
            GT_FINALIZE_PROFILING;                                             \
            return EXIT_FAILURE;                                               \
        }                                                                      \
        GT_FINALIZE_PROFILING;                                                 \
        return EXIT_SUCCESS;                                                   \
    }
