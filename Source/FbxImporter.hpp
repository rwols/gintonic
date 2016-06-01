#pragma once

#include "ForwardDeclarations.hpp"

#include <memory>
#include <vector>

// #include <fbxsdk/scene/fbxcollection.h>

namespace gintonic {

// template <class FbxType>
// class FbxInConstCollection
// {
// public:
// 	FbxInConstCollection()                                         = delete;
// 	FbxInConstCollection(const FbxInConstCollection&)              = default;
// 	FbxInConstCollection(FbxInConstCollection&&)                   = default;
// 	FbxInConstCollection& operator = (const FbxInConstCollection&) = default;
// 	FbxInConstCollection& operator = (FbxInConstCollection&&)      = default;
// 	~FbxInConstCollection() noexcept                               = default;

// 	FbxInConstCollection(const FBXSDK_NAMESPACE::FbxCollection* collection) : mCollection(collection)
// 	{
// 		if (collection == nullptr) throw std::invalid_argument("Collection is a nullptr");
// 	}

// 	class const_iterator : public std::iterator<std::random_access_iterator_tag, const FbxType*, int, const FbxType*, void>
// 	{
// 	public:

// 		using Super             = std::iterator<std::random_access_iterator_tag, const FbxType*, int, const FbxType*, void>;
// 		using iterator_category = typename Super::iterator_category;
// 		using value_type        = typename Super::value_type;
// 		using difference_type   = typename Super::difference_type;
// 		using pointer           = typename Super::pointer;
// 		using reference         = typename Super::reference;

// 		const_iterator() = delete;

// 		inline const FbxType* operator  * () const noexcept { return mCollection->GetMember<FbxType>(mCurrent); }
// 		inline const FbxType* operator -> () const noexcept { return mCollection->GetMember<FbxType>(mCurrent); }

// 		inline const_iterator& operator ++ (   )       noexcept { ++mCurrent; return *this; }
// 		inline const_iterator  operator ++ (int) const noexcept { return const_iterator(mCollection, mCurrent + 1); }
// 		inline const_iterator& operator -- (   )       noexcept { --mCurrent; return *this; }
// 		inline const_iterator  operator -- (int) const noexcept { return const_iterator(mCollection, mCurrent - 1); }

// 		inline const_iterator& operator += (const difference_type d)       noexcept { mCurrent += d; return *this; }
// 		inline const_iterator  operator +  (const difference_type d) const noexcept { return const_iterator(mCollection, mCurrent + d); }
// 		inline const_iterator& operator -= (const difference_type d)       noexcept { mCurrent -= d; return *this; }
// 		inline const_iterator  operator -  (const difference_type d) const noexcept { return const_iterator(mCollection, mCurrent - d); }

// 		inline difference_type operator - (const const_iterator& other) const noexcept { return mCurrent - other.mCurrent; }

// 		inline value_type operator [] (const const_iterator& other) const noexcept { return *(*this + other.mCurrent); }

// 		inline bool operator == (const const_iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent == other.mCurrent; }
// 		inline bool operator != (const const_iterator& other) const noexcept { return mCollection != other.mCollection || mCurrent != other.mCurrent; }
// 		inline bool operator  < (const const_iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent  < other.mCurrent; }
// 		inline bool operator  > (const const_iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent  > other.mCurrent; }
// 		inline bool operator <= (const const_iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent <= other.mCurrent; }
// 		inline bool operator >= (const const_iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent >= other.mCurrent; }

// 		inline operator difference_type() const noexcept { return mCurrent; }

// 	private:
// 		friend class FbxInConstCollection;
// 		const_iterator(const FBXSDK_NAMESPACE::FbxCollection* collection, const difference_type current) : mCollection(collection), mCurrent(current) {}
// 		const FBXSDK_NAMESPACE::FbxCollection* mCollection;
// 		difference_type mCurrent;
// 	};

// 	const_iterator begin() const noexcept { return const_iterator(mCollection, 0); }
// 	const_iterator end()   const noexcept { return const_iterator(mCollection, mCollection->GetMemberCount<FbxType>()); }

// private:
// 	const FBXSDK_NAMESPACE::FbxCollection* mCollection;
// };

// template <class FbxType>
// class FbxInCollection
// {
// public:
// 	FbxInCollection()                                    = delete;
// 	FbxInCollection(const FbxInCollection&)              = default;
// 	FbxInCollection(FbxInCollection&&)                   = default;
// 	FbxInCollection& operator = (const FbxInCollection&) = default;
// 	FbxInCollection& operator = (FbxInCollection&&)      = default;
// 	~FbxInCollection() noexcept                          = default;

// 	FbxInCollection(FBXSDK_NAMESPACE::FbxCollection* collection) : mCollection(collection)
// 	{
// 		if (collection == nullptr) throw std::invalid_argument("Collection is a nullptr");
// 	}

// 	class iterator : public std::iterator<std::random_access_iterator_tag, FbxType*, int, FbxType*, void>
// 	{
// 	public:

// 		using Super             = std::iterator<std::random_access_iterator_tag, FbxType*, int, FbxType*, void>;
// 		using iterator_category = typename Super::iterator_category;
// 		using value_type        = typename Super::value_type;
// 		using difference_type   = typename Super::difference_type;
// 		using pointer           = typename Super::pointer;
// 		using reference         = typename Super::reference;

// 		iterator() = delete;

// 		inline value_type operator  * () const noexcept { return mCollection->GetMember<FbxType>(mCurrent); }
// 		inline value_type operator -> () const noexcept { return mCollection->GetMember<FbxType>(mCurrent); }

// 		inline iterator& operator ++ (   )       noexcept { ++mCurrent; return *this; }
// 		inline iterator  operator ++ (int) const noexcept { return iterator(mCollection, mCurrent + 1); }
// 		inline iterator& operator -- (   )       noexcept { --mCurrent; return *this; }
// 		inline iterator  operator -- (int) const noexcept { return iterator(mCollection, mCurrent - 1); }

// 		inline iterator& operator += (const difference_type d)       noexcept { mCurrent += d; return *this; }
// 		inline iterator  operator +  (const difference_type d) const noexcept { return iterator(mCollection, mCurrent + d); }
// 		inline iterator& operator -= (const difference_type d)       noexcept { mCurrent -= d; return *this; }
// 		inline iterator  operator -  (const difference_type d) const noexcept { return iterator(mCollection, mCurrent - d); }

// 		inline difference_type operator - (const iterator& other) const noexcept { return mCurrent - other.mCurrent; }

// 		inline value_type operator [] (const iterator& other) const noexcept { return *(*this + other.mCurrent); }

// 		inline bool operator == (const iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent == other.mCurrent; }
// 		inline bool operator != (const iterator& other) const noexcept { return mCollection != other.mCollection || mCurrent != other.mCurrent; }
// 		inline bool operator  < (const iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent  < other.mCurrent; }
// 		inline bool operator  > (const iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent  > other.mCurrent; }
// 		inline bool operator <= (const iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent <= other.mCurrent; }
// 		inline bool operator >= (const iterator& other) const noexcept { return mCollection == other.mCollection && mCurrent >= other.mCurrent; }

// 		inline operator difference_type() const noexcept { return mCurrent; }

// 	private:
// 		friend class FbxInCollection;
// 		iterator(FBXSDK_NAMESPACE::FbxCollection* collection, const difference_type current) : mCollection(collection), mCurrent(current) {}
// 		FBXSDK_NAMESPACE::FbxCollection* mCollection;
// 		difference_type mCurrent;
// 	};

// 	iterator begin() noexcept { return iterator(mCollection, 0); }
// 	iterator end()   noexcept { return iterator(mCollection, mCollection->GetMemberCount<FbxType>()); }

// private:
// 	FBXSDK_NAMESPACE::FbxCollection* mCollection;
// };

class FbxImporter
{
public:

	FbxImporter(
		const char* filename, 
		bool normalizeUnits = true, 
		bool setAxisSystemToOpenGL = true,
		bool triangulateAllMeshes = true);

	~FbxImporter() noexcept;

	std::shared_ptr<Entity> loadEntities(
		bool setRootEntityNameAsFilename = true,
		bool setSkeletonNameAsFilename = true,
		bool cleanUpEmptyNodes = true);

	std::shared_ptr<Entity> loadFromFile(const char* filename, bool setRootEntityNameAsFilename = true);

private:

	struct ResultStructure
	{
		std::vector<std::shared_ptr<Material>> materials;
		std::vector<std::shared_ptr<Mesh>> meshes;
		std::vector<std::shared_ptr<Light>> lights;
		std::vector<std::shared_ptr<Camera>> cameras;
		std::vector<std::shared_ptr<AnimationClip>> animations;
		std::shared_ptr<Skeleton> skeleton;
	};

	FBXSDK_NAMESPACE::FbxManager* mManager;
	FBXSDK_NAMESPACE::FbxScene* mScene = nullptr;
	FBXSDK_NAMESPACE::FbxImporter* mImporter;
	std::string mFilename;

	std::shared_ptr<Skeleton> loadSkeleton();
	void processAnimations(ResultStructure&);
	std::shared_ptr<Entity> traverse(FBXSDK_NAMESPACE::FbxNode*, ResultStructure&);
	std::shared_ptr<Material> processMaterial(FBXSDK_NAMESPACE::FbxNode*, ResultStructure&);
	std::shared_ptr<Mesh> processMesh(FBXSDK_NAMESPACE::FbxNode*, ResultStructure&);
	std::shared_ptr<Camera> processCamera(FBXSDK_NAMESPACE::FbxNode*, ResultStructure&);
	std::shared_ptr<Light> processLight(FBXSDK_NAMESPACE::FbxNode*, ResultStructure&);
};

} // namespace gintonic
