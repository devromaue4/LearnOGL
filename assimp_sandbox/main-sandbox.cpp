// ----------------------------------------------------
// Roman.P
// Copyright (c) 2025.
// ----------------------------------------------------
#include "../src/types.h"

#include <iostream>
#include <map>
#include <vector>
#include <assert.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#pragma comment (lib, "assimp-vc143-mt.lib")

#define array_num_elems(a)		(sizeof(a)/sizeof(a[0]))

#define MAX_NUM_BONES_PER_VERTEX 4

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices)

struct VertexBoneData {
	uint BoneIDs[MAX_NUM_BONES_PER_VERTEX] = {};
	float Weights[MAX_NUM_BONES_PER_VERTEX] = {};

	VertexBoneData() {}

	void AddBoneData(uint BoneID, float Weight) {
		for (uint i = 0; i < array_num_elems(BoneIDs); i++) {
			if (Weights[i] == 0.0f) {
				BoneIDs[i] = BoneID;
				Weights[i] = Weight;
				//std::cout << " bone " << BoneID << " weight " << Weight << " index " << i << endl;
				return;
			}
		}

		// should never get here - more bones than we have space for
		assert(0);
	}
};

std::vector<VertexBoneData>	vertex_to_bones;
std::vector<int>			mesh_base_vertex;
std::map<std::string, uint>	bone_name_to_index_map;

static int space_count = 0;

void print_space() {
	for (int i = 0; i < space_count; i++)
		printf(" ");
}

void print_assimp_matrix(const aiMatrix4x4& m) {
	print_space(); printf("%f %f %f %f\n", m.a1, m.a2, m.a3, m.a4);
	print_space(); printf("%f %f %f %f\n", m.b1, m.b2, m.b3, m.b4);
	print_space(); printf("%f %f %f %f\n", m.c1, m.c2, m.c3, m.c4);
	print_space(); printf("%f %f %f %f\n", m.d1, m.d2, m.d3, m.d4);
}

int get_bone_id(const aiBone* pBone) {
	int bone_id = 0;
	string bone_name(pBone->mName.C_Str());
	if (bone_name_to_index_map.find(bone_name) == bone_name_to_index_map.end()) {
		// allocate an index for new bone
		bone_id = (int)bone_name_to_index_map.size();
		bone_name_to_index_map[bone_name] = bone_id;
	}
	else bone_id = bone_name_to_index_map[bone_name];

	return bone_id;
}

void parse_single_bone(int mesh_index, const aiBone* pBone) {
	std::cout << "\n Bone " << pBone->mName.C_Str() << ": " << " num vertices affected by this bone: " << pBone->mNumWeights << std::endl;

	int bone_id = get_bone_id(pBone);
	//cout << "bone " << bone_id << endl;

	print_assimp_matrix(pBone->mOffsetMatrix);

	for (unsigned int i = 0; i < pBone->mNumWeights; i++) {
		if (i == 0) std::cout << std::endl;
		const aiVertexWeight& vw = pBone->mWeights[i];

		uint global_vertex_id = mesh_base_vertex[mesh_index] + vw.mVertexId;
		//std::cout << " vertex id " << global_vertex_id;

		assert(global_vertex_id < vertex_to_bones.size());
		vertex_to_bones[global_vertex_id].AddBoneData(bone_id, vw.mWeight);
	}
	std::cout << std::endl;
}

void parse_mesh_bones(int mesh_index, const aiMesh* pMesh) {
	for (unsigned int i = 0; i < pMesh->mNumBones; i++)
		parse_single_bone(mesh_index, pMesh->mBones[i]);
}

void parse_meshes(const aiScene* pScene) {
	std::cout << "****************************************************************\n";
	std::cout << "Parsing " << pScene->mNumMeshes << " meshes\n\n";

	int total_vertices = 0;
	int total_indices = 0;
	int total_bones = 0;

	mesh_base_vertex.resize(pScene->mNumMeshes);

	for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
		const aiMesh* pMesh = pScene->mMeshes[i];
		int num_vertices = pMesh->mNumVertices;
		int num_indices = pMesh->mNumFaces * 3;
		int num_bones = pMesh->mNumBones;

		mesh_base_vertex[i] = total_vertices;

		std::cout << "Mesh " << i << " " << pMesh->mName.C_Str() << ": vertices " << num_vertices << " indices " << num_indices << " bones " << num_bones << std::endl;
		total_vertices += num_vertices;
		total_indices += num_indices;
		total_bones += num_bones;

		vertex_to_bones.resize(total_vertices);

		if (pMesh->HasBones()) parse_mesh_bones(i, pMesh);

		std::cout << std::endl;
	}

	std::cout << "\nTotal vertices " << total_vertices << " total indices " << total_indices << " total bones " << total_bones << std::endl;
}

void parse_node(const aiNode* pNode) {
	print_space(); printf(" Node name: '%s' num children %d num meshes %d\n", pNode->mName.C_Str(), pNode->mNumChildren, pNode->mNumMeshes);
	print_space(); printf(" Node transformation:\n");
	print_assimp_matrix(pNode->mTransformation);

	space_count += 4;

	for (uint i = 0; i < pNode->mNumChildren; i++) {
		printf("\n");
		print_space(); printf("--- %d ---\n", i);
		parse_node(pNode->mChildren[i]);
	}

	space_count -= 4;
}

void parse_heirarchy(const aiScene* pScene) {
	std::cout << "\n****************************************************************\n"
		"Parsing the node heirarchy\n";

	parse_node(pScene->mRootNode);
}

void parse_scene(const aiScene* pScene) {
	parse_meshes(pScene);

	parse_heirarchy(pScene);
}

int main(int argc, char* argv[]) try {
	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << "<model filename>\n";
		return 1;
	}

	char* filename = argv[1];

	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(filename, ASSIMP_LOAD_FLAGS);
	if (!pScene) {
		std::cout << "Error: " << importer.GetErrorString() << std::endl;
		throw std::exception("Assimp::Failed!");
	}

	parse_scene(pScene);
	system("pause");

	return 0;
}
catch (std::exception& e) { std::cout << "Standard error: " << e.what() << std::endl; }
catch (...) { std::cout << "Unknown error: \n"; }