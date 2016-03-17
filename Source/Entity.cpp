#include "Entity.hpp"

#include "Foundation/Octree.hpp"

#include "Math/vec4f.hpp"
#include "Math/SQTstack.hpp"
#include "Math/mat4fstack.hpp"

#include <fbxsdk.h>

namespace gintonic {

Entity::Entity()
: Entity(
	"UntitledEntity", 
	SQT(
		vec3f(1.0f, 1.0f, 1.0f), 
		quatf(1.0f, 0.0f, 0.0f, 0.0f), 
		vec3f(0.0f, 0.0f, 0.0f)
	)
)
{
	/* Empty on purpose. */
}

Entity::Entity(std::string name)
: Entity(
	std::move(name),
	SQT(
		vec3f(1.0f, 1.0f, 1.0f), 
		quatf(1.0f, 0.0f, 0.0f, 0.0f), 
		vec3f(0.0f, 0.0f, 0.0f)
	)
)
{
	/* Empty on purpose. */
}

Entity::Entity(std::string name, SQT localTransform)
: Object<Entity, std::string>(std::move(name))
, mLocalTransform(localTransform)
, mGlobalTransform(mLocalTransform)
{
	/* Empty on purpose. */
}

Entity::Entity(const FbxNode* pFbxNode)
: Object<Entity, std::string>(pFbxNode)
{
	// auto lGlobalName = boost::filesystem::path(pFbxNode->GetScene()->GetSceneInfo()->Url.Get().Buffer()).stem().string();
	// setName(std::move(lGlobalName), pFbxNode->GetName());

	auto lValue = pFbxNode->LclScaling.Get();
	mLocalTransform.scale.x = static_cast<float>(lValue[0]);
	mLocalTransform.scale.y = static_cast<float>(lValue[1]);
	mLocalTransform.scale.z = static_cast<float>(lValue[2]);

	lValue = pFbxNode->LclRotation.Get();
	mLocalTransform.rotation = quatf::yaw_pitch_roll(
		static_cast<float>(lValue[0]), 
		static_cast<float>(lValue[1]), 
		static_cast<float>(lValue[2]));

	lValue = pFbxNode->LclTranslation.Get();
	mLocalTransform.translation.x = static_cast<float>(lValue[0]);
	mLocalTransform.translation.y = static_cast<float>(lValue[1]);
	mLocalTransform.translation.z = static_cast<float>(lValue[2]);

	mGlobalTransform = mLocalTransform;
}


Entity::Entity(const Entity& other)
: mLocalTransform(other.mLocalTransform)
, mGlobalTransform(other.mGlobalTransform)
, mLocalBoundingBox(other.mLocalBoundingBox)
, mGlobalBoundingBox(other.mGlobalBoundingBox)
, mOctree(other.mOctree)
, castShadow(other.castShadow)
, material(other.material)
, mesh(other.mesh)
, light(other.light)
, camera(other.camera)
{
	/* Do NOT copy mChildren */
	/* Do NOT copy mParent */
	/* Do NOT copy shadowBuffer */
}

Entity::Entity(Entity&& other) noexcept
: mLocalTransform(std::move(other.mLocalTransform))
, mGlobalTransform(std::move(other.mGlobalTransform))
, mLocalBoundingBox(std::move(other.mLocalBoundingBox))
, mGlobalBoundingBox(std::move(other.mGlobalBoundingBox))
, mChildren(std::move(other.mChildren))
, mParent(std::move(other.mParent))
, mOctree(std::move(other.mOctree))
, castShadow(std::move(other.castShadow))
, material(std::move(other.material))
, mesh(std::move(other.mesh))
, light(std::move(other.light))
, camera(std::move(other.camera))
, shadowBuffer(std::move(other.shadowBuffer))
{
	/* DO move mChildren */
	/* DO move mParent */
	/* DO move shadowBuffer */
}

Entity& Entity::operator = (const Entity& other)
{
	mLocalTransform = other.mLocalTransform;
	mGlobalTransform = other.mGlobalTransform;
	mLocalBoundingBox = other.mLocalBoundingBox;
	mGlobalBoundingBox = other.mGlobalBoundingBox;
	mOctree = other.mOctree;
	castShadow = other.castShadow;
	material = other.material;
	mesh = other.mesh;
	light = other.light;
	camera = other.camera;

	/* Do NOT copy mChildren */
	/* Do NOT copy mParent */
	/* Do NOT copy shadowBuffer */

	return *this;
}

Entity& Entity::operator = (Entity&& other) noexcept
{
	mLocalTransform = std::move(other.mLocalTransform);
	mGlobalTransform = std::move(other.mGlobalTransform);
	mLocalBoundingBox = std::move(other.mLocalBoundingBox);
	mGlobalBoundingBox = std::move(other.mGlobalBoundingBox);
	mChildren = std::move(other.mChildren);
	mParent = std::move(other.mParent);
	mOctree = std::move(other.mOctree);
	castShadow = std::move(other.castShadow);
	material = std::move(other.material);
	mesh = std::move(other.mesh);
	light = std::move(other.light);
	camera = std::move(other.camera);
	shadowBuffer = std::move(other.shadowBuffer);

	/* DO move mChildren */
	/* DO move mParent */
	/* DO move shadowBuffer */

	return *this;
}

void Entity::updateGlobalInfo(mat4fstack& matrixStack) noexcept
{
	matrixStack.push(mLocalTransform);
	updateGlobalDatamembers(matrixStack);
	for (auto child : mChildren)
	{
		if (child) child->updateGlobalInfo(matrixStack);
	}
	matrixStack.pop();
}

void Entity::updateGlobalInfoStart() noexcept
{
	mat4fstack lMatrixStack(computeGlobalTransform());
	updateGlobalDatamembers(lMatrixStack);
	for (auto lChild : mChildren)
	{
		if (lChild) lChild->updateGlobalInfo(lMatrixStack);
	}
}

void Entity::updateGlobalDatamembers(const mat4fstack& matrixStack) noexcept
{
	mGlobalTransform = matrixStack.top();
	const vec3f t(mGlobalTransform.data[3]);
	mGlobalBoundingBox.min_corner = t + mLocalBoundingBox.min_corner;
	mGlobalBoundingBox.max_corner = t + mLocalBoundingBox.max_corner;
	if (mOctree) mOctree->notify(this);
}

void Entity::setScale(const vec3f& scale) noexcept
{
	mLocalTransform.scale = scale;
	updateGlobalInfoStart();
	onTransformChange(shared_from_this());
}

void Entity::multiplyScale(const vec3f& scale) noexcept
{
	mLocalTransform.scale *= scale;
	updateGlobalInfoStart();
	onTransformChange(shared_from_this());
}

void Entity::setTranslation(const vec3f& translation) noexcept
{
	mLocalTransform.translation = translation;
	updateGlobalInfoStart();
	onTransformChange(shared_from_this());
}

void Entity::setTranslationX(const float x) noexcept
{
	mLocalTransform.translation.x = x;
	updateGlobalInfoStart();
	onTransformChange(shared_from_this());
}
void Entity::setTranslationY(const float y) noexcept
{
	mLocalTransform.translation.y = y;
	updateGlobalInfoStart();
	onTransformChange(shared_from_this());
}

void Entity::setTranslationZ(const float z) noexcept
{
	mLocalTransform.translation.z = z;
	updateGlobalInfoStart();
	onTransformChange(shared_from_this());
}

void Entity::addTranslation(const vec3f& translation) noexcept
{
	mLocalTransform.translation += translation;
	updateGlobalInfoStart();
	onTransformChange(shared_from_this());
}

void Entity::setRotation(const quatf& rotation) noexcept
{
	mLocalTransform.rotation = rotation;
	updateGlobalInfoStart();
	onTransformChange(shared_from_this());
}

void Entity::postMultiplyRotation(const quatf& rotation) noexcept
{
	mLocalTransform.rotation *= rotation;
	updateGlobalInfoStart();
	onTransformChange(shared_from_this());
}

void Entity::preMultiplyRotation(const quatf& rotation) noexcept
{
	mLocalTransform.rotation = rotation * mLocalTransform.rotation;
	updateGlobalInfoStart();
	onTransformChange(shared_from_this());
}

void Entity::setLocalTransform(const SQT& sqt) noexcept
{
	mLocalTransform = sqt;
	updateGlobalInfoStart();
	onTransformChange(shared_from_this());
}

void Entity::postAddLocalTransform(const SQT& sqt) noexcept
{
	mLocalTransform %= sqt;
	updateGlobalInfoStart();
	onTransformChange(shared_from_this());
}

void Entity::preAddLocalTransform(const SQT& sqt) noexcept
{
	mLocalTransform = sqt % mLocalTransform;
	updateGlobalInfoStart();
	onTransformChange(shared_from_this());
}

void Entity::moveForward(const float amount) noexcept
{
	mLocalTransform.translation += amount * mLocalTransform.rotation.forward_direction();
	updateGlobalInfoStart();
	onTransformChange(shared_from_this());
}

void Entity::moveBackward(const float amount) noexcept
{
	mLocalTransform.translation -= amount * mLocalTransform.rotation.forward_direction();
	updateGlobalInfoStart();
	onTransformChange(shared_from_this());
}

void Entity::moveRight(const float amount) noexcept
{
	mLocalTransform.translation += amount * mLocalTransform.rotation.right_direction();
	updateGlobalInfoStart();
	onTransformChange(shared_from_this());
}

void Entity::moveLeft(const float amount) noexcept
{
	mLocalTransform.translation -= amount * mLocalTransform.rotation.right_direction();
	updateGlobalInfoStart();
	onTransformChange(shared_from_this());
}

void Entity::moveUp(const float amount) noexcept
{
	mLocalTransform.translation += amount * mLocalTransform.rotation.up_direction();
	updateGlobalInfoStart();
	onTransformChange(shared_from_this());
}

void Entity::moveDown(const float amount) noexcept
{
	mLocalTransform.translation -= amount * mLocalTransform.rotation.up_direction();
	updateGlobalInfoStart();
	onTransformChange(shared_from_this());
}

mat4f Entity::computeGlobalTransform() noexcept
{
	if (auto lParent = mParent.lock())
	{
		return lParent->globalTransform() * mat4f(mLocalTransform);
	}
	else
	{
		return mat4f(mLocalTransform);
	}
}

void Entity::addChild(std::shared_ptr<Entity> child)
{
	if (child->mParent.lock())
	{
		// child has a parent. Do we have to move or clone it?
		if (child.use_count() == 1)
		{
			// We can move it!
			child->mParent = shared_from_this();
			mChildren.push_front(child);
			child->updateGlobalInfoStart();
			child->onTransformChange(child);
		}
		else
		{
			// We must make a recursive copy.
			auto lCopy = child->cloneRecursive();
			lCopy->mParent = shared_from_this();
			mChildren.push_front(lCopy);
			lCopy->updateGlobalInfoStart();
			lCopy->onTransformChange(lCopy);			
		}
	}
	else
	{
		child->mParent = shared_from_this();
		mChildren.push_front(child);
		child->updateGlobalInfoStart();
		child->onTransformChange(child);
	}
}

void Entity::removeChild(std::shared_ptr<Entity> child)
{
	#ifndef NDEBUG
	bool lChildWasRemoved = false;
	#endif

	for (auto i = begin(); i != end(); ++i)
	{
		if (i->get() == child.get())
		{
			mChildren.erase(i);

			#ifndef NDEBUG
			lChildWasRemoved = true;
			#endif

			break;
		}
	}

	#ifndef NDEBUG
	if (lChildWasRemoved) child->mParent = std::shared_ptr<Entity>(nullptr);
	else throw std::logic_error("Entity was not a child.");
	#else
	child->mParent = std::shared_ptr<Entity>(nullptr);
	#endif
}

void Entity::setParent(std::shared_ptr<Entity> parent)
{
	if (auto lParent = mParent.lock())
	{
		lParent->removeChild(shared_from_this());
	}
	parent->addChild(shared_from_this()); // also sets mParent
}

void Entity::unsetParent()
{
	// Also sets mParent to nullptr
	if (auto lParent = mParent.lock()) lParent->removeChild(shared_from_this());
}

void Entity::getViewMatrix(mat4f& m) const noexcept
{
	const auto f = globalTransform() * vec4f(0.0f, 0.0f, -1.0f, 0.0f);
	const auto u = globalTransform() * vec4f(0.0f, 1.0f, 0.0f, 0.0f);
	const auto r = globalTransform() * vec4f(1.0f, 0.0f, 0.0f, 0.0f);
	const auto eye = globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f);

	m.m00 = r.x;
	m.m10 = u.x;
	m.m20 = -f.x;
	m.m30 = 0.0f;

	m.m01 = r.y;
	m.m11 = u.y;
	m.m21 = -f.y;
	m.m31 = 0.0f;

	m.m02 = r.z;
	m.m12 = u.z;
	m.m22 = -f.z;
	m.m32 = 0.0f;

	m.m03 = -dot(r, eye);
	m.m13 = -dot(u, eye);
	m.m23 =  dot(f, eye);
	m.m33 = 1.0f;
}

Entity::~Entity()
{
	onDie(*this);
	if (mOctree) mOctree->erase(this);
	unsetParent();
}

std::shared_ptr<Entity> Entity::cloneRecursive() const
{
	// Invoke the copy constructor.
	auto lClone = std::make_shared<Entity>(*this);
	for (auto lChild : mChildren)
	{
		lClone->addChild(lChild->cloneRecursive());
	}
	return lClone;
}

} // namespace gintonic
