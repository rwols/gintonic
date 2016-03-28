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

		auto lBricksDiffuseTexture = std::make_shared<Texture2D>("Resources/bricks.jpg");
		auto lBricksSpecularTexture = std::make_shared<Texture2D>("Resources/bricks_SPEC.png");

		auto lMaterial = std::make_shared<Material>();
		lMaterial->name = "Bricks";
		lMaterial->diffuseColor = vec4f(1.0f, 1.0f, 1.0f, 1.0f);
		lMaterial->specularColor = vec4f(1.0f, 1.0f, 1.0f, 20.0f);
		lMaterial->diffuseTexture = lBricksDiffuseTexture;
		lMaterial->specularTexture = lBricksSpecularTexture;

		auto lFloor = std::make_shared<Entity>();
		lFloor->material = lMaterial;
		lFloor->mesh = Renderer::getUnitQuad();
		lFloor->castShadow = true;
		lFloor->setRotation(quatf::axis_angle(vec3f(1, 0, 0), -F_PI * 0.5f));

		for (int i = -4; i <= 4; ++i)
		{
			for (int j = -4; j <= 4; ++j)
			{
				auto lClone = lFloor->cloneRecursive();
				lClone->name = "Floor tile (" + std::to_string(i) + "," + std::to_string(j) + ")";
				lClone->setTranslation(vec3f(2 * i, 0, 2 * j));
				mRootEntity->addChild(lClone);
			}
		}

		mSphere = std::make_shared<Entity>();
		mSphere->name = "Sphere";
		mSphere->material = lMaterial;
		mSphere->mesh = Renderer::getUnitSphere();
		mSphere->setTranslation(vec3f(0.0f, 2.0f, 0.0f));
		mSphere->castShadow = true;
		mRootEntity->addChild(mSphere);

		auto lLight = std::shared_ptr<Light>(new DirectionalLight());
		lLight->setIntensity(vec4f(1.0f, 1.0f, 1.0f, 0.1f));
		lLight->name = "DirectionalLight";
		mDirectionalLight->light = lLight;
		mDirectionalLight->castShadow = true;
		

		auto lAnotherLight = std::shared_ptr<Light>
		(
			new SpotLight
			(
				vec4f(1.0f, 0.0f, 0.0f, 10.0f), 
				vec4f(0.0f, 0.5f, 0.5f, 4.0f), 
				0.2f
			)
		);
		lAnotherLight->name = "SpotLight";
		mSpotLight->light = lAnotherLight;
		mSpotLight->camera = Renderer::getCameraEntity()->camera;
		mSpotLight->castShadow = true;

		auto lSpotLightChild = std::make_shared<Entity>();
		lSpotLightChild->castShadow = false;
		lSpotLightChild->material = std::make_shared<Material>();
		lSpotLightChild->material->diffuseColor = vec4f(1.0f, 0.0f, 0.0f, 0.0f);
		lSpotLightChild->material->specularColor = vec4f(0.0f, 0.0f, 0.0f, 0.0f);
		lSpotLightChild->mesh = Renderer::getUnitSphere();
		// mSpotLight->addChild(lSpotLightChild);
		

		mDefaultCamera = Renderer::getCameraEntity();
		mDefaultCamera->camera->setAngles(vec2f(-0.5f, -0.1f));
		mDefaultCamera->setTranslation(vec3f(-3.0f, 1.0f, 4.0f));
		
		// mRootEntity->addChild(mDirectionalLight);
		mRootEntity->addChild(mSpotLight);
		
		Renderer::setFreeformCursor(true);
		Renderer::show();
	}

private:

	std::shared_ptr<gintonic::Entity> mSphere;

	std::shared_ptr<gintonic::Entity> mDirectionalLight = std::make_shared<gintonic::Entity>
	(
		"DefaultDirectionalLight",
		gintonic::SQT
		(
			gintonic::vec3f(1.0f, 1.0f, 1.0f), 
			gintonic::quatf::axis_angle
			(
				gintonic::vec3f(1.0f, 0.0f, 0.0f), 
				-F_PI * 0.5f
			), 
			gintonic::vec3f(0.0f, 0.0f, 0.0f)
		)
	);

	std::shared_ptr<gintonic::Entity> mSpotLight = std::make_shared<gintonic::Entity>
	(
		"SpotLight",
		gintonic::SQT
		(
			gintonic::vec3f(0.1f, 0.1f, 0.1f),
			gintonic::quatf::axis_angle
			(
				gintonic::vec3f(1.0f, 1.0f, 1.0f),
				- F_PI * 0.5f
			),
			gintonic::vec3f(-1.0f, 6.0f, 1.0f)
		)
	);

	std::shared_ptr<gintonic::Entity> mDefaultCamera;
	bool mSwitch = false;

	virtual void onRenderUpdate() final
	{
		using namespace gintonic;
		if (Renderer::keyTogglePress(SDL_SCANCODE_L))
		{
			mCameraRespondsToKeyboard = mCameraRespondsToMouse = mSwitch;
			mSwitch = !mSwitch;
			// Renderer::setCameraEntity(mSwitch ? mSpotLight : mDefaultCamera);
			Renderer::setEntityDebugShadowBuffer(mSwitch ? mSpotLight : nullptr);
		}
		mSpotLight->setTranslation(vec3f(4.0f * std::sin(mElapsedTime), 6.0f, 0.0f));
		// mDirectionalLight->setRotation(quatf::axis_angle(vec3f(1.0f, 0.0f, 0.0f), -M_PI / 2.0f + std::sin(mElapsedTime)));
		mSpotLight->setRotation(
			quatf::look_at(
				mSpotLight->localTransform().translation, 
				mSphere->localTransform().translation, 
				vec3f(0.0f, 1.0f, 0.0f)));
		// Renderer::cerr() << mDirectionalLight->name << '\n';
	}

};

DEFINE_MAIN(ShadowsApplication);
