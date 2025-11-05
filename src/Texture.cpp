#include "Texture.h"
#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//std::vector<Texture*> gTextureStorage;

Texture::Texture() {
	m_TexType = GL_TEXTURE_2D;
	m_TexUnit = GL_TEXTURE0;
	m_pixelType = GL_UNSIGNED_BYTE;
	m_format = GL_RGB;
	m_fileName = "";
}

Texture::Texture(const char* filePath, GLenum slot, GLenum format, GLenum pixelType, GLenum texType) {
	m_TexType = texType;
	m_TexUnit = slot;

	m_pixelType = pixelType;
	m_format = format;

	m_fileName = filePath;
}

bool Texture::Load() {
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(m_fileName.c_str(), &width, &height, &nrChannels, 0);
	if (!data) throw std::exception("failed to load texture!");

	// 1-st texture
	glGenTextures(1, &m_ID);
	//glActiveTexture(m_TexUnit);
	glBindTexture(m_TexType, m_ID);

	// texture filtering
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// mipmaps
	// A common mistake is to set one of the mipmap filtering options as the magnification filter.
	// This doesn't have any effect since mipmaps are primarily used for when textures get downscaled:
	// texture magnification doesn't use mipmaps and giving it a mipmap filtering option will generate
	// an OpenGL GL_INVALID_ENUM error code.
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(m_TexType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(m_TexType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//if (flNearest) {
	//	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//}
	//else {
		glTexParameteri(m_TexType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(m_TexType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//}

	GLenum format = m_format;
	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;

	//glTexImage2D(m_TexType, 0, GL_RGBA, width, height, 0, m_format, m_pixelType, data);
	glTexImage2D(m_TexType, 0, format, width, height, 0, format, m_pixelType, data);
	glGenerateMipmap(m_TexType);
	glBindTexture(m_TexType, 0);
	stbi_image_free(data);

	log("Loaded texture: " << m_fileName);

	//gTextureStorage.push_back(this);

	return true;
}

bool Texture::Load(uint bufferSize, void* pData) {
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	ubyte* image_data = stbi_load_from_memory((const stbi_uc*)pData, bufferSize, &width, &height, &nrChannels, 0);
	if (!image_data) { 
		log(stbi_failure_reason());
		throw std::exception("failed to load texture!");
	}

	// 1-st texture
	glGenTextures(1, &m_ID);
	//glActiveTexture(m_TexUnit);
	glBindTexture(m_TexType, m_ID);

	// texture filtering
	glTexParameteri(m_TexType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(m_TexType, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(m_TexType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(m_TexType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum format = m_format;
	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;

	glTexImage2D(m_TexType, 0, format, width, height, 0, format, m_pixelType, image_data);
	glGenerateMipmap(m_TexType);
	glBindTexture(m_TexType, 0);

	stbi_image_free(image_data);

	log("Loaded texture from memory: " << width << "x" << height << " channels: " << nrChannels);

	//gTextureStorage.push_back(this);

	return true;
}

void Texture::setTexUnit(Shader& shader, const char* uniform, GLuint unit) {
	GLuint tex0Uni = glGetUniformLocation(shader.m_ID, uniform); 
	//if (tex0Uni == -1) throw glsl_error("failed getting uniform variable!");
	shader.Use(); // don't forget to activate the shader before setting uniforms! 
	glUniform1i(tex0Uni, unit);
	//glUniform1i(glGetUniformLocation(gShaderBase->m_ID, "tex0"), 0);  // set it manually
	//gShaderBase->setInt("tex1", 1); // or with shader class
}

void Texture::Bind() {
	glActiveTexture(m_TexUnit);
	//glUniform1i(glGetUniformLocation(shader.m_ID, (name + number).c_str()), i);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture::Bind(GLenum texUnit) {
	glActiveTexture(texUnit);
	//glUniform1i(glGetUniformLocation(shader.m_ID, (name + number).c_str()), i);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}