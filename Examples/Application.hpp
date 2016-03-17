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

private:

	virtual void onRenderUpdate() = 0;

	void processCameraInput();

};

#define DEFINE_MAIN(DerivedApplicationName)\
int main(int argc, char* argv[])\
{\
	try\
	{\
		DerivedApplicationName lDerivedApplicationName(argc, argv);\
		while (lDerivedApplicationName.shouldClose() == false)\
		{\
			lDerivedApplicationName.renderUpdate();\
		}\
	}\
	catch (const gintonic::exception& lException)\
	{\
		std::cerr << lException.what() << '\n';\
		return EXIT_FAILURE;\
	}\
	return EXIT_SUCCESS;\
}