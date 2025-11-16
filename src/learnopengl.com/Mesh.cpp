#include "Mesh.h"
#include "shader.h"

MMesh::MMesh(std::vector<MVertex> vertices, std::vector<uint> indices, std::vector<MTexture> textures) {
	m_vertices = vertices;
	m_indices = indices;
	m_textures = textures;

	setupMesh();
}

void MMesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(MVertex), &m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(uint), &m_indices[0], GL_STATIC_DRAW);

	GLsizei stride = sizeof(MVertex);
	glEnableVertexAttribArray(0); // pos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(1);// norm
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(MVertex, Normal));
	glEnableVertexAttribArray(2);// tex
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(MVertex, TexCoords));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(MVertex, Tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(MVertex, Bitangent));
	// ids
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 4, GL_INT, stride, (void*)offsetof(MVertex, m_BoneIDs));
	// weights
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(MVertex, m_Weights));

	glBindVertexArray(0);
}

void MMesh::Draw(const Shader& shader) {
	uint duffuseNr = 1, specularNr = 1;
	uint normalNr = 1;
	uint heightNr = 1;
	for (uint i = 0; i < m_textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)

		std::string number;
		std::string name = m_textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(duffuseNr++);
		else if(name == "texture_specular")
			number = std::to_string(specularNr++);
		else if (name == "texture_normal")
			number = std::to_string(normalNr++); // transfer unsigned int to string
		else if (name == "texture_height")
			number = std::to_string(heightNr++); // transfer unsigned int to string

		//shader.setInt(("material." + name + number).c_str(), i);
		// now set the sampler to the correct texture unit
		glUniform1i(glGetUniformLocation(shader.m_ID, (name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}