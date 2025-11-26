#include "Texture.h"
#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::map<std::string, GLuint> gGlobalTexStorage;

Texture::Texture() :
	m_ID(0),
	m_TexType(GL_TEXTURE_2D),
	m_TexUnit(GL_TEXTURE0),
	m_pixelType(GL_UNSIGNED_BYTE),
	m_format(GL_RGB),
	m_fileName(""){}

Texture::Texture(const char* filePath, GLenum slot, GLenum format, GLenum pixelType, GLenum texType) :
	m_ID(0),
	m_TexType(texType),
	m_TexUnit(slot),
	m_pixelType(pixelType),
	m_format(format),
	m_fileName(filePath) {}

GLuint Texture::Load() {
	if (m_TexType != GL_TEXTURE_2D) {
		log("Texture target is not GL_TEXTURE_2D! Support is not implemented!");
		return 0;
	}

	////////// skip if texture already loaded //////
	std::string_view texName = m_fileName;
	size_t sz = texName.find_last_of('\\') + 1;
	if(sz > 0) texName = texName.substr(sz, sz);
	sz = texName.find_last_of('/') + 1;
	if(sz > 0) texName = texName.substr(sz, sz);

	if (gGlobalTexStorage.find(texName.data()) != gGlobalTexStorage.end()) {
		//log("Texture already loaded!!!: " << texName);
		m_ID = gGlobalTexStorage[texName.data()];
		return m_ID;
	}
	/////////////////////////////////////////////////

	int width, height, nChannels;
	stbi_set_flip_vertically_on_load(true);
	u8* data = stbi_load(m_fileName.c_str(), &width, &height, &nChannels, 0);
	if (!data) {
		log_error(stbi_failure_reason());
		throw std::exception("failed to load texture!");
	} 

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
	
	glTexParameteri(m_TexType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(m_TexType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum format = m_format;
	if (nChannels == 1) format = GL_RED;
	//if (nChannels == 1) format = GL_GREEN;
	//if (nChannels == 1) format = GL_BLUE;
	else if (nChannels == 3) format = GL_RGB;
	else if (nChannels == 4) format = GL_RGBA;

	glTexImage2D(m_TexType, 0, format, width, height, 0, format, m_pixelType, data);
	glGenerateMipmap(m_TexType);
	glBindTexture(m_TexType, 0);

	stbi_image_free(data);

	log("Loaded texture: " << m_fileName);

	gGlobalTexStorage[texName.data()] = m_ID;

	return m_ID;
}

GLuint Texture::Load(uint bufferSize, void* pData) {
	if (m_TexType != GL_TEXTURE_2D) {
		log("Texture target is not GL_TEXTURE_2D! Support is not implemented!");
		return 0;
	}

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	u8* image_data = stbi_load_from_memory((const stbi_uc*)pData, bufferSize, &width, &height, &nrChannels, 0);
	if (!image_data) { 
		log(stbi_failure_reason());
		throw std::exception("failed to load texture!");
	}

	glGenTextures(1, &m_ID);
	//glActiveTexture(m_TexUnit);
	glBindTexture(m_TexType, m_ID);

	// texture filtering
	glTexParameteri(m_TexType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(m_TexType, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(m_TexType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(m_TexType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum format = m_format;
	if (nrChannels == 1) format = GL_RED;
	else if (nrChannels == 3) format = GL_RGB;
	else if (nrChannels == 4) format = GL_RGBA;

	glTexImage2D(m_TexType, 0, format, width, height/2, 0, format, m_pixelType, image_data);
	glGenerateMipmap(m_TexType);
	glBindTexture(m_TexType, 0);

	stbi_image_free(image_data);

	log("Loaded texture from memory: " << width << "x" << height << " channels: " << nrChannels);

	return m_ID;
}

// test only in release need load: width, height, nrChannels
GLuint Texture::LoadFromMemory(void* pData, int width, int height, int nrChannels) {
	if (m_TexType != GL_TEXTURE_2D) {
		log("Texture target is not GL_TEXTURE_2D! Support is not implemented!");
		return 0;
	}

	// test only in release need load: width, height, nrChannels
	//int width = 4096, height = 4096, nrChannels = 3;

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

	glTexImage2D(m_TexType, 0, format, width, height, 0, format, m_pixelType, pData);
	glGenerateMipmap(m_TexType);
	glBindTexture(m_TexType, 0);

	log("Loaded texture from memory: " << width << "x" << height << " channels: " << nrChannels);

	return m_ID;
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