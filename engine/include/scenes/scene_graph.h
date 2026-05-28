#pragma once

#include <iostream>
#include <vector>
#include "scene.h"

class SceneGraph
{
public:
	static SceneGraph& GetInstance();
	void Update(Scene* scene);
	void UpdateParenting(GameObject* gameObject, Math::Matrix4x4 transformParent);

private:
	SceneGraph() = default;
	~SceneGraph() = default;
};