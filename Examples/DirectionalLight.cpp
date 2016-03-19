#include "Application.hpp"
#include <iomanip>

#define APPNAME "DirectionalLight"

class DirectionalLightApplication : public Application
{
public:

	std::shared_ptr<gintonic::Entity> mCubeEntity;

	DirectionalLightApplication(int argc, char** argv)
	: Application(APPNAME, argc, argv)
	{
		using namespace gintonic;

		auto lMaterial = std::make_shared<Material>();
		lMaterial->name = "Bricks";
		lMaterial->diffuseColor = vec4f(0.8f, 0.8f, 0.8f, 0.9f);
		lMaterial->specularColor = vec4f(0.2f, 0.2f, 0.2f, 20.0f);
		lMaterial->diffuseTexture = std::make_shared<Texture2D>("../../Examples/bricks.jpg");
		lMaterial->specularTexture = std::make_shared<Texture2D>("../../Examples/bricks_SPEC.png");
		lMaterial->normalTexture = std::make_shared<Texture2D>("../../Examples/bricks_NRM.png");

		mCubeEntity = std::make_shared<Entity>("Cube");
		mCubeEntity->material = lMaterial;
		mCubeEntity->mesh = Renderer::getUnitCubeWithTangents();

		Renderer::getCameraEntity()->setTranslation(vec3f(0.0f, 0.0f, 3.0f));

		auto lLight = std::shared_ptr<Light>(new DirectionalLight());
		lLight->name = "DirectionalLight";
		lLight->setIntensity(vec4f(1.0f, 1.0f, 1.0f, 1.0f));

		auto lLightEntity = std::make_shared<Entity>
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

		Renderer::setFreeformCursor(true);
		Renderer::show();
	}

	virtual ~DirectionalLightApplication() noexcept = default;

private:

	virtual void onRenderUpdate() final
	{
		using namespace gintonic;
		mCubeEntity->postMultiplyRotation(quatf::axis_angle(vec3f(1.0f, 0.0f, 0.0f), mDeltaTime / 10.0f));
		mCubeEntity->postMultiplyRotation(quatf::axis_angle(vec3f(0.0f, 1.0f, 0.0f), mDeltaTime / 5.0f));
	}

};

DEFINE_MAIN(DirectionalLightApplication)
