#include "Common/Application.hpp"

#define APPNAME "TexturedCube"

class TexturedCubeApplication : public Application
{
public:

	TexturedCubeApplication(int argc, char** argv)
	: Application(APPNAME, argc, argv)
	{
		using namespace gintonic;

		auto lMaterial = Material::create();
		lMaterial->name = "RuralBrickWall";
		lMaterial->diffuseColor = vec4f(1.0f, 1.0f, 1.0f, 0.0f);
		lMaterial->specularColor = vec4f(1.0f, 1.0f, 1.0f, 8.0f);
		Texture2D::ImageLoadOptions imageOpts;
		imageOpts.relativeFilename = "assets/images/RuralBrickWall.jpg";
		lMaterial->diffuseTexture = Texture2D::fromImage(imageOpts);
		mCubeEntity = Entity::create("Cube");
		mCubeEntity->material = lMaterial;
		mCubeEntity->mesh = Renderer::getUnitCube();

		Renderer::getCameraEntity()->setTranslation(vec3f(0.0f, 0.0f, 4.0f));

		mRootEntity->addChild(mCubeEntity);

		Renderer::setFreeformCursor(true);
		Renderer::show();
	}

private:

	std::shared_ptr<gintonic::Entity> mCubeEntity;

	virtual void onRenderUpdate() final
	{
		using namespace gintonic;

		const auto lYAxis = (1.0f + std::cos(static_cast<float>(mElapsedTime))) / 2.0f;
		const auto lZAxis = (1.0f + std::sin(static_cast<float>(mElapsedTime))) / 2.0f;
		const auto lRotationAxis = vec3f(0.0f, lYAxis, lZAxis).normalize();
		const auto lQuaternion = quatf::axis_angle(lRotationAxis, -static_cast<float>(mElapsedTime) / 4.0f);

		mCubeEntity->setRotation(lQuaternion);
		mCubeEntity->material->diffuseColor.x = lYAxis;
		mCubeEntity->material->diffuseColor.y = lZAxis;
	}
};

DEFINE_MAIN(TexturedCubeApplication)
