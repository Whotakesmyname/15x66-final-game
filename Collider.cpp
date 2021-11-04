#include "Collider.hpp"

#include <glm/gtx/component_wise.hpp>


void Collider::add_component(glm::vec2 center, glm::vec2 size)
{
    map_components.emplace_back(center - size / 2.f, center + size / 2.f);
}

std::pair<bool, glm::vec2> Collider::solve_collision(glm::vec2 center, glm::vec2 size)
{
    glm::vec2 upperleft = center - size / 2.f;
    glm::vec2 lowerright = center + size / 2.f;

    for (auto &box : map_components)
    {
        glm::vec2 overlap = size + (box.lowerright - box.upperleft) -
            (glm::max(lowerright, box.lowerright) - glm::min(upperleft, box.upperleft));
        if (overlap.x >= 0 && overlap.y >= 0) {
            glm::vec2 box_center = (box.upperleft + box.lowerright) / 2.f;
            glm::vec2 resolve_vec = glm::sign(center - box_center) * overlap;
            return std::make_pair(true, resolve_vec);
        }
    }
    return std::make_pair(false, glm::vec2(0.f));
}
