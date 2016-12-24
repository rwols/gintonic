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

		Texture2D::ImageLoadOptions imageOpts;
		imageOpts.relativeFilename = "Resources/images/DaVinci.jpg";
		auto lDaVinciTexture       = Texture2D::fromImage(imageOpts);
		imageOpts.relativeFilename = "Resources/images/bricks.jpg";
		auto lBrickDiffuseTexture = Texture2D::fromImage(imageOpts);
		imageOpts.relativeFilename = "Resources/bricks_SPEC.png";
		auto lBrickSpecularTexture = Texture2D::fromImage(imageOpts);
		imageOpts.relativeFilename = "Resources/bricks_NRM.png";		
		auto lBrickNormalTexture   = Texture2D::fromImage(imageOpts);

		auto lMaterial = Material::create();
		lMaterial->name = "Bricks";
		lMaterial->diffuseColor = vec4f(1.0f, 1.0f, 1.0f, 0.9f);
		lMaterial->specularColor = vec4f(0.3f, 0.3f, 0.3f, 20.0f);
		lMaterial->diffuseTexture = lBrickDiffuseTexture;
		lMaterial->specularTexture = lBrickSpecularTexture;
		lMaterial->normalTexture = lBrickNormalTexture;

		auto lFloor = Entity::create();
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
				lClone->setTranslation(vec3f(static_cast<float>(2 * i), -1.0f, static_cast<float>(2 * j)));
				mRootEntity->addChild(lClone);
				mOctreeRoot.insert(lClone);
			}
		}

		mSphere = Entity::create();
		mSphere->name = "Sphere";
		mSphere->material = lMaterial;
		mSphere->mesh = Renderer::getUnitCubeWithTangents();
		mSphere->setTranslation(vec3f(0.0f, 2.0f, 0.0f));
		mSphere->castShadow = true;
		mRootEntity->addChild(mSphere);
		mOctreeRoot.insert(mSphere);

		mDirectionalLight->light = DirectionalLight::create(vec4f(1.0f, 1.0f, 1.0f, 0.1f));
		mDirectionalLight->light->name = "DirectionalLight";
		mDirectionalLight->castShadow = true;
		
		mSpotLight->light = SpotLight::create(vec4f(1.0f, 0.0f, 0.0f, 10.0f), vec4f(0.1f, 0.4f, 0.5f, 4.0f), SPOTLIGHT_HALF_ANGLE_START_VALUE);
		mSpotLight->light->name = "spotty the spotlight";
		mSpotLight->camera = Renderer::getCameraEntity()->camera;
		mSpotLight->castShadow = true;

		auto lSpotLightChild = Entity::create();
		lSpotLightChild->castShadow = false;
		lSpotLightChild->material = Material::create();
		lSpotLightChild->material->diffuseColor = vec4f(1.0f, 0.0f, 0.0f, 0.0f);
		lSpotLightChild->material->specularColor = vec4f(0.0f, 0.0f, 0.0f, 0.0f);
		lSpotLightChild->mesh = Renderer::getUnitSphere();
		mSpotLight->addChild(lSpotLightChild);

		mPointLight->light = PointLight::create(vec4f(1.0f, 1.0f, 1.0f, 10.0f), vec4f(0.1f, 0.4f, 0.5f, 4.0f));
		mPointLight->light->name = "pointy the point light";
		mPointLight->camera = Renderer::getCameraEntity()->camera;
		mPointLight->castShadow = true;
		auto lPointLightChild = Entity::create();
		lPointLightChild->castShadow = false;
		lPointLightChild->material = Material::create();
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

	std::shared_ptr<gintonic::Entity> mDirectionalLight = gintonic::Entity::create
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

	std::shared_ptr<gintonic::Entity> mSpotLight = gintonic::Entity::create
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
	);

	std::shared_ptr<gintonic::Entity> mPointLight = gintonic::Entity::create
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
	);

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
			mSpotlightAngle -= static_cast<float>(mDeltaTime) * ANGLE_CHANGE_SPEED;
			mSpotlightAngle = std::max(0.0f, mSpotlightAngle);
			mSpotLight->light->setCosineHalfAngle(mSpotlightAngle);

		}
		if (Renderer::key(SDL_SCANCODE_RIGHT))
		{
			mSpotlightAngle += static_cast<float>(mDeltaTime) * ANGLE_CHANGE_SPEED;
			mSpotlightAngle = std::min(1.0f, mSpotlightAngle);
			mSpotLight->light->setCosineHalfAngle(mSpotlightAngle);
		}
		if (Renderer::keyTogglePress(SDL_SCANCODE_O))
		{
			mDrawOctreeSwitch = !mDrawOctreeSwitch;
			Renderer::debugDrawOctree(mDrawOctreeSwitch ? &mOctreeRoot : nullptr);
		}

		mSpotLight->setTranslation(vec3f(-4.0f * std::sin(static_cast<float>(mElapsedTime)), 6.0f, 2.0f));
		mSpotLight->setRotation(
			quatf::look_at(
				mSpotLight->localTransform().translation, 
				mSphere->localTransform().translation,  
				vec3f(0.0f, 1.0f, 0.0f)));
		mPointLight->setTranslation(vec3f(4.0f * std::sin(static_cast<float>(mElapsedTime)), 6.0f, -2.0f));
		mSphere->postMultiplyRotation(quatf::axis_angle(vec3f(0.0f, 1.0f, 0.0f), static_cast<float>(mDeltaTime) / 10.0f));

		std::vector<std::shared_ptr<Entity>> lNearbyEntities;
		getNearbyEntities(Renderer::getCameraEntity(), 8.0f, std::back_inserter(lNearbyEntities));
		
		for (const auto& lNearbyEntity : lNearbyEntities)
		{
			Renderer::cerr() << lNearbyEntity->name << " is near!\n";
		}
	}

};

DEFINE_MAIN(ShadowsApplication);
