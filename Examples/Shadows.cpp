#include "Common/Application.hpp"

#define APPNAME "Shadows"

#define F_PI static_cast<float>(M_PI)

#define SPOTLIGHT_HALF_ANGLE_START_VALUE 0.2f
#define ANGLE_CHANGE_SPEED 0.1f

class ShadowsApplication : public Application
{
public:

	template <class OutputIter>
	void getNearbyEntities(
		std::shared_ptr<gintonic::Entity> entity, 
		const float distanceFrom, 
		OutputIter iter)
	{
		using namespace gintonic;
		const vec3f lPosition = vec3f(entity->globalTransform().data[3]);
		const vec3f lDist(distanceFrom);
		const box3f lBox(lPosition - lDist, lPosition + lDist);
		mOctreeRoot.query(lBox, iter, [&entity, &distanceFrom](const std::shared_ptr<Entity>& other)
		{
			if (gintonic::distance2(
				vec3f(other->globalTransform().data[3]), 
				vec3f(entity->globalTransform().data[3])) <= distanceFrom * distanceFrom)
			{
				return true;
			}
			else
			{
				return false;
			}
		});
	}
	
	ShadowsApplication(int argc, char** argv)
	: Application(APPNAME, argc, argv)
	{
		using namespace gintonic;

		auto lBricksDiffuseTexture = std::shared_ptr<Texture2D>(new Texture2D("Resources/bricks.jpg"));
		auto lBricksSpecularTexture = std::shared_ptr<Texture2D>(new Texture2D("Resources/bricks_SPEC.png"));
		auto lBricksNormalTexture = std::shared_ptr<Texture2D>(new Texture2D("Resources/bricks_NRM.png"));

		auto lMaterial = std::shared_ptr<Material>(new Material());
		lMaterial->name = "Bricks";
		lMaterial->diffuseColor = vec4f(1.0f, 1.0f, 1.0f, 0.9f);
		lMaterial->specularColor = vec4f(0.3f, 0.3f, 0.3f, 20.0f);
		lMaterial->diffuseTexture = lBricksDiffuseTexture;
		lMaterial->specularTexture = lBricksSpecularTexture;
		lMaterial->normalTexture = lBricksNormalTexture;

		auto lFloor = std::shared_ptr<Entity>(new Entity());
		lFloor->material = lMaterial;
		lFloor->mesh = Renderer::getUnitCubeWithTangents();
		lFloor->castShadow = true;
		lFloor->setRotation(quatf::axis_angle(vec3f(1, 0, 0), -F_PI * 0.5f));

		mOctreeRoot.subdivisionThreshold = 0.5f;

		for (int i = -4; i <= 4; ++i)
		{
			for (int j = -4; j <= 4; ++j)
			{
				auto lClone = lFloor->cloneRecursive();
				lClone->name = "Floor tile (" + std::to_string(i) + "," + std::to_string(j) + ")";
				lClone->setTranslation(vec3f(2 * i, -1, 2 * j));
				mRootEntity->addChild(lClone);
				mOctreeRoot.insert(lClone);
			}
		}

		mSphere = std::shared_ptr<Entity>(new Entity());
		mSphere->name = "Sphere";
		mSphere->material = lMaterial;
		mSphere->mesh = Renderer::getUnitCubeWithTangents();
		mSphere->setTranslation(vec3f(0.0f, 2.0f, 0.0f));
		mSphere->castShadow = true;
		mRootEntity->addChild(mSphere);
		mOctreeRoot.insert(mSphere);

		mDirectionalLight->light = std::shared_ptr<Light>
		(
			new DirectionalLight
			(
				vec4f(1.0f, 1.0f, 1.0f, 0.1f) // Intensity
			)
		);
		mDirectionalLight->light->name = "DirectionalLight";
		mDirectionalLight->castShadow = true;
		
		mSpotLight->light = std::shared_ptr<Light>
		(
			new SpotLight
			(
				vec4f(1.0f, 0.0f, 0.0f, 10.0f),  // Intensity
				vec4f(0.1f, 0.4f, 0.5f, 4.0f),   // Attenuation
				SPOTLIGHT_HALF_ANGLE_START_VALUE // Angle
			)
		);
		mSpotLight->light->name = "spotty the spotlight";
		mSpotLight->camera = Renderer::getCameraEntity()->camera;
		mSpotLight->castShadow = true;

		auto lSpotLightChild = std::shared_ptr<Entity>(new Entity());
		lSpotLightChild->castShadow = false;
		lSpotLightChild->material = std::shared_ptr<Material>(new Material());
		lSpotLightChild->material->diffuseColor = vec4f(1.0f, 0.0f, 0.0f, 0.0f);
		lSpotLightChild->material->specularColor = vec4f(0.0f, 0.0f, 0.0f, 0.0f);
		lSpotLightChild->mesh = Renderer::getUnitSphere();
		mSpotLight->addChild(lSpotLightChild);

		mPointLight->light = std::shared_ptr<Light>
		(
			new PointLight
			(
				vec4f(1.0f, 1.0f, 1.0f, 10.0f),  // Intensity
				vec4f(0.1f, 0.4f, 0.5f, 4.0f)    // Attenuation
			)
		);
		mPointLight->light->name = "pointy the point light";
		mPointLight->camera = Renderer::getCameraEntity()->camera;
		mPointLight->castShadow = true;
		auto lPointLightChild = std::shared_ptr<Entity>(new Entity());
		lPointLightChild->castShadow = false;
		lPointLightChild->material = std::shared_ptr<Material>(new Material());
		lPointLightChild->material->diffuseColor = vec4f(1.0f, 1.0f, 1.0f, 0.0f);
		lPointLightChild->material->specularColor = vec4f(0.0f, 0.0f, 0.0f, 0.0f);
		lPointLightChild->mesh = Renderer::getUnitSphere();
		mPointLight->addChild(lPointLightChild);
		
		mDefaultCamera = Renderer::getCameraEntity();
		mDefaultCamera->camera->setAngles(vec2f(-0.5f, -0.1f));
		mDefaultCamera->setTranslation(vec3f(-3.0f, 1.0f, 4.0f));
		
		mRootEntity->addChild(mSpotLight);
		mRootEntity->addChild(mPointLight);

		mOctreeRoot.insert(mSpotLight);
		mOctreeRoot.insert(mPointLight);
		
		Renderer::setFreeformCursor(true);
		Renderer::show();
	}

private:

	gintonic::Octree mOctreeRoot{{-100, -100, -100}, {100, 100, 100}};

	std::shared_ptr<gintonic::Entity> mSphere;

	std::shared_ptr<gintonic::Entity> mDirectionalLight = std::shared_ptr<gintonic::Entity>(new gintonic::Entity
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
	));

	std::shared_ptr<gintonic::Entity> mSpotLight = std::shared_ptr<gintonic::Entity>(new gintonic::Entity
	(
		"spotty",
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
	));

	std::shared_ptr<gintonic::Entity> mPointLight = std::shared_ptr<gintonic::Entity>(new gintonic::Entity
	(
		"pointy",
		gintonic::SQT
		(
			gintonic::vec3f(0.1f, 0.1f, 0.1f),
			gintonic::quatf::axis_angle
			(
				gintonic::vec3f(1.0f, 1.0f, 1.0f),
				- F_PI * 0.5f
			),
			gintonic::vec3f(2.0f, 6.0f, -2.0f)
		)
	));

	std::shared_ptr<gintonic::Entity> mDefaultCamera;

	bool mSwitch = false;
	bool mDrawOctreeSwitch = false;

	float mSpotlightAngle = SPOTLIGHT_HALF_ANGLE_START_VALUE;

	virtual void onRenderUpdate() final
	{
		using namespace gintonic;
		if (Renderer::keyTogglePress(SDL_SCANCODE_L))
		{
			mSwitch = !mSwitch;
			Renderer::setEntityDebugShadowBuffer(mSwitch ? mDirectionalLight : nullptr);
		}
		if (Renderer::key(SDL_SCANCODE_LEFT))
		{
			mSpotlightAngle -= mDeltaTime * ANGLE_CHANGE_SPEED;
			mSpotlightAngle = std::max(0.0f, mSpotlightAngle);
			mSpotLight->light->setCosineHalfAngle(mSpotlightAngle);

		}
		if (Renderer::key(SDL_SCANCODE_RIGHT))
		{
			mSpotlightAngle += mDeltaTime * ANGLE_CHANGE_SPEED;
			mSpotlightAngle = std::min(1.0f, mSpotlightAngle);
			mSpotLight->light->setCosineHalfAngle(mSpotlightAngle);
		}
		if (Renderer::keyTogglePress(SDL_SCANCODE_O))
		{
			mDrawOctreeSwitch = !mDrawOctreeSwitch;
			Renderer::debugDrawOctree(mDrawOctreeSwitch ? &mOctreeRoot : nullptr);
		}

		mSpotLight->setTranslation(vec3f(-4.0f * std::sin(mElapsedTime), 6.0f, 2.0f));
		mSpotLight->setRotation(
			quatf::look_at(
				mSpotLight->localTransform().translation, 
				mSphere->localTransform().translation,  
				vec3f(0.0f, 1.0f, 0.0f)));
		mPointLight->setTranslation(vec3f(4.0f * std::sin(mElapsedTime), 6.0f, -2.0f));
		mSphere->postMultiplyRotation(quatf::axis_angle(vec3f(0.0f, 1.0f, 0.0f), mDeltaTime / 10.0f));

		std::vector<std::shared_ptr<Entity>> lNearbyEntities;
		getNearbyEntities(Renderer::getCameraEntity(), 8.0f, std::back_inserter(lNearbyEntities));
		
		for (const auto& lNearbyEntity : lNearbyEntities)
		{
			Renderer::cerr() << lNearbyEntity->name << " is near!\n";
		}
	}

};

DEFINE_MAIN(ShadowsApplication);
