#include "Application.hpp"

#define APPNAME "PointLights"

// #define ALSO_ADD_DIRECTIONAL_LIGHT

class PointLightsApplication : public Application
{
public:

	PointLightsApplication(int argc, char** argv)
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

private:

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
			const auto lSpecularity  = vec4f(0.2f, 0.2f, 0.2f, 4.0f);
			const auto lAttenuation  = this->getRandomLightAttenuation(0.0f);
			      auto lIntensity    = this->getRandomColor(4.0f);
			const auto lUp           = vec3f(0.0f, 1.0f, 0.0f);
			const auto lAngle        = 2.0f * static_cast<float>(i) * lPi / lNumLights;

			lTransform.translation.x = lNumLights * std::cos(lAngle);
			lTransform.translation.z = lNumLights * std::sin(lAngle);

			lTransform.rotation      = quatf::axis_angle(lUp, -lAngle);

			auto lLight = std::shared_ptr<Light>(new PointLight(lIntensity, lAttenuation));

			lIntensity.w = 0.0f;

			auto lMaterial           = std::make_shared<Material>();
			lMaterial->name          = "Light" + std::to_string(i);
			lMaterial->diffuseColor  = lIntensity;
			lMaterial->specularColor = lSpecularity;

			auto lLightEntity        = std::make_shared<Entity>();
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

		std::vector<std::vector<bool>> lBoolMatrix; // Generate a random boolean matrix
		for (int i = -mNumObjects; i <= mNumObjects; ++i)
		{
			lBoolMatrix.push_back(std::vector<bool>(2 * mNumObjects + 1));
			for (int j  = -mNumObjects; j <= mNumObjects; ++j)
			{
				lBoolMatrix[i + mNumObjects][j + mNumObjects] = static_cast<bool>(std::rand() % 2);
			}
		}

		SQT lTransform;
		lTransform.scale = 1.0f;
		lTransform.rotation = quatf(1.0f, 0.0f, 0.0f, 0.0f);

		auto lDaVinciTexture       = std::make_shared<Texture2D>("../../Examples/DaVinci.jpg");
		auto lBrickDiffuseTexture  = std::make_shared<Texture2D>("../../Examples/bricks.jpg");
		auto lBrickSpecularTexture = std::make_shared<Texture2D>("../../Examples/bricks_SPEC.png");
		auto lBrickNormalTexture   = std::make_shared<Texture2D>("../../Examples/bricks_NRM.png");

		auto lDaVinciMaterial            = std::make_shared<Material>();
		lDaVinciMaterial->name           = "DaVinci";
		lDaVinciMaterial->diffuseColor   = vec4f(0.7f, 0.7f, 0.7f, 1.0f);
		lDaVinciMaterial->specularColor  = vec4f(0.3f, 0.3f, 0.3f, 80.0f);
		lDaVinciMaterial->diffuseTexture = lDaVinciTexture;

		auto lBrickMaterialWithNormalMap             = std::make_shared<Material>();
		lBrickMaterialWithNormalMap->name            = "BricksWithNormalMap";
		lBrickMaterialWithNormalMap->diffuseColor    = vec4f(0.8f, 0.8f, 0.8f,  1.0f);
		lBrickMaterialWithNormalMap->specularColor   = vec4f(0.2f, 0.2f, 0.2f, 20.0f);
		lBrickMaterialWithNormalMap->diffuseTexture  = lBrickDiffuseTexture;
		lBrickMaterialWithNormalMap->specularTexture = lBrickSpecularTexture;
		lBrickMaterialWithNormalMap->normalTexture   = lBrickNormalTexture;

		for (int i = -mNumObjects; i <= mNumObjects; ++i)
		{
			for (int j = -mNumObjects; j <= mNumObjects; ++j)
			{
				lTransform.translation.x = 3.0f * static_cast<float>(i);
				lTransform.translation.y = 0.0f;
				lTransform.translation.z = 3.0f * static_cast<float>(j);

				auto lEntity = std::make_shared<Entity>();
				lEntity->name = "Geometry(" + std::to_string(i) + "," + std::to_string(j) + ")";
				lEntity->setLocalTransform(lTransform);
				if (lBoolMatrix[i + mNumObjects][j + mNumObjects])
				{
					lEntity->material = lBrickMaterialWithNormalMap;
					lEntity->mesh = Renderer::getUnitCubeWithTangents();
				}
				else
				{
					lEntity->material = lDaVinciMaterial;
					lEntity->mesh = Renderer::getUnitSphere();
				}

				mRootEntity->addChild(lEntity);
			}
		}
	}

	virtual void onRenderUpdate() final
	{
		using namespace gintonic;
		mRootOfLights->postMultiplyRotation(quatf::axis_angle(vec3f(0.0f, 1.0f, 0.0f), mDeltaTime));
	}

};

DEFINE_MAIN(PointLightsApplication)
