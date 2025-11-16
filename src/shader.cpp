#include "shader.h"
#include "myutil.h"

// base shaders
const char* vertShaderSrc = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main() {\n"
"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragShaderSrc = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main() {\n"
"	FragColor = vec4(0.8f, 0.3f, 0.2f, 1.0f);\n"
"}\n\0";

// C++11 raw string literals
const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    //uniform mat4 mPVM;
	uniform mat4 mModel;
	uniform mat4 mView;
	uniform mat4 mProj;
    void main() {
        //gl_Position = mPVM * vec4(aPos, 1.0);
		gl_Position = mProj * mView * mModel * vec4(aPos, 1.0);
    }
)glsl";
const char* fragmentShaderSource = R"glsl(
    #version 330 core
    uniform vec3 triangleColor;
    out vec4 FragColor;
    void main() {
       //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
       FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
       //FragColor = vec4(triangleColor, 1.0f);
    }
)glsl";

Shader::Shader() {
	m_ID = glCreateProgram();
	if (!m_ID) throw glfw_error("creating shader program\n");

	AddShader(m_ID, vertexShaderSource, GL_VERTEX_SHADER);
	AddShader(m_ID, fragmentShaderSource, GL_FRAGMENT_SHADER);

	glLinkProgram(m_ID); CheckLinkShader(m_ID);
}

Shader::Shader(const char* vertFile, const char* fragFile) {
	m_ID = glCreateProgram();
	if (!m_ID) throw glfw_error("creating shader program\n");

	m_vertFile = vertFile; 
	m_fragFile = fragFile;

	std::string vs, fs;
	if (!util::ReadFile(vertFile, vs)) {
		if (!util::verifyPath(vertFile)) log_error("Can't find file: " << vertFile);
		else log_error(vertFile);
		throw glfw_error("Shader::Shader");
	}

	AddShader(m_ID, vs.c_str(), GL_VERTEX_SHADER);

	if (!util::ReadFile(fragFile, fs)) {
		if (!util::verifyPath(fragFile)) log_error("Can't find file: " << fragFile);
		else log_error(fragFile);
		throw glfw_error("Shader::Shader");
	}

	AddShader(m_ID, fs.c_str(), GL_FRAGMENT_SHADER);

	glLinkProgram(m_ID); CheckLinkShader(m_ID);

	log("Loaded shader: " << vertFile);
	log("Loaded shader: " << fragFile);
}

void Shader::AddShader(GLuint shaderProgram, const char* shaderText, GLenum shaderType) {
	GLuint ShaderObj = glCreateShader(shaderType);
	if (!ShaderObj) throw glsl_error("creating shader type!");

	glShaderSource(ShaderObj, 1, &shaderText, nullptr);// lenght);
	glCompileShader(ShaderObj);

	GLint succes;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &succes);
	if (!succes) {
		if(shaderType == GL_VERTEX_SHADER) log("VERTEX SHADER: " << m_vertFile);
		else log("FRAGMENT SHADER: " << m_fragFile);
		throw glsl_compile_error("Failed to compile shader", ShaderObj);
	}

	glAttachShader(shaderProgram, ShaderObj);
	glDeleteShader(ShaderObj);
}

void Shader::CheckLinkShader(GLuint shaderProg) {
	GLint success;
	glGetProgramiv(shaderProg, GL_LINK_STATUS, &success);
	if (!success) throw glsl_error("Failed to linking shader program");
	glValidateProgram(shaderProg);
	glGetProgramiv(shaderProg, GL_VALIDATE_STATUS, &success);
	if (!success) {
		GLchar ErrorLog[1024] = {};
		glGetProgramInfoLog(shaderProg, sizeof(ErrorLog), nullptr, ErrorLog);
		std::cout << ErrorLog << std::endl;
		throw glsl_error("Invalid shader program");
	}
}

void Shader::Use() {
	glUseProgram(m_ID);
}

void Shader::Delete() {
	glDeleteProgram(m_ID);
}

void Shader::setBool(const std::string& name, bool value) {
	if (m_uniformLocations.find(name) == m_uniformLocations.end())
		m_uniformLocations[name] = glGetUniformLocation(m_ID, name.c_str());
	glUniform1i(m_uniformLocations[name], (int)value);
}

void Shader::setInt(const std::string& name, int value) {
	if (m_uniformLocations.find(name) == m_uniformLocations.end())
		m_uniformLocations[name] = glGetUniformLocation(m_ID, name.c_str());
	glUniform1i(m_uniformLocations[name], value);
}

void Shader::setFloat(const std::string& name, float value) {
	if (m_uniformLocations.find(name) == m_uniformLocations.end())
		m_uniformLocations[name] = glGetUniformLocation(m_ID, name.c_str());
	glUniform1f(m_uniformLocations[name], value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) {
	if (m_uniformLocations.find(name) == m_uniformLocations.end())
		m_uniformLocations[name] = glGetUniformLocation(m_ID, name.c_str());
	glUniform3fv(m_uniformLocations[name], 1, &value[0]);
}
void Shader::setVec4(const std::string& name, const glm::vec4& value) {
	if (m_uniformLocations.find(name) == m_uniformLocations.end())
		m_uniformLocations[name] = glGetUniformLocation(m_ID, name.c_str());
	glUniform4fv(m_uniformLocations[name], 1, &value[0]);
}
//void Shader::setVec4(const std::string& name, float x, float y, float z, float w)
//{
//	glUniform4f(glGetUniformLocation(m_ID, name.c_str()), x, y, z, w);
//}

void Shader::setMat4(const std::string& name, const glm::mat4& value) {
	if (m_uniformLocations.find(name) == m_uniformLocations.end())
		m_uniformLocations[name] = glGetUniformLocation(m_ID, name.c_str());	
	glUniformMatrix4fv(m_uniformLocations[name], 1, GL_FALSE, &value[0][0]);
}

//void Shader::setMat4(const std::string& name, const my::mat4& value) {
//	if (m_uniformLocations.find(name) == m_uniformLocations.end())
//		m_uniformLocations[name] = glGetUniformLocation(m_ID, name.c_str());	
//	glUniformMatrix4fv(m_uniformLocations[name], 1, GL_FALSE, &value.m[0][0]);
//}

//void Shader::setMat4(const std::string& name, const glm::mat4* value) const {
//	glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, value_ptr(*value));
//}
