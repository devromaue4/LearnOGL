// ----------------------------------------------------
// Roman.P
// Copyright (c) 2025.
// ----------------------------------------------------

// CRT's memory leak detection
#if defined(DEBUG) || defined(_DEBUG)
#include <crtdbg.h>
#endif

#include <chrono>
#include <thread>
using namespace std::chrono_literals;

#include "shader.h"
//#include "camera.h"
#include "camera_ogldev.h"
//#include "skinned_mesh.h"
//#include "SkeletalModel.h"
#include "StaticModel.h"
#include "geometry.h"
#include "light.h"
#include "material.h"

#pragma warning( disable : 4100 ) // unreferenced parameter

// global variables
int WIDTH = 1280, HEIGHT = 720;

bool gFullScreen = false;
bool gWireframe = false;

std::shared_ptr<Shader> gShaderBase;
//std::shared_ptr<Shader> gSkinning;
//std::shared_ptr<Texture> gTex0, gTex1;
//GLint gScaleLocation = -1, gUColorTris = -1; // -1 means error

//Camera gCamera(WIDTH, HEIGHT, my::vec3(0, 30, 100));
//CameraOGLDEV GameCamera(WIDTH, HEIGHT, glm::vec3(-30,70,250), glm::vec3(0.0f, 0.0f, -1));
CameraOGLDEV GameCamera(WIDTH, HEIGHT, glm::vec3(0,10,50), glm::vec3(0.0f, 0.0f, -1));

std::shared_ptr<StaticModel> SM_Bunny;
std::shared_ptr<StaticModel> SM_Barrel;
std::shared_ptr<StaticModel> SM_Room;
std::shared_ptr<StaticModel> SM_Sphere;
std::shared_ptr<StaticModel> SM_Box;
std::shared_ptr<SBox> gBox;
//std::shared_ptr<SkeletalModel> pMySkelModel;

DirectLight gLight;
Material gMaterial;

glm::vec3 glightDir(25.0f, 10, 0.0f);

glm::mat4 mModel(1.0f);

//float blendFactor = 0.0f; // for blend animations
float deltaTime = 0;
float lastFrame = 0;
double StartTimeMillis = 0;

//int DisplayBoneIndex;
//const int MAX_BONES = 200;
//GLuint g_boneLocation[MAX_BONES];

void LoadTextures() {
	//gTex0 = std::make_shared<Texture>("../media_files/textures/brick.png"); gTex0->Load();
	//gTex1 = std::make_shared<Texture>("../media_files/textures/wall.jpg"); gTex1->Load();
	//gTex0 = std::make_shared<Texture>("../media_files/textures/Cobble2.tga", GL_TEXTURE0); gTex0->Load();
	//gTex0->setTexUnit(*gShaderBase,"texture_specular1", 1);
	//gTex1 = new Texture("../media_files/textures/awesomeface.png", true, GL_TEXTURE1, GL_RGBA);  gTex1->Load();
	//gTex1->setTexUnit(*gShaderBase, "texture_specular1", 1);
}

void CompileShaders() {
	gShaderBase = std::make_shared<Shader>("../media_files/shaders/dir_light.vert", "../media_files/shaders/dir_light.frag");
	//gSkinning = std::make_shared<Shader>("../media_files/shaders/skintest.vert", "../media_files/shaders/skintest.frag");
	//gScaleLocation = glGetUniformLocation(gShaderBase->m_ID, "gScale"); //if (gScaleLocation == -1) throw glsl_error("failed getting uniform variable!");
}

void InitGeo() {
	glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);
	//glCullFace(GL_FRONT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode on
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // wireframe mode off
	glEnable(GL_DEPTH_TEST);

	//scene.addModel(name, pos);
	gBox = std::make_shared<SBox>(3.f);
	SM_Barrel = std::make_shared<StaticModel>();
	SM_Bunny = std::make_shared<StaticModel>();
	SM_Room = std::make_shared<StaticModel>();
	SM_Sphere = std::make_shared<StaticModel>();
	SM_Box = std::make_shared<StaticModel>();
	SM_Bunny->Load("../media_files/models/misc/bunny.fbx");
	SM_Room->Load("../media_files/models/misc/room.fbx");
	SM_Sphere->Load("../media_files/models/misc/sphere.fbx");
	//SM_Barrel->Load("../media_files/models/wine_barrel/wine_barrel_01.fbx");
	//SM_Barrel->Load("../media_files/models/wine_barrel/barrel_01.obj");
	SM_Barrel->Load("../media_files/models/antique_ceramic_vase/antique_ceramic_vase_01.fbx");
	SM_Box->Load("../media_files/models/misc/box.fbx");
	
	//pMySkelModel = std::make_shared<SkeletalModel>();
	//pMySkelModel->Load("../media_files/skeletalmeshes/iclone_7_raptoid_mascot_-_free_download.glb");
	//pMySkelModel->Load("../media_files/skeletalmeshes/Vanguard/Vanguard_Walking_in_place.fbx");
	//pMySkelModel->Load("../media_files/skeletalmeshes/boblampclean/boblampclean.md5mesh");
	//pMySkelModel->Load("../media_files/skeletalmeshes/vampire/dancing_vampire.dae");

	GameCamera.SetSpeedMove(1.2f);

	//gShaderBase->setInt("gDisplayBoneIndex", DisplayBoneIndex);
	//for (uint i = 0; i < MAX_BONES; i++) {
	//	char Name[128];
	//	memset(Name, 0, sizeof(Name));
	//	_snprintf_s(Name, sizeof(Name), "gBones[%d]", i);
	//	g_boneLocation[i] = glGetUniformLocation(gShaderBase->m_ID, Name);
	//}

	StartTimeMillis = glfwGetTime();
}

void Render() {
	//glClearColor(0.12f, .18f, .2f, 1);
	//glClearColor(0.1f, .1f, .1f, 1);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	//double CurrentTimeMillis = glfwGetTime();
	//double AnimationTimeSec = (CurrentTimeMillis - StartTimeMillis) / 1.0;
	
	mModel = glm::translate(mModel, glm::vec3(-18, 0, 0));
	mModel = glm::rotate(mModel, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	float scaleBunny = 8.0f;
	mModel = glm::scale(mModel, glm::vec3(scaleBunny, scaleBunny, scaleBunny));
	const glm::mat4& mView = GameCamera.GetMatrix();
	const glm::mat4& mProj = GameCamera.GetProjMatrix();

	// ------------------------- skeletal model -------------------------------------
	//gSkinning->Use();
	//gSkinning->setMat4("mModel", mModel);
	//gSkinning->setMat4("mView", mView);
	//gSkinning->setMat4("mProj", mProj);

	//if(blendFactor > 0.0f) pMySkelModel->UpdateAnimBlended((float)AnimationTimeSec, 0, 3, blendFactor);
	//else pMySkelModel->UpdateAnim((float)AnimationTimeSec, 0);
	//const auto& bones = pMySkelModel->m_Bones;
	//for (int i = 0; i < bones.size(); i++)
	//	gSkinning->setMat4("gBones[" + std::to_string(i) + "]", bones[i].FinalTransform);
	//pMySkelModel->Render();

	// --------------------------- static models -----------------------------------
	
	gShaderBase->Use();
	gShaderBase->setVec3("camPos", GameCamera.GetPosition());
	gShaderBase->setVec3("gDirLight.Direction", glightDir);

	gLight.m_AmbientIntesity = .1f;
	gLight.DiffuseIntesity = 1.0f;
	gMaterial.AmbientColor = glm::vec3(1.0f, 1.0f, 1.f);
	gMaterial.DiffuseColor = glm::vec3(1.0f, 1.0f, 1.f);
	gMaterial.SpecularColor = glm::vec3(1.0f, 1.0f, 1.f);
	gLight.m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
	gShaderBase->setVec3("gMaterial.AmbientColor", gMaterial.AmbientColor);
	gShaderBase->setVec3("gMaterial.DiffuseColor", gMaterial.DiffuseColor);
	gShaderBase->setVec3("gMaterial.SpecularColor", gMaterial.SpecularColor);
	gShaderBase->setVec3("gDirLight.Color", gLight.m_Color);
	gShaderBase->setFloat("gDirLight.AmbientIntensity", gLight.m_AmbientIntesity);
	gShaderBase->setFloat("gDirLight.DiffuseIntensity", gLight.DiffuseIntesity);

	gShaderBase->setMat4("mModel", mModel);
	gShaderBase->setMat4("mView", mView);
	gShaderBase->setMat4("mProj", mProj);

	// bunny
	SM_Bunny->Render();

	// reset
	mModel = glm::mat4(1);

	// box
	//static float rot = 0;;
	//rot += 0.5f;
	//mModel = glm::rotate(mModel, glm::radians(rot), glm::vec3(0, 1.0f, 0));
	//mModel = glm::scale(mModel, glm::vec3(10, 10, 10));
	//gShaderBase->setMat4("mModel", mModel);
	//SM_Box->Render();

	// reset
	//mModel = glm::mat4(1);

	//gShaderBase->setMat4("mModel", mModel);
	//SM_Room->Render();

	// sphere
	mModel = glm::translate(mModel, glm::vec3(25, 10, -10));
	gShaderBase->setMat4("mModel", mModel);
	SM_Sphere->Render();

	// reset
	mModel = glm::mat4(1);

	// barrel
	static float rot = 0;;
	rot += 0.5f;
	float scaleModel = .5f;
	//mModel = glm::translate(mModel, glm::vec3(15, 0, 0));
	mModel = glm::rotate(mModel, glm::radians(rot), glm::vec3(0, 1.0f, 0));
	mModel = glm::scale(mModel, glm::vec3(scaleModel, scaleModel, scaleModel));
	gShaderBase->setMat4("mModel", mModel);
	SM_Barrel->Render();

	// reset
	mModel = glm::mat4(1);

	// light box
	gBox->Render(mProj, mView, glm::translate(glm::mat4(1), glightDir));

	//std::this_thread::sleep_for(5ms);
	//std::this_thread::sleep_for(20ms);
}

void framebuffer_size_callback(GLFWwindow* wnd, int width, int height) {
	WIDTH = width; HEIGHT = height;
	if(WIDTH > 0 && HEIGHT > 0)
		GameCamera.SetProjParams((float)WIDTH / HEIGHT);
	glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	//gCamera.processScroll((float)yoffset);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	//gCamera.processMouse((float)xposIn, (float)yposIn);
	GameCamera.OnMouse((float)xposIn, (float)yposIn);
}

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_F:
			gWireframe = !gWireframe;
			glPolygonMode(GL_FRONT_AND_BACK, gWireframe ? GL_LINE : GL_FILL); // wireframe mode on
			break;
		default:
			break;
		}
	}
}

void Cleanup() {}

GLFWwindow* InitWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* pWnd = nullptr;

	if(gFullScreen) {
		GLFWmonitor* primMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primMonitor);
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

		WIDTH = mode->width, HEIGHT = mode->height;
		// window mode: if monitor = null, else fullscreen
		pWnd = glfwCreateWindow(mode->width, mode->height, "OpenGL: [ ... ]", primMonitor, nullptr);
	}
	else {
		pWnd = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL: [ Lighting ]", nullptr, nullptr);
		glfwSetWindowPos(pWnd, 300, 180);
	}

	if (!pWnd) throw glfw_error("failed to create GLFW Window!");

	//const GLFWvidmode* mode = glfwGetVideoMode(primMonitor);
	//glfwSetWindowMonitor(pWnd, primMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);

	glfwMakeContextCurrent(pWnd);
	glfwSetFramebufferSizeCallback(pWnd, framebuffer_size_callback);
	glfwSetCursorPosCallback(pWnd, mouse_callback);
	glfwSetScrollCallback(pWnd, scroll_callback);
	glfwSetKeyCallback(pWnd, keyboard_callback);

	if(!gladLoadGL()) throw glfw_error("Failed to initialize GLAD!");

	// set vsync true
	glfwSwapInterval(1);

	glViewport(0, 0, WIDTH, HEIGHT);

	glfwSetInputMode(pWnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return pWnd;
}

void processInput(GLFWwindow* wnd) {
	float curFrame = (float)glfwGetTime();
	deltaTime = curFrame - lastFrame;
	lastFrame = curFrame;

	if (glfwGetKey(wnd, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(wnd, true);


	//if (glfwGetKey(wnd, GLFW_KEY_Q) == GLFW_PRESS) {
	//	mixValue += 0.01f; // change this value accordingly (might be too slow or too fast based on system hardware)
	//	if (mixValue >= 1.0f) mixValue = 1.0f;
	//}
	//if (glfwGetKey(wnd, GLFW_KEY_E) == GLFW_PRESS) {
	//	mixValue -= 0.01f;
	//	if (mixValue <= 0.0f) mixValue = 0.0f;
	//}

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
	
	//if (glfwGetKey(wnd, GLFW_KEY_MINUS) == GLFW_PRESS)
	//	blendFactor -= 0.005f; if (blendFactor < 0.0f) blendFactor = 0.0f;
	//if (glfwGetKey(wnd, GLFW_KEY_EQUAL) == GLFW_PRESS)
	//	blendFactor += 0.005f; if (blendFactor > 1.0f) blendFactor = 1.0f;

	float lightSpeed = 0.3f;
	if (glfwGetKey(wnd, GLFW_KEY_MINUS) == GLFW_PRESS) glightDir.y += lightSpeed;
	if (glfwGetKey(wnd, GLFW_KEY_EQUAL) == GLFW_PRESS) glightDir.y -= lightSpeed;

	//if (glfwGetKey(wnd, GLFW_KEY_MINUS) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_MINUS);
	//if (glfwGetKey(wnd, GLFW_KEY_EQUAL) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_EQUAL);
	if (glfwGetKey(wnd, GLFW_KEY_W) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_W);
	if (glfwGetKey(wnd, GLFW_KEY_S) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_S);
	if (glfwGetKey(wnd, GLFW_KEY_A) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_A);
	if (glfwGetKey(wnd, GLFW_KEY_D) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_D);
	if (glfwGetKey(wnd, GLFW_KEY_PAGE_UP) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_PAGE_UP);
	if (glfwGetKey(wnd, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_PAGE_DOWN);

	if (glfwGetKey(wnd, GLFW_KEY_UP) == GLFW_PRESS) glightDir.z -= lightSpeed;
	if (glfwGetKey(wnd, GLFW_KEY_DOWN) == GLFW_PRESS) glightDir.z += lightSpeed;
	if (glfwGetKey(wnd, GLFW_KEY_LEFT) == GLFW_PRESS) glightDir.x -= lightSpeed;
	if (glfwGetKey(wnd, GLFW_KEY_RIGHT) == GLFW_PRESS) glightDir.x += lightSpeed;
	

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

	if (argc > 1) {
		std::string cmdLine(argv[1]);
		if (cmdLine.find("-f") != std::string::npos) gFullScreen = true;
	}

	std::cout << ".............. start opengl app ..............\n";

	auto ShutDownApp = [](GLFWwindow* w) { Cleanup(); glfwDestroyWindow(w); glfwTerminate(); /*system("pause");*/ };
	using UWnd = std::unique_ptr<GLFWwindow, decltype(ShutDownApp)>;
	UWnd Wnd(InitWindow(), ShutDownApp);

	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl << "OpenGL version " << glGetString(GL_VERSION) << std::endl;
	std::cout << "..............................................\n";

	glViewport(0, 0, WIDTH, HEIGHT);

	auto start = std::chrono::high_resolution_clock::now();
	CompileShaders();
	LoadTextures();
	InitGeo();
	auto end = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	auto duration = std::chrono::duration<double, std::milli>(end - start); // high_res
	log("Loading took: " << duration.count() << " ms");

	while (!glfwWindowShouldClose(Wnd.get())) {
		processInput(Wnd.get());
		//start = std::chrono::high_resolution_clock::now();
		Render();
		//end = std::chrono::high_resolution_clock::now();
		//duration = std::chrono::duration<double, std::milli>(end - start);
		//log("Render: " << duration.count() << " ms");

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