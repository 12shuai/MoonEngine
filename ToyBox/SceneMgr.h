#pragma once
#include <map>
#include <vector>
#include <string>

#include "Gizmo.h"
#include "Vector2.h"
#include "Camera.h"
#include "Model.h"
#include "Light.h"
#include "Texture.h"
#include "MShader.h"
#include "Material.h"
#include "MatSphere.h"
#include "ShapeBase.h"

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

			std::vector<unsigned int> &sel = MOON_InputManager::selected;
			auto iter = itemMap.begin();
			for (int i = 0; i < itemMap.size(); i++, iter++) {
				if (ImGui::Selectable((SceneManager::GetTypeIcon(iter->second) + 
									   "  " + iter->first).c_str(),
									   MOON_InputManager::selection[iter->second->ID], 
									   ImGuiSelectableFlags_SpanAllColumns)) {
					if (!MainUI::io->KeyCtrl) {
						sel.clear();
						memset(MOON_InputManager::selection, 0, SceneManager::GetObjectNum() * sizeof(bool));
					}
					if (MOON_InputManager::selection[iter->second->ID] ^= 1)
						sel.push_back(iter->second->ID);
					else {
						auto end = sel.end();
						for (auto it = sel.begin(); it != end; it++)
							if (*it == iter->second->ID) {
								it = sel.erase(it);
								return;
							}
					}
				}
			}
		}

		static void ListID() {
			for (auto &iter : itemMap) {
				char label[32];
				if (typeid(*itemMap.begin()->second) == typeid(Texture))
					sprintf_s(label, "%d-[T%d]", iter.second->ID, dynamic_cast<Texture*>(iter.second)->localID);
				else if (typeid(*itemMap.begin()->second) == typeid(Shader))
					sprintf_s(label, "%d-[S%d]", iter.second->ID, dynamic_cast<Shader*>(iter.second)->localID);
				else
					sprintf_s(label, "%d", iter.second->ID);
				ImGui::Text(label);
			}
		}

		static void PrintAllItems() {
			if (itemMap.count() < 1) {
				std::cout << "none object of this type in the scene!" << std::endl;
				return;
			}
			std::cout << " " << SceneManager::GetType(itemMap.begin()->second) << "s in the scene: " << std::endl;

			auto end = itemMap.end();
			std::cout << "-items(" << itemMap.count() << "):" << std << std::endl;
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
				auto iter = itemMap.end(); iter--;
				std::cout << "------------------------------------------------ new " << SceneManager::GetType(item) << " added ------------------------------------------------" << std::endl;
				std::cout << "- name: \'" << iter->first << "\' added."; PrintID(iter->second);
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
								else return true;
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
					SceneManager::DelObject(ID);
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
					SceneManager::DelObject(beg->second->ID);
					delete beg->second;
					itemMap.erase(beg);

					sizeFlag = true;
					return true;
				} else for (; beg != end; beg++)
					if (beg->second->ID == ID) {
						SceneManager::DelObject(beg->second->ID);
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

						SceneManager::DelObject(itr->second->ID);
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
			RemoveItem(item->name, item->ID, false);
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
		static Vector2 SCR_SIZE;
		static float aspect;

		static bool showbbox;

		// all objects
		static std::vector<ObjectBase*> objectList;
		static std::vector<ObjectBase*> matchedList;

		static void SetWndSize(unsigned int width, unsigned int height) {
			SCR_SIZE.setValue(width, height);
			aspect = (float)width / height;
		}

		static void AddObject(ObjectBase* item) {
			while (objectList.size() <= item->ID) objectList.push_back(item);
			objectList[item->ID] = item;
		}
		static void DelObject(unsigned int ID) {
			std::cout << "object deleted: " << objectList[ID]->name << std::endl;
			objectList[ID] = NULL;
			if (ID < delID) delID = ID;
		}
		static unsigned int GenUniqueID() {
			if (delID < 0) {
				return objectCounter++;
			} else {
				while (objectList[delID] != NULL) {
					if (++delID >= objectCounter)
						return objectCounter++;
				}
				return delID++;
			}
		}

		template<class T>
		static std::string GetType(T* item) {
			std::string type;
			if (typeid(*item) == typeid(Shader)) type = "Shader";
			else if (typeid(*item) == typeid(Texture)) type = "Texture";
			else if (typeid(*item) == typeid(MoonMtl) || 
					 typeid(*item) == typeid(LightMtl) || 
					 typeid(*item) == typeid(Lambertian) ||
					 typeid(*item) == typeid(Metal) ||
					 typeid(*item) == typeid(Dielectric)) type = "Material";
			else if (typeid(*item) == typeid(DirLight) ||
					 typeid(*item) == typeid(PointLight) ||
					 typeid(*item) == typeid(SpotLight) ||
					 typeid(*item) == typeid(MoonLight) ||
					 typeid(*item) == typeid(DomeLight)) type = "Light";
			else if (typeid(*item) == typeid(Model)) type = "Model";
			else if (typeid(*item) == typeid(Camera)) type = "Camera";
			else type = "Unknown";
			return type;
		}

		template<class T>
		static std::string GetTypeIcon(T* item) {
			std::string typeIcon;
			if (typeid(*item) == typeid(Shader)) typeIcon = ICON_FA_FILE_CODE_O;
			else if (typeid(*item) == typeid(Texture)) typeIcon = ICON_FA_PICTURE_O;
			else if (typeid(*item) == typeid(MoonMtl) ||
					 typeid(*item) == typeid(LightMtl) ||
					 typeid(*item) == typeid(Lambertian) ||
					 typeid(*item) == typeid(Metal) ||
					 typeid(*item) == typeid(Dielectric)) typeIcon = ICON_FA_GLOBE;
			else if (typeid(*item) == typeid(DirLight) ||
					 typeid(*item) == typeid(PointLight) ||
					 typeid(*item) == typeid(SpotLight) ||
					 typeid(*item) == typeid(MoonLight) ||
					 typeid(*item) == typeid(DomeLight)) typeIcon = ICON_FA_LIGHTBULB_O;
			else if (typeid(*item) == typeid(Model)) typeIcon = ICON_FA_CUBE;
			else if (typeid(*item) == typeid(Camera)) typeIcon = ICON_FA_VIDEO_CAMERA;
			else typeIcon = ICON_FA_QUESTION;
			return typeIcon;
		}

		template<class T>
		static void RenameItem(T* item, const std::string &newName) {
			if (typeid(*item) == typeid(Shader)) ShaderManager::RenameItem(item, newName);
			else if (typeid(*item) == typeid(Texture)) TextureManager::RenameItem(item, newName);
			else if (typeid(*item) == typeid(MoonMtl) ||
					 typeid(*item) == typeid(LightMtl) ||
					 typeid(*item) == typeid(Lambertian) ||
					 typeid(*item) == typeid(Metal) ||
					 typeid(*item) == typeid(Dielectric)) MaterialManager::RenameItem(item, newName);
			else if (typeid(*item) == typeid(DirLight) ||
					 typeid(*item) == typeid(PointLight) ||
					 typeid(*item) == typeid(SpotLight) ||
					 typeid(*item) == typeid(MoonLight) ||
					 typeid(*item) == typeid(DomeLight)) LightManager::RenameItem(item, newName);
			else if (typeid(*item) == typeid(Model)) ModelManager::RenameItem(item, newName);
			else if (typeid(*item) == typeid(Camera)) CameraManager::RenameItem(item, newName);
			else std::cout << "Unknown type, rename failed!" << std::endl;
		}

		static unsigned int GetObjectNum() {
			return objectCounter;
		}
		static int CountObject() {
			int count = ModelManager::CountItem();
			count += TextureManager::CountItem();
			count += LightManager::CountItem();
			count += CameraManager::CountItem();
			count += ShaderManager::CountItem();
			count += MaterialManager::CountItem();

			return count;
		}

		static void Clear() {
			delete CameraManager::sceneCamera;
			delete MaterialManager::matBall;
			objectList.clear();
		}

		static void DrawGizmos() {
			ObjectBase* first = InputManager::GetFirstSelected();
			if (!first) return; if (!GetType(first)._Equal("Model")) return;

			Gizmo::Manipulate(CameraManager::sceneCamera->GetMouseRay(),
							  &dynamic_cast<Model*>(first)->transform,
							  ShaderManager::lineShader,
							  InputManager::mouse_left_hold,
							  MOON_CurrentCamera->zFar);
		}

		static void Init() {
			TextureManager::LoadImagesForUI();
			std::cout << "- Images For UI Loaded." << std::endl;
			ShaderManager::LoadDefaultShaders();
			std::cout << "- Default Shaders Loaded." << std::endl;
			MaterialManager::PrepareMatBall();
			std::cout << "- Material Ball Created." << std::endl;
			MaterialManager::CreateDefaultMats();
			std::cout << "- Default Materials Created." << std::endl;
			CameraManager::LoadSceneCamera();
			std::cout << "- Scene Camera Created." << std::endl;

			Gizmo::RecalcCircle(); Gizmo::RecalcTranslate();
			std::cout << "- Gizmo Initialized." << std::endl;

			objectList.push_back(NULL); // ID ZERO
		}

		// scene components
		struct Clock {
			static float deltaTime;
			static float lastFrame;
		};

		struct InputManager {
			static Vector2 mousePos;
			static Vector2 mouseOffset;
			static Vector2 mouseScrollOffset;
			static int mouseButton;
			static int mouseAction;
			static int mouseMods;
			static bool isHoverUI;
			static bool mouse_left_hold;
			static bool mouse_middle_hold;
			static bool mouse_right_hold;

			// selection
			static bool* selection;
			static std::vector<unsigned int> selected;

			static ObjectBase* GetFirstSelected() {
				if (selected.size() < 1) return NULL;
				else return SceneManager::objectList[selected[0]];
			}

			static void UpdateSelectionState() {
				if (ModelManager::sizeFlag || TextureManager::sizeFlag || LightManager::sizeFlag ||
					MaterialManager::sizeFlag || CameraManager::sizeFlag || ShaderManager::sizeFlag) {

					if(selection != NULL) free(selection);
					selection = (bool*)malloc(GetObjectNum() * sizeof(bool));
					memset(selection, 0, GetObjectNum() * sizeof(bool));

					ModelManager::sizeFlag = false;
					TextureManager::sizeFlag = false;
					LightManager::sizeFlag = false;
					MaterialManager::sizeFlag = false;
					CameraManager::sizeFlag = false;
					ShaderManager::sizeFlag = false;
				}
			}

			static void Clear() {
				if (selection != NULL) free(selection);
			}
		};

		struct LightManager : ObjectManager<Light> {

		};

		struct MaterialManager : ObjectManager<Material> {
			static Material* defaultMat;
			static Sphere* matBall;

			static void PrepareMatBall() {
				matBall = new Sphere(Vector3::ZERO(), 1.0f);
			}

			static void CreateDefaultMats() {
				defaultMat = MaterialManager::CreateMaterial("MoonMtl", "default");
			}

			static Material* CreateMaterial(const std::string &type, const std::string &name) {
				if (type._Equal("MoonMtl")) {
					Material* newMat = new MoonMtl(name);
					AddItem(newMat);
					return newMat;
				} else {
					
				}
			}
		};

		struct ShaderManager : ObjectManager<Shader> {
			static Shader* lineShader;

			static void LoadDefaultShaders() {
				lineShader = new Shader("ConstColor", "ConstColor.vs", "ConstColor.fs");
				AddItem(lineShader);
				//AddItem(new Shader("SimplePhong", "SimplePhong.vs", "SimplePhong.fs"));
			}

			static Shader* CreateShader(const std::string &name, const char *vs, const char *fs) {
				Shader* res = GetItem(name);
				if (res == NULL) {
					res = new Shader(name, vs, fs);
					AddItem(res);
				}
				return res;
			}

			static bool Clear() {
				auto end = itemMap.end();
				for (auto itr = itemMap.begin(); itr != end; ) {
					glDeleteShader(itr->second->ID);
					delete itr->second;
					itr = itemMap.erase(itr);
				}
				return true;
			}
		};

		struct TextureManager : ObjectManager<Texture> {
			// load resources
			static void LoadImagesForUI() {
				AddItem(new Texture("./Resources/Icon.jpg", "moon_icon"));
				AddItem(new Texture("./Resources/logo.png", "moon_logo"));
				AddItem(new Texture("./Resources/Icon_fullSize.png", "moon_logo_full"));
			}

			// TODO
			static Texture* CreateTexture();

			static Texture* LoadTexture(const std::string &path, bool gamma = false) {
				Texture* tex = new Texture(path);

				AddItem(tex);
				return tex;
			}

			static bool Clear() {
				auto end = itemMap.end();
				for (auto itr = itemMap.begin(); itr != end; ) {
					glDeleteTextures(1, &itr->second->ID);
					delete itr->second;
					itr = itemMap.erase(itr);
				}
				return true;
			}
		};

		struct ShapeManager : ObjectManager<Shape> {

		};

		struct ModelManager : ObjectManager<Model> {
			static bool Hit(const Ray &r, HitRecord &rec) {
				HitRecord tempRec;
				bool hitAnything = false;

				for (auto &iter : itemMap) {
					if (iter.second->visible) {
						if (iter.second->Hit(r, tempRec)) {
							hitAnything = true;
							rec = tempRec;
						}
					}
				}

				return hitAnything;
			}

			static void DrawModels() {
				for (auto &obj : itemMap) {
					if (obj.second->visible)
						obj.second->Draw();
				}
			}

			static Model* CreateModel() {
			
			}

			static Model* LoadModel(const std::string &path, const std::string &name = "FILENAME") {
				Model* newModel = new Model(path, name);
				AddItem(newModel);

				return newModel;
			}
		};

		struct CameraManager : ObjectManager<Camera> {
			static Camera* sceneCamera;
			static Camera* currentCamera;

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
				sceneCamera = new Camera("SceneCamera", Vector3(0.0f, 2.0f, 20.0f), 0.0f, MOON_UNSPECIFIEDID);
				currentCamera = sceneCamera;
				Renderer::targetCamera = sceneCamera;
				return true;
			}
		};

	};
}