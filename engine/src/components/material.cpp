#include "components/material.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<MaterialGPUData>("MaterialGPUData")
        .property("albedoColor", &MaterialGPUData::albedoColor)
        .property("emissiveColorIntensity", &MaterialGPUData::emissiveColorIntensity)
        .property("metallicValue", &MaterialGPUData::metallicValue)
        .property("roughnessValue", &MaterialGPUData::roughnessValue)
        .property("aoValue", &MaterialGPUData::aoValue)
        .property("normalStrength", &MaterialGPUData::normalStrength)
        .property("clearCoatFactor", &MaterialGPUData::clearCoatFactor)
        .property("clearCoatRoughFactor", &MaterialGPUData::clearCoatRoughFactor)
        .property("useAlbedoTexture", &MaterialGPUData::useAlbedoTexture)
        .property("useNormalTexture", &MaterialGPUData::useNormalTexture)
        .property("useMetallicRoughnessTexture", &MaterialGPUData::useMetallicRoughnessTexture)
        .property("useMetallicTexture", &MaterialGPUData::useMetallicTexture)
        .property("useRoughnessTexture", &MaterialGPUData::useRoughnessTexture)
        .property("useAOTexture", &MaterialGPUData::useAOTexture)
        .property("useClearCoatTexture", &MaterialGPUData::useClearCoatTexture)
        .property("useClearCoatRoughTexture", &MaterialGPUData::useClearCoatRoughTexture)
        .property("useClearCoatNormalTexture", &MaterialGPUData::useClearCoatNormalTexture)
        .property("useEmissiveTexture", &MaterialGPUData::useEmissiveTexture);

    rttr::registration::class_<Material>("Material")
        .constructor<>()(rttr::policy::ctor::as_raw_ptr)
        .property("name", &Material::name)
        .property("materialData", &Material::materialData);
}

void Material::CreateMaterialData(EnigmaRHI::IRenderInterface* rhi)
{
    size_t bufferSize = sizeof(MaterialGPUData);
    materialDataBuffer = rhi->InstantiateBuffer();
    materialDataBuffer->Create(bufferSize, EnigmaRHI::EBufferTarget::UNIFORM_BUFFER, nullptr, EnigmaRHI::EBufferUsage::STATIC_DRAW);
    materialDataBuffer->CreateDescriptorBufferInfo();
    materialDataBuffer->bufferInfo.range = sizeof(MaterialGPUData);
}

void Material::UpdateMaterialData(MaterialGPUData _materialData)
{
    materialData = _materialData;
    materialDataBuffer->CopyData(&materialData, sizeof(materialData));
}

void Material::Bind(EnigmaRHI::IDescriptor* geometryDescriptor)
{
    if (!materialDataBuffer)
        return;

    if (useAlbedoTexture)
    {
        geometryDescriptor->BindImage(3, EnigmaRHI::EImageType::TYPE_2D, albedoTexture.GetID());
        materialData.useAlbedoTexture = true;
    }

    if (useNormalTexture)
    {
        geometryDescriptor->BindImage(4, EnigmaRHI::EImageType::TYPE_2D, normalTexture.GetID());
        materialData.useNormalTexture = true;
    }

    if (useMetallicRoughnessTexture)
    {
        geometryDescriptor->BindImage(5, EnigmaRHI::EImageType::TYPE_2D, metallicRoughnessTexture.GetID());
        materialData.useMetallicRoughnessTexture = true;
    }
    else 
    {
        if (useMetallicTexture)
        {
            geometryDescriptor->BindImage(5, EnigmaRHI::EImageType::TYPE_2D, metallicTexture.GetID());
            materialData.useMetallicTexture = true;
        }
        if (useRoughnessTexture)
        {
            geometryDescriptor->BindImage(6, EnigmaRHI::EImageType::TYPE_2D, roughnessTexture.GetID());
            materialData.useRoughnessTexture = true;
        }
    }

    if (useAOTexture)
    {
        geometryDescriptor->BindImage(7, EnigmaRHI::EImageType::TYPE_2D, aoTexture.GetID());
        materialData.useAOTexture = true;
    }

    if (useClearCoatTexture)
    {
        geometryDescriptor->BindImage(8, EnigmaRHI::EImageType::TYPE_2D, clearCoatTexture.GetID());
        materialData.useClearCoatTexture = true;
    }

    if (useClearCoatRoughTexture)
    {
        geometryDescriptor->BindImage(9, EnigmaRHI::EImageType::TYPE_2D, clearCoatRoughTexture.GetID());
        materialData.useClearCoatRoughTexture = true;
    }

    if (useClearCoatNormalTexture)
    {
        geometryDescriptor->BindImage(10, EnigmaRHI::EImageType::TYPE_2D, clearCoatNormalTexture.GetID());
        materialData.useClearCoatNormalTexture = true;
    }

    if (useEmissiveTexture)
    {
        geometryDescriptor->BindImage(16, EnigmaRHI::EImageType::TYPE_2D, emissiveTexture.GetID());
        materialData.useEmissiveTexture = true;
    }

    materialDataBuffer->CopyData(&materialData, sizeof(materialData));

    geometryDescriptor->BindBuffer(11, EnigmaRHI::EBufferTarget::UNIFORM_BUFFER, materialDataBuffer);
}