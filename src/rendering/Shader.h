#pragma once

#include <glm/glm.hpp>
#include <string>

class Shader
{
private:
    unsigned int id;

    std::string loadShaderSource(const std::string& path) const;
    unsigned int compileShader(unsigned int type, const std::string& source) const;
    void checkCompileErrors(unsigned int shader, const std::string& type) const;

public:
    Shader();
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    void use() const;
    unsigned int getID() const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;

    void setMat4(const std::string& name, const glm::mat4& mat) const;

};
