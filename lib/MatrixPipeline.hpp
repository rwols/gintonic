#pragma once

#include "mat4f.hpp"

namespace gintonic {

class MatrixPipeline
{
public:

	template <class ...MatrixConstructorArguments>
	void setModelMatrix(MatrixConstructorArguments&& args)
	{
		mModelMatrix = mat4f(std::forward<MatrixConstructorArguments>(args)...);
		mViewModelMatrixIsDirty = true;
		mProjectionViewModelMatrixIsDirty = true;
	}

	template <class ...MatrixConstructorArguments>
	void setViewMatrix(MatrixConstructorArguments&& args)
	{
		mViewMatrix = mat4f(std::forward<MatrixConstructorArguments>(args)...);
		mViewModelMatrixIsDirty = true;
		mProjectionViewModelMatrixIsDirty = true;
		mProjectionViewMatrixIsDirty = true;
	}

	template <class ...MatrixConstructorArguments>
	void setProjectionMatrix(MatrixConstructorArguments&& args)
	{
		mProjectionMatrix = mat4f(std::forward<MatrixConstructorArguments>(args)...);
		mProjectionViewModelMatrixIsDirty = true;
		mProjectionViewMatrixIsDirty = true;
	}

	template <class Arg0, class Arg1>
	void setViewModelMatrices(Arg0&& viewMatrix, Arg1&& modelMatrix)
	{
		mViewMatrix = std::forward<Arg0>(viewMatrix);
		mModelMatrix = std::forward<Arg1>(modelMatrix);
		mViewModelMatrixIsDirty = true;
		mProjectionViewModelMatrixIsDirty = true;
		mProjectionViewMatrixIsDirty = true;
	}

	template <class Arg0, class Arg1, class Arg2)
	void setProjectionViewModelMatrices(Arg0&& projectionMatrix, Arg1&& viewMatrix, Arg2&& modelMatrix)
	{
		mProjectionMatrix = std::forward<Arg0>(projectionMatrix);
		mViewMatrix = std:::forward<Arg1>(viewMatrix);
		mModelMatrix = std::forward<Arg2>(modelMatrix);
		mViewModelMatrixIsDirty = true;
		mProjectionViewModelMatrixIsDirty = true;
		mProjectionViewMatrixIsDirty = true;
	}

	inline const mat4f& getProjectionMatrix() const noexcept
	{
		return mProjectionMatrix;
	}

	inline const mat4f& getViewMatrix() const noexcept
	{
		return mViewMatrix;
	}

	inline const mat4f& getModelMatrix() const noexcept
	{
		return mModelMatrix;
	}

	inline const mat4f& getProjectionViewMatrix() const noexcept
	{
		if (mProjectionViewMatrixIsDirty)
		{
			mProjectionViewMatrix = mProjectionMatrix * mViewMatrix;
			mProjectionViewMatrixIsDirty = false;
		}
		return mProjectionViewMatrix;
	}

	inline const mat4f& getViewModelMatrix() const noexcept
	{
		if (mViewModelMatrixIsDirty)
		{
			mViewModelMatrix = mViewMatrix * mModelMatrix;
			mViewModelMatrixIsDirty = false;
		}
		return mViewModelMatrix;
	}

	inline const mat4f& getProjectionViewModelMatrix() const noexcept
	{
		if (mProjectionViewModelMatrixIsDirty)
		{
			mProjectionViewModelMatrix = mProjectionMatrix * getViewModelMatrix();
			mProjectionViewModelMatrixIsDirty = false;
		}
		return mProjectionViewModelMatrix;
	}

private:
	        mat4f mProjectionMatrix;
	        mat4f mViewMatrix;
	        mat4f mModelMatrix;

	mutable mat4f mViewModelMatrix;
	mutable mat4f mProjectionViewModelMatrix;
	mutable mat4f mProjectionViewMatrix;

	mutable bool  mViewModelMatrixIsDirty = true;
	mutable bool  mProjectionViewModelMatrixIsDirty = true;
	mutable bool  mProjectionViewMatrixIsDirty = true;
};

} // namespace gintonic