#include "VBO.h"

VBO::VBO(GLfloat* pVertices, GLsizeiptr size) {
	glGenBuffers(1, &m_ID);
	glBindBuffer(GL_ARRAY_BUFFER, m_ID);
	glBufferData(GL_ARRAY_BUFFER, size, pVertices, GL_STATIC_DRAW);
}
