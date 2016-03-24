#include "Application.hpp"

#include <iomanip>

#define APPNAME "Orthographic Projection"

class HelloWorldTextApplication : public Application
{
public:

	HelloWorldTextApplication(int argc, char** argv)
	: Application(APPNAME, argc, argv)
	{
		using namespace gintonic;
		auto lCamera = std::make_shared<Camera>();
		lCamera->name = "OrthoCamera";
		lCamera->setNearPlane(1.0f);
		lCamera->setFarPlane(10.0f);
		lCamera->setWidth(5.0f);
		lCamera->setHeight(5.0f);
		lCamera->setProjectionType(Camera::kOrthographic);
		auto lCameraEntity = Renderer::getCameraEntity();
		lCameraEntity->camera = lCamera;
		lCameraEntity->setTranslationZ(4.0f);
		// Renderer::setCameraEntity(lCameraEntity);
		Renderer::setFreeformCursor(true);
		Renderer::show();

		auto lTexture = std::make_shared<Texture2D>("Resources/DaVinci.jpg");
		auto lMaterial = std::make_shared<Material>();
		lMaterial->name = "DaVinci";
		lMaterial->diffuseColor = vec4f(1.0f, 1.0f, 1.0f, 0.0f);
		lMaterial->diffuseTexture = lTexture;
		auto lCube = std::make_shared<Entity>("Cube");
		lCube->material = lMaterial;
		lCube->mesh = Renderer::getUnitCube();
		lCube->setParent(mRootEntity);
	}

	virtual ~HelloWorldTextApplication() noexcept = default;

private:

	virtual void onRenderUpdate() final
	{
		using namespace gintonic;
		Renderer::cerr() << "Hello world!\n";
	}

};

DEFINE_MAIN(HelloWorldTextApplication);
