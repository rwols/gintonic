#pragma once

#include <array>
#include "utilities.hpp"
#include "../../Foundation/filesystem.hpp"

namespace gintonic {
namespace OpenGL {

class GenericShaderProgram
{
private:

	class ExceptionBase : public std::exception
	{
	public:

		ExceptionBase(
			const boost::filesystem::path& vertexshader,
			const boost::filesystem::path& fragmentshader,
			const char* whatMessagePrefix);
		
		ExceptionBase(
			const boost::filesystem::path& vertexshader,
			const boost::filesystem::path& geometryshader,
			const boost::filesystem::path& fragmentshader,
			const char* whatMessagePrefix);

		virtual ~ExceptionBase() noexcept = default;

		inline const boost::filesystem::path& getVertexShaderFilename() const noexcept
		{
			return mVertexShader;
		}

		inline const boost::filesystem::path& getGeometryShaderFilename() const noexcept
		{
			return mGeometryShader;
		}

		inline const boost::filesystem::path& getFragmentShaderFilename() const noexcept
		{
			return mFragmentShader;
		}

		inline virtual const char* what() const noexcept
		{
			return mWhatMessage.c_str();
		}

	protected:
		std::basic_string<GLchar> mWhatMessage;

	private:
		boost::filesystem::path mVertexShader;
		boost::filesystem::path mGeometryShader;
		boost::filesystem::path mFragmentShader;
	};

public:

	/**
	 * @brief Encapsulates a Uniform variable of a shader.
	 */
	struct Uniform
	{
		/// The name of the Uniform variable.
		std::basic_string<GLchar> name;

		/// The location of the Uniform variable in a shader.
		GLuint location;

		/// The size of the Uniform variable (in bytes).
		GLint size;

		/// The type of the Uniform variable.
		GLenum type;

		/// Compares the names lexicographically.
		inline bool operator <  (const Uniform& other) const 
			noexcept
		{
			return name < other.name;
		}
		
		/// Compares the names lexicographically.
		inline bool operator >= (const Uniform& other) const 
			noexcept
		{
			return !operator<(other); 
		}

		/// Compares the names lexicographically.
		inline bool operator >  (const Uniform& other) const 
			noexcept
		{
			return name > other.name;
		}

		/// Compares the names lexicographically.
		inline bool operator <= (const Uniform& other) const 
			noexcept
		{
			return !operator>(other);
		}
	};

	typedef std::vector<Uniform>::const_iterator const_iterator;

	class UniformNotFoundError : public ExceptionBase
	{
	public:

		UniformNotFoundError(
			const boost::filesystem::path& vertexshader,
			const boost::filesystem::path& fragmentshader,
			const char* Uniformname);
		
		UniformNotFoundError(
			const boost::filesystem::path& vertexshader,
			const boost::filesystem::path& geometryshader,
			const boost::filesystem::path& fragmentshader,
			const char* Uniformname);

		virtual ~UniformNotFoundError() noexcept = default;
	};

	class CompilationError : public ExceptionBase
	{
	public:
		
		CompilationError(
			const boost::filesystem::path& vertexshader,
			const boost::filesystem::path& fragmentshader,
			const std::basic_string<GLchar>& infolog);
		
		CompilationError(
			const boost::filesystem::path& vertexshader,
			const boost::filesystem::path& geometryshader,
			const boost::filesystem::path& fragmentshader,
			const std::basic_string<GLchar>& infolog);

		virtual ~CompilationError() noexcept = default;
	};

	GenericShaderProgram(
		boost::filesystem::path vertexshader,
		boost::filesystem::path fragmentshader);

	GenericShaderProgram(
		boost::filesystem::path vertexshader,
		boost::filesystem::path geometryshader,
		boost::filesystem::path fragmentshader);

	virtual ~GenericShaderProgram() noexcept;

	template <class Type> void set(const char* name, Type&& value) const
	{
		const auto r = std::find_if(begin(), end(), Finder(name));
		if (r == end())
		{
			throw UniformNotFoundError( 
				mVertexShader, 
				mGeometryShader, 
				mFragmentShader,
				name);
		}
		else set_uniform(r->location, std::forward<Type>(value));
	}

	inline const_iterator begin() const { return mUniforms.begin(); }
	inline const_iterator end() const { return mUniforms.end(); }
	inline const_iterator cbegin() const { return mUniforms.begin(); }
	inline const_iterator cend() const { return mUniforms.end(); }

	inline operator GLuint() const noexcept
	{
		return mHandle;
	}

	inline const boost::filesystem::path& getVertexShaderFilename() const noexcept
	{
		return mVertexShader;
	}

	const boost::filesystem::path& getGeometryShaderFilename() const noexcept
	{
		return mGeometryShader;
	}

	const boost::filesystem::path& getFragmentShaderFilename() const noexcept
	{
		return mFragmentShader;
	}

private:
	GLuint mHandle;
	std::vector<Uniform> mUniforms;
	boost::filesystem::path mVertexShader;
	boost::filesystem::path mGeometryShader;
	boost::filesystem::path mFragmentShader;

	struct Finder
	{
		Finder(const char* name) : m_name(name) {}
		inline bool operator()(const Uniform& u) const noexcept
		{
			if (u.name == m_name) return true;
			else return false;
		}
		const char* m_name;
	};
};

struct ShaderProgramBinder
{
	ShaderProgramBinder(const GenericShaderProgram& s)
	: mShaderProgram(s)
	{
		glUseProgram(mShaderProgram);
	}

	~ShaderProgramBinder()
	{
		glUseProgram(0);
	}

	const GenericShaderProgram& mShaderProgram;
};

} // namespace OpenGL
} // namespace gintonic