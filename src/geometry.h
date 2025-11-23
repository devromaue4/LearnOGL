#pragma once
#include "shader.h"

// pyramid
//GLfloat vertices[] = {
//	// pos				// colors	// uv
//	 -0.5f,  0.0f,  0.5f,  1, 0, 0,	 0.0f, 0.0f,
//	 -0.5f,  0.0f, -0.5f,  0, 0, 1,	 5.0f, 0.0f,
//	  0.5f,  0.0f, -0.5f,  0, 1, 0,	 0.0f, 0.0f,
//	  0.5f,  0.0f,  0.5f,  1, 1, 0,	 5.0f, 0.0f,
//	  0.0f,  0.8f,  0.0f,  1, 1, 0,	 2.5f, 5.0f
//};
//GLuint indices[] = {
//	0, 1, 2,
//	0, 2, 3,
//	0, 1, 4,
//	1, 2, 4,
//	2, 3, 4,
//	3, 0, 4,
//};

// primmitives
class SBox {
	GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0;

	float m_Size;

	std::shared_ptr<Shader> m_ShaderBase;

	void clear() {
		if (m_EBO != 0) { glDeleteBuffers(1, &m_EBO); m_EBO = 0; }
		if (m_VBO != 0) { glDeleteBuffers(1, &m_VBO); m_VBO = 0; }
		if (m_VAO != 0) { glDeleteVertexArrays(1, &m_VAO); m_VAO = 0; }
	}

	void buildBuffers() {

		float vertices[] = {
			// bottom
			-0.5f * m_Size, -0.5f * m_Size, -0.5f * m_Size, // 0
			-0.5f * m_Size, -0.5f * m_Size, 0.5f * m_Size, // 1
			0.5f * m_Size, -0.5f * m_Size, -0.5f * m_Size,  // 2
			0.5f * m_Size, -0.5f * m_Size, 0.5f * m_Size, // 3
			// top
			-0.5f * m_Size, 0.5f * m_Size, 0.5f * m_Size, // 4
			0.5f * m_Size, 0.5f * m_Size, 0.5f * m_Size, // 5
			0.5f * m_Size, 0.5f * m_Size, -0.5f * m_Size, // 6
			-0.5f * m_Size, 0.5f * m_Size, -0.5f * m_Size, // 7
		};
		uint indices[] = {
			1, 0, 2, 2, 3, 1, 4, 1, 3, 4, 3, 5, 5, 3, 2, 5, 2, 6,
			6, 2, 0, 6, 0, 7, 7, 0, 1, 7, 1, 4, 7, 4, 5, 7, 5, 6
		};


		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, (const void*)0);

		glBindVertexArray(0);
		glDisableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_ShaderBase = std::make_shared<Shader>();
	}

public:
	SBox(float size = 1.0f) : m_Size(size){ buildBuffers(); }
	~SBox() { clear(); }

	void Render(const glm::mat4& p, const glm::mat4& v, const glm::mat4& m) {
		m_ShaderBase->Use();
		m_ShaderBase->setMat4("mModel", m);
		m_ShaderBase->setMat4("mView", v);
		m_ShaderBase->setMat4("mProj", p);

		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (const void*)0);
		glBindVertexArray(0);
	}
	//void Render(const my::mat4& p, const my::mat4& v, const my::mat4& m) {
	//	m_ShaderBase->Use();
	//	m_ShaderBase->setMat4("mModel", m);
	//	m_ShaderBase->setMat4("mView", v);
	//	m_ShaderBase->setMat4("mProj", p);

	//	glBindVertexArray(m_VAO);
	//	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (const void*)0);
	//	glBindVertexArray(0);
	//}
};