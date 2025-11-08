// ----------------------------------------------------
// Roman.P
// Copyright (c) 2025.
// ----------------------------------------------------

// CRT's memory leak detection
#if defined(DEBUG) || defined(_DEBUG)
#include <crtdbg.h>
#endif

#include "shader.h"
//#include "camera.h"
#include "camera_ogldev.h"
//#include "Model.h"
//#include "basic_mesh.h"
//#include "skinned_mesh.h"
//#include "Animator.h"
//#include "SkeletalModel.h"
#include "StaticModel.h"

#pragma warning( disable : 4100 ) // unreferenced parameter

// global variables
//int WIDTH = 800, HEIGHT = 600;
int WIDTH = 1024, HEIGHT = 720;
//int WIDTH = 1920, HEIGHT = 1080;

Shader* gShaderBase;
//Texture *gTex0, *gTex1;
//GLint gScaleLocation = -1, gUColorTris = -1; // -1 means error

// stores how much we're seeing of either texture
float mixValue = 0.2f;
//float mixValue = 1.0f;

float deltaTime = 0;
float lastFrame = 0;

//Camera gCamera(WIDTH, HEIGHT, my::vec3(0, 30, 100));
//CameraOGLDEV GameCamera(WIDTH, HEIGHT, glm::vec3(-30,70,250), glm::vec3(0.0f, 0.0f, -1));
CameraOGLDEV GameCamera(WIDTH, HEIGHT, glm::vec3(0,0,10), glm::vec3(0.0f, 0.0f, -1));

glm::mat4 gmProj;

//Model* g_pModel;
//Animation* g_pAnimation;
//Animator* g_pAnimator;
//BasicMesh* gBaseMesh;
//SkeletalModel* pMySkelModel;
StaticModel* pStaticModel;
float blendFactor = 0.0f;

//int DisplayBoneIndex;
//const int MAX_BONES = 200;
//GLuint g_boneLocation[MAX_BONES];

double StartTimeMillis = 0;

void LoadTextures() {
	//gTex0 = new Texture("../media_files/textures/container.jpg", false, GL_TEXTURE1); gTex0->Load();
	//gTex0->setTexUnit(*gShaderBase,"texture_specular1", 1);
	//gTex1 = new Texture("../media_files/textures/awesomeface.png", true, GL_TEXTURE1, GL_RGBA);  gTex1->Load();
	//gTex1->setTexUnit(*gShaderBase, "texture_specular1", 1); 
}

void CompileShaders() {

	//gShaderBase = new Shader("../media_files/shaders/skintest.vert", "../media_files/shaders/skintest.frag");
	gShaderBase = new Shader("../media_files/shaders/dir_light.vert", "../media_files/shaders/dir_light.frag");
	//gScaleLocation = glGetUniformLocation(gShaderBase->m_ID, "gScale"); //if (gScaleLocation == -1) throw glsl_error("failed getting uniform variable!");
	//gUColorTris = glGetUniformLocation(g_pShaderBase->m_ID, "ourColor"); if (gUColorTris == -1) throw glsl_error("failed getting uniform variable!");
}

void InitGeo() {
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);
	//glCullFace(GL_FRONT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode on
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // wireframe mode off
	glEnable(GL_DEPTH_TEST);

	pStaticModel = new StaticModel;
	pStaticModel->Load("../media_files/models/vis_bones/box.fbx");
	//pMySkelModel = new SkeletalModel;
	//pMySkelModel->Load("../media_files/skeletalmeshes/Vanguard/Vanguard_Walking_in_place.fbx");
	//pMySkelModel->Load("../media_files/skeletalmeshes/iclone_7_raptoid_mascot_-_free_download.glb");
	//pMySkelModel->Load("../media_files/skeletalmeshes/boblampclean/boblampclean.md5mesh");
	//pMySkelModel->Load("../media_files/skeletalmeshes/vampire/dancing_vampire.dae");

	//GameCamera.SetSpeedMove(2.1f);
	GameCamera.SetSpeedMove(.2f);

	//gShaderBase->setInt("gDisplayBoneIndex", DisplayBoneIndex);
	//for (uint i = 0; i < MAX_BONES; i++) {
	//	char Name[128];
	//	memset(Name, 0, sizeof(Name));
	//	_snprintf_s(Name, sizeof(Name), "gBones[%d]", i);
	//	g_boneLocation[i] = glGetUniformLocation(gShaderBase->m_ID, Name);
	//}

	StartTimeMillis = glfwGetTime();
	
	gmProj = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, .1f, 1000.0f);
}

void Render() {
	glClearColor(0.12f, .18f, .2f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	double CurrentTimeMillis = glfwGetTime();
	double AnimationTimeSec = (CurrentTimeMillis - StartTimeMillis) / 1.0;

	gShaderBase->Use();

	// --------------------------------------------------------------
	glm::mat4 mModel(1.0f);
	//mModel = my::translate(mModel, my::vec3(0, 0, -2));
	//mModel = glm::rotate(mModel, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	//mModel = glm::scale(mModel, glm::vec3(.5f, .5f, .5f));
	glm::mat4 mView = GameCamera.GetMatrix();
	//glm::mat4 mPVM = gmProj * mView * mModel;

	//gShaderBase->setMat4("mPVM", mPVM);
	gShaderBase->setMat4("mModel", mModel);
	gShaderBase->setMat4("mView", mView);
	gShaderBase->setMat4("mProj", gmProj);

	//if(blendFactor > 0.0f) pMySkelModel->UpdateAnimBlended((float)AnimationTimeSec, 0, 3, blendFactor);
	//else pMySkelModel->UpdateAnim((float)AnimationTimeSec, 0);
	//const auto& bones = pMySkelModel->m_Bones;
	//for (int i = 0; i < bones.size(); i++) {
	//	gShaderBase->setMat4("gBones[" + std::to_string(i) + "]", bones[i].FinalTransform);
	//}
	//pMySkelModel->Render();

	static float rot = 0;
	rot += 0.5f;

	//glm::mat4 mRoot = glm::rotate(glm::mat4(1), glm::radians(0.0f), glm::vec3(0, 0, 1.0f));
	glm::mat4 mRoot = glm::rotate(glm::mat4(1), glm::radians(rot), glm::vec3(0, 0, 1.0f));
	//glm::mat4 mRoot = glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(0, 0, 1.0f));
	//glm::mat4 mRoot(1.0f);
	glm::mat4 mJoin1_Offset = glm::translate(glm::mat4(1), glm::vec3(3.092f, 0 ,0));
	mJoin1_Offset = glm::rotate(mJoin1_Offset, glm::radians(-rot), glm::vec3(0, 0, 1.0f));
	glm::mat4 mJoin2_Offset = glm::translate(glm::mat4(1), glm::vec3(3.107, 0, 0));
	glm::mat4 mJoin3_Offset = glm::translate(glm::mat4(1), glm::vec3(3.107, 0.5f, 0));

	gShaderBase->setMat4("mModel", mRoot);
	pStaticModel->Render();

	glm::mat4 Global = mRoot * mJoin1_Offset;
	gShaderBase->setMat4("mModel", Global);
	pStaticModel->Render();

	Global = Global * mJoin2_Offset;
	gShaderBase->setMat4("mModel", Global);
	pStaticModel->Render();

	Global = Global * mJoin3_Offset;
	gShaderBase->setMat4("mModel", Global);
	pStaticModel->Render();

	//glUniform1f(gScaleLocation, scale);
	//gShaderBase->setFloat("gScale", scale);
	//gShaderBase->setFloat("gTrans", mixValue);
}

void framebuffer_size_callback(GLFWwindow* wnd, int width, int height) {
	WIDTH = width; HEIGHT = height;
	glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	//gCamera.processScroll((float)yoffset);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	//gCamera.processMouse((float)xposIn, (float)yposIn);
	GameCamera.OnMouse((float)xposIn, (float)yposIn);
}

void Cleanup() {
	//safe_delete(g_pEBO);
	//safe_delete(g_pVBO);
	//safe_delete(g_pVAO);
	//safe_delete(gTex1);
	//safe_delete(gTex0);
	//safe_delete(pMySkelModel);
	safe_delete(gShaderBase);
}

GLFWwindow* InitWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* pWnd = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL: [ Represent Bones Transformations ]", nullptr, nullptr);

	//GLFWmonitor* primMonitor = glfwGetPrimaryMonitor();
	//const GLFWvidmode* mode = glfwGetVideoMode(primMonitor);
	//glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	//glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	//glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	//glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	//// window mode: if monitor = null, else fullscreen
	//GLFWwindow* pWnd = glfwCreateWindow(mode->width, mode->height, "OpenGL: [ ... ]", primMonitor, nullptr);

	if (!pWnd) throw glfw_error("failed to create GLFW Window!");

	//const GLFWvidmode* mode = glfwGetVideoMode(primMonitor);
	//glfwSetWindowMonitor(pWnd, primMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);

	glfwMakeContextCurrent(pWnd);
	glfwSetFramebufferSizeCallback(pWnd, framebuffer_size_callback);
	glfwSetCursorPosCallback(pWnd, mouse_callback);
	glfwSetScrollCallback(pWnd, scroll_callback);

	if(!gladLoadGL()) throw glfw_error("Failed to initialize GLAD!");

	glViewport(0, 0, WIDTH, HEIGHT);

	glfwSetInputMode(pWnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return pWnd;
}

void processInput(GLFWwindow* wnd) {
	float curFrame = (float)glfwGetTime();
	deltaTime = curFrame - lastFrame;
	lastFrame = curFrame;

	if (glfwGetKey(wnd, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(wnd, true);

	if (glfwGetKey(wnd, GLFW_KEY_Q) == GLFW_PRESS) {
		mixValue += 0.01f; // change this value accordingly (might be too slow or too fast based on system hardware)
		if (mixValue >= 1.0f) mixValue = 1.0f;
	}
	if (glfwGetKey(wnd, GLFW_KEY_E) == GLFW_PRESS) {
		mixValue -= 0.01f;
		if (mixValue <= 0.0f) mixValue = 0.0f;
	}

	//if (glfwGetKey(wnd, GLFW_KEY_W) == GLFW_PRESS) gCamera.processKeyboard(FORWARD, deltaTime);
	//if (glfwGetKey(wnd, GLFW_KEY_S) == GLFW_PRESS) gCamera.processKeyboard(BACKWARD, deltaTime);
	//if (glfwGetKey(wnd, GLFW_KEY_A) == GLFW_PRESS) gCamera.processKeyboard(LEFT, deltaTime);
	//if (glfwGetKey(wnd, GLFW_KEY_D) == GLFW_PRESS) gCamera.processKeyboard(RIGHT, deltaTime);
	//if (glfwGetKey(wnd, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) gCamera.Speed = 5.0f;
	//if (glfwGetKey(wnd, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) gCamera.Speed = 2.5f;

	//if (glfwGetMouseButton(wnd, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
	//	glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	//	double mouseX, mouseY;
	//	glfwGetCursorPos(wnd, &mouseX, &mouseY);
	//	glfwSetCursorPos(wnd, (WIDTH / 2), (HEIGHT / 2));
	//}
	
	if (glfwGetKey(wnd, GLFW_KEY_MINUS) == GLFW_PRESS) {
		blendFactor -= 0.005f; if (blendFactor < 0.0f) blendFactor = 0.0f;
	}
	if (glfwGetKey(wnd, GLFW_KEY_EQUAL) == GLFW_PRESS) {
		blendFactor += 0.005f; if (blendFactor > 1.0f) blendFactor = 1.0f;
	}

	//if (glfwGetKey(wnd, GLFW_KEY_MINUS) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_MINUS);
	//if (glfwGetKey(wnd, GLFW_KEY_EQUAL) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_EQUAL);
	if (glfwGetKey(wnd, GLFW_KEY_W) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_W);
	if (glfwGetKey(wnd, GLFW_KEY_S) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_S);
	if (glfwGetKey(wnd, GLFW_KEY_A) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_A);
	if (glfwGetKey(wnd, GLFW_KEY_D) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_D);
	if (glfwGetKey(wnd, GLFW_KEY_PAGE_UP) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_PAGE_UP);
	if (glfwGetKey(wnd, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_PAGE_DOWN);

	//if (glfwGetKey(wnd, GLFW_KEY_SPACE) == GLFW_PRESS) {
	//	DisplayBoneIndex++;
	//	DisplayBoneIndex = DisplayBoneIndex % gSkinnedMesh->NumBones();
	//	gShaderBase->setInt("gDisplayBoneIndex", DisplayBoneIndex);
	//}
}

int main(int argc, char* argv[]) try {
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	std::cout << ".............. start opengl app ..............\n";

	auto ShutDownApp = [](GLFWwindow* w) { Cleanup(); glfwDestroyWindow(w); glfwTerminate(); /*system("pause");*/ };
	using UWnd = std::unique_ptr<GLFWwindow, decltype(ShutDownApp)>;
	UWnd Wnd(InitWindow(), ShutDownApp);

	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl << "OpenGL version " << glGetString(GL_VERSION) << std::endl;
	std::cout << "..............................................\n";

	glViewport(0, 0, WIDTH, HEIGHT);

	CompileShaders();
	LoadTextures();
	InitGeo();

	while (!glfwWindowShouldClose(Wnd.get())) {
		processInput(Wnd.get());
		Render();

		glfwPollEvents();
		glfwSwapBuffers(Wnd.get());
	}

	std::cout << ".............. close opengl app ..............\n";
	return 0;
}
catch (glsl_error& e) { std::cout << "GLSL error: " << e.what() << std::endl; }
catch (glfw_error& e) { std::cout << "GLFW error: " << e.what() << std::endl; }
catch (std::exception& e) { std::cout << "Standard error: " << e.what() << std::endl; }
catch (...) { std::cout << "Unknown error: \n"; }