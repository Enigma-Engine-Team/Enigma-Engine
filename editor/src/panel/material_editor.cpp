#include "panel/material_editor.h"
#include "context/editor_ui.h"

UI::MaterialEditor::MaterialEditor()
{

}

void UI::MaterialEditor::Draw()
{
	ImGui::Begin("MaterialEditor", &UIEditor::requestMaterialEditor);
		
	Material* goMat = UIEditor::materialToDisplay;

	if (!goMat->GetMaterialDataBuffer())
		goMat->CreateMaterialData(Engine::GetRHI());

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

	ImGui::End();
}

void UI::MaterialEditor::DrawTextureSlot(bool& useTexture, Texture& textureObj, unsigned int placeholderImg, const char* payloadType)
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
		}
		ImGui::EndDragDropTarget();
	}
}
