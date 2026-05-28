#pragma once

#include "grid.h"
#include "environment_system.h"
#include "gizmo_renderer.h"
#include "gameobject/gameobject.h"
#include "shadows/shadow_system.h"

struct TransparentEntry
{
	float distance;
	GameObject* go = nullptr;
};

struct ViewportTarget;

class RenderResources
{
public:

	void Create(EnigmaRHI::IRenderInterface* rhi);
	void Destroy();

	EnvironmentSystem* GetEnvironmentSystem() const { return environment; };
	GizmoRenderer& GetGizmoRenderer() { return gizmoRenderer; }
	Grid* GetGrid() const { return grid; };
	ShadowSystem* GetShadowSystem() { return shadowSystem; };

private:

	Grid* grid = nullptr;
	EnvironmentSystem* environment = nullptr;
	ShadowSystem* shadowSystem = nullptr;
	GizmoRenderer gizmoRenderer;
};