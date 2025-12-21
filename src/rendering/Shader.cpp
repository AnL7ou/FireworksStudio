#include "Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader()
	: id(0)
{
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
	: id(0)
{
	std::string vertSrc = loadShaderSource(vertexPath);
	std::string fragSrc = loadShaderSource(fragmentPath);

	if (vertSrc.empty() || fragSrc.empty())
	{
		std::cerr << "Shader::Shader - empty shader source\n";
		return;
	}

	unsigned int vertex = compileShader(GL_VERTEX_SHADER, vertSrc);
	unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, fragSrc);

	if (vertex == 0 || fragment == 0)
	{
		if (vertex) glDeleteShader(vertex);
		if (fragment) glDeleteShader(fragment);
		return;
	}

	// Link program
	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);

	// check linking errors
	checkCompileErrors(id, "PROGRAM");

	// Verify link status and log program id
	GLint linkStatus = GL_FALSE;
	glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_TRUE)
	{
		std::cout << "Shader::Shader - program linked successfully, id=" << id << "\n";
	}
	else
	{
		std::cerr << "Shader::Shader - program linking FAILED, id=" << id << "\n";
		// keep program id 0 to avoid usage if linking failed
		glDeleteProgram(id);
		id = 0;
	}

	// shaders can be deleted after linking
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader()
{
	if (id != 0)
	{
		glDeleteProgram(id);
		id = 0;
	}
}

void Shader::use() const
{
	if (id != 0)
		glUseProgram(id);
	else
		std::cerr << "Shader::use - program not created\n";
}

unsigned int Shader::getID() const
{
	return id;
}

void Shader::setBool(const std::string& name, bool value) const
{
	if (!id) return;
	glUniform1i(glGetUniformLocation(id, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const std::string& name, int value) const
{
	if (!id) return;
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	if (!id) return;
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	if (!id) return;
	glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	if (!id) return;
	glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	if (!id) return;
	glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	if (!id) return;
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

std::string Shader::loadShaderSource(const std::string& path) const
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cerr << "Shader::loadShaderSource - failed to open: " << path << "\n";
		return std::string();
	}
	std::stringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source) const
{
	const char* src = source.c_str();
	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);
	// check compile errors
	if (type == GL_VERTEX_SHADER)
		checkCompileErrors(shader, "VERTEX");
	else if (type == GL_FRAGMENT_SHADER)
		checkCompileErrors(shader, "FRAGMENT");
	else
		checkCompileErrors(shader, "SHADER");

	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

void Shader::checkCompileErrors(unsigned int object, const std::string& type) const
{
	GLint success;
	GLchar infoLog[1024];
	if (type == "PROGRAM")
	{
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(object, sizeof(infoLog), nullptr, infoLog);
			std::cerr << "Shader::checkCompileErrors - PROGRAM linking error:\n" << infoLog << "\n";
		}
	}
	else
	{
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(object, sizeof(infoLog), nullptr, infoLog);
			std::cerr << "Shader::checkCompileErrors - " << type << " compile error:\n" << infoLog << "\n";
		}
	}
}