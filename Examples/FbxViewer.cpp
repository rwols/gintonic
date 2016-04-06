#include "Application.hpp"
#include <iomanip>
#include <fstream>
#include "../Source/Foundation/portable_oarchive.hpp"
#include "../Source/Foundation/portable_iarchive.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/weak_ptr.hpp>

using OutputArchiveType = eos::portable_oarchive;
using InputArchiveType = eos::portable_iarchive;

#define APPNAME "FbxViewer"
#define LIGHT_INTENSITY_START_VALUE 1.0f
#define INTENSITY_CHANGE_SPEED 10.0f

class EntityProcessor : public gintonic::EntityVisitor
{
public:

	EntityProcessor(
		std::shared_ptr<gintonic::Entity> root, 
		std::ostream& stream, 
		const std::size_t tabwidth,
		// const float lightIntensityStartValue,
		std::vector<std::shared_ptr<gintonic::Entity>>& lights) 
	: EntityVisitor(root)
	, mStream(stream)
	, mTabWidth(tabwidth)
	// , mLightIntensityStartValue(lightIntensityStartValue)
	, mLights(lights)
	{

	}

	virtual ~EntityProcessor() = default;

	bool containsLight;
	std::size_t numMeshes;
	float niceMoveSpeed;
	float cameraOffsetZ;

private:

	virtual bool onStart()
	{
		containsLight = false;
		numMeshes = 0;
		niceMoveSpeed = 0.0f;
		cameraOffsetZ = 0.0f;
		return true;
	}

	virtual bool onVisit(std::shared_ptr<gintonic::Entity> entity)
	{
		

		mStream << std::setw(mTabWidth * this->currentDepth()) << std::setfill(' ') << "";
		mStream << entity->name << " [ ";
		if (entity->light)
		{
			containsLight = true;
			mStream << "light ";
			// entity->light->setBrightness(mLightIntensityStartValue);
			mLights.push_back(entity);
		}
		if (entity->mesh)
		{
			mStream << "mesh ";
			++numMeshes;
			const auto lBBox = entity->mesh->getLocalBoundingBox();
			auto lBBoxArea = 
				(lBBox.maxCorner.x - lBBox.minCorner.x) * 
				(lBBox.maxCorner.y - lBBox.minCorner.y) * 
				(lBBox.maxCorner.z - lBBox.minCorner.z);

			// Take the cube root. Crude approximation of a single side length.
			lBBoxArea = std::pow(lBBoxArea, 1.0f / 3.0f);

			niceMoveSpeed += lBBoxArea;

			if (lBBox.maxCorner.z > cameraOffsetZ)
			{
				cameraOffsetZ = lBBox.maxCorner.z;
			}
		}
		if (entity->material)
		{
			mStream << "material ";
		}
		if (entity->camera)
		{
			mStream << "camera ";
		}
		mStream << "]\n";
		return true;
	}

	virtual void onFinish()
	{
		niceMoveSpeed /= static_cast<float>(numMeshes);
	}

private:
	std::ostream& mStream;
	const std::size_t mTabWidth;
	// float mLightIntensityStartValue;
	std::vector<std::shared_ptr<gintonic::Entity>>& mLights;

};

std::shared_ptr<gintonic::Entity> gRootEntity;
std::shared_ptr<gintonic::Entity> gModel;
float gMoveSpeed = 1.0f;

class FbxViewerApplication : public Application
{
public:

	std::shared_ptr<gintonic::Entity> mModel;

	float mLightIntensity = LIGHT_INTENSITY_START_VALUE;

	std::vector<std::shared_ptr<gintonic::Entity>> mLights;

	FbxViewerApplication(int argc, char** argv)
	: Application(APPNAME, argc, argv)
	{
		using namespace gintonic;
		if (argc <= 2)
		{
			throw exception("Supply an FBX filename! Note: The first argument is wether we go fullscreen, the second argument is the FBX filename. This example also checks if there's already something in the cache with the same filename.");
		}
		const boost::filesystem::path lFilename(argv[2]);
		if (!boost::filesystem::is_regular_file(lFilename))
		{
			exception lException(lFilename.string());
			lException.append(" is not a regular file!");
			throw lException;
		}

		const auto lCachedFile = "Resources" / boost::filesystem::path(lFilename.stem().string() + ".entity");

		if (lFilename.extension() == ".entity")
		{
			std::cout << "Loading native file: " << lFilename << '\n';
			const auto lFilenameAsString = lFilename.string();
			std::ifstream lInput(lFilenameAsString);
			InputArchiveType lInputArchive(lInput);
			lInputArchive >> mModel;
		}
		else if (boost::filesystem::is_regular_file(lCachedFile))
		{
			std::cout << "Found cache: " << lCachedFile << '\n';
			const auto lFilenameAsString = lCachedFile.string();
			std::ifstream lInput(lFilenameAsString);
			InputArchiveType lInputArchive(lInput);
			lInputArchive >> mModel;
		}
		else
		{
			std::cout << "Loading file: " << lFilename << '\n';
			const auto lFilenameAsString = lFilename.string();
			gintonic::FbxImporter lImporter;
			mModel = lImporter.loadFromFile(lFilenameAsString.c_str());
			mModel->name = lFilename.stem().string();
			// Serialize the model for caching
			std::ofstream lOutput("Resources/" + mModel->name + ".entity");
			OutputArchiveType lOutputArchive(lOutput);
			lOutputArchive << mModel;
		}

		mRootEntity->addChild(mModel);
		mRootEntity->addChild(Renderer::createGizmo());
		
		EntityProcessor lEntityProcessor(
			mRootEntity, 
			std::cout, 
			4, 
			// mLightIntensity, 
			mLights);

		std::cout << "\n\tHere's an overview of the scene:\n\n";
		lEntityProcessor.execute();
		if (lEntityProcessor.containsLight == false)
		{
			std::cout << "\nScene contains no lights! Adding a directional light...\n";
			// Put a directional light in the scene
			// so that we see something interesting.
			// The directional light shines downwards.
			
			auto lLight = std::shared_ptr<Light>(new DirectionalLight());
			lLight->setIntensity(vec4f(1.0f, 1.0f, 1.0f, 1.0f));
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
		}
		mMoveSpeed = lEntityProcessor.niceMoveSpeed;
		Renderer::getCameraEntity()->setTranslationZ(lEntityProcessor.cameraOffsetZ + 4.0f);
		
		Renderer::setFreeformCursor(true);
		Renderer::show();
	}

private:

	virtual void onRenderUpdate() final
	{
		using namespace gintonic;
		if (Renderer::key(SDL_SCANCODE_EQUALS))
		{
			mLightIntensity += mDeltaTime * INTENSITY_CHANGE_SPEED;
			for (auto lEntity : mLights)
			{
				lEntity->light->setBrightness(mLightIntensity);
			}
		}
		if (Renderer::key(SDL_SCANCODE_MINUS))
		{
			mLightIntensity -= mDeltaTime * INTENSITY_CHANGE_SPEED;

			if (mLightIntensity < 0.0f)
			{
				mLightIntensity = 0.0f;
			}

			for (auto lEntity : mLights)
			{
				lEntity->light->setBrightness(mLightIntensity);
			}
		}
		mModel->setRotation(quatf::axis_angle(vec3f(0.0f, 1.0f, 0.0f), mElapsedTime / 10.0f));
	}
};

DEFINE_MAIN(FbxViewerApplication)
