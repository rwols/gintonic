#include "Application.hpp"
#include "Camera.hpp"

using namespace gintonic;

Application::Application(int argc, char** argv)
    : mArgCount(argc), mArgVariable((const char**)argv)
{
    bool fullScreen = false;
    const char* windowTitle = "gintonic";
    auto cameraEntity = gintonic::Entity::create("DefaultCamera");

    boost::filesystem::current_path(gintonic::get_executable_path());

    gintonic::Renderer::initialize(windowTitle, std::move(cameraEntity),
                                   fullScreen, 800, 640);
}

void Application::renderUpdate()
{
    gintonic::Renderer::getElapsedAndDeltaTime(mElapsedTime, mDeltaTime);
    processCameraInput();
    onRenderUpdate();
    gintonic::Renderer::submitEntityRecursive(mRootEntity);
    gintonic::Renderer::update();
}

void Application::processCameraInput()
{
    using namespace gintonic;

    auto cameraEntity = Renderer::getCameraEntity();
    const float lDeltaTime = static_cast<float>(mDeltaTime);

    if (Renderer::key(SDL_SCANCODE_Q))
    {
        close();
    }
    if (Renderer::keyTogglePress(SDL_SCANCODE_T))
    {
        Renderer::setWireframeMode(!Renderer::getWireframeMode());
    }
    if (Renderer::keyTogglePress(SDL_SCANCODE_G))
    {
        Renderer::setViewGeometryBuffers(!Renderer::getViewGeometryBuffers());
    }
    if (Renderer::keyTogglePress(SDL_SCANCODE_B))
    {
        Renderer::setViewCameraDepthBuffer(
            !Renderer::getViewCameraDepthBuffer());
    }
    if (mCameraRespondsToKeyboard)
    {
        if (Renderer::key(SDL_SCANCODE_W))
        {
            cameraEntity->moveForward(mMoveSpeed * lDeltaTime);
        }
        if (Renderer::key(SDL_SCANCODE_A))
        {
            cameraEntity->moveLeft(mMoveSpeed * lDeltaTime);
        }
        if (Renderer::key(SDL_SCANCODE_S))
        {
            cameraEntity->moveBackward(mMoveSpeed * lDeltaTime);
        }
        if (Renderer::key(SDL_SCANCODE_D))
        {
            cameraEntity->moveRight(mMoveSpeed * lDeltaTime);
        }
        if (Renderer::key(SDL_SCANCODE_SPACE))
        {
            cameraEntity->moveUp(mMoveSpeed * lDeltaTime);
        }
        if (Renderer::key(SDL_SCANCODE_C))
        {
            cameraEntity->moveDown(mMoveSpeed * lDeltaTime);
        }
    }
    if (mCameraRespondsToMouse)
    {
        vec2f lMouseDelta(0.0f, 0.0f);

        lMouseDelta += -deg2rad(Renderer::mouseDelta()) / 10.0f;

        cameraEntity->camera->addMouse(lMouseDelta);
        cameraEntity->setRotation(quatf::mouse(cameraEntity->camera->angles()));
    }
}

gintonic::vec3f
Application::getRandomNormalizedVector3(const int resolution) const noexcept
{
    gintonic::vec3f lResult(static_cast<float>(std::rand() % resolution),
                            static_cast<float>(std::rand() % resolution),
                            static_cast<float>(std::rand() % resolution));

    lResult.normalize();

    return lResult;
}

gintonic::vec4f
Application::getRandomNormalizedVector4(const int resolution) const noexcept
{
    gintonic::vec4f lResult(static_cast<float>(std::rand() % resolution),
                            static_cast<float>(std::rand() % resolution),
                            static_cast<float>(std::rand() % resolution),
                            static_cast<float>(std::rand() % resolution));

    lResult.normalize();
    return lResult;
}

gintonic::vec4f Application::getRandomColor(const float wCoordinate,
                                            const int resolution) const noexcept
{
    auto lRandomVec3 = getRandomNormalizedVector3(resolution);
    auto lCeiling =
        std::max(std::max(lRandomVec3.x, lRandomVec3.y), lRandomVec3.z);
    lCeiling = 1.0f - lCeiling;
    lRandomVec3.x += lCeiling;
    lRandomVec3.y += lCeiling;
    lRandomVec3.z += lCeiling;
    return {lRandomVec3.x, lRandomVec3.y, lRandomVec3.z, wCoordinate};
}

gintonic::vec4f
Application::getRandomLightAttenuation(const float wCoordinate,
                                       const int resolution) const noexcept
{
    gintonic::vec4f lAttenuation(getRandomNormalizedVector3(resolution), 0.0f);
    if (lAttenuation.x > 0.9f)
    {
        lAttenuation.x = 0.0f;
        lAttenuation.normalize();
    }
    lAttenuation.w = wCoordinate;
    return lAttenuation;
}
