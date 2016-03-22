#pragma once
#include "gintonic.hpp"

class Application
{
public:
	Application(const char* windowTitle, int argc, char** argv);

	virtual ~Application() noexcept = default;

	void renderUpdate();

	inline bool shouldClose() const noexcept
	{
		return gintonic::Renderer::shouldClose();
	}

	inline void close() const noexcept
	{
		gintonic::Renderer::close();
	}

protected:

	std::shared_ptr<gintonic::Entity> mRootEntity = std::make_shared<gintonic::Entity>("Root");

	float mMoveSpeed = 1.0f;

	double mElapsedTime = 0.0;

	double mDeltaTime = 1.0 / 60.0;

	gintonic::vec3f getRandomNormalizedVector3(const int resolution = 1000) const noexcept;

	gintonic::vec4f getRandomNormalizedVector4(const int resolution = 1000) const noexcept;

	gintonic::vec4f getRandomColor(const float wCoordinate, const int resolution = 1000) const noexcept;

	gintonic::vec4f getRandomLightAttenuation(const float wCoordinate, const int resolution = 1000) const noexcept;

private:

	virtual void onRenderUpdate() = 0;

	void processCameraInput();

};

#define DEFINE_MAIN(DerivedApplicationName)\
int main(int argc, char* argv[])\
{\
	DEBUG_PRINT;\
	try\
	{\
		DEBUG_PRINT;\
		DerivedApplicationName lDerivedApplicationName(argc, argv);\
		while (lDerivedApplicationName.shouldClose() == false)\
		{\
			lDerivedApplicationName.renderUpdate();\
		}\
	}\
	catch (const gintonic::exception& lException)\
	{\
		DEBUG_PRINT;\
		std::cerr << lException.what() << '\n';\
		return EXIT_FAILURE;\
	}\
	DEBUG_PRINT;\
	return EXIT_SUCCESS;\
}