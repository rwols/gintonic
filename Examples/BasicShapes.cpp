#include "Common/Application.hpp"

#define APPNAME "BasicShapes"

class BasicShapesApplication : public Application
{
public:

	std::shared_ptr<gintonic::Entity> mRootOfSolidShapes;

	BasicShapesApplication(int argc, char** argv)
	: Application(APPNAME, argc, argv)
	{
		using namespace gintonic;

		// Put a directional light in the scene
		// so that we see something interesting.
		// The directional light shines downwards.
		auto lLight = DirectionalLight::create();
		lLight->setIntensity(vec4f(1.0f, 1.0f, 1.0f, 1.0f));
		lLight->name = "DefaultDirectionalLight";
		auto lLightEntity = gintonic::Entity::create
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

		auto lMaterial = Material::create();
		lMaterial->diffuseColor   = vec4f(1.0f, 1.0f, 1.0f, 0.9f);
		lMaterial->specularColor  = vec4f(0.0f, 0.0f, 0.0f, 0.0f);
		lMaterial->diffuseTexture = Texture2D::create("../../Examples/DaVinci.jpg");

		auto lGizmo = Renderer::createGizmo();

		auto lInsideOutCubeEntity = Entity::create("InsideOutCube");
		lInsideOutCubeEntity->mesh = Renderer::getInsideOutUnitCube();
		lInsideOutCubeEntity->material = lMaterial;
		lInsideOutCubeEntity->setScale(vec3f(20.0f, 20.0f, 20.0f));
		lInsideOutCubeEntity->setRotation(quatf(1.0f, 0.0f, 0.0f, 0.0f));
		lInsideOutCubeEntity->setTranslation(vec3f(0.0f, 0.0f, 0.0f));

		auto lCubeEntity = Entity::create("Cube");
		lCubeEntity->mesh = Renderer::getUnitCube();
		lCubeEntity->material = lMaterial;
		lCubeEntity->setScale(vec3f(1.0f, 1.0f, 1.0f));
		lCubeEntity->setRotation(quatf(1.0f, 0.0f, 0.0f, 0.0f));
		lCubeEntity->setTranslation(vec3f(-3.0f, 0.0f, 0.0f));
		lCubeEntity->addChild(lGizmo);

		auto lSphereEntity = Entity::create("Sphere");
		lSphereEntity->mesh = Renderer::getUnitSphere();
		lSphereEntity->material = lMaterial;
		lSphereEntity->setScale(vec3f(1.0f, 1.0f, 1.0f));
		lSphereEntity->setRotation(quatf(1.0f, 0.0f, 0.0f, 0.0f));
		lSphereEntity->setTranslation(vec3f(3.0f, 0.0f, 0.0f));
		lSphereEntity->addChild(lGizmo);

		auto lConeEntity = Entity::create("Cone");
		lConeEntity->mesh = Renderer::getUnitCone();
		lConeEntity->material = lMaterial;
		lConeEntity->setScale(vec3f(1.0f, 1.0f, 1.0f));
		lConeEntity->setRotation(quatf(1.0f, 0.0f, 0.0f, 0.0f));
		lConeEntity->setTranslation(vec3f(0.0f, 0.0f, -3.0f));
		lConeEntity->addChild(lGizmo);

		auto lCylinderEntity = Entity::create("Cylinder");
		lCylinderEntity->mesh = Renderer::getUnitCylinder();
		lCylinderEntity->material = lMaterial;
		lCylinderEntity->setScale(vec3f(1.0f, 1.0f, 1.0f));
		lCylinderEntity->setRotation(quatf(1.0f, 0.0f, 0.0f, 0.0f));
		lCylinderEntity->setTranslation(vec3f(0.0f, 0.0f, 3.0f));
		lCylinderEntity->addChild(lGizmo);

		mRootOfSolidShapes = Entity::create("RootOfSolidShapes");
		mRootOfSolidShapes->addChild(lCubeEntity);
		mRootOfSolidShapes->addChild(lSphereEntity);
		mRootOfSolidShapes->addChild(lConeEntity);
		mRootOfSolidShapes->addChild(lCylinderEntity);

		mRootEntity->addChild(mRootOfSolidShapes);
		mRootEntity->addChild(lLightEntity);
		mRootEntity->addChild(lInsideOutCubeEntity);

		Renderer::setFreeformCursor(true);
		Renderer::show();
	}

private:

	virtual void onRenderUpdate() final
	{
		using namespace gintonic;

		for (auto lShape : *mRootOfSolidShapes)
		{
			lShape->setRotation(quatf::axis_angle(vec3f(0.0f, 1.0f, 0.0f), mElapsedTime));
		}

		// All the simple solids are a child of the "invisible" root entity, centered
		// at the origin. So we apply a rotation to that too, in order to make the solids "spin around".
		mRootOfSolidShapes->setRotation(quatf::axis_angle(vec3f(0.0f, 1.0f, 0.0f), -mElapsedTime / 4.0f));
	}
};

DEFINE_MAIN(BasicShapesApplication)
