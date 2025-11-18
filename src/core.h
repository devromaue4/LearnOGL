#pragma once

#include "types.h"

#include <iostream>
#include <vector>
#include <map>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
//#include <glm/vec3.hpp> // glm::vec3
//#include <glm/vec4.hpp> // glm::vec4
//#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

//#include "mymath.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glfw3.lib")
#pragma comment (lib, "assimp-vc143-mt.lib")

struct glfw_error : public std::runtime_error { glfw_error(const char* s) : std::runtime_error(s) {} };

void error_callback(int, const char* err_str);

struct glsl_error : public std::runtime_error { std::string shaderLog; glsl_error(const char* s) : std::runtime_error(s) {} };

struct glsl_compile_error : glsl_error {
	glsl_compile_error(const char* s, GLuint shaderID);
};

#define FINL __forceinline // !!! this should be used only in critical places found by PROFILER
#define INL inline

#define log(s) std::cout << s << '\n'
#define log_error(s) std::cerr << "Error: " << s << '\n'

#define safe_delete_array(p)	{ if(p) { delete [] p; } }
#define safe_delete(p)			{ if(p) { delete p; } }
#define array_num_elems(a)		(sizeof(a)/sizeof(a[0]))

