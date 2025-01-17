#pragma once
#pragma region headers
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <shellapi.h>
#include <cstdlib>
#include <string.h>
#include <algorithm>
#include <string>
#include <shlobj.h>
#include <commDlg.h>

#include "Renderer.h"
#include "SceneMgr.h"
#include "SmartMeshes.h"
#include "HotkeyMgr.h"
#include "ThreadPool.h"
#include "Coroutine.h"
#include "Texture.h"
#include "Gizmo.h"
#include "Utility.h"
#include "Plotter.h"
#include "ImTimeline.h"
#include "ImGradient.h"
#include "FlowMenu.h"
#include "NGraph.h"
#include "NNManager.h"
#include "CodeEditor.h"
#include "ButtonEx.h"
#include "Icons.h"
#include "StackWindow.h"
#include "WinDiagHandler.h"
#include "AssetLoader.h"
#include "MaterialEditor.h"
#pragma endregion
#pragma warning(disable:4996)

namespace MOON {
	class MainUI {
	public:
	#pragma region parameters
		static bool sceneWndFocused;

		// image resources
		static Texture *icon, *logo, *logoFull;

		// ImGui IO
		static ImGuiIO* io;
		static ImGuiStyle* style;

		// bool for wnds
		static bool show_control_window;
		static bool show_demo_window;
		static bool show_preference_window;
		static bool show_about_window;

		static bool show_explorer_window;
		static bool show_scene_window;
		static bool show_inspector_window;
		static bool show_project_window;
		static bool show_console_window;

		static bool show_create_window;
		static bool show_material_editor;
		static bool show_enviroment_editor;

		static bool show_code_editor;
		static bool show_ribbon;
		static bool show_timeline;

		static bool show_curve_editor;
		static bool show_operator_editor;

		static bool show_profiler;
		static bool show_render_setting;

		static bool show_nn_manager;
		static bool show_right_click_menu;

		// class-like wnds
		static MaterialEditor matEditor;
		static CodeEditor CEditor;

		// VFB parameters
		static std::string VFB_History_Path;
	#pragma endregion

		static void CleanUp() {
			//matEditor.CleanUp();
		}

		// Right click popup wnd
		static void RightClickMenu() {
			if (show_right_click_menu) {
				// legacy menu
				//ImGui::OpenPopup("QuadMenu");
				ImGui::OpenPopup("FlowMenu");
				show_right_click_menu = false;
			}
			MOON_FlowMenu::FlowMenu();
		}

		// window definition
		static void MainMenu() {
			if (icon == NULL) icon = MOON_TextureManager::GetItem("moon_icon");
			ImGui::Image((void*)(intptr_t)icon->localID, ImVec2(20, 20));

			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New")) {}
				if (ImGui::MenuItem("Open", "Ctrl+O")) {}
				if (ImGui::BeginMenu("Open Recent")) {
					ImGui::MenuItem("test_1.moon");
					ImGui::MenuItem("test_2.moon");
					if (ImGui::BeginMenu("More...")) {
						ImGui::MenuItem("test_3.moon");
						ImGui::MenuItem("test_4.moon");
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Save", "Ctrl+S")) {}
				if (ImGui::MenuItem("Save As...")) {}

				ImGui::Separator();
				if (ImGui::BeginMenu("Import")) {
					if (ImGui::MenuItem("Model...")) {
						std::string path = WinDiagMgr::FileDialog();
						std::cout << "Selected file: " << path << std::endl;
						MOON_ModelManager::LoadModel(path);
						RegistStackWnd("Loading", StackWndType::PROGRESS);
					}
					if (ImGui::MenuItem("Scene...")) {
						std::cout << "Selected file: " << WinDiagMgr::OpenFolder() << std::endl;
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Export")) {
					if (ImGui::MenuItem("Export All...")) {
						std::cout << "Selected folder: " << WinDiagMgr::OpenFolder() << std::endl;
					}
					if (ImGui::MenuItem("Export Selected...")) {
						std::cout << "Selected folder: " << WinDiagMgr::OpenFolder() << std::endl;
					}
					ImGui::EndMenu();
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Quit", "Alt+F4")) {
					SceneManager::exitFlag = true;
				}

				ImGui::EndMenu();
			}
			{
				ProgressWnd("Loading", &OBJLoader::info, &OBJLoader::progress);
			}
			if (ImGui::BeginMenu("Edit")) {
				if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
				if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "CTRL+X")) {}
				if (ImGui::MenuItem("Copy", "CTRL+C")) {}
				if (ImGui::MenuItem("Paste", "CTRL+V")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("Preferences", NULL, MainUI::show_preference_window)) {
					MainUI::show_preference_window = !MainUI::show_preference_window;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Animation")) {
				if (ImGui::BeginMenu("Constraints")) {
					if (ImGui::MenuItem("IK")) {}
					if (ImGui::MenuItem("Look At")) {}
					if (ImGui::MenuItem("Position")) {}
					if (ImGui::MenuItem("Rotation")) {}
					ImGui::EndMenu();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Bone Tools")) {}
				if (ImGui::MenuItem("Emotionless")) {}
				if (ImGui::MenuItem("Retargeting")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Simulation")) {
				if (ImGui::MenuItem("Fire")) {}
				if (ImGui::MenuItem("Fluid")) {}
				if (ImGui::MenuItem("Fracture")) {}
				ImGui::Separator();
				if (ImGui::BeginMenu("MoonGrav")) {
					if (ImGui::MenuItem("Rigibody")) {}
					if (ImGui::MenuItem("Cloth")) {}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Crowd")) {
					if (ImGui::MenuItem("Create Crowd")) {}
					if (ImGui::MenuItem("Character Editor")) {}
					ImGui::EndMenu();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("ParticleSys", "6")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Rabbit")) {
				if (ImGui::MenuItem("Enviroment", "8", MainUI::show_enviroment_editor)) {
					MainUI::show_enviroment_editor = !MainUI::show_enviroment_editor;
				}
				ImGui::Separator();
				if (ImGui::MenuItem("VFB Window", NULL, VFB::show)) {
					VFB::show = !VFB::show;
				}
				if (ImGui::MenuItem("Render", "F9")) {
					VFB::show = true;
					Renderer::StartRendering();
				}
				if (ImGui::MenuItem("Settings", "F10", MainUI::show_render_setting)) {
					MainUI::show_render_setting = !MainUI::show_render_setting;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("MoonNN")) {
				if (ImGui::MenuItem("NN Graph", "Ctrl+N", MainUI::show_nn_manager)) {
					MainUI::show_nn_manager = !MainUI::show_nn_manager;
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Plot Manager")) {

				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Script")) {
				if (ImGui::MenuItem("Code Editor", NULL, MainUI::show_code_editor)) {
					MainUI::show_code_editor = !MainUI::show_code_editor;
					if (MainUI::show_code_editor) MainUI::show_console_window = true;
				}
				if (ImGui::MenuItem("Console Window", NULL, MainUI::show_console_window)) {
					MainUI::show_console_window = !MainUI::show_console_window;
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Plugin Manager", NULL)) {
					
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Utility")) {
				if (ImGui::MenuItem("Baker", "0")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window")) {
				if (ImGui::BeginMenu("Layout")) {
					if (ImGui::MenuItem("Default")) {}
					if (ImGui::MenuItem("Modeling")) {}
					if (ImGui::MenuItem("Animation")) {}
					if (ImGui::MenuItem("Rendering")) {}
					if (ImGui::MenuItem("FX")) {}
					ImGui::EndMenu();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Control Panel", NULL, MainUI::show_control_window)) {
					MainUI::show_control_window = !MainUI::show_control_window;
				}
				if (ImGui::MenuItem("Profiler", NULL, MainUI::show_profiler)) {
					MainUI::show_profiler = !MainUI::show_profiler;
				}
				if (ImGui::MenuItem("Explorer Window", NULL, MainUI::show_explorer_window)) {
					MainUI::show_explorer_window = !MainUI::show_explorer_window;
				}
				if (ImGui::MenuItem("Inspector Window", NULL, MainUI::show_inspector_window)) {
					MainUI::show_inspector_window = !MainUI::show_inspector_window;
				}
				if (ImGui::MenuItem("Scene Window", NULL, MainUI::show_scene_window)) {
					MainUI::show_scene_window = !MainUI::show_scene_window;
				}
				if (ImGui::MenuItem("Project Window", NULL, MainUI::show_project_window)) {
					MainUI::show_project_window = !MainUI::show_project_window;
				}
				if (ImGui::MenuItem("Material Editor", NULL, MainUI::show_material_editor)) {
					MainUI::show_material_editor = !MainUI::show_material_editor;
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Ribbon", NULL, MainUI::show_ribbon)) {
					MainUI::show_ribbon = !MainUI::show_ribbon;
				}
				if (ImGui::MenuItem("Timeline", NULL, MainUI::show_timeline)) {
					MainUI::show_timeline = !MainUI::show_timeline;
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Demo Window", NULL, MainUI::show_demo_window)) {
					MainUI::show_demo_window = !MainUI::show_demo_window;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help")) {
				if (ImGui::MenuItem("Document", "F11")) {}
				if (ImGui::MenuItem("About", "F12")) {
					MainUI::show_about_window = !MainUI::show_about_window;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		static void ShowDockSpace(bool p_open) {
			static bool opt_fullscreen_persistant = true;
			bool opt_fullscreen = opt_fullscreen_persistant;
			static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			if (opt_fullscreen) {
				ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->GetWorkPos());
				ImGui::SetNextWindowSize(viewport->GetWorkSize());
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			}

			if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
				window_flags |= ImGuiWindowFlags_NoBackground;

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace", &p_open, window_flags);
			ImGui::PopStyleVar();

			if (opt_fullscreen) ImGui::PopStyleVar(2);

			// DockSpace
			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}

			if (ImGui::BeginMenuBar()) MainMenu();

			ImGui::End();
		}

		static void ControlPanel() {
			ImGui::Begin(Icon_Name_To_ID(ICON_FA_COGS, " ControlPanel"));

			if (ImGui::Button("Test Btn")) {
				SceneManager::BuildBVH();
			}

			if (SceneManager::sceneBVH != nullptr) {
				SceneManager::sceneBVH->Draw();

				for (auto& it : MOON_ModelManager::itemMap) {
					if (!it.second->visible) continue;
					else if (CheckType(it.second->opstack.GetDeliver(), "Model"))
						((Model*)it.second)->DebugBVH();
				}
			}

			// gradient editor
			static ImGradient gradient;

			static ImGradientMark* draggingMark = nullptr;
			static ImGradientMark* selectedMark = nullptr;

			bool updated = ImGui::GradientEditor(&gradient, draggingMark, selectedMark);

			if (ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered() && !draggingMark) {
				selectedMark = nullptr;
			}

			//float color[3];
			//gradient.getColorAt(0.3f, color); //position from 0 to 1

			//gradient.getMarks().clear();
			//gradient.addMark(0.0f, ImColor(0xA0, 0x79, 0x3D));
			//gradient.addMark(0.4f, ImColor(0xBE, 0x97, 0x5B));
			//gradient.addMark(0.8f, ImColor(0xDC, 0xB5, 0x79));
			//gradient.addMark(1.0f, ImColor(0xE6, 0xBF, 0x83));
			// end gradient editor

			ImGui::End();
		}

		static void RenderSettingWnd() {
			ImGui::Begin(Icon_Name_To_ID(ICON_FA_COGS, " RendererSettings"), &MainUI::show_render_setting);

			float availWidth = ImGui::GetContentRegionAvailWidth() / 2.4f;
			static int width = MOON_OutputSize.x, height = MOON_OutputSize.y;

			ImGui::Text("Output Size:");
			ImGui::SetNextItemWidth(availWidth);
			ButtonEx::InputIntNoLabel("output_width", &width, 100, 1000, 0);
			ImGui::SameLine();
			ImGui::Text(u8"×");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(availWidth);
			ButtonEx::InputIntNoLabel("output_height", &height, 100, 1000, 0);

			if (MOON_OutputSize.x != width || MOON_OutputSize.y != height) {
				if (width < 1) width = 1; if (height < 1) height = 1;
				Renderer::SetOutputSize(width, height);
			}

			ImGui::Text("Sampling Rate:");
			ButtonEx::InputIntNoLabel("samplint_rate", (int*)&Renderer::samplingRate, 1, 10, 0);

			ImGui::Text("Max Reflection Depth:");
			ButtonEx::InputIntNoLabel("ref_depth", (int*)&Renderer::maxReflectionDepth, 1, 5, 0);

			if (Renderer::renderMode == Renderer::RenderMode::bucket) {
				ImGui::Text("Bucket Size:");
				ButtonEx::InputIntNoLabel("bucketSize", &Renderer::bucketSize, 50, 100, 0);
			}

			const static char* accitems[] = { "NONE", "AABB", "BVH", "BSP", "KDTree" };
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Ray Acceleration:");
			ButtonEx::ComboNoLabel("accType", (int*)&Renderer::acc, accitems, IM_ARRAYSIZE(accitems));

			const static char* filteritems[] = { "Nearest", "Bilinear", "Trilinear", "Anisotropic" };
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Texture Filter:");
			ButtonEx::ComboNoLabel("filterType", (int*)&Renderer::filter, filteritems, IM_ARRAYSIZE(filteritems));

			const static char* renderModeList[] = { "Scanline", "Bucket", "Progressive" };
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Rendering Mode:");
			ButtonEx::ComboNoLabel("renderMode", (int*)&Renderer::renderMode, renderModeList, IM_ARRAYSIZE(renderModeList));

			const static char* sampleModeList[] = { "BruteForce", "PhotonMapping", "LightCache", "IrradianceMap" };
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Sampling Mode:");
			ButtonEx::ComboNoLabel("sampleMode", (int*)&Renderer::sampleMode, sampleModeList, IM_ARRAYSIZE(sampleModeList));

			ImGui::Checkbox("Depth of field ", &Renderer::depth);
			ImGui::Checkbox("Motion blur ", &Renderer::motion);
			ImGui::Checkbox("Importance sampling ", &Renderer::useIS);
			ImGui::Checkbox("Auto update preview ", &Material::autoPrevUpdate);

			ImGui::Spacing();
			if (ImGui::Button("Rendering")) {
				VFB::show = true;
				Renderer::StartRendering();
			}

			ImGui::End();
		}

		static void Profiler() {
			ImGui::Begin(Icon_Name_To_ID(ICON_FA_BAR_CHART, " Profiler"), &MainUI::show_profiler, ImGuiWindowFlags_NoResize);

			ImGui::Text("[Statistics]");
			ImGui::Text("FPS: %.1f (%.2f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
			ImGui::Text("ObjCount: %d", MOON_ObjectNumber);
			ImGui::Text("Hover UI: %d", MOON_InputManager::isHoverUI);

			ImGui::Separator();

			ImGui::End();
		}

		static void PreferencesWnd() {
			ImGui::Begin(Icon_Name_To_ID(ICON_FA_COG, " Preferences"), &MainUI::show_preference_window);
			if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None)) {
				if (ImGui::BeginTabItem("Graphics")) {
					ImGui::Spacing(); ImGui::AlignTextToFramePadding();
					ImGui::Text("Shadow:");
					ImGui::Indent(10.0f);
					ImGui::Text("Enable"); ImGui::SameLine(80);
					ButtonEx::CheckboxNoLabel("enableShadow", &Graphics::enableShadow);
					ImGui::SameLine(0, 10.0f);
					ImGui::Text("Cascade"); ImGui::SameLine(0, 20.0f);
					ButtonEx::CheckboxNoLabel("CSM", &Graphics::cascadeShadow);
					ImGui::Text("Distance:"); ImGui::SameLine(80);
					if (Graphics::cascadeShadow) {
						ButtonEx::DragVec4NoLabel("csmDist", &Graphics::shadowDistance[0], 1.0f, 0, 0, "%.1f", 1.0f);
					} else {
						ButtonEx::DragFloatNoLabel("shadowDist", &Graphics::shadowDistance[0], 1.0f, 0, 0, "%.1f", 1.0f);
					}
					ImGui::Unindent(10.0f);
					ImGui::Separator();

					ImGui::Text("Anti-Aliasing:");
					ImGui::Indent(10.0f);
					ImGui::Text("Enable"); ImGui::SameLine(80);
					ButtonEx::CheckboxNoLabel("enableAA", &Graphics::antiAliasing);
					const char* items[] = { "MSAA", "TAA", "SRAA" };
					ImGui::AlignTextToFramePadding();
					ImGui::Text("Type"); ImGui::SameLine(80);
					ButtonEx::ComboNoLabel("aaType", (int*)&Graphics::AAType, items, IM_ARRAYSIZE(items));

					ImGui::Unindent(10.0f);
					ImGui::Separator();

					ImGui::Text("Post-Processing:");
					ImGui::Indent(10.0f);
					Graphics::DrawPostProcessingStack();
					ImGui::Unindent(10.0f);
					//ImGui::Separator();

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Hotkey")) {
					ImGui::Text("WIP");
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("EditorStyle")) {
					ImGui::ShowStyleEditor();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			
			ImGui::End();
		}

		static void DrawScene(Vector2 wndSize, SceneView view = persp) {
			auto buffer = MOON_TextureManager::SCENEBUFFERS[view];
			auto offset = ImGui::GetCursorPos();
			ImGui::Image((void*)(intptr_t)buffer->localID, ImVec2(buffer->width, buffer->height), ImVec2(0, 1), ImVec2(1, 0));

			if (view == MOON_ActiveView) {
				MOON_InputManager::isHoverUI = !ImGui::IsWindowHovered();
				// get mouse pos
				Vector2 newPos = Vector2(
					ImGui::GetMousePos().x - ImGui::GetWindowPos().x - offset.x,
					ImGui::GetMousePos().y - ImGui::GetWindowPos().y - offset.y
				);
				if (MOON_MousePos[0] == -2) MOON_MousePos = newPos;
				else {
					MOON_InputManager::mouseOffset.setValue(newPos.x - MOON_MousePos.x, MOON_MousePos.y - newPos.y);
					MOON_MousePos = newPos;
				}
				//std::cout << MOON_MousePos << std::endl;
			}
			if (buffer->width != wndSize.x || buffer->height != wndSize.y) {
				SceneManager::SetWndSize(wndSize.x, wndSize.y, view);
				buffer->Reallocate(wndSize.x, wndSize.y);
			}
		}

		static void ForceNoTabBar(const char* name, bool& docked) {
			if (ImGui::IsWindowDocked()) {
				if (!docked) {
					docked = true;
					auto* wnd = ImGui::FindWindowByName(name);
					if (wnd) {
						ImGuiDockNode* node = wnd->DockNode;
						if (node && !node->IsHiddenTabBar())
							node->WantHiddenTabBarToggle = true;
					}
				}
			} else docked = false;
		}

		static void SplitSceneWnd(const char* name, const SceneView view, bool (&docked)[4], bool focus = false) {
			ImGui::Begin(name, 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			sceneWndFocused |= ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows | ImGuiFocusedFlags_RootWindow);
			
			ForceNoTabBar(name, docked[view]);
			Vector2 wndSize(
				ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x,
				ImGui::GetWindowSize().y - ImGui::GetStyle().WindowPadding.y
			);
			if (focus || (ImGui::IsMouseDown(0) && 
				ImGui::IsMouseHoveringRect(ImGui::GetWindowPos(), 
				ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, 
				ImGui::GetWindowPos().y + ImGui::GetWindowSize().y)))) {
				MOON_ActiveView = view;
				MOON_ActiveCamera = MOON_SceneCameras[view];
				MOON_MousePos.setValue(-2, -2);
				SceneManager::SetWndSize(wndSize.x, wndSize.y, view);
			}
			if (focus) ImGui::SetWindowFocus();
			DrawScene(wndSize, view);
			if (view == MOON_ActiveView) HotKeyManager::SelectRegion();

			// overlay options
			ImGui::SetCursorPos(ImVec2(10.0f, 10.0f));
			const char* viewCon[] = {"PERSP", "FRONT", "LEFT", "TOP"};
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
			if (ImGui::SmallButton(viewCon[view])) ImGui::OpenPopup("ViewControl");
			ImGui::SameLine(0.0f, 1.0f); ImGui::Text("|"); ImGui::SameLine(0.0f, 1.0f);
			const char*	shadeStr[] = {
				"DEFAULT", "FACET", "WIRE",
				"DEFWIRE", "ALBEDO", "NORMAL",
				"CHECKER", "OVERLAY"
			};
			auto& shading = SceneManager::splitShading[view];
			auto& lightModel = SceneManager::lightModel[view];
			if (ImGui::SmallButton(shadeStr[shading])) ImGui::OpenPopup("ShadingControl");

			if (ImGui::BeginPopup("ViewControl")) {
				if (ImGui::MenuItem("Persp", 0, view == persp)) {}
				if (ImGui::MenuItem("Front", 0, view == front)) {}
				if (ImGui::MenuItem("Left", 0, view == left)) {}
				if (ImGui::MenuItem("Top", 0, view == top)) {}
				ImGui::EndPopup();
			}

			if (ImGui::BeginPopup("ShadingControl")) {
				if (ImGui::BeginMenu("Shading")) {
					if (ImGui::MenuItem("Blinn", 0, lightModel == PHONG)) { lightModel = PHONG; }
					if (ImGui::MenuItem("PBR", 0, lightModel == PBR)) { lightModel = PBR; }
					ImGui::EndMenu();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Default", 0, shading == DEFAULT)) { shading = DEFAULT; }
				if (ImGui::MenuItem("Facet", 0, shading == FACET)) { shading = FACET; }
				if (ImGui::MenuItem("Wire", 0, shading == WIRE)) { shading = WIRE; }
				if (ImGui::MenuItem("Def+Wire", 0, shading == DEFWIRE)) { shading = DEFWIRE; }
				ImGui::Separator();
				if (ImGui::BeginMenu("Overlay")) {
					if (ImGui::MenuItem("Albedo", 0, shading == ALBEDO)) { shading = ALBEDO; }
					if (ImGui::MenuItem("Normal", 0, shading == NORMAL)) { shading = NORMAL; }
					if (ImGui::MenuItem("Roughness", 0, shading == ROUGHNESS)) { shading = ROUGHNESS; }
					if (ImGui::MenuItem("Metallic", 0, shading == METALLIC)) { shading = METALLIC; }
					if (ImGui::MenuItem("Checker", 0, shading == CHECKER)) { shading = CHECKER; }
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Debug", 0, SceneManager::debug)) { SceneManager::debug = !SceneManager::debug; }
				ImGui::Separator();
				if (ImGui::BeginMenu("Background")) {
					if (ImGui::MenuItem("Color", 0, MOON_Enviroment == env_pure_color)) {
						MOON_Enviroment = env_pure_color;
					}
					if (ImGui::MenuItem("CubeMap", 0, MOON_Enviroment == env_cubemap)) {
						MOON_Enviroment = env_cubemap;
					}
					if (ImGui::MenuItem("ProSky", 0, MOON_Enviroment == env_procedural_sky)) {
						MOON_Enviroment = env_procedural_sky;
					}
					if (ImGui::MenuItem("HDRI", 0, MOON_Enviroment == env_hdri)) {
						MOON_Enviroment = env_hdri;
					}
					if (ImGui::MenuItem("Settings...")) {
						MainUI::show_enviroment_editor = !MainUI::show_enviroment_editor;
					}
					ImGui::EndMenu();
				}
				ImGui::EndPopup();
			}
			ImGui::PopStyleColor();

			ImGui::End();
		}

		static void SceneWnd() {
			static bool old = true, currentLayout_m = false;
			static bool docked[4] = { false, false, false, false };
			auto switchControl = sceneWndFocused && MOON_InputManager::IsKeyDown(KEY_SPACE);
			if (switchControl) currentLayout_m = !currentLayout_m;

			const char* dockspaceName;
			if (currentLayout_m) dockspaceName = "layout_default";
			else dockspaceName = "layout_split";
			auto dockSpaceId_m = ImGui::GetID(dockspaceName);

			ImGui::Begin(Icon_Name_To_ID(ICON_FA_GAMEPAD, " Scene"), &MainUI::show_scene_window,
				ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			if (old != currentLayout_m) {
				old = currentLayout_m;
				ImGui::DockBuilderRemoveNode(dockSpaceId_m); // Clear out existing layout
				ImGui::DockBuilderAddNode(dockSpaceId_m); // Add empty node
				ImGuiID dock_main_id = dockSpaceId_m;
				if (currentLayout_m) ImGui::DockBuilderDockWindow("persp", dock_main_id);
				else {
					ImGuiID topView = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.5f, NULL, &dock_main_id);
					ImGuiID frontView = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.5f, NULL, &dock_main_id);

					ImGuiID leftView = ImGui::DockBuilderSplitNode(topView, ImGuiDir_Down, 0.5f, NULL, &topView);
					ImGuiID perspView = ImGui::DockBuilderSplitNode(leftView, ImGuiDir_Down, 0.5f, NULL, &leftView);

					ImGui::DockBuilderDockWindow("top", dock_main_id);
					ImGui::DockBuilderDockWindow("front", frontView);
					ImGui::DockBuilderDockWindow("left", leftView);
					ImGui::DockBuilderDockWindow("persp", perspView);
				}
				ImGui::DockBuilderFinish(dockSpaceId_m);
				for (int i = 0; i < 4; i++) docked[i] = false;
			}
			ImGui::DockSpace(dockSpaceId_m, ImVec2(0.0f, 0.0f), 0);
			ImGui::End();

			sceneWndFocused = false;
			if (!currentLayout_m) {
				SplitSceneWnd("top", top, docked);
				SplitSceneWnd("left", left, docked);
				SplitSceneWnd("front", front, docked);
			}
			SplitSceneWnd("persp", persp, docked, switchControl);
		}

		static void AboutWnd() {
			if (logo == NULL) logo = MOON_TextureManager::GetItem("moon_logo");

			ImGui::Begin(Icon_Name_To_ID(ICON_FA_USER, " ABOUT ME"), &MainUI::show_about_window, ImGuiWindowFlags_NoResize);
			ImGui::Image((void*)(intptr_t)logo->localID, ImVec2(logo->width / 3, logo->height / 3));

			ImGui::Text(u8"【HU ANIME】");
			ImGui::Text(u8">独立动画/游戏创作者");
			ImGui::Text(">Indie Game/Animation Creator");

			ImGui::Separator();
			ImGui::Text(u8"【CONTACT】");
			ImGui::Text(u8">联系 / Contact me：");
			if (ImGui::Button("Email")) ShellExecute(NULL, "open", "mailto:hztmailbox@gmail.com", NULL, NULL, SW_SHOW);
			ImGui::SameLine(); ImGui::Text("hztmailbox@gmail.com");

			ImGui::Separator();
			ImGui::Text(u8"【LINKS】");
			ImGui::Text(u8">动画作品 / My animation works：");
			if (ImGui::Button("Bilibili Homepage")) ShellExecute(NULL, "open", "https://space.bilibili.com/7868260", NULL, NULL, SW_SHOW);
			ImGui::Text(u8">其他CG作品 / My other CG works：");
			if (ImGui::Button("Pixiv")) ShellExecute(NULL, "open", "https://www.pixiv.net/member.php?id=22929868", NULL, NULL, SW_SHOW); ImGui::SameLine();
			if (ImGui::Button("ArtStation")) ShellExecute(NULL, "open", "https://www.artstation.com/huanime", NULL, NULL, SW_SHOW);
			ImGui::Text(u8">My Unity publisher page：");
			if (ImGui::Button("Publisher Page")) ShellExecute(NULL, "open", "https://assetstore.unity.com/publishers/34649", NULL, NULL, SW_SHOW);
			ImGui::Text(u8">投食 / Support me：");
			if (ImGui::Button("Aifadian")) ShellExecute(NULL, "open", "https://afdian.net/@MrMaoMao", NULL, NULL, SW_SHOW);

			ImGui::End();
		}
		
		static void ExplorerWnd() {
			ImGui::Begin(
				Icon_Name_To_ID(ICON_FA_SHOPPING_BASKET, " Explorer"), 
				&MainUI::show_explorer_window, 
				ImGuiWindowFlags_NoScrollbar | 
				ImGuiWindowFlags_NoScrollWithMouse
			);

			static bool showAll		= true;
			static bool showModel	= true;
			static bool showMat		= true;
			static bool showTex		= true;
			static bool showLight	= true;
			static bool showCam		= true;
			static bool showShader	= true;
			static bool showShape	= true;
			static bool showFX		= true;
			static bool showHelper	= true;
			static bool showVolume	= true;

			static bool allFlag = true;
			static char pattern[128] = "";
			bool notEmpty = strcmp(pattern, "");

			ImGui::Columns(2, "columns", false);
			ImGui::SetColumnWidth(-1, 32);
			ImGui::BeginChild("categories", ImVec2(25, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			//ImGui::Checkbox("All", &showAll);
			ButtonEx::SwitchButton("A", "N", showAll, ImVec2(22, 22));

			ButtonEx::SwitchButton(ICON_FA_CUBE, ICON_FA_CUBE, showModel);
			ButtonEx::SwitchButton(ICON_FA_CLOUD, ICON_FA_CLOUD, showVolume);
			ButtonEx::SwitchButton(ICON_FA_GLOBE, ICON_FA_GLOBE, showMat);
			ButtonEx::SwitchButton(ICON_FA_FILE_IMAGE_O, ICON_FA_FILE_IMAGE_O, showTex);
			ButtonEx::SwitchButton(ICON_FA_LIGHTBULB_O, ICON_FA_LIGHTBULB_O, showLight);
			ButtonEx::SwitchButton(ICON_FA_VIDEO_CAMERA, ICON_FA_VIDEO_CAMERA, showCam);
			ButtonEx::SwitchButton(ICON_FA_FILE_CODE_O, ICON_FA_FILE_CODE_O, showShader);
			ButtonEx::SwitchButton(ICON_FA_LEMON_O, ICON_FA_LEMON_O, showShape);
			ButtonEx::SwitchButton(ICON_FA_THUMB_TACK, ICON_FA_THUMB_TACK, showHelper);
			ButtonEx::SwitchButton("FX", "FX", showFX);

			ImGui::EndChild();
			ImGui::NextColumn();

			ImGui::BeginChild("content", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			//ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 1.65 - (notEmpty ? 20 : 0));
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() - (notEmpty ? 30 : 0));
			if (ImGui::InputTextWithHint("search", "type to search", pattern, IM_ARRAYSIZE(pattern))) {
				SceneManager::matchedList.clear();
				Utility::FuzzySearch(
					pattern, MOON_ObjectList, 
					[](ObjectBase* obj) -> std::string& {
						return obj->name;
					}, SceneManager::matchedList
				);
			}
			/*if (ImGui::IsItemDeactivated() && 
			ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) {

			}*/

			if (notEmpty) {
				ImGui::SameLine();
				if (ImGui::Button(ICON_FA_TIMES, ImVec2(22, 22))) {
					sprintf_s(pattern, "");
					SceneManager::matchedList.clear();
				}
			}
			/*if (ImGui::CollapsingHeader("Categories")) {
				ImGui::Checkbox("Model", &showModel); ImGui::SameLine(80);
				ImGui::Checkbox("Mat", &showMat); ImGui::SameLine(160);
				ImGui::Checkbox("Image", &showTex);
				ImGui::Checkbox("Light", &showLight); ImGui::SameLine(80);
				ImGui::Checkbox("Camera", &showCam); ImGui::SameLine(160);
				ImGui::Checkbox("Shader", &showShader);
				ImGui::Checkbox("Shape", &showShape); ImGui::SameLine(80);
				ImGui::Checkbox("Helper", &showHelper); ImGui::SameLine(160);
				ImGui::Checkbox("FXnode", &showFX);
			}*/

			if (allFlag != showAll) {
				allFlag = showAll; showVolume = showAll;
				showModel = showAll; showMat = showAll; showTex = showAll;
				showLight = showAll; showCam = showAll; showShader = showAll;
				showShape = showAll; showFX = showAll; showHelper = showAll;
			}

			static float colWidth = 140;

			ImGui::Spacing();
			ImGui::Columns(2, "mycolumns", false);
			ImGui::Separator();
			ImGui::SetColumnWidth(-1, colWidth);
			ImGui::Text("Name"); ImGui::NextColumn();
			ImGui::Text("ID");
			ImGui::Separator();
			ImGui::Columns(1);

			ImGui::BeginChild("explorer_scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
			ImGui::Columns(2, "mycolumns", false);
			ImGui::SetColumnWidth(-1, colWidth);

			MOON_InputManager::ResetSizeChangeState();
			if (notEmpty) {
				SceneManager::ListMatchedItems();
			} else {
				if (showModel) MOON_ModelManager::ListItems_Hierarchial();
				if (showVolume) MOON_VolumeManager::ListItems_Hierarchial();
				if (showShape) MOON_ShapeManager::ListItems_Hierarchial();
				if (showHelper) MOON_HelperManager::ListItems_Hierarchial();
				if (showLight) MOON_LightManager::ListItems_Hierarchial();
				if (showCam) MOON_CameraManager::ListItems_Hierarchial();
				if (showMat) MOON_MaterialManager::ListItems_Hierarchial();
				if (showTex) MOON_TextureManager::ListItems_Hierarchial();
				if (showShader) MOON_ShaderManager::ListItems_Hierarchial();
			}

			if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered()) {
				if (ImGui::IsMouseReleased(0)) {
					const ImGuiPayload* payload = ImGui::GetDragDropPayload();
					if (payload != NULL) {
						MObject* payload_m = *(MObject**)payload->Data;
						payload_m->transform.SetParent(nullptr);
					}
				} else if (ImGui::IsMouseClicked(0)) MOON_InputManager::ClearSelection();
			}

			ImGui::Columns(1);
			ImGui::EndChild();

			ImGui::EndChild();

			ImGui::Columns(1);
			ImGui::End();
		}
		
		static void ConsoleWnd() {
			static MOON::ConsoleWnd console;
			console.Draw(Icon_Name_To_ID(ICON_FA_TERMINAL, " Console"), &MainUI::show_console_window);
		}

		static void ProjectWnd() {
			ImGui::Begin(Icon_Name_To_ID(ICON_FA_FOLDER_OPEN, " Project"), &MainUI::show_project_window);

			// tree view
			ImGui::BeginChild("TreeView", ImVec2(150, 0), true);
			static DirNode* selNode = NULL;
			if (AssetLoader::DirTree != NULL) AssetLoader::DirTree->ListNode(selNode);
			ImGui::EndChild();
			ImGui::SameLine();

			// folder view
			ImGui::BeginChild("FolderView", ImVec2(0, 0), true);
			float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			if (selNode != NULL) selNode->RenderFolderView(window_visible_x2, selNode);
			ImGui::EndChild();
			
			ImGui::End();
		}

		static void InspectorWnd() {
			ImGui::Begin(Icon_Name_To_ID(ICON_FA_SEARCH, " Inspector"), &MainUI::show_inspector_window);
			
			// loop all selected objects and list their properties
			for (auto &iter : MOON_InputManager::selection) {
				bool checker = MOON_ObjectList[iter]->selected;

				if (ImGui::CollapsingHeader((SceneManager::GetTypeIcon(MOON_ObjectList[iter]) + "  " + 
											 MOON_ObjectList[iter]->name).c_str(), &MOON_ObjectList[iter]->selected, 
											 ImGuiTreeNodeFlags_DefaultOpen, MOON_ObjectList[iter]->ID)) {
					MOON_ObjectList[iter]->ListProperties();
				}
				// keep selection in edit mode
				if (MOON_EditTarget && MOON_EditTarget->ID == iter)
					MOON_ObjectList[iter]->selected = true;

				if (iter >= SceneManager::objectList.size()) break;
				// remove ID in selection slot while click close button in the collapsing header
				if (checker && !MOON_ObjectList[iter]->selected) {
					auto end = MOON_InputManager::selection.end();
					for (auto it = MOON_InputManager::selection.begin(); it != end; it++)
						if (*it == MOON_ObjectList[iter]->ID) {
							MOON_InputManager::selection.erase(it);
							break;
						}
				}
			}

			ImGui::End();
		}

		static void ShowTimeline() {
			static int rows = 6;
			static float offset = 50.0f;
			static double range = 100.0, in = 0.0, out = 1.0;
			static double events[12] = { 10.f,20.f,0.5f,30.f,40.f,50.f,20.f,40.f,15.f,22.5f,35.f,45.f };

			ImGui::Begin(Icon_Name_To_ID(ICON_FA_CLOCK_O, " TimeLine"), &MainUI::show_timeline, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			if (ImGui::ImTimeline::BeginTimeline("MyTimeline", offset, range, rows)) {
				if (ImGui::ImTimeline::TimelineEvent("Event1", events[0], events[1], out - in)) {
					// events[0] and/or events[1] modified
				}
				ImGui::ImTimeline::TimelineEvent("Event2", events[2], events[3], out - in);
				ImGui::ImTimeline::TimelineEvent("Event3", events[4], events[5], out - in);
				ImGui::ImTimeline::TimelineEvent("Event4", events[6], events[7], out - in);
				//ImGui::ImTimeline::TimelineEvent("Event5", events[8], events[9], out - in);
				//ImGui::ImTimeline::TimelineEvent("Event6", events[10], events[11], out - in);
			}
			ImGui::ImTimeline::EndTimeline(5, in, out, range);
			ImGui::End();
		}

		static void MaterialEditorWnd() {
			ImGui::Begin(Icon_Name_To_ID(ICON_FA_FUTBOL_O, " MaterialEditor"), &MainUI::show_material_editor);

			ImGui::Columns(2, "mycolumns");
			static bool firstLoop = true;
			if (firstLoop) ImGui::SetColumnWidth(-1, 160);
			float floatWidth = ImGui::GetColumnWidth(-1) - 20;

			// left
			static Material* selectedMat = NULL;
			static unsigned int prevID = MOON_TextureManager::GetItem("moon_logo_full")->localID;

			ImGui::BeginChild("Previewer", ImVec2(floatWidth, 140), true, ImGuiWindowFlags_NoScrollbar);
			//ImGui::Image((void*)(intptr_t)prevID, ImVec2(125, 125));
			matEditor.Previewer(style);
			ImGui::EndChild();

			// materials in the scene ------------------------------------------------------
			ImGui::BeginChild("Mat Explorer", ImVec2(floatWidth, 0), true);
			int loopID = 0;
			for (auto &mat : MOON_MaterialManager::itemMap) {
				if (ImGui::Selectable(mat.second->name.c_str(), selectedMat == mat.second))
					selectedMat = mat.second;
			}
			ImGui::EndChild();

			ImGui::NextColumn();

			// right
			ImGui::BeginChild("item view", ImVec2(0, 0));
			ImGui::Button("Save"); ImGui::SameLine();
			ImGui::Button("Load"); ImGui::SameLine();
			if (ImGui::Button("Clear")) {
				RegistStackWnd("Information");
			}
			QueryWnd("Information", "Clear node editor?", &matEditor, &MaterialEditor::ClearEditor);
			ImGui::SameLine();
			ImGui::Text(u8"|"); ImGui::SameLine();
			ImGui::Text("Material: %d", MOON_MaterialManager::CountItem());
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Columns(2, "matView");
			if (firstLoop) {
				firstLoop = false;
				ImGui::SetColumnWidth(-1, 620);
			}
			floatWidth = ImGui::GetColumnWidth(-1) - 15;

			// node editor -------------------------------------------------------------
			ImGui::BeginChild("matEditor", ImVec2(floatWidth, 0), true,
				ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			{
				matEditor.Draw();
			}
			ImGui::EndChild();

			ImGui::NextColumn();

			// properties view ---------------------------------------------------------
			ImGui::BeginChild("propView");
			{
				if (matEditor.anythingSelected) {
					selectedMat = NULL; // TODO

					matEditor.selectedNode->ProcessContent(false);
				} else {
					if (selectedMat == NULL) ImGui::Text("Nothing selected");
					else selectedMat->ListProperties();
				}
			}
			ImGui::EndChild();

			ImGui::Columns(1);
			ImGui::EndChild();
			ImGui::End();
		}

		static void EnvironmentWnd() {
			ImGui::Begin(Icon_Name_To_ID(ICON_FA_MIXCLOUD, " Enviroment"), &MainUI::show_enviroment_editor);

			ImGui::Text("Fog");
			ImGui::Indent(10.0f);
			ImGui::Text("Enable"); ImGui::SameLine(80.0f);
			ButtonEx::CheckboxNoLabel("enable_fog", &Graphics::enableFog);

			const char* fogType[] = { "Linear", "Exponential", "ExpoSquared" };
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Type"); ImGui::SameLine(80.0f);
			ButtonEx::ComboNoLabel("fogType", (int*)&Graphics::fogType, fogType, IM_ARRAYSIZE(fogType));

			ImGui::Text("Color"); ImGui::SameLine(80.0f);
			ButtonEx::ColorEdit3NoLabel("fogCol", (float*)&Graphics::fogColor);

			ImGui::Text("Density"); ImGui::SameLine(80.0f);
			ButtonEx::DragFloatNoLabel("fogDens", (float*)&Graphics::density);
			ImGui::Unindent(10.0f);
			ImGui::Separator();

			ImGui::Text("IBL");
			ImGui::Indent(10.0f);
			const char* envType[] = { "HDRI", "CubeMap", "PureColor", "ProSky" };
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Enviroment"); ImGui::SameLine();
			ButtonEx::ComboNoLabel("envType", (int*)&MOON_Enviroment, envType, IM_ARRAYSIZE(envType));

			if (MOON_Enviroment == EnviromentType::env_pure_color) {
				ImGui::Text("Pure Color:");
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
				ButtonEx::ColorEdit3NoLabel("pureCol", (float*)&Graphics::clearColor);
			} else if (MOON_Enviroment == EnviromentType::env_hdri) {
				MOON_TextureManager::HDRI->ListProperties();
				ImGui::Text("Rotate"); ImGui::SameLine(80.0f);
				auto euler = MOON_ModelManager::skyDome->transform.rotation.eulerAngles;
				float rotEuler[3] = { euler.x, euler.y, euler.z };
				if (ButtonEx::DragVec3NoLabel("rot", rotEuler, 0.1f, -INFINITY, INFINITY, "%.3f", 1.0f)) {
					Quaternion deltaQ = Quaternion(
						rotEuler[0] - euler.x,
						rotEuler[1] - euler.y,
						rotEuler[2] - euler.z
					);
					MOON_ModelManager::skyDome->transform.Rotate(deltaQ);
				}
			} else if (MOON_Enviroment == EnviromentType::env_cubemap) {
				ImGui::Text("Cubemap Path:");
				ImGui::Button("[Cubemap]", ImVec2(ImGui::GetContentRegionAvailWidth(), 0));
			} else if (MOON_Enviroment == EnviromentType::env_procedural_sky) {
				ImGui::Text("Sun:");
				ImGui::Button("[Sun]", ImVec2(ImGui::GetContentRegionAvailWidth(), 0));
			}
			ImGui::Unindent(10.0f);

			ImGui::End();
		}

		static void CreateWnd() {
			ImGui::Begin(Icon_Name_To_ID(ICON_FA_CUBES, " Creator"), &MainUI::show_create_window);

			ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None | 
											 ImGuiTabBarFlags_FittingPolicyScroll | 
											 ImGuiTabBarFlags_TabListPopupButton;
			if (ImGui::BeginTabBar("CreatorTabBar", tab_bar_flags)) {
				float width = ImGui::GetWindowWidth() / 3.5;
				if (ImGui::BeginTabItem("Basic")) {
					ImGui::Spacing();
					if (ImGui::Button("Box", ImVec2(width, 20.0))) {
						MOON_ModelManager::CreateSmartMesh(box, "box", true);
					}
					ImGui::SameLine();
					if (ImGui::Button("Sphere", ImVec2(width, 20.0))) {
						MOON_ModelManager::CreateSmartMesh(sphere, "sphere", true);
					}
					ImGui::SameLine();
					if (ImGui::Button("Cylinder", ImVec2(width, 20.0))) {}

					if (ImGui::Button("Plane", ImVec2(width, 20.0))) {
						MOON_ModelManager::CreateSmartMesh(plane, "plane", true);
					}
					ImGui::SameLine();
					if (ImGui::Button("Capsule", ImVec2(width, 20.0))) {}
					ImGui::SameLine();
					if (ImGui::Button("Text", ImVec2(width, 20.0))) {}

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Advanced")) {
					ImGui::Spacing();
					if (ImGui::Button("Starry", ImVec2(width, 20.0))) {}
					ImGui::SameLine();
					if (ImGui::Button("Volume", ImVec2(width, 20.0))) {
						MOON_VolumeManager::CreateVolume("cloud", true);
					}

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Shape")) {
					ImGui::Spacing();
					if (ImGui::Button("Line", ImVec2(width, 20.0))) {}
					ImGui::SameLine();
					if (ImGui::Button("Rectangle", ImVec2(width, 20.0))) {}
					ImGui::SameLine();
					if (ImGui::Button("Circle", ImVec2(width, 20.0))) {}

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Light")) {
					ImGui::Spacing();
					if (ImGui::Button("Direct", ImVec2(width, 20.0))) {}
					ImGui::SameLine();
					if (ImGui::Button("Spot", ImVec2(width, 20.0))) {}
					ImGui::SameLine();
					if (ImGui::Button("Point", ImVec2(width, 20.0))) {}

					if (ImGui::Button("MoLight", ImVec2(width, 20.0))) {}
					ImGui::SameLine();
					if (ImGui::Button("Dome", ImVec2(width, 20.0))) {}

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Camera")) {
					ImGui::Spacing();
					if (ImGui::Button("Target", ImVec2(width, 20.0))) {}
					ImGui::SameLine();
					if (ImGui::Button("Free", ImVec2(width, 20.0))) {}

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Helper")) {
					ImGui::Spacing();
					if (ImGui::Button("Dummy", ImVec2(width, 20.0))) {}
					ImGui::SameLine();
					if (ImGui::Button("Bone", ImVec2(width, 20.0))) {}
					ImGui::SameLine();
					if (ImGui::Button("Tape", ImVec2(width, 20.0))) {}

					if (ImGui::Button("Magnet", ImVec2(width, 20.0))) {}
					ImGui::SameLine();
					if (ImGui::Button("Proxy", ImVec2(width, 20.0))) {}

					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();

			ImGui::End();
		}

		static void CodeEditor() {
			CEditor.Draw(&MainUI::show_code_editor);
		}

		static void RibbonBar() {
			ImGui::Begin("Ribbon", &MainUI::show_ribbon, ImGuiWindowFlags_NoScrollbar | 
				ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

			ImGui::Button(ICON_FA_FILE, ImVec2(22, 22)); ImGui::SameLine();
			ImGui::Button(ICON_FA_FLOPPY_O, ImVec2(22, 22)); ImGui::SameLine();
			ImGui::Button(ICON_FA_HISTORY, ImVec2(22, 22)); ImGui::SameLine();

			ImGui::Text(u8"|"); ImGui::SameLine();

			ButtonEx::RatioButton(ICON_FA_LINK, Gizmo::gizmoMode == GizmoMode::link,
				[]() { Gizmo::gizmoMode = GizmoMode::link; }); ImGui::SameLine();
			ImGui::Button(ICON_FA_CHAIN_BROKEN, ImVec2(22, 22)); ImGui::SameLine();

			ImGui::Text(u8"|"); ImGui::SameLine();

			ButtonEx::SwitchButtonEx(ICON_FA_TOGGLE_OFF, ICON_FA_TOGGLE_ON, Gizmo::manipCoord == CoordSys::WORLD,
						[]() { Gizmo::manipCoord = CoordSys::LOCAL; },
						[]() { Gizmo::manipCoord = CoordSys::WORLD; }); ImGui::SameLine();
			ButtonEx::SwitchButton(ICON_FA_MAGNET, ICON_FA_MAGNET, HotKeyManager::enableSnap, ImVec2(22, 22)); ImGui::SameLine();
			ButtonEx::SwitchButtonEx(ICON_FA_DOT_CIRCLE_O, ICON_FA_LIFE_RING, Gizmo::gizmoPos == GizmoPos::pivot,
				[]() { Gizmo::gizmoPos = GizmoPos::center; },
				[]() { Gizmo::gizmoPos = GizmoPos::pivot; });
			ImGui::SameLine();

			ImGui::Text(u8"|"); ImGui::SameLine();

			ButtonEx::RatioButton(ICON_FA_MOUSE_POINTER, Gizmo::gizmoMode == GizmoMode::none,
				[]() { Gizmo::gizmoMode = GizmoMode::none; }); ImGui::SameLine();
			ButtonEx::RatioButton(ICON_FA_ARROWS, Gizmo::gizmoMode == GizmoMode::translate,
				[]() { Gizmo::gizmoMode = GizmoMode::translate; }); ImGui::SameLine();
			ButtonEx::RatioButton(ICON_FA_REFRESH, Gizmo::gizmoMode == GizmoMode::rotate,
				[]() { Gizmo::gizmoMode = GizmoMode::rotate; }); ImGui::SameLine();
			ButtonEx::RatioButton(ICON_FA_EXPAND, Gizmo::gizmoMode == GizmoMode::scale,
				[]() { Gizmo::gizmoMode = GizmoMode::scale; }); ImGui::SameLine();

			//ImGui::Text(u8"|"); ImGui::SameLine();
			//ImGui::Button(ICON_FA_DELICIOUS, ImVec2(22, 22)); ImGui::SameLine();

			ImGui::Text(u8"|"); ImGui::SameLine();

			ImGui::Button(ICON_FA_BOLT, ImVec2(22, 22)); ImGui::SameLine();
			ImGui::Button(ICON_FA_FIRE, ImVec2(22, 22)); ImGui::SameLine();
			ImGui::Button(ICON_FA_TINT, ImVec2(22, 22)); ImGui::SameLine();
			ImGui::Button(ICON_FA_FLAG, ImVec2(22, 22)); ImGui::SameLine();
			ImGui::Button(ICON_FA_GLASS, ImVec2(22, 22)); ImGui::SameLine();

			ImGui::Text(u8"|"); ImGui::SameLine();

			ImGui::Button(ICON_FA_TH, ImVec2(22, 22)); ImGui::SameLine();
			ImGui::Button(ICON_FA_RANDOM, ImVec2(22, 22)); ImGui::SameLine();

			ImGui::Text(u8"|"); ImGui::SameLine();

			ImGui::Button(ICON_FA_PAINT_BRUSH, ImVec2(22, 22)); ImGui::SameLine();
			ImGui::Button(ICON_FA_AREA_CHART, ImVec2(22, 22)); ImGui::SameLine();

			ImGui::Text(u8"|"); ImGui::SameLine();

			ImGui::Button(ICON_FA_LEAF, ImVec2(22, 22)); ImGui::SameLine();
			ImGui::Button(ICON_FA_ROAD, ImVec2(22, 22)); ImGui::SameLine();
			ImGui::Button(ICON_FA_UNIVERSAL_ACCESS, ImVec2(22, 22)); ImGui::SameLine();
			ImGui::Button(ICON_FA_STREET_VIEW, ImVec2(22, 22)); ImGui::SameLine();

			ImGui::Text(u8"|"); ImGui::SameLine();

			if (ImGui::Button(ICON_FA_LIST_ALT, ImVec2(22, 22))) {
				MainUI::show_render_setting = !MainUI::show_render_setting;
			} ImGui::SameLine();
			ButtonEx::SwitchButton(ICON_FA_WINDOW_MAXIMIZE, ICON_FA_WINDOW_MAXIMIZE, VFB::show);
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_CAMERA, ImVec2(22, 22))) {
				VFB::show = true;
				Renderer::StartRendering();
			} ImGui::SameLine();

			ImGui::Text(u8"|"); ImGui::SameLine();

			ImGui::Button(ICON_FA_QUESTION_CIRCLE, ImVec2(22, 22));

			//ImGui::Separator();

			ImGui::End();
		}

		static void CurveEditor() {
		
		}

		static void OperatorExplorer() {
			
		}

		static void NNManagerWnd() {
			ImGui::Begin(Icon_Name_To_ID(ICON_FA_BRAILLE, " MOON NN"), &show_nn_manager);

			ImGui::Columns(2, "nn_col_1");
			static bool firstLoop = true;
			if (firstLoop) ImGui::SetColumnWidth(-1, 160);
			float floatWidth = ImGui::GetColumnWidth(-1) - 20;

			// left
			static ImNodes::MyNode* selectedNode = NULL;

			ImGui::BeginChild("Previewer", ImVec2(floatWidth, 140), true, ImGuiWindowFlags_NoScrollbar);
			NN::NNM::currentGraph->Previewer(style);
			ImGui::EndChild();

			// neurons in the scene ------------------------------------------------------
			ImGui::BeginChild("NeuronList", ImVec2(floatWidth, 0), true);
			int nodeid = 0;
			for (auto &node : NN::NNM::currentGraph->nodes) {
				ImGui::PushID(nodeid++);
				if (ImGui::Selectable(node->title.c_str(), selectedNode == node)) {
					if (selectedNode != node) {
						if (selectedNode != NULL) selectedNode->selected = false;
						node->selected = true;
						selectedNode = node;
					}
				}
				ImGui::PopID();
			}
			ImGui::EndChild();
			ImGui::NextColumn();

			// right
			ImGui::BeginChild("GraphZone", ImVec2(0, 0));
			ImGui::Button(ICON_FA_FLOPPY_O); ImGui::SameLine();
			ImGui::Button(ICON_FA_FOLDER_OPEN_O); ImGui::SameLine();
			if (ImGui::Button(ICON_FA_FILE_O)) RegistStackWnd("Information"); ImGui::SameLine(); ImGui::Text("|"); ImGui::SameLine();
			if (ImGui::Button(ICON_FA_PLUS_SQUARE_O)) NN::NNM::NewGraph();
			QueryWnd("Information", "Clear node editor?", NN::NNM::currentGraph, &NN::NGraph::ClearGraph);
			ImGui::SameLine();
			ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_AutoSelectNewTabs;
			if (ImGui::BeginTabBar("NGraphTabBar", tab_bar_flags)) {
				for (auto graph = NN::NNM::graphList.begin(); graph != NN::NNM::graphList.end(); graph++) {
					// TODO: if invisible, delete it
					if (ImGui::BeginTabItem((*graph)->name.c_str(), (*graph == NN::NNM::globalNNGraph) ? NULL : &(*graph)->visible)) {
						ImGui::Columns(2, "nn_col_2");
						if (firstLoop) {
							firstLoop = false;
							ImGui::SetColumnWidth(-1, ImGui::GetWindowSize().x * 0.75f);
						}
						floatWidth = ImGui::GetColumnWidth(-1) - 15;

						// node editor -------------------------------------------------------------
						ImGui::BeginChild("nGraphView", ImVec2(floatWidth, 0), true,
							ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
						(*graph)->Draw();
						ImGui::EndChild();
						ImGui::NextColumn();

						// properties view ---------------------------------------------------------
						ImGui::BeginChild("nnPropView");
						if ((*graph)->anythingSelected) {
							selectedNode = (*graph)->selectedNode;
							try { selectedNode->ProcessContent(false); } 
							catch (...) {
								try { selectedNode->content(selectedNode, false); } 
								catch (...) {}
								ImGui::Separator();
								ImGui::TextColored(ImVec4(0.84f, 0.075f, 0.27f, 1.0f), "Error occured!");
							}
						} else ImGui::Text("Nothing selected");

						ImGui::EndChild();
						ImGui::Columns(1);
						ImGui::EndTabItem();

						NN::NNM::currentGraph = (*graph);
					}
				}

				/*if (ImGui::BeginTabItem(ICON_FA_PLUS_SQUARE_O)) {
					if (newGraph != NULL) newGraph = NULL;
					else newGraph = NN::NNM::NewGraph();
					ImGui::EndTabItem();
				}*/
			}
			ImGui::EndTabBar();
			//ImGui::Text(u8"|"); ImGui::SameLine();
			//ImGui::Text("Neuron: %d", NN::NNM::globalNNGraph.nodes.size());
			ImGui::EndChild();
			ImGui::End();
		}

		static void DrawPlotWnds() {
			for (auto &plt : MOON_PlotList) {
				if (plt->open && !plt->wndName._Equal(""))
					plt->DrawInWindow();
			}
		}
	};

}