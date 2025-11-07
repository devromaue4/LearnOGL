#include "basic_mesh.h"

using namespace std;

BasicMesh::~BasicMesh() {
	Clear();
}

void BasicMesh::Clear() {
	for (uint i = 0; i < m_Textures.size(); i++)
		safe_delete(m_Textures[i]);

	if (m_Buffers[0] != 0) 
		glDeleteBuffers(array_num_elems(m_Buffers), m_Buffers);

	if (m_VAO != 0) {
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
}

bool BasicMesh::LoadMesh(const std::string& fileName) {
	Clear();

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(array_num_elems(m_Buffers), m_Buffers);

	bool Ret = false;
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(fileName, ASSIMP_LOAD_FLAGS);
	if (pScene)
		Ret = InitFromScene(pScene, fileName);
	else {
		cout << "Error parsing: " << fileName << " : " << importer.GetErrorString() << endl;
		throw std::exception("failed to LoadMesh!");
	}

	glBindVertexArray(0);

	return Ret;
}

bool BasicMesh::InitFromScene(const aiScene* pScene, const std::string& fileName) {
	m_Meshes.resize(pScene->mNumMeshes);
	m_Textures.resize(pScene->mNumMaterials);

	uint NumVertices = 0, NumIndices = 0;
	CountVerticesAndIndices(pScene, NumVertices, NumIndices);
	ReserveSpace(NumVertices, NumIndices);
	InitAllMeshes(pScene);

	if (!InitMaterials(pScene, fileName))
		return false;

	PopulateBuffers();

	return true;
}

void BasicMesh::CountVerticesAndIndices(const aiScene* pScene, uint& NumVertices, uint& NumIndices) {
	for (uint i = 0; i < m_Meshes.size(); i++) {
		m_Meshes[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		m_Meshes[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		m_Meshes[i].BaseVertex = NumVertices;
		m_Meshes[i].BaseIndex = NumIndices;

		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += m_Meshes[i].NumIndices;
	}
}

void BasicMesh::ReserveSpace(uint NumVertices, uint NimIndices) {
	m_Positons.reserve(NumVertices);
	m_Normals.reserve(NumVertices);
	m_TexCoords.reserve(NumVertices);
	m_Indices.reserve(NimIndices);
}

void BasicMesh::InitAllMeshes(const aiScene* pScene) {
	for (uint i = 0; i < m_Meshes.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitSingleMesh(paiMesh);
	}
}

void BasicMesh::InitSingleMesh(const aiMesh* paiMesh) {
	const aiVector3D Zero3D(.0f, .0f, .0f);
	for (uint i = 0; i < paiMesh->mNumVertices; i++) {
		const aiVector3D& pPos = paiMesh->mVertices[i];
		const aiVector3D& pNormal = paiMesh->mNormals[i];
		const aiVector3D& pTexCoord = paiMesh->HasTextureCoords(0) ? paiMesh->mTextureCoords[0][i] : Zero3D;

		m_Positons.push_back(glm::vec3(pPos.x, pPos.y, pPos.z));
		m_Normals.push_back(glm::vec3(pNormal.x, pNormal.y, pNormal.z));
		m_TexCoords.push_back(glm::vec2(pTexCoord.x, pTexCoord.y));
	}

	for (uint i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		m_Indices.push_back(Face.mIndices[0]);
		m_Indices.push_back(Face.mIndices[1]);
		m_Indices.push_back(Face.mIndices[2]);
	}
}

bool BasicMesh::InitMaterials(const aiScene* pScene, const std::string& fileName) {
	string::size_type SlashIndex = fileName.find_last_of("/");
	string Dir;
	if (SlashIndex == string::npos)
		Dir = ".";
	else if (SlashIndex == 0)
		Dir = "/";
	else
		Dir = fileName.substr(0, SlashIndex);

	bool Ret = true;
	for (uint i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];
		m_Textures[i] = nullptr;
		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;
			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path) == AI_SUCCESS) {
				string p(Path.data);
				if (p.substr(0, 2) == ".\\")
					p = p.substr(2, p.size() - 2);

				string FullPath = Dir + "/" + p;

				//optimize
				//bool skipLoad = false;
				//for (uint t = 0; t < gTextureStorage.size(); t++) {
				//	if (gTextureStorage[t]->m_fileName == FullPath) {
				//		m_Textures[i] = gTextureStorage[t];
				//		skipLoad = true;
				//	}
				//}
				
				// optimize
				//if(!skipLoad) {
					m_Textures[i] = new Texture(FullPath.c_str(), 0, GL_RGB, GL_UNSIGNED_BYTE, GL_TEXTURE_2D);
					if (!m_Textures[i]->Load()) {
						cout << "Error loading texture " << FullPath.c_str() << endl;
						safe_delete(m_Textures[i]);
						m_Textures[i] = nullptr;
						Ret = false;
					}
				//}
			}
		}
	}
	
	return Ret;
}

void BasicMesh::PopulateBuffers() {
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_Positons[0]) * m_Positons.size(), &m_Positons[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_Normals[0]) * m_Normals.size(), &m_Normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_TexCoords[0]) * m_TexCoords.size(), &m_TexCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEXT_COORD_LOCATION);
	glVertexAttribPointer(TEXT_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0]) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);
}

void BasicMesh::Render() {
	glBindVertexArray(m_VAO);

	for (uint i = 0; i < m_Meshes.size(); i++) {
		uint MaterialIndex = m_Meshes[i].MaterialIndex;
		assert(MaterialIndex < m_Textures.size());

		if (m_Textures[MaterialIndex])
			m_Textures[MaterialIndex]->Bind(COLOR_TEXTURE_UNIT);

		glDrawElementsBaseVertex(GL_TRIANGLES, m_Meshes[i].NumIndices,
			GL_UNSIGNED_INT, (void*)(sizeof(uint) * m_Meshes[i].BaseIndex), m_Meshes[i].BaseVertex);
	}

	// make sure the vao is not chenged from the outside
	glBindVertexArray(0);
}

//void BasicMesh::Render(uint NumInstances, const glm::mat4* mWVP, const glm::mat4* mWorld) {
//}