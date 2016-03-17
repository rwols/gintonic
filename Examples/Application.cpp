#include "Application.hpp"

Application::Application(const char* windowTitle, int argc, char** argv)
{
	gintonic::initializeEverything(windowTitle);
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

	auto lCameraEntity = Renderer::getCameraEntity();

	if (Renderer::key(SDL_SCANCODE_Q))
	{
		close();
	}
	if (Renderer::key(SDL_SCANCODE_W))
	{
		lCameraEntity->moveForward(mMoveSpeed * mDeltaTime);
	}
	if (Renderer::key(SDL_SCANCODE_A))
	{
		lCameraEntity->moveLeft(mMoveSpeed * mDeltaTime);
	}
	if (Renderer::key(SDL_SCANCODE_S))
	{
		lCameraEntity->moveBackward(mMoveSpeed * mDeltaTime);
	}
	if (Renderer::key(SDL_SCANCODE_D))
	{
		lCameraEntity->moveRight(mMoveSpeed * mDeltaTime);
	}
	if (Renderer::key(SDL_SCANCODE_SPACE))
	{
		lCameraEntity->moveUp(mMoveSpeed * mDeltaTime);
	}
	if (Renderer::key(SDL_SCANCODE_C))
	{
		lCameraEntity->moveDown(mMoveSpeed * mDeltaTime);
	}
	if (Renderer::keyTogglePress(SDL_SCANCODE_T))
	{
		Renderer::setWireframeMode(!Renderer::getWireframeMode());
	}

	const auto lMouseDelta = -deg2rad(Renderer::mouseDelta()) / 10.0f;
	lCameraEntity->camera->addMouse(lMouseDelta);
	lCameraEntity->setRotation(quatf::mouse(lCameraEntity->camera->angles()));
}