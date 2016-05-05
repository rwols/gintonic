
#include "Application.hpp"

#define APPNAME "Octree"

class OctreeApplication : public Application
{
public:

	OctreeApplication(int argc, char** argv)
	: Application(APPNAME, argc, argv)
	{
		using namespace gintonic;

		auto lMaterial = std::make_shared<Material>();
		lMaterial->name = "RuralBrickWall";
		lMaterial->diffuseColor = vec4f(1.0f, 1.0f, 1.0f, 0.0f);
		lMaterial->specularColor = vec4f(1.0f, 1.0f, 1.0f, 8.0f);
		lMaterial->diffuseTexture = std::make_shared<Texture2D>("Resources/RuralBrickWall.jpg");
		mCubeEntity = std::make_shared<Entity>("Cube");
		mCubeEntity->material = lMaterial;
		mCubeEntity->mesh = Renderer::getUnitCube();

		auto lCamera = std::make_shared<Camera>();
		lCamera->name = "OrthoCamera";
		lCamera->setNearPlane(-5.0f);
		lCamera->setFarPlane(5.0f);
		// lCamera->setWidth(4.0f * Renderer::aspectRatio());
		// lCamera->setHeight(4.0f);
		lCamera->setWidth(1.0f);
		lCamera->setHeight(1.0f);
		lCamera->setProjectionType(Camera::kOrthographic);
		Renderer::getCameraEntity()->camera = lCamera;

		mRootEntity->addChild(mCubeEntity);

		mOctreeRoot.subdivisionThreshold = 0.1f;

		mMouseEntity = std::make_shared<Entity>("Mouse");

		mOctreeRoot.insert(mMouseEntity);

		mCameraRespondsToMouse = false;

		Renderer::onMouseMove.connect( [this] (const vec2f& position)
		{
			Renderer::cerr() << position << '\n';
			mMouseEntity->setTranslation(vec3f(position.x, position.y, 0.0f));
		});

		Renderer::debugDrawOctree(&mOctreeRoot);

		Renderer::setFreeformCursor(false);
		Renderer::enableCursor();
		Renderer::centerCursor();
		Renderer::show();
	}

	virtual ~OctreeApplication() noexcept = default;

private:

	gintonic::Octree mOctreeRoot{ {-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f} };

	std::shared_ptr<gintonic::Entity> mMouseEntity;

	std::shared_ptr<gintonic::Entity> mCubeEntity;

	virtual void onRenderUpdate() final
	{
		using namespace gintonic;

		

		// const auto lYAxis = (1.0f + std::cos(mElapsedTime)) / 2.0f;
		// const auto lZAxis = (1.0f + std::sin(mElapsedTime)) / 2.0f;
		// const auto lRotationAxis = vec3f(0.0f, lYAxis, lZAxis).normalize();
		// const auto lQuaternion = quatf::axis_angle(lRotationAxis, -mElapsedTime / 4.0f);

		// mCubeEntity->setRotation(lQuaternion);
		// mCubeEntity->material->diffuseColor.x = lYAxis;
		// mCubeEntity->material->diffuseColor.y = lZAxis;
	}
};

DEFINE_MAIN(OctreeApplication)
