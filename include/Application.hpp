#pragma once
#include "gintonic.hpp"
#include "Foundation/Profiler.hpp"
#include "cxxopts.hpp"

namespace cxxopts
{
	class Options;
}

class Application
{
public:
	Application(
		int argc, 
		char** argv, 
		cxxopts::Options& options);

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

	std::shared_ptr<gintonic::Entity> mRootEntity = gintonic::Entity::create("Root");

	bool mCameraRespondsToKeyboard = true;
	bool mCameraRespondsToMouse = true;

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

#define DEFINE_MAIN(DerivedApplicationName, programName, shortDescription) \
int main(int argc, char* argv[])                                    \
{                                                                   \
	cxxopts::Options options(programName, shortDescription);        \
	options.add_options()("help", "produce this help message");     \
	try                                                             \
	{                                                               \
		DerivedApplicationName app(argc, argv, options);            \
		if (options.count("help"))                                  \
		{                                                           \
			std::cout << options.help() << '\n';                    \
			return EXIT_SUCCESS;                                    \
		}                                                           \
		while (app.shouldClose() == false)                          \
		{                                                           \
			app.renderUpdate();                                     \
		}                                                           \
	}                                                               \
	catch (const std::exception& ex)                                \
	{                                                               \
		std::cerr << ex.what() << '\n';                             \
		GT_FINALIZE_PROFILING;                                      \
		return EXIT_FAILURE;                                        \
	}                                                               \
	GT_FINALIZE_PROFILING;                                          \
	return EXIT_SUCCESS;                                            \
}
