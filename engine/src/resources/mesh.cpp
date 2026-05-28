#include "resources/mesh.h"

#include "debug/log.h"

void Mesh::Draw(EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::EDrawMode drawMode)
{
    cmd.BindVertexBuffer(vertexBuffer, 8 * sizeof(float)); // pos + normal + texcoord
    cmd.BindIndexBuffer(indexBuffer);

    cmd.DrawIndexed(drawMode, static_cast<uint32_t>(indices.size()));
}

void Mesh::CreateVertexBuffer(EnigmaRHI::IRenderInterface* rhi)
{
    size_t bufferSize = sizeof(vertices[0]) * vertices.size();

    vertexBuffer = rhi->InstantiateBuffer();
    vertexBuffer->Create(bufferSize, EnigmaRHI::EBufferTarget::ARRAY_BUFFER, vertices.data(), EnigmaRHI::EBufferUsage::STATIC_DRAW);
}

void Mesh::CreateIndexBuffer(EnigmaRHI::IRenderInterface* rhi)
{
    size_t bufferSize = sizeof(indices[0]) * indices.size();

    indexBuffer = rhi->InstantiateBuffer();
    indexBuffer->Create(bufferSize, EnigmaRHI::EBufferTarget::ELEMENT_BUFFER, indices.data(), EnigmaRHI::EBufferUsage::STATIC_DRAW);
}

Vertex& Mesh::GetVertex(int index) 
{ 
    return vertices[index]; 
}

std::vector<Vertex>& Mesh::GetVertices() 
{ 
    return vertices; 
}

std::vector<uint32_t>& Mesh::GetIndices() 
{ 
    return indices;
}