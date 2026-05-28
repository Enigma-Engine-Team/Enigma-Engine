#pragma once

#include <rttr/registration.h>
#include "../utilities/macro.h"
#include <rttr/registration_friend.h>
#include "emath/emath.h"

enum class EUIType
{
	COLOR3,
	COLOR4,
	POSITION,
	VEC2,
	VEC3,
	DRAG_FLOAT,
	SLIDER_FLOAT,
	MATERIAL_INPUT,
	INT,
	BOOL,
	IMAGE,
	COMBO,
	TEXT,
	MULTILINE_TEXT,
	FONT_INPUT,
	GAMEOBJECT_INPUT
};

struct UIData
{
	EUIType type;
	float minValue;
	float maxValue;
	const char* format;
	std::string category;
	std::vector<std::string> comboOptions;
};

inline constexpr int UI_DATA_METADATA_KEY = 1;

class GameObject;

class ENIGMA_API IComponent
{
public:
	IComponent() = default;
	virtual ~IComponent() = default;
	virtual void Create() = 0;
	virtual void Destroy(){}
	virtual void SetComponent(){}
	virtual void SetParent(GameObject* go);
	virtual void OnDraw(Math::Vector3D camPos = Math::Vector3D::Zero){}

	GameObject* gameObject = nullptr;
protected:
	RTTR_ENABLE()
};