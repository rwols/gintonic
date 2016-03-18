#include "Application.hpp"

#define APPNAME "Spot Lights"

#define ANGLE_CHANGE_SPEED 0.1f
#define EXPONENT_CHANGE_SPEED 0.5f

// #define ALSO_ADD_DIRECTIONAL_LIGHT

class SpotLightsApplication : public Application
{
public:

	SpotLightsApplication(int argc, char** argv)
	: Application(APPNAME, argc, argv)
	{
		using namespace gintonic;
		if (argc < 2)
		{
			exception lException("Usage: ");
			lException.append(argv[0]);
			lException.append(" <grid-size> <number-of-point-lights>");
			throw lException;
		}

		mNumObjects = std::atoi(argv[1]);
		mNumLights = std::atoi(argv[2]);

		if (mNumObjects <= 0)
		{
			exception lException("You cannot have ");
			lException.append(std::to_string(mNumObjects));
			lException.append(" objects in the scene.");
			throw lException;
		}

		if (mNumLights <= 0)
		{
			exception lException("You cannot have ");
			lException.append(std::to_string(mNumLights));
			lException.append(" lights in the scene.");
			throw lException;
		}

		std::srand((int)std::clock()); // random number generator seed

		initializeLights();

		initializeGeometry();

		auto lCameraEntity = Renderer::getCameraEntity();
		lCameraEntity->setTranslation(vec3f(4.0f, 5.0f, 4.0f));
		lCameraEntity->setRotation(quatf::mouse(vec2f(0.0f, -static_cast<float>(M_PI) / 4.0f)));

		mMoveSpeed = static_cast<float>(mNumObjects);

		Renderer::setFreeformCursor(true);
		Renderer::show();
	}

	virtual ~SpotLightsApplication() noexcept = default;

private:

	float mSpotlightExponent = 1.0f;

	float mSpotlightAngle = static_cast<float>(M_PI) * 0.5f;

	std::shared_ptr<gintonic::Entity> mRootOfLights;

	int mNumObjects, mNumLights;

	void initializeLights()
	{
		using namespace gintonic;

		const float lNumLights = static_cast<float>(mNumLights);
		const float lPi = static_cast<float>(M_PI);
		SQT lTransform;
		lTransform.scale = 0.1f;
		lTransform.rotation = quatf(1.0f, 0.0f, 0.0f, 0.0f);
		lTransform.translation = vec3f(0.0f, 0.0f, 0.0f);

		mRootOfLights = std::make_shared<Entity>("RootOfLights");
		mRootOfLights->setTranslationY(3.0f);

		mRootEntity->addChild(mRootOfLights);

		for (int i = 0; i < mNumLights; ++i)
		{
			vec4f lAttenuation(std::rand() % 1000, std::rand() % 1000, std::rand() % 1000, 0.0f);
			lAttenuation.normalize();
			if (lAttenuation.x > 0.9f)
			{
				lAttenuation.x = 0.0f;
				lAttenuation.normalize();
			}
			lAttenuation.w = mSpotlightExponent;
			const vec4f lSpecularity(0.2f, 0.2f, 0.2f, 4.0f);
			vec4f lIntensity;
			vec3f lColor;
			float lCeiling;
			lColor.x = std::rand() % 1000;
			lColor.y = std::rand() % 1000;
			lColor.z = std::rand() % 1000;
			lColor.normalize();
			lCeiling = std::max(std::max(lColor.x, lColor.y), lColor.z);
			lCeiling = 1.0f - lCeiling;
			lColor.x += lCeiling;
			lColor.y += lCeiling;
			lColor.z += lCeiling;
			lIntensity = {lColor.x, lColor.y, lColor.z, 100.0f};

			const auto lUp = vec3f(0.0f, 1.0f, 0.0f);
			const auto lAngle = 2.0f * static_cast<float>(i) * lPi / lNumLights;

			lTransform.translation.x = lNumLights * std::cos(lAngle);
			lTransform.translation.z = lNumLights * std::sin(lAngle);

			lTransform.rotation = quatf::axis_angle(lUp, -lAngle);
			lTransform.rotation *= quatf::axis_angle(vec3f(1.0f, 0.0f, 0.0f), -static_cast<float>(M_PI) / 2.0f);

			auto lLight = std::shared_ptr<Light>(new SpotLight(lIntensity, lAttenuation, mSpotlightAngle));

			lIntensity.w = 0.0f;

			auto lMaterial = std::make_shared<Material>();
			lMaterial->name = "Light" + std::to_string(i);
			lMaterial->diffuseColor = lIntensity;
			lMaterial->specularColor = lSpecularity;

			auto lLightEntity = std::make_shared<Entity>();
			lLightEntity->name = "Light" + std::to_string(i);
			lLightEntity->setLocalTransform(lTransform);
			lLightEntity->light = lLight;
			lLightEntity->material = lMaterial;
			lLightEntity->mesh = Renderer::getUnitSphere();

			lLightEntity->castShadow = false;

			lLightEntity->addChild(Renderer::createGizmo());

			mRootOfLights->addChild(lLightEntity);
		}

		#ifdef ALSO_ADD_DIRECTIONAL_LIGHT
			auto lLight = std::shared_ptr<Light>(new DirectionalLight());
			lLight->intensity = 0.3f;
			lLight->name = "DefaultDirectionalLight";

			auto lLightEntity = std::make_shared<gintonic::Entity>
			(
				"DefaultDirectionalLight", 
				SQT
				(
					vec3f(1.0f, 1.0f, 1.0f), 
					quatf::axis_angle
					(
						vec3f(1.0f, 0.0f, 0.0f), 
						-M_PI / 2.0f + 1.0f
					), 
					vec3f(0.0f, 0.0f, 0.0f)
				)
			);
			lLightEntity->light = lLight;
			mRootEntity->addChild(lLightEntity);
		#endif
	}

	void initializeGeometry()
	{
		using namespace gintonic;

		SQT lTransform;
		lTransform.scale = 1.0f;
		lTransform.rotation = quatf(1.0f, 0.0f, 0.0f, 0.0f);

		auto lBrickDiffuseTexture = std::make_shared<Texture2D>("../../Examples/bricks.jpg");
		auto lBrickSpecularTexture = std::make_shared<Texture2D>("../../Examples/bricks_SPEC.png");
		auto lBrickNormalTexture = std::make_shared<Texture2D>("../../Examples/bricks_NRM.png");

		auto lBrickMaterialWithNormalMap = std::make_shared<Material>();
		lBrickMaterialWithNormalMap->name = "BricksWithNormalMap";
		lBrickMaterialWithNormalMap->diffuseColor = vec4f(0.8f, 0.8f, 0.8f,  1.0f);
		lBrickMaterialWithNormalMap->specularColor = vec4f(0.2f, 0.2f, 0.2f, 20.0f);
		lBrickMaterialWithNormalMap->diffuseTexture = lBrickDiffuseTexture;
		lBrickMaterialWithNormalMap->specularTexture = lBrickSpecularTexture;
		lBrickMaterialWithNormalMap->normalTexture = lBrickNormalTexture;

		for (int i = -mNumObjects; i <= mNumObjects; ++i)
		{
			for (int j = -mNumObjects; j <= mNumObjects; ++j)
			{
				lTransform.translation.x = 2.0f * static_cast<float>(i);
				lTransform.translation.y = 0.0f;
				lTransform.translation.z = 2.0f * static_cast<float>(j);

				auto lEntity = std::make_shared<Entity>();
				lEntity->name = "Geometry(" + std::to_string(i) + "," + std::to_string(j) + ")";
				lEntity->setLocalTransform(lTransform);
				lEntity->material = lBrickMaterialWithNormalMap;
				lEntity->mesh = Renderer::getUnitCubeWithTangents();

				mRootEntity->addChild(lEntity);
			}
		}
	}

	virtual void onRenderUpdate() final
	{
		using namespace gintonic;

		vec3f lAxis;
		float lAngle;
		quatf lQuaternion;
		
		if (Renderer::key(SDL_SCANCODE_UP))
		{
			mSpotlightExponent += mDeltaTime * EXPONENT_CHANGE_SPEED;

			for (auto lChild : *mRootOfLights)
			{
				auto lAtt = lChild->light->getAttenuation();
				lAtt.w = mSpotlightExponent;
				lChild->light->setAttenuation(lAtt);
			}
		}
		if (Renderer::key(SDL_SCANCODE_DOWN))
		{
			mSpotlightExponent -= mDeltaTime * EXPONENT_CHANGE_SPEED;

			for (auto lChild : *mRootOfLights)
			{
				auto lAtt = lChild->light->getAttenuation();
				lAtt.w = mSpotlightExponent;
				lChild->light->setAttenuation(lAtt);
			}
		}
		if (Renderer::key(SDL_SCANCODE_LEFT))
		{
			mSpotlightAngle -= mDeltaTime * ANGLE_CHANGE_SPEED;

			if (mSpotlightAngle < 0.0f)
			{
				mSpotlightAngle = 0.0f;
			}

			for (auto lChild : *mRootOfLights)
			{
				lChild->light->setAngle(mSpotlightAngle);
			}
		}
		if (Renderer::key(SDL_SCANCODE_RIGHT))
		{
			mSpotlightAngle += mDeltaTime * ANGLE_CHANGE_SPEED;

			if (mSpotlightAngle > static_cast<float>(M_PI) * 0.5f)
			{
				mSpotlightAngle = static_cast<float>(M_PI) * 0.5f;
			}

			for (auto lChild : *mRootOfLights)
			{
				lChild->light->setAngle(mSpotlightAngle);
			}
		}

		lAxis = vec3f(0.0f, 1.0f, 0.0f);
		lAngle = mDeltaTime;
		lQuaternion = quatf::axis_angle(lAxis, lAngle);
		mRootOfLights->postMultiplyRotation(lQuaternion);
	}
};

DEFINE_MAIN(SpotLightsApplication)
