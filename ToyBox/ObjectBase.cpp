#include <iostream>

#include "ObjectBase.h"
#include "SceneMgr.h"
#include "OperatorBase.h"

namespace MOON {
	ObjectBase::ObjectBase(const int &_id) : visible(true) {
		if (_id == MOON_AUTOID) {
			ID = SceneManager::GenUniqueID();
		} else ID = _id;
		name = "Object_" + ID;
	}

	ObjectBase::ObjectBase(const std::string &_name, const int &_id) : name(_name), visible(true) {
		if (_id == MOON_AUTOID) {
			ID = SceneManager::GenUniqueID();
		} else ID = _id;
	}

	void ObjectBase::Rename(const std::string &newName) {
		SceneManager::RenameItem(this, newName);
	}

	void ObjectBase::ListName() {
		// list name
		char buf[64]; strcpy(buf, name.c_str());

		ImGui::Text("Name:"); ImGui::SameLine();
		ImGui::InputText(UniquePropName("Name"), buf, 64); ImGui::SameLine();
		ImGui::Checkbox(UniquePropName("Visible"), &visible, true);

		// if user renamed this object
		if (strcmp(buf, name.c_str())) Rename(buf);
	}

	void ObjectBase::ListProperties() {
		ListName();
		ImGui::Spacing();
	}

	void MObject::OPStack::ExecuteAll() {
		for (auto &iter : opList) {
			iter->Execute();
		}
	}
	void MObject::OPStack::ListStacks() {
		static auto stackName = std::string(ICON_FA_WRENCH) + " OP-Stack";
		ImGui::Checkbox(UniquePropNameFromParent("enableOPStack"), &enable, true); ImGui::SameLine();
		ImGui::Button(ICON_FA_PLUS, ImVec2(22, 22), parent->ID); ImGui::SameLine();
		if (ImGui::CollapsingHeader(stackName.c_str(), ImGuiTreeNodeFlags_DefaultOpen, parent->ID)) {
			for (auto &iter : opList) {
				iter->ListProperties();
			}
		}
	}

	void MObject::OPStack::AddStack(Operator* op) {
		opList.push_back(op);
	}
	void MObject::OPStack::RemoveStack(Operator* op) {
		auto end = opList.end();
		for (auto iter = opList.begin(); iter != end; iter++) {
			if ((*iter)->name._Equal(op->name)) {
				iter = opList.erase(iter);
				break;
			}
		}
	}
	void MObject::OPStack::ClearStack() {
		auto end = opList.end();
		for (auto iter = opList.begin(); iter != end;) {
			delete *iter;
			iter = opList.erase(iter);
		}
	}

	void MObject::ListTransform() {
		// list transform
		Vector3 &euler = transform.rotation.eulerAngles;
		float pos[3] = { transform.position.x, transform.position.y, transform.position.z };
		float scale[3] = { transform.scale.x, transform.scale.y, transform.scale.z };
		float rotEuler[3] = { euler.x, euler.y, euler.z };

		ImGui::Text("Transform:");
		ImGui::Indent(10.0f);
		ImGui::Text("Position"); ImGui::SameLine(80.0f);
		ImGui::DragFloat3(UniquePropName("Pos"), pos, 0.1f, -INFINITY, INFINITY, "%.3f", 1.0f, true);
		ImGui::Text("Rotation"); ImGui::SameLine(80.0f);
		ImGui::DragFloat3(UniquePropName("Rot"), rotEuler, 0.1f, -INFINITY, INFINITY, "%.3f", 1.0f, true);
		ImGui::Text("Scale"); ImGui::SameLine(80.0f);
		ImGui::DragFloat3(UniquePropName("Sca"), scale, 0.1f, -INFINITY, INFINITY, "%.3f", 1.0f, true);
		ImGui::Unindent(10.0f);

		Quaternion deltaQ = Quaternion(rotEuler[0] - euler.x,
									   rotEuler[1] - euler.y,
									   rotEuler[2] - euler.z);
		transform.Rotate(deltaQ);

		transform.set(&Vector3(pos), NULL, &Vector3(scale));
	}

	void MObject::ListProperties() {
		// list name
		ListName();
		ImGui::Separator();

		// list transform
		ListTransform();
		ImGui::Separator();

		// list operators
		opstack.ListStacks();
		ImGui::Spacing();
	}
}