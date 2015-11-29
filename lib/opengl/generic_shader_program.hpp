#pragma once

#include "shader_program.hpp"

namespace gintonic {
namespace opengl {

class generic_shader_program
{
private:

	class exception_base : public std::exception
	{
	public:

		exception_base(
			const boost::filesystem::path& vertexshader,
			const boost::filesystem::path& fragmentshader,
			const char* what_msg_prefix);
		
		exception_base(
			const boost::filesystem::path& vertexshader,
			const boost::filesystem::path& geometryshader,
			const boost::filesystem::path& fragmentshader,
			const char* what_msg_prefix);

		virtual ~exception_base() noexcept = default;

		inline const boost::filesystem::path& vertex_shader() const noexcept
		{
			return m_vertexshader;
		}

		inline const boost::filesystem::path& geometry_shader() const noexcept
		{
			return m_geometryshader;
		}

		inline const boost::filesystem::path& fragment_shader() const noexcept
		{
			return m_fragmentshader;
		}

		inline virtual const char* what() const noexcept
		{
			return m_what_msg.c_str();
		}

	protected:
		std::basic_string<GLchar> m_what_msg;

	private:
		boost::filesystem::path m_vertexshader;
		boost::filesystem::path m_geometryshader;
		boost::filesystem::path m_fragmentshader;
	};

public:

	/**
	 * @brief Encapsulates a uniform variable of a shader.
	 */
	struct uniform
	{
		/// The name of the uniform variable.
		std::basic_string<GLchar> name;

		/// The location of the uniform variable in a shader.
		GLuint location;

		/// The size of the uniform variable (in bytes).
		GLint size;

		/// The type of the uniform variable.
		GLenum type;

		/// Compares the names lexicographically.
		inline bool operator <  (const uniform& other) const 
			noexcept
		{
			return name < other.name;
		}
		
		/// Compares the names lexicographically.
		inline bool operator >= (const uniform& other) const 
			noexcept
		{
			return !operator<(other); 
		}

		/// Compares the names lexicographically.
		inline bool operator >  (const uniform& other) const 
			noexcept
		{
			return name > other.name;
		}

		/// Compares the names lexicographically.
		inline bool operator <= (const uniform& other) const 
			noexcept
		{
			return !operator>(other);
		}
	};

	typedef std::vector<uniform>::const_iterator const_iterator;

	class uniform_not_found_error : public exception_base
	{
	public:

		uniform_not_found_error(
			const boost::filesystem::path& vertexshader,
			const boost::filesystem::path& fragmentshader,
			const char* uniformname);
		
		uniform_not_found_error(
			const boost::filesystem::path& vertexshader,
			const boost::filesystem::path& geometryshader,
			const boost::filesystem::path& fragmentshader,
			const char* uniformname);

		virtual ~uniform_not_found_error() noexcept = default;
	};

	class compilation_error : public exception_base
	{
	public:
		
		compilation_error(
			const boost::filesystem::path& vertexshader,
			const boost::filesystem::path& fragmentshader,
			const std::basic_string<GLchar>& infolog);
		
		compilation_error(
			const boost::filesystem::path& vertexshader,
			const boost::filesystem::path& geometryshader,
			const boost::filesystem::path& fragmentshader,
			const std::basic_string<GLchar>& infolog);

		virtual ~compilation_error() noexcept = default;
	};

	generic_shader_program(
		boost::filesystem::path vertexshader,
		boost::filesystem::path fragmentshader);

	generic_shader_program(
		boost::filesystem::path vertexshader,
		boost::filesystem::path geometryshader,
		boost::filesystem::path fragmentshader);

	virtual ~generic_shader_program() noexcept;

	template <class Type> void set(const char* name, Type&& value) const
	{
		const auto r = std::find_if(begin(), end(), finder(name));
		if (r == end())
		{
			throw uniform_not_found_error(
				name, 
				m_vertexshader, 
				m_geometryshader, 
				m_fragmentshader);
		}
		else set_uniform(r->location, std::forward<Type>(value));
	}

	inline const_iterator begin() const { return m_uniforms.begin(); }
	inline const_iterator end() const { return m_uniforms.end(); }
	inline const_iterator cbegin() const { return m_uniforms.begin(); }
	inline const_iterator cend() const { return m_uniforms.end(); }

	inline operator GLuint() const noexcept
	{
		return m_handle;
	}

	inline const boost::filesystem::path& vertex_shader() const noexcept
	{
		return m_vertexshader;
	}

	const boost::filesystem::path& geometry_shader() const noexcept
	{
		return m_geometryshader;
	}

	const boost::filesystem::path& fragment_shader() const noexcept
	{
		return m_fragmentshader;
	}

private:
	GLuint m_handle;
	std::vector<uniform> m_uniforms;
	boost::filesystem::path m_vertexshader;
	boost::filesystem::path m_geometryshader;
	boost::filesystem::path m_fragmentshader;

	struct finder
	{
		finder(const char* name) : m_name(name) {}
		inline bool operator()(const uniform& u) const noexcept
		{
			if (u.name == m_name) return true;
			else return false;
		}
		const char* m_name;
	};
};

struct shader_program_binder
{
	shader_program_binder(const generic_shader_program& s)
	: m_shader_program(s)
	{
		glUseProgram(m_shader_program);
	}

	~shader_program_binder()
	{
		glUseProgram(0);
	}

	const generic_shader_program& m_shader_program;
};

} // namespace opengl
} // namespace gintonic