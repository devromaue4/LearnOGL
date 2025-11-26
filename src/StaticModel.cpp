#include "StaticModel.h"

void StaticModel::clear() {
	if (m_EBO != 0) { glDeleteBuffers(1, &m_EBO); m_EBO = 0; }
	if (m_VBO != 0) { glDeleteBuffers(1, &m_VBO); m_VBO = 0; }
	if (m_VAO != 0) { glDeleteVertexArrays(1, &m_VAO); m_VAO = 0; }
}

void StaticModel::Load(std::string_view fileName, bool bFlipUVs) {
	clear();

	uint flags = aiProcess_Triangulate
		| aiProcess_GenSmoothNormals
		| aiProcess_CalcTangentSpace
		| aiProcess_JoinIdenticalVertices
		| aiProcess_ImproveCacheLocality
		| aiProcess_RemoveRedundantMaterials;
	if (bFlipUVs) flags |= aiProcess_FlipUVs;

	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(fileName.data(), flags);
	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
		if(!util::verifyPath(fileName.data()))
			log_error("Can't find file: " << fileName.data());
		else log_error(importer.GetErrorString());
		throw std::exception("StaticModel::Load!");
	}

	m_Directory = fileName.substr(0, fileName.find_last_of('/'));

	m_Meshes.resize(pScene->mNumMeshes);
	m_Materials.resize(pScene->mNumMaterials);

	uint numVertices = 0, numIndices = 0;
	calcVertices(pScene, numVertices, numIndices);

	m_Vertices.reserve(numVertices);
	m_Indices.reserve(numIndices);

	loadGeoData(pScene);
	loadMaterials(pScene);
	buildBuffers();

	importer.FreeScene();
}

void StaticModel::calcVertices(const aiScene* pScene, uint& numVertices, uint& numIndices) {
	for (uint iMesh = 0; iMesh < pScene->mNumMeshes; iMesh++) {
		m_Meshes[iMesh].MaterialIndex = pScene->mMeshes[iMesh]->mMaterialIndex;
		m_Meshes[iMesh].BaseVertex = numVertices;
		m_Meshes[iMesh].BaseIndex = numIndices;

		numVertices += pScene->mMeshes[iMesh]->mNumVertices;
		m_Meshes[iMesh].NumIndices = pScene->mMeshes[iMesh]->mNumFaces * 3;
		numIndices += m_Meshes[iMesh].NumIndices;
	}
}

void StaticModel::loadGeoData(const aiScene* pScene) {
	SVertex myVert;

	for (uint iMesh = 0; iMesh < pScene->mNumMeshes; iMesh++) {
		const aiMesh* pMesh = pScene->mMeshes[iMesh];
		for (uint iVert = 0; iVert < pMesh->mNumVertices; iVert++) {
			myVert.pos = util::toGlm(pMesh->mVertices[iVert]);

			if (pMesh->HasNormals())
				myVert.normal = util::toGlm(pMesh->mNormals[iVert]);
			else myVert.normal = glm::vec3(.0f, 1.f, .0f);

			if (pMesh->HasTextureCoords(0)) {
				const aiVector3D& tc = pMesh->mTextureCoords[0][iVert];
				myVert.texCoord = glm::vec2(tc.x, tc.y);
			}

			m_Vertices.push_back(myVert);
		}

		for (uint iface = 0; iface < pMesh->mNumFaces; iface++) {
			m_Indices.push_back(pMesh->mFaces[iface].mIndices[0]);
			m_Indices.push_back(pMesh->mFaces[iface].mIndices[1]);
			m_Indices.push_back(pMesh->mFaces[iface].mIndices[2]);
		}
	}
}

void StaticModel::loadMaterials(const aiScene* pScene) {
	for (uint i = 0; i < pScene->mNumMaterials; i++) {
		loadDiffuseTexture(pScene, pScene->mMaterials[i], i);
		loadSpecularTexture(pScene, pScene->mMaterials[i], i);
	}
}

void StaticModel::loadDiffuseTexture(const aiScene* pScene, const aiMaterial* pMaterial, int index) {
	m_Materials[index].texDiffuse = nullptr;

	if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		aiString path;
		if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
			const aiTexture* pAiTexture = pScene->GetEmbeddedTexture(path.data);
			if (pAiTexture) {
				m_Materials[index].texDiffuse = std::make_shared<Texture>();
				if (!m_Materials[index].texDiffuse->Load(pAiTexture->mWidth, pAiTexture->pcData))
					log_error("loading Embedded texture!");
			}
			else {
				std::string p(path.data);
				//if (p.substr(0, 2) == ".\\") // don't know this code
				//	p = p.substr(2, p.size() - 2);

				//int sz = p.find_last_of('/') + 1;
				//if(sz > 0) p = p.substr(sz, sz);

				std::string fullPath = m_Directory + "/" + p;

				m_Materials[index].texDiffuse = std::make_shared<Texture>(fullPath.c_str());
				if (!m_Materials[index].texDiffuse->Load()) {
					log_error("loading texture " << fullPath);
					m_Materials[index].texDiffuse = nullptr;
					return;
				}
			}
		}
	}

	if (pMaterial->GetTextureCount(aiTextureType_NORMALS) > 0) {
		log("Normal textures are not implemented yet!!!");
	}
	if (pMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0) {
		log("Specular textures are not implemented yet!!!");
	}
	if (pMaterial->GetTextureCount(aiTextureType_REFLECTION) > 0) {
		log("Reflection textures are not implemented yet!!!");
	}
}

void StaticModel::loadSpecularTexture(const aiScene* pScene, const aiMaterial* pMaterial, int index) {
	m_Materials[index].texSpecular = nullptr;

	if (pMaterial->GetTextureCount(aiTextureType_SHININESS) > 0) {
		aiString path;
		if (pMaterial->GetTexture(aiTextureType_SHININESS, 0, &path) == AI_SUCCESS) {
			const aiTexture* pAiTexture = pScene->GetEmbeddedTexture(path.data);
			if (pAiTexture) {
				m_Materials[index].texSpecular = std::make_shared<Texture>();
				if (!m_Materials[index].texSpecular->Load(pAiTexture->mWidth, pAiTexture->pcData))
					log_error("loading Embedded texture!");
			}
			else {
				std::string fullPath = m_Directory + "/" + path.data;
				m_Materials[index].texSpecular = std::make_shared<Texture>(fullPath.c_str(), GL_TEXTURE6);
				if (!m_Materials[index].texSpecular->Load()) {
					log_error("loading texture " << fullPath);
					m_Materials[index].texSpecular = nullptr;
					return;
				}
			}
		}
	}
}

void StaticModel::buildBuffers() {
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SVertex) * m_Vertices.size(), &m_Vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);

	GLsizei stride = sizeof(SVertex);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(SVertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(SVertex, texCoord));

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void StaticModel::Render() {
	glBindVertexArray(m_VAO);

	for (uint i = 0; i < m_Meshes.size(); i++) {
		uint MatIndex = m_Meshes[i].MaterialIndex;
		assert(MatIndex < m_Materials.size());
		if (m_Materials.size()) {
			if (m_Materials[MatIndex].texDiffuse) m_Materials[MatIndex].texDiffuse->Bind();
			//if (m_Materials[MatIndex].texSpecular) m_Materials[MatIndex].texSpecular->Bind(GL_TEXTURE6);
			if (m_Materials[MatIndex].texSpecular) m_Materials[MatIndex].texSpecular->Bind();
		}

		glDrawElementsBaseVertex(GL_TRIANGLES, m_Meshes[i].NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(uint) * m_Meshes[i].BaseIndex), m_Meshes[i].BaseVertex);
	}

	glBindVertexArray(0);
}

// used only by instancing
//void StaticModel::Render(uint NumInstances, const glm::mat4* WVPMats, const glm::mat4* WorldMats) {
//
//	// !!!! not inmplemented yet !!!!
//	//glBindBuffer(GL_ARRAY_BUFFER, buffers_WVP_MAT);
//	//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * NumInstances, WVPMats, GL_DYNAMIC_DRAW);
//	//glBindBuffer(GL_ARRAY_BUFFER, buffers_WORLD_MAT);
//	//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * NumInstances, worldMat, GL_DYNAMIC_DRAW);
//
//	glBindVertexArray(m_VAO);
//
//	for (uint i = 0; i < m_Meshes.size(); i++) {
//		uint MatIndex = m_Meshes[i].MaterialIndex;
//		assert(MatIndex < m_Textures.size());
//		if (m_Textures.size())
//			if (m_Textures[MatIndex]) m_Textures[MatIndex]->Bind();
//
//		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, m_Meshes[i].NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(uint) * m_Meshes[i].BaseIndex), NumInstances, m_Meshes[i].BaseVertex);
//	}
//
//	glBindVertexArray(0);
//}

void StaticModel::Write(std::string_view fileName) {
	std::ofstream file(fileName.data(), std::ios::binary);
	if (!file) {
		log_error("can't create file: " << fileName);
		throw std::exception("failed to write!");
	}

	MeshHeader mh;
	mh.mNumMeshes = (uint)m_Meshes.size();
	mh.mNumMaterials = (uint)m_Materials.size();
	mh.numVertices = (uint)m_Vertices.size();
	mh.numIndices = (uint)m_Indices.size();

	// write header info
	if (!file.write(reinterpret_cast<const char*>(&mh), sizeof(mh))) log_error("can't write!");

	// write MeshIndexedData
	if (!file.write(reinterpret_cast<const char*>(m_Meshes.data()), sizeof(MeshIndexedData) * m_Meshes.size())) log_error("can't write!");

	// write vertex data
	if (!file.write(reinterpret_cast<const char*>(m_Vertices.data()), sizeof(SVertex) * m_Vertices.size())) log_error("can't write!");

	// write index data
	if (!file.write(reinterpret_cast<const char*>(m_Indices.data()), sizeof(uint) * m_Indices.size())) log_error("can't write!");

	// write mat info
	std::vector<MaterialHeader> materHd(m_Materials.size());

	for (int i = 0; i < m_Materials.size(); i++) {
		materHd[i].AmbientColor = toColor(m_Materials[i].AmbientColor);
		materHd[i].DiffuseColor = toColor(m_Materials[i].DiffuseColor);
		materHd[i].SpecularColor = toColor(m_Materials[i].SpecularColor);

		if (!file.write(reinterpret_cast<const char*>(materHd.data()), sizeof(MaterialHeader))) log_error("can't write!");

		// write texture path

		// write size of string diff path
		size_t strSize = 0;
		if (m_Materials[i].texDiffuse) {
			strSize = m_Materials[i].texDiffuse->GetFileName().size() + 1;
			if (!file.write(reinterpret_cast<const char*>(&strSize), sizeof(size_t))) log_error("can't write!");
			// write path diff tex
			if (!file.write(reinterpret_cast<const char*>(m_Materials[i].texDiffuse->GetFileName().data()), strSize)) log_error("can't write!");
		}
		else // write zero
			if (!file.write(reinterpret_cast<const char*>(&strSize), sizeof(size_t))) log_error("can't write!");

		strSize = 0;

		// write size of string spec path
		if (m_Materials[i].texSpecular) {
			strSize = m_Materials[i].texSpecular->GetFileName().size() + 1;
			if (!file.write(reinterpret_cast<const char*>(&strSize), sizeof(size_t))) log_error("can't write!");
			if (!file.write(reinterpret_cast<const char*>(m_Materials[i].texSpecular->GetFileName().data()), strSize)) log_error("can't write!");
		}
		else // write zero
			if (!file.write(reinterpret_cast<const char*>(&strSize), sizeof(size_t))) log_error("can't write!");
	}

	file.close();
	log("writed: " << fileName);
}

void StaticModel::Read(std::string_view fileName) {
	std::ifstream file_r(fileName.data(), std::ios::binary);
	if (!file_r) {
		log_error("can't read file: " << fileName);
		throw std::exception("failed to read!");
	}

	MeshHeader mh;
	if (!file_r.read(reinterpret_cast<char*>(&mh), sizeof(mh))) log_error("can't read file!");

	m_Meshes.resize(mh.mNumMeshes);
	m_Materials.resize(mh.mNumMaterials);

	// read MeshIndexedData
	if (!file_r.read(reinterpret_cast<char*>(&m_Meshes[0]), sizeof(MeshIndexedData) * m_Meshes.size())) log_error("can't read file!");

	m_Vertices.resize(mh.numVertices);
	m_Indices.resize(mh.numIndices);

	// read vertex data
	if (!file_r.read(reinterpret_cast<char*>(&m_Vertices[0]), sizeof(SVertex) * m_Vertices.size())) log_error("can't read file!");

	// read index data
	if (!file_r.read(reinterpret_cast<char*>(&m_Indices[0]), sizeof(uint) * m_Indices.size())) log_error("can't read file!");

	// read mat info
	std::vector<MaterialHeader> materHd(m_Materials.size());

	std::vector<char> data;
	std::string path;
	for (int i = 0; i < m_Materials.size(); i++) {
		if (!file_r.read(reinterpret_cast<char*>(&materHd[i]), sizeof(MaterialHeader))) log_error("can't read file!");

		// read string diff path
		size_t strSize = 0;
		if (!file_r.read(reinterpret_cast<char*>(&strSize), sizeof(size_t))) log_error("can't write!");

		if (strSize > 0) {
			data.resize(strSize);
			if (!file_r.read(reinterpret_cast<char*>(data.data()), strSize)) log_error("can't read file!");

			path = data.data();
			data.clear();

			m_Materials[i].texDiffuse = std::make_shared<Texture>(path.c_str());
			if (!m_Materials[i].texDiffuse->Load()) {
				log_error("loading texture " << path);
				m_Materials[i].texDiffuse = nullptr;
			}
		}
		else m_Materials[i].texDiffuse = nullptr;

		strSize = 0;

		// read string spec path
		if (!file_r.read(reinterpret_cast<char*>(&strSize), sizeof(size_t))) log_error("can't write!");
		if (strSize > 0) {
			data.resize(strSize);
			if (!file_r.read(reinterpret_cast<char*>(data.data()), strSize)) log_error("can't read file!");

			path = data.data();
			data.clear();

			m_Materials[i].texSpecular = std::make_shared<Texture>(path.c_str(), GL_TEXTURE6);
			if (!m_Materials[i].texSpecular->Load()) {
				log_error("loading texture " << path);
				m_Materials[i].texSpecular = nullptr;
			}
		}
		else m_Materials[i].texSpecular = nullptr;
	}

	file_r.close();
	log("readed: " << fileName);

	buildBuffers();
}