#pragma once
#include <map>
#include <vector>
#include <string>
#include <iomanip>
#include <time.h>

#include "Gizmo.h"
#include "SmartMeshes.h"
#include "Debugger.h"
#include "Vector2.h"
#include "Camera.h"
#include "Model.h"
#include "Volume.h"
#include "Light.h"
#include "Shapes.h"
#include "Helpers.h"
#include "Texture.h"
#include "ProceduralMap.h"
#include "MShader.h"
#include "Material.h"
#include "MatSphere.h"
#include "Graphics.h"
#include "HotkeyMgr.h"

namespace MOON {
	template <class T>
	struct ObjectManager {
		static bool sizeFlag;
		static std::multimap<std::string, T*> itemMap;

		static int CountItem() {
			return itemMap.size();
		}

		static void PrintID(T* item) {
			std::cout << " id: " << item->ID << std::endl;

			if (typeid(*item) == typeid(Shader)) std::cout << " local(shader) id: [" << dynamic_cast<Shader*>(item)->localID << "]" << std::endl;
			else if (typeid(*item) == typeid(Texture)) std::cout << " local(texture) id: [" << dynamic_cast<Texture*>(item)->localID << "]" << std::endl;
		}

		static void ListItems() {
			if (itemMap.size() < 1) return;

			//std::vector<unsigned int> &sel = MOON_InputManager::selected;
			auto iter = itemMap.begin();
			for (int i = 0; i < itemMap.size(); i++, iter++) {
				if (ImGui::Selectable((SceneManager::GetTypeIcon(iter->second) + "  " + 
									   iter->first).c_str(), iter->second->selected, 
									   ImGuiSelectableFlags_SpanAllColumns))
					MOON_InputManager::Select(iter->second->ID);
			}
		}

		static void ListItems_Recursively(MObject* item) {
			ImGuiTreeNodeFlags baseFlag = ImGuiTreeNodeFlags_OpenOnArrow |
										  ImGuiTreeNodeFlags_OpenOnDoubleClick |
										  ImGuiTreeNodeFlags_SpanAvailWidth |
										  ImGuiTreeNodeFlags_DefaultOpen;
			if (item->selected) baseFlag |= ImGuiTreeNodeFlags_Selected;
			if (ImGui::TreeNodeEx((SceneManager::GetTypeIcon(item) + "  " + item->name).c_str(), 
				item->transform.childs.size() ? baseFlag : baseFlag | ImGuiTreeNodeFlags_Leaf)) {
				if (item->transform.childs.size()) {
					if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0)) MOON_InputManager::Select(item->ID);
					// drag and drop
					ButtonEx::MakeDragAndDropWidget<MObject>(item, "MObject", (SceneManager::GetTypeIcon(item) + " " + item->name).c_str(),
						[](MObject* &payload, MObject* &input) -> void { input->transform.SetParent(&payload->transform); });

					ImGui::NextColumn(); ImGui::Text(std::to_string(item->ID).c_str()); ImGui::NextColumn();

					ImGui::Indent(-10);
					for (auto& child : item->transform.childs) ListItems_Recursively(child->mobject);
					ImGui::Unindent(-10);
				} else {
					if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0)) MOON_InputManager::Select(item->ID);
					// drag and drop
					ButtonEx::MakeDragAndDropWidget<MObject>(item, "MObject", (SceneManager::GetTypeIcon(item) + " " + item->name).c_str(),
						[](MObject* &payload, MObject* &input) -> void { input->transform.SetParent(&payload->transform); });

					ImGui::NextColumn(); ImGui::Text(std::to_string(item->ID).c_str()); ImGui::NextColumn();
				}
				ImGui::TreePop();
			} else {
				ImGui::NextColumn(); ImGui::Text(std::to_string(item->ID).c_str()); ImGui::NextColumn();
			}
		}

		static void ListItems_Hierarchial() {
			if (itemMap.size() < 1) return;
			ImGuiTreeNodeFlags baseFlag = ImGuiTreeNodeFlags_OpenOnArrow |
										  ImGuiTreeNodeFlags_OpenOnDoubleClick |
										  ImGuiTreeNodeFlags_SpanAvailWidth |
										  ImGuiTreeNodeFlags_DefaultOpen;
			auto iter = itemMap.begin();
			ImGui::Indent(-10);
			for (int i = 0; i < itemMap.size(); i++, iter++) {
				baseFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
						   ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
				if (iter->second->selected) baseFlag |= ImGuiTreeNodeFlags_Selected;
				if (SceneManager::GetSuperClass(SceneManager::GetType(iter->second))._Equal("MObject")) {
					auto m = (MObject*)iter->second;
					if (m->transform.parent == NULL) {
						if (!m->transform.childs.size()) baseFlag |= ImGuiTreeNodeFlags_Leaf;
						if (ImGui::TreeNodeEx((SceneManager::GetTypeIcon(m) + "  " + iter->first).c_str(), baseFlag)) {
							if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0)) MOON_InputManager::Select(m->ID);
							// drag and drop
							ButtonEx::MakeDragAndDropWidget<MObject>(m, "MObject", (SceneManager::GetTypeIcon(m) + " " + m->name).c_str(),
								[](MObject* &payload, MObject* &input) -> void { input->transform.SetParent(&payload->transform); });

							ImGui::NextColumn(); ImGui::Text(std::to_string(m->ID).c_str()); ImGui::NextColumn();
							ImGui::Indent(-10);
							for (auto& child : m->transform.childs) ListItems_Recursively(child->mobject);
							ImGui::Unindent(-10);
							ImGui::TreePop();
						} else {
							if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0)) MOON_InputManager::Select(m->ID);
							// drag and drop
							ButtonEx::MakeDragAndDropWidget<MObject>(m, "MObject", (SceneManager::GetTypeIcon(m) + " " + m->name).c_str(),
								[](MObject* &payload, MObject* &input) -> void { input->transform.SetParent(&payload->transform); });

							ImGui::NextColumn(); ImGui::Text(std::to_string(m->ID).c_str()); ImGui::NextColumn();
						}
					}
				} else {
					if (ImGui::TreeNodeEx((SceneManager::GetTypeIcon(iter->second) + "  " + iter->first).c_str(), baseFlag | ImGuiTreeNodeFlags_Leaf)) {
						if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0)) MOON_InputManager::Select(iter->second->ID);
						// drag and drop
						ButtonEx::MakeDragAndDropWidget(iter->second, SceneManager::GetType(iter->second).c_str(), 
							(SceneManager::GetTypeIcon(iter->second) + " " + iter->second->name).c_str());

						ImGui::NextColumn(); ImGui::Text(GetIDText(iter->second).c_str()); ImGui::NextColumn();
						ImGui::TreePop();
					} else {
						if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0)) MOON_InputManager::Select(iter->second->ID);
						// drag and drop
						ButtonEx::MakeDragAndDropWidget(iter->second, SceneManager::GetType(iter->second).c_str(),
							(SceneManager::GetTypeIcon(iter->second) + " " + iter->second->name).c_str());

						ImGui::NextColumn(); ImGui::Text(GetIDText(iter->second).c_str()); ImGui::NextColumn();
					}
				}
			}
			ImGui::Unindent(-10);
		}

		static std::string GetIDText(ObjectBase* object) {
			char label[32];
			if (typeid(*object) == typeid(Texture))
				sprintf_s(label, "%d-[T%d]", object->ID, dynamic_cast<Texture*>(object)->localID);
			else if (typeid(*object) == typeid(Shader))
				sprintf_s(label, "%d-[S%d]", object->ID, dynamic_cast<Shader*>(object)->localID);
			else
				sprintf_s(label, "%d", object->ID);
			return label;
		}

		static void PrintAllItems() {
			if (itemMap.size() < 1) {
				std::cout << "none object of this type in the scene!" << std::endl;
				return;
			}
			std::cout << " " << ClassOf(itemMap.begin()->second) << "s in the scene: " << std::endl;

			auto end = itemMap.end();
			std::cout << "-items(" << itemMap.size() << "):" << std::endl;
			for (auto it = itemMap.begin(); it != end; it++) {
				std::cout << "- name: " << it->first; PrintID(it->second);
			}
		}

		static bool AddItem(T* item, const bool &autoSizeFlag = true) {			
			//itemMap.insert(std::make_pair(static_cast<ObjectBase*>(item)->name, item));
			itemMap.insert(std::pair<std::string, T*>(item->name, item));
			SceneManager::AddObject(item);

			if (autoSizeFlag) {
				std::cout << std::endl;
				//auto iter = itemMap.end(); iter--;
				std::cout << "------------------------------------------------ new " << ClassOf(item) << " added ------------------------------------------------" << std::endl;
				//std::cout << "- name: \'" << iter->first << "\' added."; PrintID(iter->second);
				std::cout << "- name: \'" << item->name << "\' added."; PrintID(item);
				sizeFlag = true;
			}
			return true;
		}

		static T* GetItem(const int &ID) {
			auto end = itemMap.end();
			for (auto it = itemMap.begin(); it != end; it++) {
				if (it->second->ID == ID) return it->second;
			}
			return NULL;
		}

		static bool GetItems(const std::string &name, 
							 typename std::multimap<std::string, T*>::iterator &lower, 
							 typename std::multimap<std::string, T*>::iterator &upper) {
								lower = itemMap.lower_bound(name);
								upper = itemMap.upper_bound(name);
								if (lower == itemMap.end()) return false;
								else if (name._Equal(lower->first)) {
									return true;
								}
							}

		static T* GetItem(const std::string &name, const int &ID = MOON_FIRSTMATCH) {
			typename std::multimap<std::string, T*>::iterator beg, end;
			if (GetItems(name, beg, end)) {
				if (ID == MOON_FIRSTMATCH) return beg->second;
				else for (; beg != end; beg++)
					if (beg->second->ID == ID) return beg->second;
			}
			return NULL;
		}

		static bool IsUnique(const std::string &name, const int &ID = MOON_FIRSTMATCH) {
			if (GetItem(name, ID) != NULL) return false;
			else return true;
		}

		static bool DeleteItem(const int &ID) {
			auto end = itemMap.end();
			for (auto it = itemMap.begin(); it != end; it++) {
				if (it->second->ID == ID) {
					SceneManager::RemoveObject(ID);
					delete it->second;
					itemMap.erase(it);

					sizeFlag = true;
					return true;
				}
			}
			return false;
		}

		static bool DeleteItem(const std::string &name, const int &ID = MOON_FIRSTMATCH) {
			typename std::multimap<std::string, T*>::iterator beg, end;
			if (GetItems(name, beg, end)) {
				if (ID == MOON_FIRSTMATCH) {
					SceneManager::RemoveObject(beg->second->ID);
					delete beg->second;
					itemMap.erase(beg);

					sizeFlag = true;
					return true;
				} else for (; beg != end; beg++)
					if (beg->second->ID == ID) {
						SceneManager::RemoveObject(beg->second->ID);
						delete beg->second;
						itemMap.erase(beg);

						sizeFlag = true;
						return true;
					}
			}
			return false;
		}

		static bool DeleteItem(const T* item) {
			return DeleteItem(item->name, item->ID);
		}

		static bool DeleteItems(const std::string &name) {
			typename std::multimap<std::string, T*>::iterator beg, end;
			if (GetItems(name, beg, end)) {
				for (auto itr = beg; itr != end; ) {
					if (itr->first == name) {
						std::cout << "item: " << itr->first << "\t" << itr->second->name << " deleted." << std::endl;

						SceneManager::RemoveObject(itr->second->ID);
						delete itr->second;
						itr = itemMap.erase(itr);

						sizeFlag = true;
					} else itr++;
				}
			} else return false;
		}

		static T* RemoveItem(const int &ID, const bool &autoSetSizeFlag = true) {
			T* res = NULL;
			auto end = itemMap.end();
			for (auto it = itemMap.begin(); it != end; it++) {
				if (it->second->ID == ID) {
					res = it->second;
					itemMap.erase(it);

					if (autoSetSizeFlag) sizeFlag = true;
					return res;
				}
			}
			return res;
		}

		static T* RemoveItem(const std::string &name, const int &ID = MOON_FIRSTMATCH, const bool &autoSetSizeFlag = true) {
			T* res = NULL;
			typename std::multimap<std::string, T*>::iterator beg, end;
			if (GetItems(name, beg, end)) {
				if (ID == MOON_FIRSTMATCH) {
					res = beg->second;
					itemMap.erase(beg);

					if (autoSetSizeFlag) sizeFlag = true;
					return res;
				} else for (; beg != end; beg++)
					if (beg->second->ID == ID) {
						res = beg->second;
						itemMap.erase(beg);

						if (autoSetSizeFlag) sizeFlag = true;
						return res;
					}
			}
			return res;
		}

		static T* RemoveItem(const T* item, const bool &autoSetSizeFlag = true) {
			return RemoveItem(item->name, item->ID, autoSetSizeFlag);
		}

		static bool RemoveItems(const std::string &name,
								typename std::multimap<std::string, T*>::iterator &lower,
								typename std::multimap<std::string, T*>::iterator &upper) {
			if (GetItems(name, lower, upper)) {
				for (auto itr = lower; itr != upper; ) {
					if (itr->first == name) {
						itr = itemMap.erase(itr);
						sizeFlag = true;
					} else itr++;
				}
			} else return false;
		}

		static bool RenameItem(const int &ID, const std::string newName) {
			T* item = RemoveItem(ID, false);
			item->name = newName;
			AddItem(item, false);
			return true;
		}

		static bool RenameItem(ObjectBase* item, const std::string newName) {
			std::cout << "renamed \'" << item->name << "\' to: \'" << newName << "\'" << std::endl;
			//RemoveItem(item->name, item->ID, false);
			RemoveItem(item->ID, false);
			item->name = newName;
			AddItem(dynamic_cast<T*>(item), false);
			return true;
		}

		// all items will be deleted
		static bool Clear() {
			auto end = itemMap.end();
			for (auto itr = itemMap.begin(); itr != end; ) {
				delete itr->second;
				itr = itemMap.erase(itr);
			}

			sizeFlag = true;
			return true;
		}
	};

	class SceneManager {
	private:
		static int delID;
		static unsigned int objectCounter;
	public:
		// global parameters
		static BVH* sceneBVH;

		static Vector2 WND_SIZE;
		static Vector2 SCR_SIZE;
		static float aspect;
		static SceneView activeView;
		static ShadingMode splitShading[4];
		static LightModel lightModel[4];

		static bool debug;
		static bool exitFlag;

		// all objects
		static std::vector<MObject*>	treeList;
		static std::vector<ObjectBase*> objectList;
		static std::vector<ObjectBase*> matchedList;

		static void ReleaseBVH() {
			if (sceneBVH != nullptr) delete sceneBVH;
		}

		// building BVH tree for all models in the scene
		static void BuildBVH() {
			std::cout << "Building Scene BVH... ..." << std::endl;
			std::vector<Hitable*> renderList;
			for (auto& it : MOON_ModelManager::itemMap) {
				if (!it.second->visible) continue;
				else if (CheckType(it.second->opstack.GetDeliver(), "Model"))
					renderList.push_back((Model*)it.second->opstack.GetDeliver());
			}
			for (auto& it : MOON_ShapeManager::itemMap) {
				if (!it.second->visible || !it.second->opstack.HasDeliver()) continue;
				else if (CheckType(it.second->opstack.deliver, "Model"))
					renderList.push_back((Model*)it.second->opstack.deliver);
			}
			ReleaseBVH();
			sceneBVH = new BVH(renderList, Vector2(0, renderList.size() - 1));
			std::cout << "Root Node: min:";
			std::cout << sceneBVH->bbox.min << " max: ";
			std::cout << sceneBVH->bbox.max << std::endl;

			std::cout << "Building Local BVH... ..." << std::endl;
			std::cout << "total model number:" << renderList.size() << std::endl;
			for (auto& it : renderList) {
				dynamic_cast<Model*>(it)->BuildBVH();
			}
			std::cout << "Done." << std::endl;
		}

		static void SetWndSize(unsigned int width, unsigned int height, SceneView view) {
			MOON_SceneCameras[view]->aspect = (float)width / height;
			MOON_SceneCameras[view]->width = width / 25.0f;
			MOON_SceneCameras[view]->height = height / 25.0f;
			MOON_SceneCameras[view]->width -= aspect * MOON_SceneCameras[view]->orthoDelta;
			MOON_SceneCameras[view]->height -= MOON_SceneCameras[view]->orthoDelta;

			MOON_SceneCameras[view]->UpdateMatrix();

			if (view == MOON_ActiveView) {
				MOON_ScrSize.setValue(width, height);
				aspect = MOON_SceneCameras[view]->aspect;
			}
		}

		static void PrintAllObjects() {
			std::cout << "total object num: " << GetObjectNum() << std::endl;
			for (auto &iter : objectList) {
				std::cout << "name: " << std::setw(18) << iter->name << " |type: " << std::setw(10) << GetType(iter) << " |id: " << iter->ID << std::endl;
			}
		}
		// TODO
		static void PrintTreeList() {

		}
		static void PrintMatchedList() {

		}

		static void ListMatchedItems() {
			if (matchedList.size() < 1) return;
			ImGui::Indent(-10);
			for (auto& item : matchedList) {
				ImGuiTreeNodeFlags baseFlag = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
				if (item->selected) baseFlag |= ImGuiTreeNodeFlags_Selected;
				if (ImGui::TreeNodeEx((SceneManager::GetTypeIcon(item) + "  " + item->name).c_str(), baseFlag)) {
					if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0)) MOON_InputManager::Select(item->ID);
					// drag & drop
					ButtonEx::MakeDragAndDropWidget(item, "MObject", (SceneManager::GetTypeIcon(item) + " " + item->name).c_str());

					ImGui::NextColumn(); ImGui::Text(std::to_string(item->ID).c_str()); ImGui::NextColumn();
					ImGui::TreePop();
				} else {
					ImGui::NextColumn(); ImGui::Text(std::to_string(item->ID).c_str()); ImGui::NextColumn();
				}
			}
			ImGui::Unindent(-10);
		}

		static void AddObject(ObjectBase* item) {
			while (objectList.size() <= item->ID) objectList.push_back(item);
			objectList[item->ID] = item;
		}

		// *only remove target object from objectList, but not remove it from manager
		static void RemoveObject(unsigned int ID) {
			std::cout << "object removed: " << objectList[ID]->name << std::endl;
			objectList[ID] = nullptr;
			if (ID < delID || delID < 0) delID = ID;
		}
		static unsigned int GenUniqueID() {
			if (delID < 0) {
				return objectCounter++;
			} else {
				while (delID < objectList.size() && objectList[delID] != nullptr) {
					if (++delID >= objectCounter)
						return objectCounter++;
				}
				if (delID < objectList.size()) return delID++;
				else return objectCounter++;
			}
		}
		// delete target object from both objectList and manager
		static void DeleteObject(ObjectBase* obj) {
			std::cout << "object deleted: " << obj->name << std::endl;
			auto type = GetType(obj);
			if (type._Equal("Shader")) {
				ShaderManager::DeleteItem(obj->ID);
			} else if (type._Equal("Texture")) {
				TextureManager::DeleteItem(obj->ID);
			} else if (type._Equal("Material")) {
				MaterialManager::DeleteItem(obj->ID);
			} else if (type._Equal("Light")) {
				LightManager::DeleteItem(obj->ID);
			} else if (type._Equal("Model")) {
				ModelManager::DeleteItem(obj->ID);
			} else if (type._Equal("Camera")) {
				CameraManager::DeleteItem(obj->ID);
			} else if (type._Equal("Shape")) {
				ShapeManager::DeleteItem(obj->ID);
			} else if (type._Equal("Helper")) {
				HelperManager::DeleteItem(obj->ID);
			} else if (type._Equal("Volume")) {
				VolumeManager::DeleteItem(obj->ID);
			}
		}

		template<class T>
		static std::string GetSuperClass(T* item) {
			return GetSuperClass(GetType(item));
		}

		static std::string GetSuperClass(const std::string& _type) {
			std::string type;
			if (_type._Equal("Shader"))			type = "Shader";
			else if (_type._Equal("Texture"))	type = "Texture";
			else if (_type._Equal("Material"))	type = "Material";
			else if (_type._Equal("Camera")		||
					 _type._Equal("Model")		||
					 _type._Equal("Shape")		||
					 _type._Equal("Helper")		||
					 _type._Equal("Volume")		||
					 _type._Equal("Light"))		type = "MObject";
			else if (_type._Equal("BVH"))		type = "BVH";
			else type = "Unknown";
			return type;
		}

		template<class T>
		static std::string GetType(T* item) {
			if (item == nullptr) return "Unknown";

			std::string type;
			if (typeid(*item) == typeid(Shader))			type = "Shader";
			else if (typeid(*item) == typeid(FrameBuffer)	||
					 typeid(*item) == typeid(Texture))		type = "Texture";
			else if (typeid(*item) == typeid(MoonMtl)		|| 
					 typeid(*item) == typeid(LightMtl)		|| 
					 typeid(*item) == typeid(Lambertian)	||
					 typeid(*item) == typeid(Metal)			||
					 typeid(*item) == typeid(SEM)			||
					 typeid(*item) == typeid(SkinMtl)		||
					 typeid(*item) == typeid(HairMtl)		||
					 typeid(*item) == typeid(MatteMtl)		||
					 typeid(*item) == typeid(Dielectric))	type = "Material";
			else if (typeid(*item) == typeid(DirLight)		||
					 typeid(*item) == typeid(PointLight)	||
					 typeid(*item) == typeid(SpotLight)		||
					 typeid(*item) == typeid(MoonLight)		||
					 typeid(*item) == typeid(DomeLight))	type = "Light";
			else if (typeid(*item) == typeid(Model)			||
					 typeid(*item) == typeid(Sphere)		||
					 typeid(*item) == typeid(Plane)			||
					 typeid(*item) == typeid(Box)			||
					 typeid(*item) == typeid(Cylinder)		||
					 typeid(*item) == typeid(Ring)			||
					 typeid(*item) == typeid(Capsule))		type = "Model";
			else if (typeid(*item) == typeid(Camera))		type = "Camera";
			else if (typeid(*item) == typeid(Shape)			||
					 typeid(*item) == typeid(Line)			||
					 typeid(*item) == typeid(Circle)		||
					 typeid(*item) == typeid(Rectangle)		||
					 typeid(*item) == typeid(NGone))		type = "Shape";
			else if (typeid(*item) == typeid(Dummy)			||
					 typeid(*item) == typeid(Tape)			||
					 typeid(*item) == typeid(Proxy))		type = "Helper";
			else if (typeid(*item) == typeid(Volume))		type = "Volume";
			else if (typeid(*item) == typeid(BVH))			type = "BVH";
			else type = "Unknown";
			return type;
		}

		template<class T>
		static std::string GetTypeIcon(T* item) {
			std::string typeIcon;
			std::string type = GetType(item);
			if (typeid(*item) == typeid(Texture))			typeIcon = ICON_FA_FILE_IMAGE_O;
			else if (typeid(*item) == typeid(FrameBuffer))	typeIcon = ICON_FA_PICTURE_O;
			else if (type._Equal("Shader"))					typeIcon = ICON_FA_FILE_CODE_O;
			else if (type._Equal("Material"))				typeIcon = ICON_FA_GLOBE;
			else if (type._Equal("Light"))					typeIcon = ICON_FA_LIGHTBULB_O;
			else if (type._Equal("Model"))					typeIcon = ICON_FA_CUBE;
			else if (type._Equal("Camera"))					typeIcon = ICON_FA_VIDEO_CAMERA;
			else if (type._Equal("Shape"))					typeIcon = ICON_FA_LEMON_O;
			else if (type._Equal("Helper"))					typeIcon = ICON_FA_THUMB_TACK;
			else if (type._Equal("Volume"))					typeIcon = ICON_FA_CLOUD;
			else typeIcon = ICON_FA_QUESTION;
			return typeIcon;
		}

		template<class T>
		static void RenameItem(T* item, const std::string &newName) {
			std::string type = GetType(item);
			if (type._Equal("Shader"))				ShaderManager::RenameItem(item, newName);
			else if (type._Equal("Texture"))		TextureManager::RenameItem(item, newName);
			else if (type._Equal("Material"))		MaterialManager::RenameItem(item, newName);
			else if (type._Equal("Light"))			LightManager::RenameItem(item, newName);
			else if (type._Equal("Model"))			ModelManager::RenameItem(item, newName);
			else if (type._Equal("Camera"))			CameraManager::RenameItem(item, newName);
			else if (type._Equal("Shape"))			ShapeManager::RenameItem(item, newName);
			else if (type._Equal("Helper"))			HelperManager::RenameItem(item, newName);
			else if (type._Equal("Volume"))			VolumeManager::RenameItem(item, newName);
			else std::cout << "Unknown type, rename failed!" << std::endl;
		}

		// total object with unique ID in the scene, but some maybe null reference
		static unsigned int GetObjectNum() {
			return objectCounter;
		}

		// object number without null reference
		static int CountObject() {
			int count = ModelManager::CountItem();
			count += TextureManager::CountItem();
			count += LightManager::CountItem();
			count += CameraManager::CountItem();
			count += ShaderManager::CountItem();
			count += MaterialManager::CountItem();
			count += ShapeManager::CountItem();
			count += HelperManager::CountItem();
			count += VolumeManager::CountItem();

			return count;
		}

		static void Clear() {
			objectList.clear();
		}

		static void DrawGizmos() {
			if (MOON_ViewportState == EDIT) {
				Gizmo::ManipulateMassive(MOON_ActiveCamera->zFar);
			} else {
				ObjectBase* first = InputManager::GetFirstSelected();
				if (!first) return;
				if (!SuperClassOf(first)._Equal("MObject")) return;

				MObject* object = dynamic_cast<MObject*>(first);
				Gizmo::Manipulate(&object->transform, MOON_ActiveCamera->zFar);
			}
		}

		static void Init() {
			CameraManager::LoadSceneCamera();
			std::cout << "- Scene Camera Created." << std::endl;
			TextureManager::LoadImagesForUI();
			std::cout << "- Images For UI Loaded." << std::endl;
			ShaderManager::LoadDefaultShaders();
			std::cout << "- Default Shaders Loaded." << std::endl;
			ModelManager::CreateSkyDome();
			std::cout << "- Sky Dome Created." << std::endl;
			TextureManager::LoadHDRI("./Assets/Textures/HDRI/Newport_Loft_Ref.hdr");
			std::cout << "- Default HDRI Loaded." << std::endl;
			/*TextureManager::brdfLUT = TextureManager::LoadTexture(
				"./Assets/Textures/LUTs/ibl_brdf_lut.png", "brdfLUT", false
			);*/
			TextureManager::GenerateBRDFLUT();
			std::cout << "- BRDF LUT Created." << std::endl;
			TextureManager::GenerateIrradianceMap();
			std::cout << "- Irradiance Map Created." << std::endl;
			MaterialManager::CreateDefaultMats();
			std::cout << "- Default Materials Created." << std::endl;

			Gizmo::RecalcCircle(); Gizmo::RecalcTranslate();
			std::cout << "- Gizmo Initialized." << std::endl;

			objectList.push_back(NULL); // ID ZERO
		}

		// scene components
		struct Clock {
			static float deltaTime;
			static float lastFrame;

			static std::string GetTimeStamp() {
				struct tm *ptr;
				time_t timestamp;
				char str[80];

				timestamp = time(NULL);
				ptr = localtime(&timestamp);
				strftime(str, 100, "%Y.%m.%d %H:%M:%S", ptr);
				std::cout << "new time stamp: " << str << std::endl;

				return str;
			}
		};

		struct InputManager {
			static GLFWwindow* window;

			static Vector2 mousePos;
			static Vector2 mouseOffset;
			static Vector2 mouseScrollOffset;

			static int mouseButton;
			static int mouseAction;
			static int mouseMods;

			static int keyButton;
			static int keyAction;
			static int keyMods;

			static bool isHoverUI;
			static bool mouse_left_hold;
			static bool mouse_middle_hold;
			static bool mouse_right_hold;
			static bool left_ctrl_hold;
			static bool left_shift_hold;
			static bool right_ctrl_hold;
			static bool right_shift_hold;

			// selection
			static unsigned int hoverID;
			static bool lockSelection;
			static bool isSelectionChanged;
			static std::vector<unsigned int> selection;

			class Selector {
			public:
				template<class T>
				static void ClearSelectionPrototype(std::vector<T>& list, std::vector<unsigned int>& selected) {
					selected.clear();
					for (auto& item : list) item.selected = false;
					MOON_SelectionChanged = true;
				}

				template<class T>
				static void ClearSelectionPrototype(std::vector<T*>& list, std::vector<unsigned int>& selected) {
					selected.clear();
					for (auto& item : list) {
						if (item != nullptr) item->selected = false;
					}
					MOON_SelectionChanged = true;
				}

				template<class T>
				static void SelectPrototype(std::vector<T>& list, std::vector<unsigned int>& selected, const unsigned int ID, const bool& canIDZero = true) {
					if (!InputManager::left_ctrl_hold && !InputManager::right_ctrl_hold) {
						ClearSelectionPrototype(list, selected);
					}
					Select_AppendPrototype(list, selected, ID, true, canIDZero);
				}

				template<class T>
				static void SelectPrototype(std::vector<T*>& list, std::vector<unsigned int>& selected, const unsigned int ID, const bool& canIDZero = true) {
					if (!InputManager::left_ctrl_hold && !InputManager::right_ctrl_hold) {
						ClearSelectionPrototype(list, selected);
					}
					Select_AppendPrototype(list, selected, ID, true, canIDZero);
				}

				template<class T>
				static void Select_AppendPrototype(std::vector<T>& list, std::vector<unsigned int>& selected,
					unsigned int ID, const bool& autoInvertSelect = true, const bool& canIDZero = true) {
					if (ID >= (1 - canIDZero) && ID < list.size()) {
						if (autoInvertSelect) {
							if (list[ID].selected ^= 1) selected.push_back(ID);
							else Utility::RemoveElem(selected, ID);
							MOON_SelectionChanged = true;
						} else {
							if (list[ID].selected) return;
							else {
								list[ID].selected = true;
								selected.push_back(ID);
								MOON_SelectionChanged = true;
							}
						}
					}
				}

				template<class T>
				static void Select_AppendPrototype(std::vector<T*>& list, std::vector<unsigned int>& selected,
					unsigned int ID, const bool& autoInvertSelect = true, const bool& canIDZero = true) {
					if (ID >= (1 - canIDZero) && ID < list.size()) {
						if (autoInvertSelect) {
							if (list[ID]->selected ^= 1) selected.push_back(ID);
							else Utility::RemoveElem(selected, ID);
							MOON_SelectionChanged = true;
						} else {
							if (list[ID]->selected) return;
							else {
								list[ID]->selected = true;
								selected.push_back(ID);
								MOON_SelectionChanged = true;
							}
						}
					}
				}
			};

			static void ResetKeyState() {
				keyButton = -1;
				keyAction = -1;
				mouseButton = -1;
				mouseAction = -1;
			}

			static bool IsMouseDown(const int& button) {
				if (mouseAction == GLFW_PRESS && mouseButton == button) return true;
				return false;
			}

			static bool IsMouseRelease(const int& button) {
				if (mouseAction == GLFW_RELEASE && mouseButton == button) return true;
				return false;
			}

			static bool IsMouseRepeat(const int& button) {
				if (button == LEFT_MOUSE) return mouse_left_hold;
				if (button == RIGHT_MOUSE) return mouse_right_hold;
				if (button == MIDDLE_MOUSE) return mouse_middle_hold;
			}

			static bool IsKeyRepeat(const int& key) {
				if (keyAction == GLFW_REPEAT && key == keyButton) {
					return true;
				} else return false;
			}

			static bool IsKeyDown(const int& key) {
				if (keyAction == GLFW_PRESS && key == keyButton) {
					return true;
				} else return false;
			}

			static bool IsKeyRelease(const int& key) {
				if (keyAction == GLFW_RELEASE && key == keyButton) {
					return true;
				} else return false;
			}

			static ObjectBase* GetFirstSelected() {
				if (selection.size() < 1) return NULL;
				else return SceneManager::objectList[selection[0]];
			}

			static void ResetSizeChangeState() {
				if (ModelManager::sizeFlag  || TextureManager::sizeFlag  || 
					LightManager::sizeFlag  || MaterialManager::sizeFlag || 
					CameraManager::sizeFlag || ShaderManager::sizeFlag) {
					ModelManager::sizeFlag = false;
					TextureManager::sizeFlag = false;
					LightManager::sizeFlag = false;
					MaterialManager::sizeFlag = false;
					CameraManager::sizeFlag = false;
					ShaderManager::sizeFlag = false;
				}
			}

			static void ClearSelection() {
				if (MOON_ViewportState == EDIT) return;

				Selector::ClearSelectionPrototype(MOON_ObjectList, selection);
				/*MOON_InputManager::selection.clear();
				for (ObjectBase *obj : MOON_ObjectList) {
					if (obj != nullptr) obj->selected = false;
				}*/
			}

			static void Select(const unsigned int ID) {
				if (MOON_ViewportState == EDIT) return;

				Selector::SelectPrototype(MOON_ObjectList, selection, ID, false);
				/*if (!MOON_InputManager::left_ctrl_hold && !MOON_InputManager::right_ctrl_hold) {
					ClearSelection();
				}
				Select_Append(ID);*/
			}

			static void Select_Append(const unsigned int ID, const bool& autoInvertSelect = true) {
				Selector::Select_AppendPrototype(MOON_ObjectList, selection, ID, autoInvertSelect, false);
				/*if (ID > 0 && ID < MOON_ObjectList.size()) {
					if (autoInvertSelect) {
						if (MOON_ObjectList[ID]->selected ^= 1) {
							MOON_InputManager::selection.push_back(ID);
						} else {
							auto end = MOON_InputManager::selection.end();
							for (auto it = MOON_InputManager::selection.begin(); it != end; it++)
								if (*it == ID) {
									it = MOON_InputManager::selection.erase(it);
									return;
								}
						}
					} else {
						if (MOON_ObjectList[ID]->selected) return;
						else {
							MOON_ObjectList[ID]->selected = true;
							MOON_InputManager::selection.push_back(ID);
						}
					}
				}*/
			}

			static unsigned int GetIDFromLUT(const Vector2& screenPos) {
				Vector4 col;

				glBindFramebuffer(GL_READ_FRAMEBUFFER, TextureManager::IDLUT->fbo);
				glReadBuffer(GL_COLOR_ATTACHMENT0);
				glReadPixels(screenPos.x, TextureManager::IDLUT->height - screenPos.y - 1, 1, 1, GL_RGBA, GL_FLOAT, &col[0]);
				glReadBuffer(GL_NONE);
				glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

				hoverID = isHoverUI ? 0 : Color::IDDecoder(col);
				//std::cout << hoverID << std::endl;
				//std::cout << col << std::endl;
				return hoverID;
			}

			static void Clear() {}

			static Vector3 PickGroundPoint(const Vector2& mousePos) {
				if (MoonMath::IsRayPlaneIntersect(MOON_ActiveCamera->GetMouseRayAccurate(),
					Vector3::WORLD(UP), Vector3::ZERO())) {
					return MoonMath::RayPlaneIntersect(MOON_ActiveCamera->GetMouseRayAccurate(),
						Vector3::WORLD(UP), Vector3::ZERO());
				} else return Vector3::ZERO();
			}
		};

		struct LightManager : ObjectManager<Light> {
			static Light* CreateLight(const LightType &type, const std::string &name, 
				const Vector3 &pos = Vector3::ZERO(), const Vector4 &color = Color::WHITE()) {
				Light* newLight = nullptr;
				switch (type) {
					case directional_light:
						newLight = new DirLight(name, pos, Vector3::WORLD(DOWN), color);
						break;
					case point_light:
						newLight = new PointLight(name, pos, color);
						break;
				}
				if (newLight != nullptr) AddItem(newLight);
				return newLight;
			}
		};

		struct MaterialManager : ObjectManager<Material> {
			static Material* defaultMat;

			static void CreateDefaultMats() {
				defaultMat = MaterialManager::CreateMaterial(moonMtl, "default");
				MaterialManager::CreateMaterial(semMtl, "SEM");
				MaterialManager::CreateMaterial(skinMtl, "PreIntegerSSS");
				MaterialManager::CreateMaterial(lightMtl, "LightMat");
				MoonMtl* pbr = dynamic_cast<MoonMtl*>(MaterialManager::CreateMaterial(moonMtl, "PBRMat"));
				pbr->glossiness = Vector3::ONE() * 0.08f; pbr->metalness.x = 0.0f; pbr->reflectW.x = 1.0f;
			}

			static Material* CreateMaterial(const MatType &type, const std::string &name) {
				Material* newMat = nullptr;
				switch (type) {
					case moonMtl:
						newMat = new MoonMtl(name);
						break;
					case skinMtl:
						newMat = new SkinMtl(name);
						break;
					case lightMtl:
						newMat = new LightMtl(name);
						break;
					case semMtl:
						newMat = new SEM(name);
						break;
				}
				if (newMat != nullptr) AddItem(newMat);
				return newMat;
			}

			static Material* CreateMaterial(const std::string &type, const std::string &name) {
				Material* newMat = nullptr;
				if (type._Equal("MoonMtl")) {
					newMat = new MoonMtl(name);
				} else if (type._Equal("LightMtl")) {
					newMat = new LightMtl(name);
				} else if (type._Equal("SEM")) {
					newMat = new SEM(name);
				}
				if (newMat != nullptr) AddItem(newMat);
				return newMat;
			}
		};

		struct ShaderManager : ObjectManager<Shader> {
			static Shader* lineShader;
			static Shader* overlayShader;
			static Shader* outlineShader;

			static void LoadDefaultShaders() {
				lineShader = new Shader("ConstColor", "ConstColor.vs", "ConstColor.fs");
				outlineShader = new Shader("Outline", "Outline.vs", "Outline.fs");
				AddItem(lineShader); AddItem(outlineShader); 

				//AddItem(new Shader("SimplePhong", "SimplePhong.vs", "SimplePhong.fs"));
				AddItem(new Shader("BlinnPhong", "SimplePhong.vs", "BlinnPhong.fs"));
				AddItem(new Shader("IrMapGenerator", "IRMapGen.vs", "IRMapGen.fs"));
				AddItem(new Shader("Prefilter", "IRMapGen.vs", "Prefilter.fs"));
				AddItem(new Shader("FlatShader", "Flat.vs", "Flat.fs")); 
				AddItem(new Shader("VertexID", "VertexID.vs", "VertexID.fs")); 
				AddItem(new Shader("EnvSphere", "EnvSphere.vs", "EnvSphere.fs"));
				//AddItem(new Shader("RayMarching", "ScreenBuffer.vs", "Cloud.fs"));
				//AddItem(new Shader("Galaxy", "ScreenBuffer.vs", "Galaxy.fs"));
			}

			static Shader* CreateShader(const std::string &name, const char *vs, const char *fs) {
				Shader* res = GetItem(name);
				if (res == NULL) {
					res = new Shader(name, vs, fs);
					AddItem(res);
				}
				return res;
			}

			/*static bool Clear() {
				auto end = itemMap.end();
				for (auto itr = itemMap.begin(); itr != end; ) {
					delete itr->second;
					itr = itemMap.erase(itr);
				}
				return true;
			}*/
		};

		struct TextureManager : ObjectManager<Texture> {
			static FrameBuffer* SHADOWMAP;
			static FrameBuffer* IDLUT;

			static Texture* HDRI;
			static FrameBuffer* brdfLUT;
			static FrameBuffer* Irradiance;
			static FrameBuffer* prefilterMap;

			static std::vector<FrameBuffer*> SCENEBUFFERS;

			// create buffers
			static void CreateBuffers() {
				CreateIDLUT();
				CreateShadowMap();
				CreateSceneBuffer();
			}

			// TODO
			static void CreateShadowMap() {

			}

			static void CreateIDLUT() {
				// create IDLUT
				IDLUT = new FrameBuffer(MOON_ScrSize.x, MOON_ScrSize.y, "IDLUT", MOON_AUTOID);
				AddItem(IDLUT);
			}

			static void CreateSceneBuffer() {
				SCENEBUFFERS.push_back(new FrameBuffer(-1, -1, "PerspView", MOON_AUTOID, GL_RGB16F, GL_FLOAT));
				SCENEBUFFERS.push_back(new FrameBuffer(-1, -1, "FrontView", MOON_AUTOID, GL_RGB16F, GL_FLOAT));
				SCENEBUFFERS.push_back(new FrameBuffer(-1, -1, "LeftView", MOON_AUTOID, GL_RGB16F, GL_FLOAT));
				SCENEBUFFERS.push_back(new FrameBuffer(-1, -1, "TopView", MOON_AUTOID, GL_RGB16F, GL_FLOAT));
				for (auto it = SCENEBUFFERS.begin(); it != SCENEBUFFERS.end(); it++) {
					AddItem(*it);
				}
			}

			// load resources
			static void LoadImagesForUI() {
				AddItem(new Texture("./Resources/Icon.jpg", "moon_icon"));
				AddItem(new Texture("./Resources/logo.png", "moon_logo"));
				AddItem(new Texture("./Resources/Icon_fullSize.png", "moon_logo_full"));
			}

			static void UpdateEnvironmentTex() {
				static std::string hdriPath = HDRI->path;
				if (!hdriPath._Equal(HDRI->path)) {
					GenerateIrradianceMap();
					hdriPath = HDRI->path;
				}
			}

			static void LoadHDRI(const std::string& path) {
				if (HDRI != nullptr) DeleteItem(HDRI);
				HDRI = new Texture(path, "SkyHDRI", Linear, true);
				AddItem(HDRI);
			}

			static void GenerateBRDFLUT(const Vector2& mapSize = Vector2(512, 512)) {
				if (brdfLUT == nullptr) {
					brdfLUT = new FrameBuffer(mapSize, "brdfLUT", MOON_AUTOID, GL_RGB16F, GL_FLOAT);
					AddItem(brdfLUT);
				}
				auto lutShader = ShaderManager::CreateShader("brdfLUTGen", "BRDF.vs", "BRDF.fs");
				lutShader->use();
				
				// rendering brdf lut
				glDisable(GL_DEPTH_TEST);
				glViewport(0, 0, mapSize.x, mapSize.y);

				glBindFramebuffer(GL_FRAMEBUFFER, brdfLUT->fbo);
				glBindVertexArray(Graphics::GetScreenQuad());
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glBindVertexArray(0);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glViewport(0, 0, MOON_WndSize.x, MOON_WndSize.y);
				glEnable(GL_DEPTH_TEST);
			}

			static void GenerateIrradianceMap(const Vector2& mapSize = Vector2(512, 512)) {
				if (Irradiance == nullptr) {
					Irradiance = new FrameBuffer(mapSize, "irMap", MOON_AUTOID, GL_RGB16F, GL_FLOAT);
					AddItem(Irradiance);
				}
				if (prefilterMap == nullptr) {
					prefilterMap = new FrameBuffer(mapSize, "prefilteredEnv", MOON_AUTOID, GL_RGB16F, GL_FLOAT, Linear, true);
					AddItem(prefilterMap);
				}

				// PBR: solve diffuse integral by convolution to create an irradiance map ----
				auto irradianceShader = ShaderManager::GetItem("IrMapGenerator");
				irradianceShader->use();
				irradianceShader->setTexture("environmentMap", HDRI, 0);
				
				// rendering irradiance map
				glDisable(GL_DEPTH_TEST);
				glViewport(0, 0, Irradiance->width, Irradiance->height);
				
				glBindFramebuffer(GL_FRAMEBUFFER, Irradiance->fbo);
				glBindVertexArray(Graphics::GetScreenQuad());
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glBindVertexArray(0);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				// ---------------------------------------------------------------------------

				// PBR: create prefiltered environment reflection map ------------------------
				// run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
				glBindFramebuffer(GL_FRAMEBUFFER, prefilterMap->fbo);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
				//glDeleteRenderbuffers(1, &prefilterMap->rbo);
				//glGenRenderbuffers(1, &prefilterMap->rbo);
				glBindRenderbuffer(GL_RENDERBUFFER, prefilterMap->rbo);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mapSize.x, mapSize.y);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, prefilterMap->rbo);
				
				unsigned int maxMipLevels = 5;
				auto prefilterShader = ShaderManager::GetItem("Prefilter");
				prefilterShader->use();
				prefilterShader->setTexture("environmentMap", HDRI, 0);
				for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
					// resize framebuffer according to mip-level size.
					unsigned int mipWidth = mapSize.x * std::pow(0.5, mip);
					unsigned int mipHeight = mapSize.y * std::pow(0.5, mip);
					glBindRenderbuffer(GL_RENDERBUFFER, prefilterMap->rbo);
					glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
					glViewport(0, 0, mipWidth, mipHeight);

					float roughness = (float)mip / (float)(maxMipLevels - 1);
					prefilterShader->setFloat("roughness", roughness);

					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, prefilterMap->localID, mip);
					//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					// rendering prefiltered map
					glBindVertexArray(Graphics::GetScreenQuad());
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
					glBindVertexArray(0);
				}
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				// ---------------------------------------------------------------------------

				glViewport(0, 0, MOON_WndSize.x, MOON_WndSize.y);
				glEnable(GL_DEPTH_TEST);
			}

			// TODO
			static Texture* CreateTexture();

			static Texture* LoadTexture(const std::string &path, const std::string &name = UseFileName,
				const ColorSpace& _colorSpace = sRGB, bool mipmap = false) {
				Texture* tex = new Texture(path, name, _colorSpace, mipmap);

				AddItem(tex);
				return tex;
			}
		};

		struct ShapeManager : ObjectManager<Shape> {
			static Shape* CreateShape(const ShapeType &type, const std::string &name) {
				Shape* newShape = nullptr;
				switch (type) {
					case line:
						newShape = new Line(name);
						break;
				}
				if (newShape != nullptr) AddItem(newShape);
				return newShape;
			}
		};

		struct VolumeManager : ObjectManager<Volume> {
			static Volume* LoadVolume(const std::string &path, const std::string &name = UseFileName) {
				Volume* newVolume = nullptr;

				if (newVolume != nullptr) AddItem(newVolume);
				return newVolume;
			}

			static Volume* CreateVolume(const std::string &name, const bool& interactive = false, 
				const Vector3& position = Vector3::ZERO(), const Vector3& boundMin = Vector3(-1, -1, -1), 
				const Vector3& boundMax = Vector3::ONE()) {
				Volume* newVolume = nullptr;

				newVolume = new Volume(name, interactive);
				if (!interactive) {
					newVolume->transform.position = position;
					newVolume->bbox.min = boundMin;
					newVolume->bbox.max = boundMax;
				}

				if (newVolume != nullptr) AddItem(newVolume);
				return newVolume;
			}
		};

		struct ModelManager : ObjectManager<Model> {
			static Model* skyDome;

			static bool Clear() {
				if (skyDome != nullptr) delete skyDome;
				for (auto itr = itemMap.begin(); itr != itemMap.end(); ) {
					delete itr->second;
					itr = itemMap.erase(itr);
				}
				return true;
			}

			static void CreateSkyDome() {
				skyDome = new Sphere("SkyDome", false, 900.0f, 24, MOON_UNSPECIFIEDID);
				//skyDome->transform.rotation = Quaternion(Vector3(0, 0, 180.0f));
				//skyDome->transform.Rotate(Quaternion::Rotate(Vector3(1, 0, 0), 90 * Deg2Rad));
			}

			static bool Hit(const Ray &r, HitRecord &rec, const int& acc) {
				HitRecord tempRec;
				bool hitAnything = false;

				if (acc == 2) { // BVH
					if (sceneBVH->Hit(r, rec)) hitAnything = true;
				} else {
					for (auto &iter : itemMap) {
						if (iter.second->visible) {
							if (iter.second->Hit(r, tempRec)) {
								hitAnything = true;
								rec = tempRec;
							}
						}
					}
				}

				return hitAnything;
			}

			/*static Model* CreateModel(const std::string &path, const std::string &name = UseFileName) {
				Model* newModel = new Model(path, name);
				AddItem(newModel);

				return newModel;
			}*/

			static Model* CreateSmartMesh(const SmartMesh& type, const std::string &name, const bool& interactive = false) {
				Model* model = nullptr;
				switch (type) {
					case sphere:
						model = new Sphere(name, interactive);
						break;
					case plane:
						model = new Plane(name, interactive);
						break;
					case box:
						model = new Box(name, interactive);
						break;
				}
				if (model != nullptr) AddItem(model);
				return model;
			}

			static Model* LoadModel(const std::string &path, const bool asynchronous = true, const std::string &name = UseFileName) {
				Model* newModel = new Model(path, asynchronous, name);

				return newModel;
			}
		};

		struct CameraManager : ObjectManager<Camera> {
			static std::vector<Camera*> sceneCameras;
			static Camera* activeCamera;

			static Camera* CreateCamera() {
				Camera* newCam = new Camera();
				AddItem(newCam);

				return newCam;
			}
			static Camera* CreateCamera(const std::string name) {
				Camera* newCam = new Camera(name);
				AddItem(newCam);

				return newCam;
			}

			static bool LoadSceneCamera() {
				sceneCameras.push_back(new Camera("PerspCamera", Vector3(0.0f, 2.0f, 20.0f), Vector3::WORLD(BACKWARD), false, 0.0f, MOON_AUTOID));
				sceneCameras.push_back(new Camera("FrontCamera", Vector3(0.0f, 0.0f, 20.0f), Vector3::WORLD(BACKWARD), true, 0.0f, MOON_AUTOID));
				sceneCameras.push_back(new Camera("LeftCamera", Vector3(20.0f, 0.0f, 0.0f), Vector3::WORLD(RIGHT), true, 0.0f, MOON_AUTOID));
				sceneCameras.push_back(new Camera("TopCamera", Vector3(0.0f, 20.0f, 0.0f), Vector3::WORLD(DOWN), true, 0.0f, MOON_AUTOID));
				AddItem(sceneCameras[0]); AddItem(sceneCameras[1]); AddItem(sceneCameras[2]); AddItem(sceneCameras[3]);
				sceneCameras[0]->visible = sceneCameras[1]->visible = sceneCameras[2]->visible = sceneCameras[3]->visible = false;
				sceneCameras[1]->zFar = sceneCameras[2]->zFar = sceneCameras[3]->zFar = 100.0f;
				Renderer::targetCamera = sceneCameras[0]; activeCamera = sceneCameras[3];
				MOON_ActiveCamera = MOON_SceneCameras[persp];
				return true;
			}

			static bool Clear() {
				/*for (auto it = MOON_SceneCameras.begin(); it != MOON_SceneCameras.end(); it++)
					delete *it;
				MOON_SceneCameras.clear();*/

				for (auto itr = itemMap.begin(); itr != itemMap.end(); ) {
					delete itr->second;
					itr = itemMap.erase(itr);
				}
				return true;
			}
		};

		struct HelperManager : ObjectManager<Helper> {
			static Helper* CreateHelper(const HelperType &type, const std::string &name) {
				Helper* newHelper = nullptr;
				switch (type) {
					case dummy:
						newHelper = new Dummy(name);
						break;
				}
				if (newHelper != nullptr) AddItem(newHelper);
				return newHelper;
			}
		};

	};
}