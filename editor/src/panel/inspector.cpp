#include "panel/inspector.h"
#include "components/material_manager.h"
#include "components/mesh_renderer.h"
#include "components/text.h"
#include "components/text_mesh.h"
#include "components/button.h"
#include "gameobject/transform.h"
#include "scripting/iscript.h"
#include "scripting/scripting.h"
#include "scripting/script_manager.h"
#include "context/editor_ui.h"
#include "imgui_internal.h"
#include "resources/resource_manager.h"
#include "resources/iresource.h"
#include "serialization/serializer.h"
#include "renderer/widget_renderer.h"

void UI::InspectorPanel::Draw()
{
	ImGui::Begin("Inspector");

	currentGameObject = UIEditor::engine.GetRenderer()->GetSelectedGameObject();

	if (currentGameObject)
	{
		ImGui::SetWindowFontScale(1.5f);

		char buffer[256];

		strcpy(buffer, currentGameObject->GetName().c_str());

		buffer[sizeof(buffer) - 1] = '\0';

		if (ImGui::InputTextWithHint("##GameObject Name", "Enter name...", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
			currentGameObject->SetName(std::string(buffer));

		ImGui::SetWindowFontScale(1.f);

		ImGui::SameLine();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.f);

		if (ImGui::Button("Make prefab"))
		{
			Serializer::GetInstance().WritePrefab(currentGameObject);
		}

		ImGui::SetCursorPos({ ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 10.f });

		DrawTransform(currentGameObject);

		DrawAllComponents(currentGameObject);

		DrawMaterial(currentGameObject, UIEditor::engine.GetEditorTexture("material_icon.png"));

		ImGui::SetCursorPos({ ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 20.f});
		ImGui::Separator();

		DrawAddComponentPopUp(currentGameObject);
	}

	ImGui::End();
}

void UI::InspectorPanel::DrawAllComponents(GameObject* go)
{
	if (go->GetComponents().empty())
		return;

	for (IComponent* component : go->GetComponents())
	{
		rttr::type t = rttr::type::get(*component);
		auto it = UIEditor::engine.componentsMap.find(t);
		if (it != UIEditor::engine.componentsMap.end())
		{
			DrawComponent(component, it->second.nameToDisplay, UIEditor::engine.GetEditorTexture(it->second.iconName));
		}
	}
}

void UI::InspectorPanel::DrawComponent(IComponent* component, const std::string &name, unsigned int textureID, const std::string& scriptTypeName)
{
	ImGui::PushID(component);
	bool isOpen = UI::CollapsingHeaderImage(name.c_str(), textureID, ImGuiTreeNodeFlags_DefaultOpen);

	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (ImGui::GetFontSize() * 4.f));

	UI::Style::PushStyleFrom(ImGuiCol_Button, ImGuiCol_Header);
	UI::Style::PushStyle(ImGuiCol_ButtonHovered, UI::Color::EnigmaGrey5);
	std::string s = "##RemoveComponent" + name;

	if (typeid(*component) == typeid(GameCamera))
	{
		const char* buttonText = "Make main camera";
		ImVec2 buttonSize = { 200.f, 30.f };

		if (ImGui::ImageButton(buttonText, UIEditor::engine.GetEditorTexture("active_icon.png"), ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize())))
			SceneManager::GetInstance().GetCurrentScene()->SetGameCam(currentGameObject);
	}

	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (2.f * ImGui::GetFontSize()));

	if (ImGui::ImageButton(s.c_str(), UIEditor::engine.GetEditorTexture("trash_icon.png"), ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize())))
	{
		UIEditor::engine.GetRenderer()->GetSelectedGameObject()->DeleteComponent(component);
		UI::Style::PopStyle();
		ImGui::PopID();
		return;
	}
	UI::Style::PopStyle();

	if (!isOpen)
	{
		ImGui::PopID();
		return;
	}

	ImGui::Indent();

	const std::string componentTypeName = rttr::type::get(*component).get_name().to_string();
	const auto* descriptors = UIEditor::engine.GetComponentPropertyDescriptors(componentTypeName);

	if (descriptors && !descriptors->empty())
	{
		std::map<std::string, std::vector<const ComponentPropertyDescriptor*>> categories;
		for (const ComponentPropertyDescriptor& descriptor : *descriptors)
		{
			const std::string category = descriptor.uiData.category.empty() ? "" : descriptor.uiData.category;
			categories[category].push_back(&descriptor);
		}

		for (auto& [category, properties] : categories)
		{
			bool draw = true;

			if (!category.empty())
			{
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;
				draw = ImGui::TreeNodeEx(category.c_str(), flags);
			}

			if (draw)
			{
				for (const ComponentPropertyDescriptor* descriptor : properties)
					DrawProperty(component, *descriptor);

				if (!category.empty())
					ImGui::TreePop();
			}
		}
	}
	ImGui::PopID();
	ImGui::Unindent();
}

void UI::InspectorPanel::DrawTransform(GameObject* currentGameOject)
{
	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_DefaultOpen;
	flag |= ImGuiTreeNodeFlags_OpenOnArrow;
	flag |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
	flag |= ImGuiTreeNodeFlags_SpanAvailWidth;

	if (UI::CollapsingHeaderImage("Transform", UIEditor::engine.GetEditorTexture("gizmo_icon.png"), flag))
	{
		Transform& transform = currentGameObject->transform;
		Math::Vector3D eulerRotation = transform.rotation.ToEuler();

		ImGui::Indent();

		if (CreateVector3Control("Position", &transform.position))
			transform.SetPosition(transform.position);

		if (CreateVector3Control("Rotation", &eulerRotation))
		{
			transform.rotation = Math::Quaternion::FromEuler(eulerRotation);
			transform.SetRotation(transform.rotation);
		}

		if (CreateVector3Control("Scale", &transform.scale, true))
			transform.SetScale(transform.scale);

		ImGui::Unindent();
	}
}

void UI::InspectorPanel::DrawMaterial(GameObject* currentGameObject, unsigned int textureID)
{
	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	auto* mr = currentGameObject->GetComponent<MeshRenderer>();
	if (!mr)
		return;

	bool isOpen = UI::CollapsingHeaderImage("Material", textureID, flag);

	ImGui::SameLine();

	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (2.f * ImGui::GetFontSize()));

	UI::Style::PushStyleFrom(ImGuiCol_Button, ImGuiCol_Header);
	UI::Style::PushStyle(ImGuiCol_ButtonHovered, UI::Color::EnigmaGrey5);
	std::string s = "##SaveMaterial" + currentGameObject->GetName();

	if (ImGui::ImageButton(s.c_str(), UIEditor::engine.GetEditorTexture("save_icon.png"), ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize())))
	{
		UIEditor::engine.SaveMaterial(mr, currentGameObject->GetName());
	}
	UI::Style::PopStyle();

	if (isOpen)
	{
		Material* goMat = mr->GetMaterial();
		if (!goMat) return;

		ImGui::Indent();

		unsigned int placeholderImg = UIEditor::engine.GetEditorTexture("image_frame_icon.png");
		const char* payloadType = UI::ToLabel(EDragAndDropType::TEXTURE_FILE);

		MaterialGPUData matData = goMat->GetMaterialGPUData();

		//ALBEDO
		Math::Vector4D color = matData.albedoColor;
		DrawTextureSlot(goMat->useAlbedoTexture, goMat->albedoTexture, placeholderImg, payloadType);
		ImGui::SameLine();
		ImGui::ColorEdit4("Albedo Color", &color.x);

		//NORMAL
		float normal = matData.normalStrength;
		DrawTextureSlot(goMat->useNormalTexture, goMat->normalTexture, placeholderImg, payloadType);
		ImGui::SameLine();
		ImGui::DragFloat("Normal", &normal, 0.1f, 0.f, 100.f, "%.1f");

		//METALIC
		float metalic = matData.metallicValue;
		DrawTextureSlot(goMat->useMetallicTexture, goMat->metallicTexture, placeholderImg, payloadType);
		ImGui::SameLine();
		ImGui::DragFloat("Metalic", &metalic, 0.01f, 0.f, 1.f, "%.2f");

		//ROUGHNESS
		float roughness = matData.roughnessValue;
		DrawTextureSlot(goMat->useRoughnessTexture, goMat->roughnessTexture, placeholderImg, payloadType);
		ImGui::SameLine();
		ImGui::DragFloat("Roughness", &roughness, 0.01f, 0.f, 1.f, "%.2f");

		//AO
		float ao = matData.aoValue;
		DrawTextureSlot(goMat->useAOTexture, goMat->aoTexture, placeholderImg, payloadType);
		ImGui::SameLine();
		ImGui::DragFloat("Ambient Occlusion", &ao, 0.01f, 0.01f, 1.f, "%.2f");

		//EMISSIVE
		float emissiveIntensity = matData.emissiveColorIntensity.w;
		Math::Vector3D emissiveColor = { matData.emissiveColorIntensity.x, matData.emissiveColorIntensity.y, matData.emissiveColorIntensity.z };
		DrawTextureSlot(goMat->useEmissiveTexture, goMat->emissiveTexture, placeholderImg, payloadType);
		ImGui::SameLine();
		ImGui::DragFloat("Emissive Intensity", &emissiveIntensity, 0.01f, 0.00f, 1000.f, "%.2f");
		ImGui::ColorEdit3("Emissive Color", &emissiveColor.x);

		//CLEAR COAT
		float clearCoat = matData.clearCoatFactor;
		DrawTextureSlot(goMat->useClearCoatTexture, goMat->clearCoatTexture, placeholderImg, payloadType);
		ImGui::SameLine();
		ImGui::DragFloat("Clear Coat", &clearCoat, 0.01f, 0.f, 1.f, "%.2f");

		//CLEAR COAT ROUGHNESS
		float clearCoatRough = matData.clearCoatRoughFactor;
		DrawTextureSlot(goMat->useClearCoatRoughTexture, goMat->clearCoatRoughTexture, placeholderImg, payloadType);
		ImGui::SameLine();
		ImGui::DragFloat("Clear Coat Roughness", &clearCoatRough, 0.01f, 0.f, 1.f, "%.2f");

		ImGui::Unindent();

		goMat->UpdateMaterialData({ color, {emissiveColor.x, emissiveColor.y, emissiveColor.z, emissiveIntensity}, metalic, roughness, ao, normal, clearCoat, clearCoatRough });
	}
}

void UI::InspectorPanel::DrawTextureSlot(bool& useTexture, Texture& textureObj, unsigned int placeholderImg, const char* payloadType)
{
	if (useTexture)
		ImGui::Image(textureObj.GetID(), { 30, 30 });
	else
		ImGui::Image(placeholderImg, { 30, 30 });

	if (useTexture && ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();

		float textureSize = 200.f;

		ImGui::Text(textureObj.GetName().c_str());
		ImGui::SetCursorPosX(((ImGui::GetContentRegionAvail().x - textureSize) * 0.5f) + 7.5f);
		ImGui::Image(textureObj.GetID(), { textureSize, textureSize });

		ImGui::EndTooltip();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadType))
		{
			std::string fileName(static_cast<const char*>(payload->Data), payload->DataSize);
			Texture* newTex = ResourceManager::GetInstance().Get<Texture>(fileName);
			if (newTex)
			{
				textureObj = *newTex;
				useTexture = true;
			}
			else
			{
				newTex = ResourceManager::GetInstance().Create<Texture>(fileName, UIEditor::engine.GetRHI());
				textureObj = *newTex;
				useTexture = true;
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void UI::InspectorPanel::DrawProperty(IComponent* component, const ComponentPropertyDescriptor& descriptor)
{
	ImGui::PushID(component);
	ComponentPropertyValue value;
	if (!UIEditor::engine.GetComponentPropertyValue(component, descriptor.path, value))
	{
		ImGui::PopID();
		return;
	}

	const std::string& name = descriptor.name;
	const char* label = name.c_str();

	switch (descriptor.uiData.type)
	{
		case EUIType::SLIDER_FLOAT:
		{
			float currentValue = std::get<float>(value);
			if (ImGui::SliderFloat(label, &currentValue, descriptor.uiData.minValue, descriptor.uiData.maxValue, descriptor.uiData.format))
				UIEditor::engine.SetComponentPropertyValue(component, descriptor.path, currentValue);
			break;
		}
		case EUIType::DRAG_FLOAT:
		{
			float currentValue = std::get<float>(value);
			if (ImGui::DragFloat(label, &currentValue, 0.1f, descriptor.uiData.minValue, descriptor.uiData.maxValue, descriptor.uiData.format))
				UIEditor::engine.SetComponentPropertyValue(component, descriptor.path, currentValue);
			break;
		}
		case EUIType::INT:
		case EUIType::COMBO:
		{
			int currentValue = std::get<int>(value);
			if (descriptor.uiData.type == EUIType::INT)
			{
				if (ImGui::SliderInt(label, &currentValue, static_cast<int>(descriptor.uiData.minValue), static_cast<int>(descriptor.uiData.maxValue), descriptor.uiData.format))
					UIEditor::engine.SetComponentPropertyValue(component, descriptor.path, currentValue);
			}
			else
			{
				std::string items;
				for (const std::string& option : descriptor.uiData.comboOptions)
				{
					items += option;
					items += '\0';
				}

				if (ImGui::Combo(label, &currentValue, items.c_str()))
					UIEditor::engine.SetComponentPropertyValue(component, descriptor.path, currentValue);
			}
			break;
		}
		case EUIType::BOOL:
		{
			bool currentValue = std::get<bool>(value);
			if (ImGui::Checkbox(label, &currentValue))
				UIEditor::engine.SetComponentPropertyValue(component, descriptor.path, currentValue);
			break;
		}
		case EUIType::COLOR3:
		{
			Math::Vector3D currentValue = std::get<Math::Vector3D>(value);
			if (ImGui::ColorEdit3(label, &currentValue.x))
				UIEditor::engine.SetComponentPropertyValue(component, descriptor.path, currentValue);
			break;
		}
		case EUIType::COLOR4:
		{
			Math::Vector4D currentValue = std::get<Math::Vector4D>(value);
			if (ImGui::ColorEdit4(label, &currentValue.x))
				UIEditor::engine.SetComponentPropertyValue(component, descriptor.path, currentValue);
			break;
		}
		case EUIType::VEC2:
		{
			Math::Vector2D currentValue = std::get<Math::Vector2D>(value);
			if (UI::CreateVector2Control(label, &currentValue))
				UIEditor::engine.SetComponentPropertyValue(component, descriptor.path, currentValue);
			break;
		}
		case EUIType::VEC3:
		{
			Math::Vector3D currentValue = std::get<Math::Vector3D>(value);
			if (UI::CreateVector3Control(label, &currentValue))
				UIEditor::engine.SetComponentPropertyValue(component, descriptor.path, currentValue);
			break;
		}
		case EUIType::MATERIAL_INPUT:
		{
			float width = ImGui::CalcItemWidth();
			float height = 30.f;
			std::string materialName = std::get<std::string>(value);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.5f);
			ImGui::Text("Material");
			ImGui::SameLine();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2.5f);

			if (materialName != "")
			{
				UI::Style::PushStyleFrom(ImGuiCol_Button, ImGuiCol_MenuBarBg);
				UI::Style::PushStyleFrom(ImGuiCol_ButtonActive, ImGuiCol_ButtonHovered);
				UI::ImageTextButton(materialName.c_str(), UIEditor::engine.GetEditorTexture("material_icon.png"), Math::Vector2D{width, height});
				UI::Style::PopStyle();
			}
			else
			{
				UI::Style::PushStyleFrom(ImGuiCol_Button, ImGuiCol_MenuBarBg);
				ImGui::Button("No Material", ImVec2{ width, height });
				UI::Style::PopStyle();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(UI::ToLabel(EDragAndDropType::MATERIAL_FILE)))
				{
					std::string fileName(static_cast<const char*>(payload->Data), payload->DataSize);
					std::filesystem::path path = fileName;
					fileName = path.stem().string();
					MeshRenderer* meshRenderer = currentGameObject->GetComponent<MeshRenderer>();
					meshRenderer->SetMaterial(MaterialManager::GetInstance().GetMaterial(fileName));
					meshRenderer->GetMaterial()->CreateMaterialData(UIEditor::engine.GetRHI());
				}
				ImGui::EndDragDropTarget();
			}

			break;
		}
		case EUIType::FONT_INPUT:
		{
			FontTexture* fT = std::get<FontTexture*>(value);
			float width = ImGui::CalcItemWidth();
			float height = 30.f;

			if (fT)
			{
				UI::Style::PushStyleFrom(ImGuiCol_Button, ImGuiCol_MenuBarBg);
				UI::Style::PushStyleFrom(ImGuiCol_ButtonActive, ImGuiCol_ButtonHovered);
				UI::ImageTextButton(fT->GetName().c_str(), UIEditor::engine.GetEditorTexture("text_icon.png"), Math::Vector2D{width, height});
				UI::Style::PopStyle();
			}
			else
			{
				UI::Style::PushStyleFrom(ImGuiCol_Button, ImGuiCol_MenuBarBg);
				ImGui::Button("No Font", ImVec2{ width, height });
				UI::Style::PopStyle();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(UI::ToLabel(EDragAndDropType::FONT_FILE)))
				{
					std::string fileName(static_cast<const char*>(payload->Data), payload->DataSize);
					FontTexture* newFt = ResourceManager::GetInstance().Get<FontTexture>(fileName);

					if (!newFt)
					{
						WidgetRenderer::GetInstance().LoadFont("assets\\fonts\\" + fileName);
						newFt = ResourceManager::GetInstance().Get<FontTexture>(fileName);
					}

					if (Text* compT = currentGameObject->GetComponent<Text>())
					{
						compT->SetFontTexture(newFt);
						ImGui::EndDragDropTarget();
						break;
					}
					if (TextMesh* compTM = currentGameObject->GetComponent<TextMesh>())
					{
						compTM->SetFontTexture(newFt);
						ImGui::EndDragDropTarget();
						break;
					}
					if (Button* compB = currentGameObject->GetComponent<Button>())
					{
						compB->SetFontTexture(newFt);
						ImGui::EndDragDropTarget();
						break;
					}
					
				}
				ImGui::EndDragDropTarget();
			}

			break;
		}
		case EUIType::TEXT:
		{
			break;
		}
		case EUIType::MULTILINE_TEXT:
		{
			std::string currentValue = std::get<std::string>(value);

			TextRenderer* tR = WidgetRenderer::GetInstance().GetTextRenderer();
			char buffer[tR->textLimit];

			strcpy(buffer, currentValue.c_str());

			buffer[sizeof(buffer) - 1] = '\0';

			if (ImGui::InputTextMultiline(label, buffer, sizeof(buffer), ImVec2(0, 0), ImGuiInputTextFlags_WordWrap))
			{
				currentValue = std::string(buffer);
				UIEditor::engine.SetComponentPropertyValue(component, descriptor.path, currentValue);
			}

			break;
		}
		case EUIType::GAMEOBJECT_INPUT:
		{
			GameObject* currentValue = std::get<GameObject*>(value);

			float width = ImGui::CalcItemWidth();
			float height = 30.f;

			ImGui::PushID(descriptor.path.c_str());

			if (currentValue)
			{
				const std::string& goName = currentValue->GetName();
				UI::Style::PushStyleFrom(ImGuiCol_Button, ImGuiCol_MenuBarBg);
				UI::Style::PushStyleFrom(ImGuiCol_ButtonActive, ImGuiCol_ButtonHovered);
				ImGui::Text(descriptor.name.c_str());
				ImGui::SameLine();
				UI::ImageTextButton(goName.c_str(), UIEditor::engine.GetEditorTexture("object_icon.png"), Math::Vector2D{ width, height });
				UI::Style::PopStyle();
			}
			else
			{
				UI::Style::PushStyleFrom(ImGuiCol_Button, ImGuiCol_MenuBarBg);
				ImGui::Text(descriptor.name.c_str());
				ImGui::SameLine();
				ImGui::Button("No GameObject", ImVec2{ width, height });
				UI::Style::PopStyle();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(UI::ToLabel(EDragAndDropType::GAMEOBJECT)))
				{
					currentValue = *static_cast<GameObject**>(payload->Data);
					UIEditor::engine.SetComponentPropertyValue(component, descriptor.path, currentValue);
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::PopID();
			break;
		}

		default:
			break;
	}

	ImGui::PopID();
}

void UI::InspectorPanel::DrawScriptProperty(Scripting::IScript* script, const Scripting::ScriptPropertyDescriptor& descriptor)
{
	const std::string& name = descriptor.name;

	switch (descriptor.uiData.type)
	{
		case EUIType::DRAG_FLOAT:
		case EUIType::SLIDER_FLOAT:
		{
			float value = std::get<float>(descriptor.getter(script));
			bool changed = false;
			if (descriptor.uiData.type == EUIType::SLIDER_FLOAT)
				changed = ImGui::SliderFloat(name.c_str(), &value, descriptor.uiData.minValue, descriptor.uiData.maxValue, descriptor.uiData.format);
			else
				changed = ImGui::DragFloat(name.c_str(), &value, 0.1f, descriptor.uiData.minValue, descriptor.uiData.maxValue, descriptor.uiData.format);

			if (changed)
				descriptor.setter(script, value);
			break;
		}
		case EUIType::INT:
		{
			int value = std::get<int>(descriptor.getter(script));
			if (ImGui::SliderInt(name.c_str(), &value, static_cast<int>(descriptor.uiData.minValue), static_cast<int>(descriptor.uiData.maxValue), descriptor.uiData.format))
				descriptor.setter(script, value);
			break;
		}
		case EUIType::BOOL:
		{
			bool value = std::get<bool>(descriptor.getter(script));
			if (ImGui::Checkbox(name.c_str(), &value))
				descriptor.setter(script, value);
			break;
		}
		case EUIType::VEC3:
		{
			Math::Vector3D value = std::get<Math::Vector3D>(descriptor.getter(script));
			if (UI::CreateVector3Control(name.c_str(), &value))
				descriptor.setter(script, value);
			break;
		}
		case EUIType::COLOR3:
		{
			Math::Vector3D value = std::get<Math::Vector3D>(descriptor.getter(script));
			if (ImGui::ColorEdit3(name.c_str(), &value.x))
				descriptor.setter(script, value);
			break;
		}
		case EUIType::COLOR4:
		{
			Math::Vector4D value = std::get<Math::Vector4D>(descriptor.getter(script));
			if (ImGui::ColorEdit4(name.c_str(), &value.x))
				descriptor.setter(script, value);
			break;
		}
		case EUIType::COMBO:
		{
			int current = std::get<int>(descriptor.getter(script));
			std::string items;
			for (const std::string& option : descriptor.uiData.comboOptions)
			{
				items += option;
				items += '\0';
			}

			if (ImGui::Combo(name.c_str(), &current, items.c_str()))
				descriptor.setter(script, current);
			break;
		}
		default:
			break;
	}
}

void UI::InspectorPanel::DrawComponentEntry(const char* name, const char* icon, const std::string& search, const std::function<void()>& onAdd)
{
	std::string nameLower = name;
	std::ranges::transform(nameLower, nameLower.begin(), ::tolower);
	if (!search.empty() && nameLower.find(search) == std::string::npos)
		return;

	bool clicked = ImGui::Button(("##button" + std::string(name)).c_str(), { ImGui::GetContentRegionAvail().x, 30.f });
	ImGui::SameLine();
	ImGui::SetCursorPosX(10.f);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.f);
	ImGui::Image(UIEditor::engine.GetEditorTexture(icon), { 20.f, 20.f });
	ImGui::SameLine();
	ImGui::Text(name);

	if (clicked)
	{
		onAdd();
		ImGui::CloseCurrentPopup();
	}

	ImGui::Spacing();
}

void UI::InspectorPanel::DrawAddComponentPopUp(GameObject* currentGameObject)
{
	static char searchBuffer[128] = "";

	ImVec2 buttonSize = { 250.f, 30.f };
	ImGui::SetCursorPos({ ImGui::GetCursorPosX() + ((ImGui::GetContentRegionAvail().x - buttonSize.x) * 0.5f), ImGui::GetCursorPosY() + 20.f });


	UI::Style::PushStyleFrom(ImGuiCol_Button, ImGuiCol_MenuBarBg);
	if (ImGui::Button("Add Component", buttonSize))
	{
		ImGui::OpenPopup("AddComponentPopup");
		memset(searchBuffer, 0, sizeof(searchBuffer));
	}
	UI::Style::PopStyle();

	ImVec2 buttonPos = ImGui::GetItemRectMin();
	ImVec2 buttonSizeV = ImGui::GetItemRectSize();

	constexpr float popupWidth = 250.f;
	constexpr float popupHeight = 350.f;

	ImVec2 popupPos = { buttonPos.x, buttonPos.y + buttonSizeV.y };

	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();
	float  windowBottom = windowPos.y + windowSize.y;
	float  windowRight = windowPos.x + windowSize.x;

	if (popupPos.y + popupHeight > windowBottom)
		popupPos.y = buttonPos.y - popupHeight;
	if (popupPos.x + popupWidth > windowRight)
		popupPos.x = windowRight - popupWidth;

	ImGui::SetNextWindowPos(popupPos);
	ImGui::SetNextWindowSize({ popupWidth, popupHeight });

	if (ImGui::BeginPopup("AddComponentPopup", ImGuiWindowFlags_NoResize))
	{
		ImGui::SetNextItemWidth(-1);
		ImGui::InputTextWithHint("##search", "Search...", searchBuffer, sizeof(searchBuffer));

		ImGui::Separator();
		ImGui::TextDisabled("Components");

		std::string search = searchBuffer;
		std::ranges::transform(search, search.begin(), ::tolower);

		ImGui::BeginChild("##componentList", { 0.f, 0.f }, false);

		bool isScript = false;
		for (auto& entry : UIEditor::engine.componentsMap)
		{
			for (auto scriptName : Scripting::ScriptRegistry::GetInstance().GetRegisteredNames())
			{
				if (entry.first.get_name() == scriptName)
				{
					isScript = true;
					break;
				}
			}
			if (isScript)
			{
				isScript = false;
				continue;
			}
			DrawComponentEntry(entry.second.nameToDisplay.c_str(), entry.second.iconName.c_str(), search, entry.second.func);
		}

		ImGui::Separator();
		ImGui::TextDisabled("Scripts");

		auto& registry = Scripting::ScriptRegistry::GetInstance();
		for (const std::string& scriptName : registry.GetRegisteredNames())
		{
			DrawComponentEntry(scriptName.c_str(), "cpp_icon.png", search, [&, scriptName] {
				currentGameObject->AddScript(scriptName);
			});
		}

		ImGui::EndChild();
		ImGui::EndPopup();
	}
}
