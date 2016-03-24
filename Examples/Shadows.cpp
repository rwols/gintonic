#include "Application.hpp"

#define APPNAME "Shadows"

#define F_PI static_cast<float>(M_PI)

class ShadowsApplication : public Application
{
public:
	
	ShadowsApplication(int argc, char** argv)
	: Application(APPNAME, argc, argv)
	{
		using namespace gintonic;
		Renderer::setFreeformCursor(true);
		Renderer::show();

		auto lDaVinci = std::make_shared<Texture2D>("Resources/DaVinci.jpg");

		auto lMaterial = std::make_shared<Material>();
		lMaterial->name = "DaVinci";
		lMaterial->diffuseColor = vec4f(1.0f, 1.0f, 1.0f, 1.0f);
		lMaterial->specularColor = vec4f(0.0f, 0.0f, 0.0f, 0.0f);
		lMaterial->diffuseTexture = lDaVinci;

		auto lWall = std::make_shared<Entity>();
		lWall->name = "Wall";
		lWall->material = lMaterial;
		lWall->mesh = Renderer::getUnitQuad();
		lWall->castShadow = true;

		auto lFloor = lWall->cloneRecursive();
		lFloor->name = "Floor";
		lFloor->setRotation(quatf::axis_angle(vec3f(1.0f, 0.0f, 0.0f), - F_PI * 0.5f));
		lFloor->setTranslation(vec3f(0.0f, -1.0f, 1.0f));
		lFloor->castShadow = true;

		auto lSphere = std::make_shared<Entity>();
		lSphere->name = "Sphere";
		lSphere->material = lMaterial;
		lSphere->mesh = Renderer::getUnitSphere();
		lSphere->setScale(vec3f(0.3f, 0.3f, 0.3f));
		lSphere->setTranslation(vec3f(0.0f, 0.5f, 0.5f));
		lSphere->castShadow = true;

		auto lLight = std::shared_ptr<Light>(new DirectionalLight());
		lLight->setIntensity(vec4f(1.0f, 1.0f, 1.0f, 1.0f));
		lLight->name = "DefaultDirectionalLight";
		mDirectionalLight->light = lLight;
		mDirectionalLight->castShadow = true;

		mRootEntity->addChild(lWall);
		mRootEntity->addChild(lFloor);
		mRootEntity->addChild(lSphere);
		mRootEntity->addChild(mDirectionalLight);
	}

private:

	std::shared_ptr<gintonic::Entity> mDirectionalLight = std::make_shared<gintonic::Entity>
	(
		"DefaultDirectionalLight",
		gintonic::SQT
		(
			gintonic::vec3f(1.0f, 1.0f, 1.0f), 
			gintonic::quatf::axis_angle
			(
				gintonic::vec3f(1.0f, 0.0f, 0.0f), 
				-F_PI * 0.5f + 1.0f
			), 
			gintonic::vec3f(0.0f, 0.0f, 0.0f)
		)
	);

	virtual void onRenderUpdate() final
	{
		using namespace gintonic;
		if (Renderer::keyTogglePress(SDL_SCANCODE_L))
		{
			if (Renderer::getEntityDebugShadowBuffer())
			{
				Renderer::setEntityDebugShadowBuffer(nullptr);
			}
			else
			{
				Renderer::setEntityDebugShadowBuffer(mDirectionalLight);
			}
		}
		mDirectionalLight->setRotation(quatf::axis_angle(vec3f(1.0f, 0.0f, 0.0f), -M_PI / 2.0f + std::sin(mElapsedTime)));
		Renderer::cerr() << mDirectionalLight->name << '\n';
	}

};

DEFINE_MAIN(ShadowsApplication);