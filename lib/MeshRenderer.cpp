#include "MeshRenderer.hpp"
#include "Graphics/Material.hpp"
#include "Graphics/Mesh.hpp"

using namespace gintonic;

std::unique_ptr<Component> MeshRenderer::clone(EntityBase* newOwner) const
{
    auto renderer = std::make_unique<MeshRenderer>(newOwner);
    renderer->mesh = mesh;
    renderer->materials = materials;
    return std::move(renderer);
}

void MeshRenderer::lateUpdate()
{
    if (!mesh) return;
    for (const auto& material : materials)
    {
        material->bind();
        mesh->draw();
    }
}
