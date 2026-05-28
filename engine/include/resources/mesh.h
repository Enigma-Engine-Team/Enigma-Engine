#pragma once
#include <vector>
#include "iresource.h"
#include "emath/emath.h"
#include "IRenderInterface.h"

struct Vertex
{
	Math::Vector3D position;
	Math::Vector3D normal;
	Math::Vector2D texcoord;
};

class Mesh
{
public:
	Mesh() = default;
	void Draw(EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::EDrawMode drawMode = EnigmaRHI::EDrawMode::TRIANGLES);
	Vertex& GetVertex(int index);
	std::vector<Vertex>& GetVertices();
	std::vector<uint32_t>& GetIndices();
	
	EnigmaRHI::IBuffer* vertexBuffer = nullptr;
	EnigmaRHI::IBuffer* indexBuffer = nullptr;

private:
	void CreateVertexBuffer(EnigmaRHI::IRenderInterface* rhi);
	void CreateIndexBuffer(EnigmaRHI::IRenderInterface* rhi);
	
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	
	friend class Model;
};