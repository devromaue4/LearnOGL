#include "Model.h"
#include "shader.h"
//#include "Texture.h"
#include "myutil.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

uint TextureFromFile(const char* path, const string& directory/*, bool gamma*/) {
	string filename = string(path);
	filename = directory + '/' + filename;

	uint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);
	ubyte* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (!data) {
		cout << filename << std::endl;
		stbi_image_free(data);
		throw std::exception("failed to load texture!");
	}

	GLenum format = 0;
	if (nrComponents == 1) 
		format = GL_RED;
	else if (nrComponents == 3)
		format = GL_RGB;
	else if (nrComponents == 4)
		format = GL_RGBA;

	glBindTexture(GL_TEXTURE_2D, textureID);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);

	std::cout << "Loaded texture: " << filename << std::endl;

	return textureID;
}

void Model::SetVertexBoneDataToDefault(MVertex& vertex) {
	for (int i = 0; i < MAX_BONE_INF; i++) {
		vertex.m_BoneIDs[i] = -1;
		vertex.m_Weights[i] = 0.0f;
	}
}

void Model::SetVertexBoneData(MVertex& vertex, int boneID, float weight) {
	for (int i = 0; i < MAX_BONE_INF; i++) {
		if(vertex.m_BoneIDs[i] < 0) {
			vertex.m_BoneIDs[i] = boneID;
			vertex.m_Weights[i] = weight;
			break;
		}
	}
}

void Model::ExtractBoneWeightForVertices(std::vector<MVertex>& vertices, aiMesh* mesh/*, const aiScene* scene*/) {
	for (uint boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
		int boneID = -1;
		string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) {
			BoneInfo newBoneInfo;
			newBoneInfo.id = m_BoneCounter;
			newBoneInfo.offset = toGlm(mesh->mBones[boneIndex]->mOffsetMatrix);
			m_BoneInfoMap[boneName] = newBoneInfo;
			boneID = m_BoneCounter;
			m_BoneCounter++;
		}
		else 
			boneID = m_BoneInfoMap[boneName].id;

		assert(boneID != -1);

		auto weights = mesh->mBones[boneIndex]->mWeights;
		int mNumWeights = mesh->mBones[boneIndex]->mNumWeights;
		for (int weightIndex = 0; weightIndex < mNumWeights; ++weightIndex) {
			int vertexID = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexID <= vertices.size());
			SetVertexBoneData(vertices[vertexID], boneID, weight);
		}
	}
}

void Model::Draw(const Shader& shader) {
	for (uint i = 0; i < meshes.size(); i++) meshes[i].Draw(shader);
}

void Model::loadModel(string path, bool flipUVs) {
	uint flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices
		| aiProcess_ImproveCacheLocality
		| aiProcess_RemoveRedundantMaterials;
	if (flipUVs) flags |= aiProcess_FlipUVs; // set flag
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, flags);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		cout << "loadModel() " << importer.GetErrorString() << endl;
		throw std::exception("failed to loadModel!");
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	// process all the node's meshes (if any)
	for (uint i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (uint i = 0; i < node->mNumChildren; i++)
		processNode(node->mChildren[i], scene);
}

MMesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	vector<MVertex> vertices;
	vector<uint> indices;
	vector<MTexture> textures;

	for (uint i = 0; i < mesh->mNumVertices; i++) {
		MVertex vertex;

		SetVertexBoneDataToDefault(vertex);

		vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

		if(mesh->HasNormals())
			vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

		if (mesh->HasTangentsAndBitangents()) {
			vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
			vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
		}
		
		// does the mesh contain texture coordinates?	
		if (mesh->mTextureCoords[0])
			vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);	
		else
			vertex.TexCoords = glm::vec2(.0f, .0f);

		// process vertex positions, normals and texture coordinates
		vertices.push_back(vertex);
	}
	// process indices
	for (uint i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (uint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}	
	// process material
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		vector<MTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		vector<MTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<MTexture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<MTexture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	ExtractBoneWeightForVertices(vertices, mesh);// , scene);

	return MMesh(vertices, indices, textures);
}

vector<MTexture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, string typeName) {
	vector<MTexture> textures;
	for (uint i = 0; i < material->GetTextureCount(type); i++) {
		aiString str;
		material->GetTexture(type, i, &str);
		bool skip = false;
		for (uint j = 0; j < textures_loaded.size(); j++) {
			if (strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {
			// if texture hasn't been loaded already, load it
			MTexture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture); // add to loaded textures
		}
	}
	return textures;
}

void Model::cleanup() {
	// .. no implement yet
}