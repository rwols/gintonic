
#include "Common/Application.hpp"

#define APPNAME "Octree"

class OctreeApplication : public Application
{
public:

	OctreeApplication(int argc, char** argv)
	: Application(APPNAME, argc, argv)
	{
		using namespace gintonic;

		auto lCamera = std::shared_ptr<Camera>(new Camera());
		lCamera->name = "OrthoCamera";
		lCamera->setNearPlane(-5.0f);
		lCamera->setFarPlane(5.0f);
		// lCamera->setWidth(4.0f * Renderer::aspectRatio());
		// lCamera->setHeight(4.0f);
		lCamera->setWidth(1.0f);
		lCamera->setHeight(1.0f);
		lCamera->setProjectionType(Camera::kOrthographic);
		Renderer::getCameraEntity()->camera = lCamera;

		// mRootEntity->addChild(mCubeEntity);

		mOctreeRoot.subdivisionThreshold = 0.02f;

		mMouseEntity = std::shared_ptr<Entity>(new Entity("Mouse"));

		mOctreeRoot.insert(mMouseEntity);

		mCameraRespondsToMouse = false;

		Renderer::onMouseMove.connect( [this] (const vec2f& position)
		{
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
