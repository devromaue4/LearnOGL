// ----------------------------------------------------
// Roman.P
// Copyright (c) 2025.
// ----------------------------------------------------

// CRT's memory leak detection
#if defined(DEBUG) || defined(_DEBUG)
#include <crtdbg.h>
#endif

#include <chrono>
//#include <thread>
using namespace std::chrono_literals;

#include "shader.h"
#include "camera_euler.h"
//#include "camera_quat.h"
#include "StaticModel.h"
#include "geometry.h"
#include "light.h"

#pragma warning( disable : 4100 ) // unreferenced parameter

// global variables
int WIDTH = 1280, HEIGHT = 720;

bool gFullScreen = false;
bool gWireframe = false;
bool gUseTextures = true;

std::shared_ptr<Shader> gShaderBase;
//std::shared_ptr<Texture> gTex0, gTex1;

CameraEuler GameCamera(WIDTH, HEIGHT, my::vec3(0, 10.f, 50.f));
//CameraQuat GameCamera(WIDTH, HEIGHT, glm::vec3(0,10,50), glm::vec3(0.0f, 0.0f, -1));

std::shared_ptr<StaticModel> SM_Bunny;
std::shared_ptr<StaticModel> SM_Barrel;
std::shared_ptr<StaticModel> SM_Room;
std::shared_ptr<StaticModel> SM_Sphere;
std::shared_ptr<SBox> gBox;

constexpr int MAX_POINT_LIGHTS = 3;

DirectLight gLight;
PointLight gPointLights[MAX_POINT_LIGHTS];
Material gMaterial;

my::vec3 glightDir(25.0f, 10, 0.0f);

glm::mat4 mModel(1.0f);

float deltaTime = 0;
float lastFrame = 0;

void LoadTextures() {
	//gTex0 = std::make_shared<Texture>("../media_files/textures/brick.png"); gTex0->Load();
	//gTex1 = std::make_shared<Texture>("../media_files/textures/wall.jpg"); gTex1->Load();
	//gTex0 = std::make_shared<Texture>("../media_files/textures/Cobble2.tga", GL_TEXTURE0); gTex0->Load();
	//gTex0->setTexUnit(*gShaderBase,"texture_specular1", 1);
	//gTex1 = new Texture("../media_files/textures/awesomeface.png", true, GL_TEXTURE1, GL_RGBA);  gTex1->Load();
	//gTex1->setTexUnit(*gShaderBase, "texture_specular1", 1);
}

void CompileShaders() {
	gShaderBase = std::make_shared<Shader>("../media_files/shaders/light.vert", "../media_files/shaders/light.frag");
	//gScaleLocation = glGetUniformLocation(gShaderBase->m_ID, "gScale"); //if (gScaleLocation == -1) throw glsl_error("failed getting uniform variable!");
}

void InitGeo() {
	glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);

	//scene.addModel(name, pos);
	gBox = std::make_shared<SBox>(3.f);
	SM_Barrel = std::make_shared<StaticModel>();
	SM_Bunny = std::make_shared<StaticModel>();
	SM_Room = std::make_shared<StaticModel>();
	SM_Sphere = std::make_shared<StaticModel>();
	SM_Bunny->Load("../media_files/models/misc/bunny.fbx");
	SM_Room->Load("../media_files/models/misc/room.fbx");
	SM_Sphere->Load("../media_files/models/misc/sphere.fbx");
	//SM_Barrel->Load("../media_files/models/wine_barrel/wine_barrel_01.fbx");
	SM_Barrel->Load("../media_files/models/antique_ceramic_vase/antique_ceramic_vase_01.fbx");

	//GameCamera.SetSpeedMove(1.2f);
	GameCamera.Speed = 50.2f;

	////////////////////////////////////////////////////////////////////
	gPointLights[0].m_WorldPos = my::vec3(12.f, 10.f, 0);
	gPointLights[1].m_WorldPos = my::vec3(-10.f, 10.f, 15.f);
	gPointLights[2].m_WorldPos = my::vec3(-10.f, 10.f, -15.f);
	// pont lights
	gPointLights[0].m_DiffuseIntesity = 1.0f;
	gPointLights[0].m_Color = glm::vec3(1.0f, 1.f, 0.f);
	gPointLights[0].Attenuation.Linear = 0.2f; // def
	gPointLights[0].Attenuation.Exp = 0.f;

	//////////////////////////////////////////////////////////////////////

	gPointLights[1].m_DiffuseIntesity = 1.0f;
	gPointLights[1].m_Color = glm::vec3(.0f, 1.f, 1.f);
	gPointLights[1].Attenuation.Linear = 0.0f; // def
	//gPointLights[1].Attenuation.Exp = 0.2f; // def
	gPointLights[1].Attenuation.Exp = 0.02f;
	////////////////////////////////////////////////////////////////////
	gPointLights[2].m_DiffuseIntesity = 1.0f;
	gPointLights[2].m_Color = glm::vec3(1.0f, 0.f, 1.f);
	gPointLights[2].Attenuation.Linear = 0.2f; // def
	gPointLights[2].Attenuation.Exp = 0.0f;
	////////////////////////////////////////////////////////////////////
}

void Render() {
	//glClearColor(0.12f, .18f, .2f, 1);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	
	mModel = glm::translate(mModel, glm::vec3(-18, 0, 0));
	mModel = glm::rotate(mModel, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	float scaleBunny = 8.0f;
	mModel = glm::scale(mModel, glm::vec3(scaleBunny, scaleBunny, scaleBunny));
	const my::mat4& mView = GameCamera.getMat();
	const my::mat4& mProj = GameCamera.getProj();

	// --------------------------- static models -----------------------------------
	
	gShaderBase->Use();
	gShaderBase->setVec3("gCamPos", GameCamera.Pos);
	gShaderBase->setVec3("gDirLight.Direction", glightDir);

	gShaderBase->setBool("gUseTextures", gUseTextures);

	// direction light
	gLight.m_AmbientIntesity = .1f;
	gLight.m_DiffuseIntesity = .2f;
	gMaterial.AmbientColor = glm::vec3(1.0f, 1.0f, 1.f);
	gMaterial.DiffuseColor = glm::vec3(1.0f, 1.0f, 1.f);
	gMaterial.SpecularColor = glm::vec3(1.0f, 1.0f, 1.f);
	gLight.m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
	gShaderBase->setVec3("gMaterial.AmbientColor", gMaterial.AmbientColor);
	gShaderBase->setVec3("gMaterial.DiffuseColor", gMaterial.DiffuseColor);
	gShaderBase->setVec3("gMaterial.SpecularColor", gMaterial.SpecularColor);
	gShaderBase->setVec3("gDirLight.Base.Color", gLight.m_Color);
	gShaderBase->setFloat("gDirLight.Base.AmbientIntensity", gLight.m_AmbientIntesity);
	gShaderBase->setFloat("gDirLight.Base.DiffuseIntensity", gLight.m_DiffuseIntesity);

	////////////////////////////////////////////////////////////////////
	// set point lights
	gShaderBase->setInt("gNumPointLights", MAX_POINT_LIGHTS);

	for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
		gShaderBase->setVec3("gPointLights[" + std::to_string(i) + "].Base.Color", gPointLights[i].m_Color);
		gShaderBase->setFloat("gPointLights[" + std::to_string(i) + "].Base.AmbientIntensity", gPointLights[i].m_AmbientIntesity);
		gShaderBase->setFloat("gPointLights[" + std::to_string(i) + "].Base.DiffuseIntensity", gPointLights[i].m_DiffuseIntesity);
		gShaderBase->setVec3("gPointLights[" + std::to_string(i) + "].LocalPos", gPointLights[i].m_WorldPos);
		gShaderBase->setFloat("gPointLights[" + std::to_string(i) + "].Atten.Constant", gPointLights[i].Attenuation.Constant);
		gShaderBase->setFloat("gPointLights[" + std::to_string(i) + "].Atten.Linear", gPointLights[i].Attenuation.Linear);
		gShaderBase->setFloat("gPointLights[" + std::to_string(i) + "].Atten.Exp", gPointLights[i].Attenuation.Exp);
	}
	////////////////////////////////////////////////////////////////////

	gShaderBase->setMat4("mModel", mModel);
	gShaderBase->setMat4("mView", mView);
	gShaderBase->setMat4("mProj", mProj);

	// bunny
	SM_Bunny->Render();

	// reset
	mModel = glm::mat4(1);

	gShaderBase->setMat4("mModel", mModel);
	SM_Room->Render();

	// sphere
	mModel = glm::translate(mModel, glm::vec3(25, 10, -10));
	gShaderBase->setMat4("mModel", mModel);
	SM_Sphere->Render();

	// reset
	mModel = glm::mat4(1);

	// barrel
	static float rot = 0;
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
	gBox->Render(mProj, mView, my::translate(my::mat4(1), glightDir));
	gBox->Render(mProj, mView, my::translate(my::mat4(1), gPointLights[0].m_WorldPos));
	gBox->Render(mProj, mView, my::translate(my::mat4(1), gPointLights[1].m_WorldPos));
	gBox->Render(mProj, mView, my::translate(my::mat4(1), gPointLights[2].m_WorldPos));

	//std::this_thread::sleep_for(5ms);
}

void framebuffer_size_callback(GLFWwindow* wnd, int width, int height) {
	WIDTH = width; HEIGHT = height;
	if(WIDTH > 0 && HEIGHT > 0)
		GameCamera.setProj((float)WIDTH / HEIGHT);
	glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	//gCamera.processScroll((float)yoffset);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	GameCamera.processMouse((float)xposIn, (float)yposIn);
	//GameCamera.OnMouse((float)xposIn, (float)yposIn);
}

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_F:
			gWireframe = !gWireframe;
			glPolygonMode(GL_FRONT_AND_BACK, gWireframe ? GL_LINE : GL_FILL); // wireframe mode on
			break;
		case GLFW_KEY_T:
			gUseTextures = !gUseTextures;
			break;
		default:
			break;
		}
	}
}

void processInput(GLFWwindow* wnd) {
	float curFrame = (float)glfwGetTime();
	deltaTime = curFrame - lastFrame;
	lastFrame = curFrame;

	if (glfwGetKey(wnd, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(wnd, true);

	////if (glfwGetKey(wnd, GLFW_KEY_MINUS) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_MINUS);
	////if (glfwGetKey(wnd, GLFW_KEY_EQUAL) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_EQUAL);
	//if (glfwGetKey(wnd, GLFW_KEY_W) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_W);
	//if (glfwGetKey(wnd, GLFW_KEY_S) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_S);
	//if (glfwGetKey(wnd, GLFW_KEY_A) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_A);
	//if (glfwGetKey(wnd, GLFW_KEY_D) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_D);
	////if (glfwGetKey(wnd, GLFW_KEY_PAGE_UP) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_PAGE_UP);
	////if (glfwGetKey(wnd, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) GameCamera.OnKeyboard(GLFW_KEY_PAGE_DOWN);

	if (glfwGetKey(wnd, GLFW_KEY_W) == GLFW_PRESS) GameCamera.processKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(wnd, GLFW_KEY_S) == GLFW_PRESS) GameCamera.processKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(wnd, GLFW_KEY_A) == GLFW_PRESS) GameCamera.processKeyboard(LEFT, deltaTime);
	if (glfwGetKey(wnd, GLFW_KEY_D) == GLFW_PRESS) GameCamera.processKeyboard(RIGHT, deltaTime);
	//if (glfwGetKey(wnd, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) gCamera.Speed = 5.0f;
	//if (glfwGetKey(wnd, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) gCamera.Speed = 2.5f;

	//if (glfwGetMouseButton(wnd, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
	//	glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	//	double mouseX, mouseY;
	//	glfwGetCursorPos(wnd, &mouseX, &mouseY);
	//	glfwSetCursorPos(wnd, (WIDTH / 2), (HEIGHT / 2));
	//}

	float atten_step = 0.01f;
	float atten_exp_step = 0.001f;
	if (glfwGetKey(wnd, GLFW_KEY_MINUS) == GLFW_PRESS) {
		gPointLights[0].Attenuation.Linear -= atten_step;
		gPointLights[1].Attenuation.Linear -= atten_step;

		//if (gPointLights[0].Attenuation.Linear <= 0.0f) gPointLights[0].Attenuation.Linear = 0.0f;
		//if (gPointLights[1].Attenuation.Linear <= 0.0f) gPointLights[1].Attenuation.Linear = 0.0f;

		log(gPointLights[1].Attenuation.Linear);
	}
	if (glfwGetKey(wnd, GLFW_KEY_EQUAL) == GLFW_PRESS) {
		gPointLights[0].Attenuation.Linear += atten_step;
		gPointLights[1].Attenuation.Linear += atten_step;

		//if (gPointLights[0].Attenuation.Linear >= 2.0f) gPointLights[0].Attenuation.Linear = 2.0f;
		//if (gPointLights[1].Attenuation.Linear >= 2.0f) gPointLights[1].Attenuation.Linear = 2.0f;

		log(gPointLights[1].Attenuation.Linear);
	}

	if (glfwGetKey(wnd, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
		gPointLights[0].Attenuation.Exp += atten_exp_step;
		gPointLights[1].Attenuation.Exp += atten_exp_step;
		log("Exp: " << gPointLights[1].Attenuation.Exp);
	}
	if (glfwGetKey(wnd, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
		gPointLights[0].Attenuation.Exp -= atten_exp_step;
		gPointLights[1].Attenuation.Exp -= atten_exp_step;

		if (gPointLights[0].Attenuation.Exp <= 0.0f) gPointLights[0].Attenuation.Exp = 0.0f;
		if (gPointLights[1].Attenuation.Exp <= 0.0f) gPointLights[1].Attenuation.Exp = 0.0f;
		log("Exp: " << gPointLights[1].Attenuation.Exp);
	}

	float lightStep = 0.3f;
	//if (glfwGetKey(wnd, GLFW_KEY_UP) == GLFW_PRESS) glightDir.z -= lightStep;
	//if (glfwGetKey(wnd, GLFW_KEY_DOWN) == GLFW_PRESS) glightDir.z += lightStep;
	//if (glfwGetKey(wnd, GLFW_KEY_LEFT) == GLFW_PRESS) glightDir.x -= lightStep;
	//if (glfwGetKey(wnd, GLFW_KEY_RIGHT) == GLFW_PRESS) glightDir.x += lightStep;
	//if (glfwGetKey(wnd, GLFW_KEY_MINUS) == GLFW_PRESS) glightDir.y += lightStep;
	//if (glfwGetKey(wnd, GLFW_KEY_EQUAL) == GLFW_PRESS) glightDir.y -= lightStep;
	 
	if (glfwGetKey(wnd, GLFW_KEY_UP) == GLFW_PRESS) gPointLights[1].m_WorldPos.z -= lightStep;
	if (glfwGetKey(wnd, GLFW_KEY_DOWN) == GLFW_PRESS) gPointLights[1].m_WorldPos.z += lightStep;
	if (glfwGetKey(wnd, GLFW_KEY_LEFT) == GLFW_PRESS) gPointLights[1].m_WorldPos.x -= lightStep;
	if (glfwGetKey(wnd, GLFW_KEY_RIGHT) == GLFW_PRESS) gPointLights[1].m_WorldPos.x += lightStep;
	if (glfwGetKey(wnd, GLFW_KEY_MINUS) == GLFW_PRESS) gPointLights[1].m_WorldPos.y += lightStep;
	if (glfwGetKey(wnd, GLFW_KEY_EQUAL) == GLFW_PRESS) gPointLights[1].m_WorldPos.y -= lightStep;

	//if (glfwGetKey(wnd, GLFW_KEY_SPACE) == GLFW_PRESS) {
	//	DisplayBoneIndex++;
	//	DisplayBoneIndex = DisplayBoneIndex % gSkinnedMesh->NumBones();
	//	gShaderBase->setInt("gDisplayBoneIndex", DisplayBoneIndex);
	//}
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
		pWnd = glfwCreateWindow(WIDTH, HEIGHT, "Lighting: [ Multiple Point Lights ]", nullptr, nullptr);
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