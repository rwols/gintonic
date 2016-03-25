/**
 * @file MatrixPipeline.hpp
 * @author Raoul Wols
 * @brief Defines the MatrixPipeline class.
 */

#pragma once

#include "mat4f.hpp"

namespace gintonic {

/**
 * @brief Easy manipulation of projection, view and model matrices.
 */
class MatrixPipeline
{
public:

	/**
	 * @brief Set the model matrix.
	 * @param args The constructor arguments for the model matrix.
	 * @tparam MatrixConstructorArguments Template pack.
	 */
	template <class ...MatrixConstructorArguments>
	void setModelMatrix(MatrixConstructorArguments&&... args)
	{
		mModelMatrix = mat4f(std::forward<MatrixConstructorArguments>(args)...);
		mViewModelMatrixIsDirty = true;
		mProjectionViewModelMatrixIsDirty = true;
		mNormalMatrixIsDirty = true;
	}

	/**
	 * @brief Set the view matrix.
	 * @param args The constructor arguments for the view matrix.
	 * @tparam MatrixConstructorArguments Template pack.
	 */
	template <class ...MatrixConstructorArguments>
	void setViewMatrix(MatrixConstructorArguments&&... args)
	{
		mViewMatrix = mat4f(std::forward<MatrixConstructorArguments>(args)...);
		mViewModelMatrixIsDirty = true;
		mProjectionViewModelMatrixIsDirty = true;
		mProjectionViewMatrixIsDirty = true;
		mNormalMatrixIsDirty = true;
	}

	/**
	 * @brief Set the projection matrix.
	 * @param args The constructor arguments for the projection matrix.
	 * @tparam MatrixConstructorArguments Template pack.
	 */
	template <class ...MatrixConstructorArguments>
	void setProjectionMatrix(MatrixConstructorArguments&&... args)
	{
		mProjectionMatrix = mat4f(std::forward<MatrixConstructorArguments>(args)...);
		mProjectionViewModelMatrixIsDirty = true;
		mProjectionViewMatrixIsDirty = true;
	}

	/**
	 * @brief Set the view and model matrix.
	 * @param viewMatrix The new view matrix.
	 * @param modelMatrix the new model matrix.
	 * @tparam Arg0 Universal reference.
	 * @tparam Arg1 Universal reference.
	 */
	template <class Arg0, class Arg1>
	void setViewModelMatrices(Arg0&& viewMatrix, Arg1&& modelMatrix)
	{
		mViewMatrix = std::forward<Arg0>(viewMatrix);
		mModelMatrix = std::forward<Arg1>(modelMatrix);
		mViewModelMatrixIsDirty = true;
		mProjectionViewModelMatrixIsDirty = true;
		mProjectionViewMatrixIsDirty = true;
		mNormalMatrixIsDirty = true;
	}

	/**
	 * @brief Set the projection, view and model matrix.
	 * @param projectionMatrix The new projection matrix.
	 * @param viewMatrix The new view matrix.
	 * @param modelMatrix the new model matrix.
	 * @tparam Arg0 Universal reference.
	 * @tparam Arg1 Universal reference.
	 * @tparam Arg2 Universal reference.
	 */
	template <class Arg0, class Arg1, class Arg2>
	void setProjectionViewModelMatrices(Arg0&& projectionMatrix, Arg1&& viewMatrix, Arg2&& modelMatrix)
	{
		mProjectionMatrix = std::forward<Arg0>(projectionMatrix);
		mViewMatrix = std::forward<Arg1>(viewMatrix);
		mModelMatrix = std::forward<Arg2>(modelMatrix);
		mViewModelMatrixIsDirty = true;
		mProjectionViewModelMatrixIsDirty = true;
		mProjectionViewMatrixIsDirty = true;
		mNormalMatrixIsDirty = true;
	}

	/**
	 * @brief Get the projection matrix.
	 * @return The projection matrix.
	 */
	inline const mat4f& getProjectionMatrix() const noexcept
	{
		return mProjectionMatrix;
	}

	/**
	 * @brief Get the view matrix.
	 * @return The view matrix.
	 */
	inline const mat4f& getViewMatrix() const noexcept
	{
		return mViewMatrix;
	}

	/**
	 * @brief Get the model matrix.
	 * @return The model matrix.
	 */
	inline const mat4f& getModelMatrix() const noexcept
	{
		return mModelMatrix;
	}

	/**
	 * @brief Get the product of the projection and view matrix.
	 * @return The product of the projection and view matrix.
	 */
	inline const mat4f& getProjectionViewMatrix() const noexcept
	{
		if (mProjectionViewMatrixIsDirty)
		{
			mProjectionViewMatrix = mProjectionMatrix * mViewMatrix;
			mProjectionViewMatrixIsDirty = false;
		}
		return mProjectionViewMatrix;
	}

	/**
	 * @brief Get the product of the view and model matrix.
	 * @return The product of the view and model matrix.
	 */
	inline const mat4f& getViewModelMatrix() const noexcept
	{
		if (mViewModelMatrixIsDirty)
		{
			mViewModelMatrix = mViewMatrix * mModelMatrix;
			mViewModelMatrixIsDirty = false;
		}
		return mViewModelMatrix;
	}

	/**
	 * @brief Get the product of the projection, view and model matrix.
	 * @return The product of the projection, view and model matrix.
	 */
	inline const mat4f& getProjectionViewModelMatrix() const noexcept
	{
		if (mProjectionViewModelMatrixIsDirty)
		{
			mProjectionViewModelMatrix = mProjectionMatrix * getViewModelMatrix();
			mProjectionViewModelMatrixIsDirty = false;
		}
		return mProjectionViewModelMatrix;
	}

	/**
	 * @brief Get the normal matrix.
	 * @return The normal matrix.
	 */
	inline const mat3f& getNormalMatrix() const noexcept
	{
		if (mNormalMatrixIsDirty)
		{
			mNormalMatrix = getViewModelMatrix().upper_left_33().invert().transpose();
			mNormalMatrixIsDirty = false;
		}
		return mNormalMatrix;
	}

private:
	        mat4f mProjectionMatrix;
	        mat4f mViewMatrix;
	        mat4f mModelMatrix;

	mutable mat4f mViewModelMatrix;
	mutable mat4f mProjectionViewModelMatrix;
	mutable mat4f mProjectionViewMatrix;
	mutable mat3f mNormalMatrix;

	mutable bool  mViewModelMatrixIsDirty = true;
	mutable bool  mProjectionViewModelMatrixIsDirty = true;
	mutable bool  mProjectionViewMatrixIsDirty = true;
	mutable bool  mNormalMatrixIsDirty = true;
};

} // namespace gintonic