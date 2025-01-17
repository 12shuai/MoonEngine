#include <imgui.h>
#include <string>
#include <vector>

#include "Icons.h"
#include "Gizmo.h"
#include "Matrix.h"
#include "Vector3.h"
#include "SceneMgr.h"
#include "ButtonEx.h"
#include "MoonEnums.h"
#include "StackWindow.h"
#include "ObjectBase.h"
#include "OperatorMgr.h"
#include "HotkeyMgr.h"
#include "Graphics.h"


namespace MOON {
	#define GetSingle(i)		node->dataHolder.id[i]
	#define GetVec(i)			node->dataHolder.col[i]
	#define GetMatrix()			node->dataHolder.mat

	// return: model/result
	#define ProcessInput()\
			auto model = dynamic_cast<Model*>(input);\
			if (node->processed == nullptr) node->processed = new Model(*model);\
			Model* result = dynamic_cast<Model*>(node->processed);\

	Operators* OperatorManager::DefineNativeOperators() {
		return new Operators{
			// Mesh Operators -----------------------------------------------------------------------
			#pragma region model_operators
			{ "Model", []() -> Operator* { return new Operator("EditModel",
				{	/// data holder: int[], Vector3[], Matrix
					1, { -1 },
					0, {},
					0, Matrix()
				}, /// content renderer
				[](Operator* node, MObject* input) -> bool {
					if (CheckType(input, "Model")) {
						Model* model = dynamic_cast<Model*>(input);
						int& editMode = GetSingle(0);
						ImGui::Text("Element:");
						#pragma region buttons
						ButtonEx::SwitchButtonEx(
							ICON_MD_GRAIN, ICON_MD_GRAIN, editMode == VERT,
							[&editMode]() -> void { editMode = -1; HotKeyManager::state = FREE; MOON_EditTarget = nullptr; },
							[&editMode, &node]() -> void { editMode = VERT; MOON_EditElem = VERT; MOON_EditTarget = node->processed; },
							ImVec2(22, 20)
						); ImGui::SameLine();
						ButtonEx::SwitchButtonEx(
							ICON_MD_SHOW_CHART, ICON_MD_SHOW_CHART, editMode == EDGE,
							[&editMode]() -> void { editMode = -1; HotKeyManager::state = FREE; MOON_EditTarget = nullptr; },
							[&editMode, &node]() -> void { editMode = EDGE;  MOON_EditElem = EDGE; MOON_EditTarget = node->processed; },
							ImVec2(22, 20)
						); ImGui::SameLine();
						ButtonEx::SwitchButtonEx(
							ICON_MD_MULTILINE_CHART, ICON_MD_MULTILINE_CHART, editMode == LOOP,
							[&editMode]() -> void { editMode = -1; HotKeyManager::state = FREE; MOON_EditTarget = nullptr; },
							[&editMode, &node]() -> void { editMode = LOOP;  MOON_EditElem = LOOP; MOON_EditTarget = node->processed; },
							ImVec2(22, 20)
						); ImGui::SameLine();
						ButtonEx::SwitchButtonEx(
							ICON_MD_SIGNAL_CELLULAR_NULL, ICON_MD_SIGNAL_CELLULAR_NULL, editMode == FACE,
							[&editMode]() -> void { editMode = -1; HotKeyManager::state = FREE; MOON_EditTarget = nullptr; },
							[&editMode, &node]() -> void { editMode = FACE;  MOON_EditElem = FACE; MOON_EditTarget = node->processed; },
							ImVec2(22, 20)
						); ImGui::SameLine();
						ButtonEx::SwitchButtonEx(
							ICON_MD_WIDGETS, ICON_MD_WIDGETS, editMode == ELEM,
							[&editMode]() -> void { editMode = -1; HotKeyManager::state = FREE; MOON_EditTarget = nullptr; },
							[&editMode, &node]() -> void { editMode = ELEM;  MOON_EditElem = ELEM; MOON_EditTarget = node->processed; },
							ImVec2(22, 20)
						);
						#pragma endregion

						if (editMode >= 0) {
							HotKeyManager::state = EDIT;
							if (MOON_EditTarget != node->processed) editMode = -1;
						} else if (HotKeyManager::state != EDIT) {
							editMode = -1;
						}

						if (node->processed != nullptr) {
							Model* pmodel = dynamic_cast<Model*>(node->processed);
							
							glDepthMask(GL_FALSE);
							if (editMode == VERT) {
								for (int i = 0; i < pmodel->meshList.size(); i++) {
									Gizmo::DrawPoints(
										pmodel->meshList[i]->VAO,
										pmodel->meshList[i]->vertices.size(),
										Color::BLUE(), Graphics::edit_mode_point_size, 
										true, pmodel->transform.localToWorldMat
									);
									// selected points
									HalfMesh* pmesh = dynamic_cast<HalfMesh*>(pmodel->meshList[i]);
									Gizmo::DrawPoints(
										pmesh->GetSelectedPoints(), Color::RED(),
										Graphics::edit_mode_point_size,
										true, pmodel->transform.localToWorldMat
									);
								}
							}
							glDepthMask(GL_TRUE);
						}
						
					} else {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0, 0, 1.0f));
						ImGui::Text("Incorrect input type.");
						ImGui::PopStyleColor();
					}

					return false;
				},	/// process
				[](Operator* node, MObject* input) -> MObject* {
					if (input == nullptr) return nullptr;
					else {
						ProcessInput();

						return result;
					}
				}
			); } },

			{ "Model", []() -> Operator* { return new Operator("Push",
				{	/// data holder: int[], Vector3[], Matrix
					0, 1, 0
				}, /// content renderer
				[](Operator* node, MObject* input) -> bool {
					if (CheckType(input, "Model")) {
						ImGui::Text("Amount:");
						if (ImGui::DragFloat("pushAmount", &GetVec(0).x, 0.1f, 0, 0, "%.1f", 1.0f, true))
							return true;
					} else {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0, 0, 1.0f));
						ImGui::Text("Incorrect input type.");
						ImGui::PopStyleColor();
					}
					return false;
				},	/// process
				[](Operator* node, MObject* input) -> MObject* {
					if (input == nullptr) return nullptr;
					else {
						ProcessInput();

						for (int i = 0; i < result->meshList.size(); i++) {
							HalfMesh* mesh = dynamic_cast<HalfMesh*>(result->meshList[i]);
							for (int j = 0; j < mesh->vertices.size(); j++) {
								mesh->vertices[j].Position = model->meshList[i]->vertices[j].Position +
									model->meshList[i]->vertices[j].Normal * GetVec(0).x;
							}
							result->meshList[i]->UpdateMesh();
						}
						return result;
					}
				}
			); } },

			{ "Model", []() -> Operator* { return new Operator("Tessellation",
				{	/// data holder: int[], Vector3[], Matrix
					0, 0, 0
				}, /// content renderer
				[](Operator* node, MObject* input) -> bool {

					return false;
				},	/// process
				[](Operator* node, MObject* input) -> MObject* {

					return nullptr;
				}
			); } },

			{ "Model", []() -> Operator* { return new Operator("Smooth",
				{	/// data holder: int[], Vector3[], Matrix
					0, 0, 0
				}, /// content renderer
				[](Operator* node, MObject* input) -> bool {

					return false;
				},	/// process
				[](Operator* node, MObject* input) -> MObject* {

					return nullptr;
				}
			); } },

			{ "Model", []() -> Operator* { return new Operator("Displacement",
				{	/// data holder: int[], Vector3[], Matrix
					0, 0, 0
				}, /// content renderer
				[](Operator* node, MObject* input) -> bool {

					return false;
				},	/// process
				[](Operator* node, MObject* input) -> MObject* {

					return nullptr;
				}
			); } },
			#pragma endregion

			// Spline Operators ---------------------------------------------------------------------
			#pragma region spline_operators
			{ "Spline", []() -> Operator* { return new Operator("EditSpline",
				{	/// data holder: int[], Vector3[], Matrix
					0, 0, 0
				}, /// content renderer
				[](Operator* node, MObject* input) -> bool {

					return false;
				},	/// process
				[](Operator* node, MObject* input) -> MObject* {

					return nullptr;
				}
			); } },

			{ "Spline", []() -> Operator* { return new Operator("SplineMesher",
				{	/// data holder: int[], Vector3[], Matrix
					0, 0, 0
				}, /// content renderer
				[](Operator* node, MObject* input) -> bool {

					return false;
				},	/// process
				[](Operator* node, MObject* input) -> MObject* {

					return nullptr;
				}
			); } },

			{ "Spline", []() -> Operator* { return new Operator("Conform",
				{	/// data holder: int[], Vector3[], Matrix
					0, 0, 0
				}, /// content renderer
				[](Operator* node, MObject* input) -> bool {

					return false;
				},	/// process
				[](Operator* node, MObject* input) -> MObject* {

					return nullptr;
				}
			); } },

			{ "Spline", []() -> Operator* { return new Operator("RopeDynamic",
				{	/// data holder: int[], Vector3[], Matrix
					0, 0, 0
				}, /// content renderer
				[](Operator* node, MObject* input) -> bool {

					return false;
				},	/// process
				[](Operator* node, MObject* input) -> MObject* {

					return nullptr;
				}
			); } },
			#pragma endregion

			// Shape Operators ----------------------------------------------------------------------
			#pragma region shape_operators
				{ "Shape", []() -> Operator* { return new Operator("Extrude",
					{	/// data holder: int[], Vector3[], Matrix
						0, 0, 0
					}, /// content renderer
					[](Operator* node, MObject* input) -> bool {

						return false;
					},	/// process
					[](Operator* node, MObject* input) -> MObject* {

						return nullptr;
					}
				); } },
			#pragma endregion

			// Volume Operators ----------------------------------------------------------------------
			#pragma region volume_operators
				{ "Volume", []() -> Operator* { return new Operator("Reconstruction",
					{	/// data holder: int[], Vector3[], Matrix
						0, 0, 0
					}, /// content renderer
					[](Operator* node, MObject* input) -> bool {

						return false;
					},	/// process
					[](Operator* node, MObject* input) -> MObject* {

						return nullptr;
					}
				); } }
			#pragma endregion
		};
	}
}