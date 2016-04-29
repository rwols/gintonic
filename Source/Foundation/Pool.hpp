
#pragma once

#include <vector>

namespace gintonic {

/**
 * The idea of the Pool is:
 * we store "dead" objects at the end of the vector, and we
 * keep track of the point where the "alive" objects end and
 * the "dead" objects starts. The vector possibly never shrinks.
 * This allows fast deletion and insertion of objects.
 * 
 * vector size = 5
 * mNumAlive = 3
 * [ alive alive alive | dead dead ]
 * 
 */

class Pool
{
public:

	class Object
	{
	public:

		virtual ~Object()
		{
			try
			{
				onExpired();
			}
			catch (...)
			{

			}
		}

		/**
		 * @brief Return true if the Object has expired. Return false
		 * if the Object has not expired.
		 * @param deltaTime The delta time.
		 * @return True if the Object has expired and is ready for deletion.
		 * False if the Object has not expired.
		 */
		inline virtual bool update(const float deltaTime) { return false; }

		boost::signals2::signal<void()> onExpired;
	};

	Pool() = default;
	~Pool()
	{
		assert(mExpired.size() + mNotExpired.size() == mObjects.size());
		for (auto lObject : mObjects) if (lObject) delete lObject;
	}

	void insert(Object* object)
	{
		if (mExpired.empty())
		{
			mNotExpired.insert(mObjects.size());
			mObjects.push_back(object);
		}
		else
		{
			const auto lIndex = *(mExpired.begin());
			mObjects[lIndex] = object;
			mExpired.erase(mExpired.begin());
			mNotExpired.insert(lIndex);
		}
		assert(mExpired.size() + mNotExpired.size() == mObjects.size());
	}

	inline std::size_t size() const noexcept
	{
		assert(mExpired.size() + mNotExpired.size() == mObjects.size());
		return mObjects.size() - mExpired.size();
	}

	void update(const float deltaTime)
	{
		for (auto lIter = mNotExpired.begin(); lIter != mNotExpired.end(); ++lIter)
		{
			if (mObjects[*lIter]->update(deltaTime))
			{
				delete mObjects[*lIter];
				mExpired.insert(*lIter);
				lIter = mNotExpired.erase(lIter);
			}
		}
		assert(mExpired.size() + mNotExpired.size() == mObjects.size());
	}

	void shrink()
	{
		for (auto lIter = mExpired.begin(); lIter != mExpired.end(); ++lIter)
		{
			mObjects.erase(mObjects.begin() + *lIter);
		}
	}

private:

	std::vector<Object*> mObjects;
	std::set<std::size_t> mExpired;
	std::set<std::size_t> mNotExpired;
}

} // namespace gintonic