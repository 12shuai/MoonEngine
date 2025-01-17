﻿#include "PreInclude.h"
//#define MOON_DEBUG_MODE

// global settings ------------------------------------------------
const char *title = u8"MoonEngine - v0.20 朔";

Vector2 MOON_WndSize = Vector2(1600, 900);
Vector2 MOON_ScrSize = Vector2(800, 450);
float SceneManager::aspect = MOON_ScrSize.x / MOON_ScrSize.y;
SceneView SceneManager::activeView = top;
Vector2 MOON_OutputSize = Vector2(600, 300);
float Renderer::aspect = MOON_OutputSize.x / MOON_OutputSize.y;
Vector4 Graphics::clearColor(0.45f, 0.55f, 0.60f, 1.00f);
Vector4 grdLineColor(0.8f, 0.8f, 0.8f, 1.0f);

float Camera::MouseSensitivity = 0.025f;
// ----------------------------------------------------------------

int main() {
	std::cout << "starting moon engine... ..." << std::endl;

	// glfw window creation
	GLFWwindow* window = InitWnd();
	if (window == NULL) return -1;

	// engine resources initialization
	MOON_InitEngine();

	// test objects ------------------------------------------------------------------------
	Model* teapot = MOON_ModelManager::LoadModel("Assets\\Models\\teapot.obj", false);
	teapot->transform.Scale(Vector3(0.2f, 0.2f, 0.2f));
	MoonMtl* pbr = dynamic_cast<MoonMtl*>(teapot->meshList[0]->material);
	pbr->glossiness = Vector3::ONE() * 0.4f; pbr->reflectW.x = 1.0f; pbr->illumination = Vector3::ZERO();

	//Model* rabbit = MOON_ModelManager::LoadModel("Assets\\Models\\bunny.obj");
	//rabbit->transform.Scale(Vector3::ONE() * 5.0f);
	//rabbit->transform.Translate(Vector3::WORLD(LEFT) * 2.0f);

	Model* sphere = MOON_ModelManager::CreateSmartMesh(SmartMesh::sphere, "sphere");
	sphere->meshList[0]->material = MOON_MaterialManager::GetItem("PBRMat");
	//sphere->transform.Translate(Vector3::WORLD(UP) + Vector3::WORLD(LEFT) * 2.0f);
	sphere->transform.Translate(Vector3::WORLD(UP) + Vector3::WORLD(UP) * 3.25f);
	sphere->transform.Rotate(Quaternion::Rotate(Vector3(1, 0, 0), 90 * Deg2Rad));

	// test half mesh
	/*HalfMesh* edMesh = new HalfMesh(sphere->meshList[0]);
	ReleaseVector(sphere->meshList);
	sphere->meshList.push_back(edMesh);*/

	/*Model* boxes = MOON_ModelManager::LoadModel("Assets\\Models\\box_stack.obj");
	boxes->transform.Translate(Vector3::WORLD(UP));
	boxes->transform.Translate(Vector3::WORLD(RIGHT) * 1.0f);*/
	//boxes->transform.Rotate(Quaternion(Vector3(0, 0, 45)));

	/*Shape* sp = MOON_ShapeManager::CreateShape(line, "spline");
	sp->splineList[0].knotNum = 5;
	sp->splineList[0].Update();
	sp->splineList[0].knotList[2].setValue(1, 1, 0);
	sp->splineList[0].knotList[3].setValue(0, 1, 0);
	sp->splineList[0].knotList[4].setValue(0, 0, 0);
	sp->transform.Translate(Vector3::WORLD(RIGHT) * 4.0f);
	sp->transform.Translate(Vector3::WORLD(UP));
	sp->transform.Rotate(Quaternion(Vector3(0, 90, 0)));*/

	// duplicate random spheres
	/*for (int i = 0; i < 10; i++) {
		Model* dup = MOON_ModelManager::CreateSmartMesh(SmartMesh::sphere, "sphere_" + std::to_string(i));
		dup->transform.Translate(Vector3(
			MoonMath::RandomRange(-10, 10),
			MoonMath::RandomRange(-10, 10),
			MoonMath::RandomRange(-10, 10)
		));
		dup->transform.Scale(Vector3::ONE() * MoonMath::RandomRange(0.5, 2.0));
		dup->meshList[0]->material = MOON_MaterialManager::GetItem("PBRMat");
	}*/

	//sp->transform.SetParent(&boxes->transform);
	//rabbit->transform.SetParent(&sp->transform);

	/*Helper* dum = MOON_HelperManager::CreateHelper(dummy, "dummy");
	dum->transform.Translate(Vector3(0, 2, -3));*/

	/*MOON_VolumeManager::CreateVolume(
		"cloud", false, Vector3::ZERO(), 
		Vector3::ONE() * -5, Vector3::ONE() * 5
	)->source = Volume::SourceType::Galaxy;*/

	MOON_LightManager::CreateLight(point_light, "pointLight_01", Vector3(-10.0f, 10.0f, 10.0f));
	MOON_LightManager::CreateLight(point_light, "pointLight_02", Vector3(10.0f, 10.0f, 10.0f));
	MOON_LightManager::CreateLight(point_light, "pointLight_03", Vector3(-10.0f, -10.0f, 10.0f));
	MOON_LightManager::CreateLight(point_light, "pointLight_04", Vector3(10.0f, -10.0f, 10.0f));

	//Graphics::clearColor = Color::BLACK();
	Graphics::enviroment = env_hdri;
	//Graphics::enviroment = env_procedural_sky;
	//Graphics::postStack.push_back(new FXAA());
	//Graphics::postStack.push_back(new DepthOfField());
	Graphics::postStack.push_back(new Bloom());
	//Graphics::postStack.push_back(new Flare());
	Graphics::postStack.push_back(new ToneMapping());
	//Graphics::postStack.push_back(new Chromatic());
	Graphics::postStack.push_back(new Vignette());
	//Graphics::postStack[0]->enabled = false;
	//Graphics::postStack[1]->enabled = false;
	//Graphics::postStack[4]->enabled = false;
	// -------------------------------------------------------------------------------------
	std::cout << "done." << std::endl;

	// main loop  --------------------------------------------------------------------------
	while (!glfwWindowShouldClose(window)) {
		// per-frame time logic
		MOON_UpdateClock();

		// configure global opengl state ------------------------------------------
		//glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		//glDepthMask(GL_TRUE); // is depth mask writable
		//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		// Start the Dear ImGui frame -----------------------------------------------
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// draw Scene View ----------------------------------------------------------
		Graphics::process = sys_draw_scene;
		Graphics::SetShadingMode(DEFAULT);
		MOON_TextureManager::UpdateEnvironmentTex();
		Graphics::DrawSceneView(persp);
		Graphics::DrawSceneView(front);
		Graphics::DrawSceneView(top);
		Graphics::DrawSceneView(left);

		// schedule all coroutines --------------------------------------------------
		Graphics::process = sys_coroutine;
		MOON_CoroutineLoop();

		// clear background
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// draw main UI ------------------------------------------------------------
		Graphics::process = sys_draw_ui;
		MOON_DrawMainUI();

		// process user input ------------------------------------------------------
		HotKeyManager::MOON_InputProcessor(window);

		// Rendering UI ------------------------------------------------------------
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		MOON_InputManager::ResetKeyState();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup  ----------------------------------------------------------------------------
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	MOON_CleanUp();

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

// glfw: initialize and configure ----------------------------------------------------------
GLFWwindow* InitWnd() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(MOON_WndSize.x, MOON_WndSize.y, title, NULL, NULL);

	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		window = NULL;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		window = NULL;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	ImGui::StyleBlack();
	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	// glsl_version can be replaced with string "#version 150"
	ImGui_ImplOpenGL3_Init("#version 150");

	// add fonts
	MainUI::io = &ImGui::GetIO();
	MainUI::style = &ImGui::GetStyle();

	// merge custom fonts
	LoadIcons(MainUI::io);

	MainUI::io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	MainUI::io->ConfigDockingWithShift = true;
	MainUI::io->ConfigWindowsMoveFromTitleBarOnly = true;

	return window;
}

void LoadIcons(ImGuiIO *io) {
	// add simplified Chinese
	MainUI::io->Fonts->AddFontFromFileTTF(
		"./Resources/msyh.ttc",
		16.0f,
		NULL,
		MainUI::io->Fonts->GetGlyphRangesChineseSimplifiedCommon()
	);

	ImFontConfig config;
	config.MergeMode = true;
	config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced

	// merge FontAwesome
	// https://fontawesome.dashgame.com/.
	static const ImWchar icon_ranges_fa[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	io->Fonts->AddFontFromFileTTF(
		"./Resources/fontawesome-webfont.ttf",
		13.0f,
		&config,
		icon_ranges_fa
	);

	// merge Fontaudio
	/*static const ImWchar icon_ranges_fad[] = { ICON_MIN_FAD, ICON_MAX_FAD, 0 };
	io->Fonts->AddFontFromFileTTF(
		"./Resources/fontaudio.ttf",
		13.0f,
		&config,
		icon_ranges_fad
	);*/

	// merge kenney icons
	/*static const ImWchar icon_ranges_ki[] = { ICON_MIN_KI, ICON_MAX_KI, 0 };
	io->Fonts->AddFontFromFileTTF(
		"./Resources/kenney-icon-font.ttf",
		13.0f,
		&config,
		icon_ranges_ki
	);*/

	// merge MaterialIcons
	// https://www.wp2.cn/material_icons/.
	static const ImWchar icon_ranges_md[] = { ICON_MIN_MD, ICON_MAX_MD, 0 };
	io->Fonts->AddFontFromFileTTF(
		"./Resources/MaterialIcons-Regular.ttf",
		13.0f,
		&config,
		icon_ranges_md
	);
}

// glfw callbacks --------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	//SceneManager::SetWndSize(width, height);
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	/*if (MOON_MousePos[0] == -2.0f) MOON_MousePos.setValue(xpos, ypos);

	float xoffset = xpos - MOON_MousePos.x;
	float yoffset = MOON_MousePos.y - ypos;

	MOON_MousePos.setValue(xpos, ypos);
	MOON_InputManager::mouseOffset.setValue(xoffset, yoffset);*/
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	MOON_InputManager::mouseButton = button;
	MOON_InputManager::mouseAction = action;
	MOON_InputManager::mouseMods = mods;

	if (action == GLFW_PRESS)
		switch (button) {
			case GLFW_MOUSE_BUTTON_LEFT:
				MOON_InputManager::mouse_left_hold = true;
				break;
			case GLFW_MOUSE_BUTTON_MIDDLE:
				MOON_InputManager::mouse_middle_hold = true;
				break;
			case GLFW_MOUSE_BUTTON_RIGHT:
				MOON_InputManager::mouse_right_hold = true;
				break;
			default:
				return;
		} else
	if (action == GLFW_RELEASE)
		switch (button) {
			case GLFW_MOUSE_BUTTON_LEFT:
				MOON_InputManager::mouse_left_hold = false;
				break;
			case GLFW_MOUSE_BUTTON_MIDDLE:
				MOON_InputManager::mouse_middle_hold = false;
				break;
			case GLFW_MOUSE_BUTTON_RIGHT:
				MOON_InputManager::mouse_right_hold = false;
				break;
			default:
				return;
		}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	MOON_InputManager::mouseScrollOffset.setValue(xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	MOON_InputManager::keyButton = key;
	MOON_InputManager::keyAction = action;
	MOON_InputManager::keyMods = mods;

	//if (key == GLFW_KEY_M && action == GLFW_PRESS)
	//	MainUI::show_material_editor = !MainUI::show_material_editor;
	//// Gizmo --------------------------------------------------------------------------
	//if (key == GLFW_KEY_Q) Gizmo::gizmoMode = GizmoMode::none;
	//if (key == GLFW_KEY_W) Gizmo::gizmoMode = GizmoMode::translate;
	//if (key == GLFW_KEY_E) Gizmo::gizmoMode = GizmoMode::rotate;
	//if (key == GLFW_KEY_R) Gizmo::gizmoMode = GizmoMode::scale;
	//if (key == GLFW_KEY_Z) {
	//	if (MOON_InputManager::selection.size() > 0) {
	//		if (SceneManager::GetType(MOON_Selection(0))._Equal("Model"))
	//			MOON_CurrentCamera->CatchTarget(dynamic_cast<Model*>(MOON_Selection(0)));
	//		else MOON_CurrentCamera->CatchTarget(NULL);
	//	} else MOON_CurrentCamera->CatchTarget(NULL);
	//}
	//
	//// Escape -------------------------------------------------------------------------
	//if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);
}

// moon callbacks --------------------------------------------------------------------------
void MOON_CleanUp() {
	std::cout << "Do Clean..." << std::endl;
	std::cout << "Wait all thread exit..." << std::endl;
	ThreadPool::WaitAllThreadExit();
	ThreadPool::Clean();
	std::cout << "All thread exit." << std::endl;
	Renderer::Clear();
	std::cout << "Output render buffer released." << std::endl;
	Graphics::Clear();
	std::cout << "Post-Processing stack released." << std::endl;
	Gizmo::ReleaseDummyMap();
	Gizmo::DeleteVirtualDummy();
	SceneManager::ReleaseBVH();
	std::cout << "Virtual dummy deleted." << std::endl;
	MOON_PlotManager::Release();
	std::cout << "PlotManager cleared." << std::endl;
	MOON_NeuralNetworkManager::Release();
	std::cout << "NNManager cleared." << std::endl;
	MOON_InputManager::Clear();
	std::cout << "InputManager cleared." << std::endl;
	MOON_LightManager::Clear();
	std::cout << "LightManager cleared." << std::endl;
	MOON_MaterialManager::Clear();
	std::cout << "MaterialManager cleared." << std::endl;
	MOON_ShaderManager::Clear();
	std::cout << "ShaderManager cleared." << std::endl;
	MOON_TextureManager::Clear();
	std::cout << "TextureManager cleared." << std::endl;
	MOON_ShapeManager::Clear();
	std::cout << "ShapeManager cleared." << std::endl;
	MOON_HelperManager::Clear();
	std::cout << "HelperManager cleared." << std::endl;
	MOON_ModelManager::Clear();
	std::cout << "ModelManager cleared." << std::endl;
	MOON_CameraManager::Clear();
	std::cout << "CameraManager cleared." << std::endl;
	MainUI::CleanUp();
	std::cout << "MainUI resources cleared." << std::endl;
	SceneManager::Clear();
	std::cout << "SceneManager cleared." << std::endl;
	AssetLoader::CleanUp();
	std::cout << "AssetLoader cleared." << std::endl;
	std::cout << "Done." << std::endl;
}

void MOON_UpdateClock() {
	float currentFrame = glfwGetTime();
	MOON_Clock::deltaTime = currentFrame - MOON_Clock::lastFrame;
	MOON_Clock::lastFrame = currentFrame;
}

void MOON_CoroutineLoop() {
	auto poolSize = Coroutine::co_pool.size();
	for (auto& it : Coroutine::co_pool) {
		if (poolSize != Coroutine::co_pool.size()) {
			MOON_CoroutineLoop(); break;
		}
		if (it.first && it.second->get_state() != CoroutineState::term) it.second->resume();
	}
}

void MOON_GenerateGround(const float &space, const int &gridCnt) {
	Graphics::ground.clear();
	// generate Graphics::ground points
	for (int i = -gridCnt; i <= gridCnt; i++) {
		Graphics::ground.push_back(i * space);
		Graphics::ground.push_back(0.0);
		Graphics::ground.push_back(gridCnt * -space);

		Graphics::ground.push_back(i * space);
		Graphics::ground.push_back(0.0);
		Graphics::ground.push_back(gridCnt * space);

		Graphics::ground.push_back(gridCnt * -space);
		Graphics::ground.push_back(0.0);
		Graphics::ground.push_back(i * space);

		Graphics::ground.push_back(gridCnt * space);
		Graphics::ground.push_back(0.0);
		Graphics::ground.push_back(i * space);
	}
}

void MOON_DrawMainUI() {
	MainUI::ShowDockSpace(true);
	if (VFB::show)						VFB::Draw();
	if (MainUI::show_ribbon)			MainUI::RibbonBar();
	if (MainUI::show_timeline)			MainUI::ShowTimeline();
	if (MainUI::show_control_window)	MainUI::ControlPanel();
	if (MainUI::show_preference_window)	MainUI::PreferencesWnd();
	if (MainUI::show_about_window)		MainUI::AboutWnd();
	if (MainUI::show_scene_window)		MainUI::SceneWnd();
	if (MainUI::show_inspector_window)	MainUI::InspectorWnd();
	if (MainUI::show_explorer_window)	MainUI::ExplorerWnd();
	if (MainUI::show_console_window)	MainUI::ConsoleWnd();
	if (MainUI::show_project_window)	MainUI::ProjectWnd();
	if (MainUI::show_create_window)		MainUI::CreateWnd();
	if (MainUI::show_enviroment_editor) MainUI::EnvironmentWnd();
	if (MainUI::show_code_editor)		MainUI::CodeEditor();
	if (MainUI::show_material_editor)	MainUI::MaterialEditorWnd();
	if (MainUI::show_render_setting)	MainUI::RenderSettingWnd();
	if (MainUI::show_profiler)			MainUI::Profiler();
	if (MainUI::show_nn_manager)		MainUI::NNManagerWnd();
	if (MainUI::show_demo_window)		ImGui::ShowDemoWindow(&MainUI::show_demo_window);
	MainUI::RightClickMenu();
	MainUI::DrawPlotWnds();

	// update output image realtime while rendering
	if (Renderer::progress && !Renderer::prevInQueue) {
		VFB::UpdateOutput();
		if (Renderer::progress < 0) {
			Renderer::progress = 0;
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

void MOON_InitEngine() {
	std::cout << "- Max CPU thread num: " << MAX_THREADSNUM << std::endl;
	std::cout << "- Loading Assets..." << std::endl;
	AssetLoader::BuildDirTree(".\\Assets");
	std::cout << "- Dir Tree Created." << std::endl;
	SceneManager::Init();
	std::cout << "- Scene Manager Initialized." << std::endl;
	NN::NNManager::Init();
	std::cout << "- Neural Network Manager Initialized." << std::endl;
	MOON_GenerateGround(1.0, 5);
	MOON_TextureManager::CreateBuffers();
	std::cout << "- IDLUT Created." << std::endl;
	OperatorManager::LoadNativeOperators();
	std::cout << "- Native Operators Loaded." << std::endl;
	Gizmo::CreateVirtualDummy();
	std::cout << "- Virtual Dummy Created." << std::endl;
}
