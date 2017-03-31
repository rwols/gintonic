#include <gintonic/Component/MeshRenderer.hpp>
#include <gintonic/Graphics/Material.hpp>
#include <gintonic/Graphics/Mesh.hpp>

using namespace gintonic;

std::unique_ptr<Component> MeshRenderer::clone(Entity& newOwner) const
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
