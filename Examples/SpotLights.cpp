#include "Common/Application.hpp"

#define APPNAME "Spot Lights"

#define SPOTLIGHT_EXPONENT_START_VALUE 1.0f
#define SPOTLIGHT_HALF_ANGLE_START_VALUE 0.7f
#define SPOTLIGHT_INTENSITY_START_VALUE 4.0f

#define MATERIAL_DIFFUSE_COLOR vec4f(0.8f, 0.8f, 0.8f, 1.0f)
#define MATERIAL_SPECULAR_COLOR vec4f(0.2f, 0.2f, 0.2f, 20.0f)

#define ANGLE_CHANGE_SPEED 0.1f
#define EXPONENT_CHANGE_SPEED 5.0f
#define INTENSITY_CHANGE_SPEED 10.0f

// #define ALSO_ADD_DIRECTIONAL_LIGHT

class SpotLightsApplication : public Application
{
public:

	SpotLightsApplication(int argc, char** argv)
	: Application(APPNAME, argc, argv)
	{
		using namespace gintonic;
		if (argc < 4)
		{
			exception lException("Usage: ");
			lException.append(argv[0]);
			lException.append("<fullscreen?> <grid-size> <number-of-point-lights>");
			throw lException;
		}

		mNumObjects = std::atoi(argv[2]);
		mNumLights = std::atoi(argv[3]);

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

	virtual ~SpotLightsApplication() noexcept
	{
		try
		{
			std::cout << "Last spot light exponent: " << mSpotlightExponent << '\n';
			std::cout << "Last spot light angle: " << mSpotlightAngle << '\n';
			std::cout << "Last intensity.w value: " << mLightIntensity << '\n';
		}
		catch (...)
		{
			// do nothing
		}
	}

private:

	float mSpotlightExponent = SPOTLIGHT_EXPONENT_START_VALUE;

	float mSpotlightAngle = SPOTLIGHT_HALF_ANGLE_START_VALUE;

	float mLightIntensity = SPOTLIGHT_INTENSITY_START_VALUE;

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

		mRootOfLights = std::shared_ptr<Entity>(new Entity("RootOfLights"));
		mRootOfLights->setTranslationY(3.0f);

		mRootEntity->addChild(mRootOfLights);

		for (int i = 0; i < mNumLights; ++i)
		{

			const auto lSpecularity  = MATERIAL_SPECULAR_COLOR;
			const auto lAttenuation  = this->getRandomLightAttenuation(mSpotlightExponent);
			      auto lIntensity    = this->getRandomColor(SPOTLIGHT_INTENSITY_START_VALUE);
			const auto lUp           = vec3f(0.0f, 1.0f, 0.0f);
			const auto lAngle        = 2.0f * static_cast<float>(i) * lPi / lNumLights;

			lTransform.translation.x = lNumLights * std::cos(lAngle);
			lTransform.translation.z = lNumLights * std::sin(lAngle);

			lTransform.rotation      = quatf::axis_angle(lUp, -lAngle);
			lTransform.rotation     *= quatf::axis_angle(vec3f(1.0f, 0.0f, 0.0f), -static_cast<float>(M_PI) / 2.0f);

			// auto lLight              = std::shared_ptr<Light>(new PointLight(lIntensity, lAttenuation));
			// lLight->name             = "PointLight" + std::to_string(i);
			auto lLight              = std::shared_ptr<Light>(new SpotLight(lIntensity, lAttenuation, mSpotlightAngle));
			lLight->name             = "SpotLight" + std::to_string(i);

			lIntensity.w             = 0.0f;

			auto lMaterial           = std::shared_ptr<Material>(new Material());
			lMaterial->name          = "Light" + std::to_string(i);
			lMaterial->diffuseColor  = lIntensity;
			lMaterial->specularColor = lSpecularity;

			auto lLightEntity        = std::shared_ptr<Entity>(new Entity());
			lLightEntity->name       = "Light" + std::to_string(i);
			lLightEntity->light      = lLight;
			lLightEntity->material   = lMaterial;
			lLightEntity->mesh       = Renderer::getUnitSphere();
			lLightEntity->castShadow = false;
			lLightEntity->setLocalTransform(lTransform);

			mRootOfLights->addChild(lLightEntity);
		}

		#ifdef ALSO_ADD_DIRECTIONAL_LIGHT
		auto lLight = std::shared_ptr<Light>(new DirectionalLight());
		lLight->intensity = 0.3f;
		lLight->name = "DefaultDirectionalLight";

		auto lLightEntity = std::shared_ptr<gintonic::Entity>(new Entity
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
		));
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

		auto lBrickDiffuseTexture = std::shared_ptr<Texture2D>(new Texture2D("../../Examples/bricks.jpg"));
		auto lBrickSpecularTexture = std::shared_ptr<Texture2D>(new Texture2D("../../Examples/bricks_SPEC.png"));
		auto lBrickNormalTexture = std::shared_ptr<Texture2D>(new Texture2D("../../Examples/bricks_NRM.png"));

		auto lBrickMaterialWithNormalMap = std::shared_ptr<Material>(new Material());
		lBrickMaterialWithNormalMap->name = "BricksWithNormalMap";
		lBrickMaterialWithNormalMap->diffuseColor = MATERIAL_DIFFUSE_COLOR;
		lBrickMaterialWithNormalMap->specularColor = MATERIAL_SPECULAR_COLOR;
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

				auto lEntity = std::shared_ptr<Entity>(new Entity());
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

			if (mSpotlightExponent < 1.0f)
			{
				mSpotlightExponent = 1.0f;
			}

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
			mSpotlightAngle = std::max(0.0f, mSpotlightAngle);

			for (auto lChild : *mRootOfLights)
			{
				lChild->light->setCosineHalfAngle(mSpotlightAngle);
			}
		}
		if (Renderer::key(SDL_SCANCODE_RIGHT))
		{
			mSpotlightAngle += mDeltaTime * ANGLE_CHANGE_SPEED;
			mSpotlightAngle = std::min(1.0f, mSpotlightAngle);

			for (auto lChild : *mRootOfLights)
			{
				lChild->light->setCosineHalfAngle(mSpotlightAngle);
			}
		}
		if (Renderer::key(SDL_SCANCODE_EQUALS))
		{
			mLightIntensity += mDeltaTime * INTENSITY_CHANGE_SPEED;

			for (auto lChild : *mRootOfLights)
			{
				lChild->light->setBrightness(mLightIntensity);
			}
		}
		if (Renderer::key(SDL_SCANCODE_MINUS))
		{
			mLightIntensity -= mDeltaTime * INTENSITY_CHANGE_SPEED;

			if (mLightIntensity < 0.0f)
			{
				mLightIntensity = 0.0f;
			}

			for (auto lChild : *mRootOfLights)
			{
				lChild->light->setBrightness(mLightIntensity);
			}
		}

		lAxis = vec3f(0.0f, 1.0f, 0.0f);
		lAngle = mDeltaTime;
		lQuaternion = quatf::axis_angle(lAxis, lAngle);
		mRootOfLights->postMultiplyRotation(lQuaternion);
	}
};

DEFINE_MAIN(SpotLightsApplication)
