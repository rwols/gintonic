#include "Application.hpp"
#include <boost/program_options.hpp>
#include <iomanip>

#define APPNAME "DirectionalLight"

class DirectionalLightApplication : public Application
{
public:

	std::shared_ptr<gintonic::Entity> mCubeEntity;

	DirectionalLightApplication(int argc, char** argv, cxxopts::Options& options)
	: Application(argc, argv, options)
	{
		using namespace gintonic;

		mCubeEntity = Entity::create("Cube");
		mCubeEntity->material = Material::create();
		mCubeEntity->material->name = "RuralBrickWall";
		mCubeEntity->material->diffuseColor = vec4f(1.0f, 1.0f, 1.0f, 0.9f);
		mCubeEntity->material->specularColor = vec4f(1.0f, 1.0f, 1.0f, 20.0f);
		Texture2D::ImageLoadOptions imageOptions;
		imageOptions.relativeFilename = "assets/images/RuralBrickWall.jpg";
		mCubeEntity->material->diffuseTexture = Texture2D::fromImage(imageOptions);
		imageOptions.relativeFilename = "assets/images/RuralBrickWall_SPEC.png";
		mCubeEntity->material->specularTexture = Texture2D::fromImage(imageOptions);
		imageOptions.relativeFilename = "assets/images/RuralBrickWall_NRM.png";
		mCubeEntity->material->normalTexture = Texture2D::fromImage(imageOptions);
		mCubeEntity->mesh = Renderer::getUnitCubeWithTangents();

		Renderer::getCameraEntity()->setTranslation(vec3f(2.0f, 1.0f, 4.0f));
		Renderer::getCameraEntity()->camera->addMouse(vec2f(0.2f, -0.20f));

		auto lLight = DirectionalLight::create();
		lLight->name = "DirectionalLight";
		lLight->setIntensity(vec4f(1.0f, 1.0f, 1.0f, 1.0f));

		auto lLightEntity = Entity::create
		(
			"DirectionalLight",
			SQT
			(
				vec3f(1.0f, 1.0f, 1.0f),
				quatf::axis_angle(vec3f(1.0f, 0.0f, 0.0f), -static_cast<float>(M_PI) / 2.0f),
				vec3f(0.0f, 0.0f, 0.0f)
			)
		);
		lLightEntity->light = lLight;

		mRootEntity->addChild(mCubeEntity);
		mRootEntity->addChild(lLightEntity);
		mRootEntity->addChild(Renderer::createGizmo());

		Renderer::disableCursor();
		Renderer::setFreeformCursor(true);
		Renderer::show();
	}

	virtual ~DirectionalLightApplication() noexcept = default;

private:

	virtual void onRenderUpdate() final
	{
		using namespace gintonic;
		mCubeEntity->postMultiplyRotation(quatf::axis_angle(vec3f(1.0f, 0.0f, 0.0f), static_cast<float>(mDeltaTime) / 10.0f));
		mCubeEntity->postMultiplyRotation(quatf::axis_angle(vec3f(0.0f, 1.0f, 0.0f), static_cast<float>(mDeltaTime) / 5.0f));
	}

};

DEFINE_MAIN(DirectionalLightApplication, "DirectionalLight", "Draws a cube with a directional light shining on it.")
