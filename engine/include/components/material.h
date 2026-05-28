#pragma once
#include "IRenderInterface.h"
#include "emath/emath.h"
#include "resources/texture.h"
#include <rttr/registration.h>
#include <rttr/registration_friend.h>
#include <string>


struct ENIGMA_API MaterialGPUData
{
    Math::Vector4D albedoColor = { 1.f, 1.f, 1.f, 1.f };
    Math::Vector4D emissiveColorIntensity = { 0.f, 0.f, 0.f, 1.f }; // -> emissive color = XYZ, emissive intensity = W

    float metallicValue = 0.f;
    float roughnessValue = 1.f;
    float aoValue = 1.f;
    float normalStrength = 1.f;

    float clearCoatFactor = 0.f;
    float clearCoatRoughFactor = 0.f;
    float padding1 = 0.f;
    float padding2 = 0.f;

    int useAlbedoTexture = 0;
    int useNormalTexture = 0;
    int useMetallicRoughnessTexture = 0;
    int useMetallicTexture = 0;

    int useRoughnessTexture = 0;
    int useAOTexture = 0;
    int useClearCoatTexture = 0;
    int useClearCoatRoughTexture = 0;

    int useClearCoatNormalTexture = 0;
    int useEmissiveTexture = 0;
    float padding3 = 0.f;
    float padding4 = 0.f;
};

class ENIGMA_API Material
{
public:
    Material() = default;
    void CreateMaterialData(EnigmaRHI::IRenderInterface* rhi);
    void UpdateMaterialData(MaterialGPUData _materialData);
    void Bind(EnigmaRHI::IDescriptor* objectDescriptor);
    EnigmaRHI::IBuffer* GetMaterialDataBuffer() const { return materialDataBuffer; };
    MaterialGPUData& GetMaterialGPUData() { return materialData; };
    void SetMaterialGPUData(MaterialGPUData _materialData) { materialData = _materialData; };

    Texture albedoTexture;                      bool useAlbedoTexture = false;
    Texture normalTexture;                      bool useNormalTexture = false;
    Texture metallicTexture;                    bool useMetallicTexture = false;
    Texture roughnessTexture;                   bool useRoughnessTexture = false;
    Texture metallicRoughnessTexture;           bool useMetallicRoughnessTexture = false;
    Texture aoTexture;                          bool useAOTexture = false;
    Texture clearCoatTexture;                   bool useClearCoatTexture = false;
    Texture clearCoatRoughTexture;              bool useClearCoatRoughTexture = false;
    Texture clearCoatNormalTexture;             bool useClearCoatNormalTexture = false;
    Texture emissiveTexture;                    bool useEmissiveTexture = 0;

    bool forceTransparent = false;

    std::string name = "DefaultMaterial";

private:
    RTTR_REGISTRATION_FRIEND
    MaterialGPUData materialData;
    EnigmaRHI::IBuffer* materialDataBuffer = nullptr;
};