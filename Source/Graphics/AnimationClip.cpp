#include "AnimationClip.hpp"

#include "Skeleton.hpp"

#include <fbxsdk.h>

#include <algorithm>

namespace {

template <class FbxType>
class FbxInConstCollection
{
public:
	FbxInConstCollection()                                         = delete;
	FbxInConstCollection(const FbxInConstCollection&)              = default;
	FbxInConstCollection(FbxInConstCollection&&)                   = default;
	FbxInConstCollection& operator = (const FbxInConstCollection&) = default;
	FbxInConstCollection& operator = (FbxInConstCollection&&)      = default;
	~FbxInConstCollection() noexcept                               = default;

	FbxInConstCollection(const FbxCollection* collection) : mCollection(collection)
	{
		if (collection == nullptr) throw std::invalid_argument("Collection is a nullptr");
	}

	class const_iterator : public std::iterator<std::random_access_iterator_tag, const FbxType*, int, const FbxType*, void>
	{
	public:

		using Super             = std::iterator<std::random_access_iterator_tag, const FbxType*, int, const FbxType*, void>;
		using iterator_category = typename Super::iterator_category;
		using value_type        = typename Super::value_type;
		using difference_type   = typename Super::difference_type;
		using pointer           = typename Super::pointer;
		using reference         = typename Super::reference;

		const_iterator() = delete;

		inline const FbxType* operator  * () const noexcept { return mCollection->GetMember<FbxType>(mCurrent); }
		inline const FbxType* operator -> () const noexcept { return mCollection->GetMember<FbxType>(mCurrent); }

		inline const_iterator& operator ++ (   )       noexcept { ++mCurrent; return *this; }
		inline const_iterator  operator ++ (int) const noexcept { return const_iterator(mCollection, mCurrent + 1); }
		inline const_iterator& operator -- (   )       noexcept { --mCurrent; return *this; }
		inline const_iterator  operator -- (int) const noexcept { return const_iterator(mCollection, mCurrent - 1); }

		inline const_iterator& operator += (const difference_type d)       noexcept { mCurrent += d; return *this; }
		inline const_iterator  operator +  (const difference_type d) const noexcept { return const_iterator(mCollection, mCurrent + d); }
		inline const_iterator& operator -= (const difference_type d)       noexcept { mCurrent -= d; return *this; }
		inline const_iterator  operator -  (const difference_type d) const noexcept { return const_iterator(mCollection, mCurrent - d); }

		inline difference_type operator - (const const_iterator& other) const noexcept { return mCurrent - other.mCurrent; }

		inline value_type operator [] (const const_iterator& other) const noexcept { return *(*this + other.mCurrent); }

		inline bool operator == (const const_iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent == other.mCurrent; }
		inline bool operator != (const const_iterator& other) const noexcept { return mCollection != other.mCollection || mCurrent != other.mCurrent; }
		inline bool operator  < (const const_iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent  < other.mCurrent; }
		inline bool operator  > (const const_iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent  > other.mCurrent; }
		inline bool operator <= (const const_iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent <= other.mCurrent; }
		inline bool operator >= (const const_iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent >= other.mCurrent; }

		inline operator difference_type() const noexcept { return mCurrent; }

	private:
		friend class FbxInConstCollection;
		const_iterator(const FbxCollection* collection, const difference_type current) : mCollection(collection), mCurrent(current) {}
		const FbxCollection* mCollection;
		difference_type mCurrent;
	};

	const_iterator begin() const noexcept { return const_iterator(mCollection, 0); }
	const_iterator end()   const noexcept { return const_iterator(mCollection, mCollection->GetMemberCount<FbxType>()); }

private:
	const FbxCollection* mCollection;
};

template <class FbxType>
class FbxInCollection
{
public:
	FbxInCollection()                                    = delete;
	FbxInCollection(const FbxInCollection&)              = default;
	FbxInCollection(FbxInCollection&&)                   = default;
	FbxInCollection& operator = (const FbxInCollection&) = default;
	FbxInCollection& operator = (FbxInCollection&&)      = default;
	~FbxInCollection() noexcept                          = default;

	FbxInCollection(FbxCollection* collection) : mCollection(collection)
	{
		if (collection == nullptr) throw std::invalid_argument("Collection is a nullptr");
	}

	class iterator : public std::iterator<std::random_access_iterator_tag, FbxType*, int, FbxType*, void>
	{
	public:

		using Super             = std::iterator<std::random_access_iterator_tag, FbxType*, int, FbxType*, void>;
		using iterator_category = typename Super::iterator_category;
		using value_type        = typename Super::value_type;
		using difference_type   = typename Super::difference_type;
		using pointer           = typename Super::pointer;
		using reference         = typename Super::reference;

		iterator() = delete;

		inline value_type operator  * () const noexcept { return mCollection->GetMember<FbxType>(mCurrent); }
		inline value_type operator -> () const noexcept { return mCollection->GetMember<FbxType>(mCurrent); }

		inline iterator& operator ++ (   )       noexcept { ++mCurrent; return *this; }
		inline iterator  operator ++ (int) const noexcept { return iterator(mCollection, mCurrent + 1); }
		inline iterator& operator -- (   )       noexcept { --mCurrent; return *this; }
		inline iterator  operator -- (int) const noexcept { return iterator(mCollection, mCurrent - 1); }

		inline iterator& operator += (const difference_type d)       noexcept { mCurrent += d; return *this; }
		inline iterator  operator +  (const difference_type d) const noexcept { return iterator(mCollection, mCurrent + d); }
		inline iterator& operator -= (const difference_type d)       noexcept { mCurrent -= d; return *this; }
		inline iterator  operator -  (const difference_type d) const noexcept { return iterator(mCollection, mCurrent - d); }

		inline difference_type operator - (const iterator& other) const noexcept { return mCurrent - other.mCurrent; }

		inline value_type operator [] (const iterator& other) const noexcept { return *(*this + other.mCurrent); }

		inline bool operator == (const iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent == other.mCurrent; }
		inline bool operator != (const iterator& other) const noexcept { return mCollection != other.mCollection || mCurrent != other.mCurrent; }
		inline bool operator  < (const iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent  < other.mCurrent; }
		inline bool operator  > (const iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent  > other.mCurrent; }
		inline bool operator <= (const iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent <= other.mCurrent; }
		inline bool operator >= (const iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent >= other.mCurrent; }

		inline operator difference_type() const noexcept { return mCurrent; }

	private:
		friend class FbxInCollection;
		iterator(FbxCollection* collection, const difference_type current) : mCollection(collection), mCurrent(current) {}
		FbxCollection* mCollection;
		difference_type mCurrent;
	};

	iterator begin() noexcept { return iterator(mCollection, 0); }
	iterator end()   noexcept { return iterator(mCollection, mCollection->GetMemberCount<FbxType>()); }

private:
	FbxCollection* mCollection;
};


}

namespace gintonic {

AnimationClip::AnimationClip(FbxAnimStack* pStack, std::shared_ptr<Skeleton> targetSkeleton)
: name(pStack->GetName())
, skeleton(targetSkeleton)
{
	std::cerr << "\nProcessing AnimStack: " << name << "\n";

	if (targetSkeleton == nullptr) return;
	if (pStack->GetMemberCount<FbxAnimLayer>() == 0) return;

	FbxTime lFrameRate;
	lFrameRate.SetFrame(1, FbxTime::eFrames30);

	const auto lStart = pStack->LocalStart.Get();
	const auto lStop  = pStack->LocalStop.Get();
	const auto lScene = pStack->GetScene();
	auto lEvaluator   = lScene->GetAnimationEvaluator();

	if (pStack->GetMemberCount<FbxAnimLayer>() > 1)
	{
		if (!pStack->BakeLayers(lEvaluator, lStart, lStop, lFrameRate))
		{
			throw BakeLayersException(pStack);
		}	
	}
	

	// auto lAnimLayer = pStack->GetMember<FbxAnimLayer>(0);
	const auto& lSkeleton = *skeleton;
	for (uint8_t j = 0; j < targetSkeleton->joints.size(); ++j)
	{
		auto lFbxJoint = lScene->FindNodeByName(lSkeleton[j].name.c_str());
		if (!lFbxJoint)
		{
			std::cerr << "WARNING: Could not find joint \"" << lSkeleton[j].name << "\".\n";
			continue;
		}
		frames.emplace_back(std::vector<SQT, allocator<SQT>>());
		for (FbxTime t = lStart; t != lStop; t += lFrameRate)
		{
			const auto& lAffineMatrix = lEvaluator->GetNodeLocalTransform(lFbxJoint, t);
			frames.back().emplace_back(SQT(lAffineMatrix));
		}
	}
}

mat4f AnimationClip::evaluate(const uint8_t jointIndex, const float startTime, const float currentTime) const noexcept
{
	float lExactFrame;
	std::size_t lLowerFrame;
	std::size_t lUpperFrame;

	#define FRAMECOUNTi frames[jointIndex].size()
	#define FRAMECOUNTf static_cast<float>(FRAMECOUNTi)
	
	if (isLooping)
	{
		lExactFrame = std::fmod(framesPerSecond * (currentTime - startTime), FRAMECOUNTf);
		lLowerFrame = static_cast<std::size_t>(lExactFrame);
		lUpperFrame = (lLowerFrame + 1) % FRAMECOUNTi;
	}
	else
	{
		lExactFrame = std::min(framesPerSecond * (currentTime - startTime), FRAMECOUNTf - 1.0f);
		lLowerFrame = static_cast<std::size_t>(lExactFrame);
		lUpperFrame = std::min(lLowerFrame + 1, static_cast<std::size_t>(FRAMECOUNTi - 1));
	}

	mat4f lResult(1.0f);
	const float lLambda = lExactFrame - static_cast<float>(lLowerFrame);
	uint8_t j = jointIndex;
	while (j != GT_JOINT_NONE)
	{
		const auto lCurrent = mat4f(mix(frames[j][lLowerFrame], frames[j][lUpperFrame], lLambda));
		lResult = lCurrent * lResult;
		j = skeleton->joints[j].parent;
	}
	return skeleton->joints[jointIndex].inverseBindPose * lResult;
}

} // namespace gintonic