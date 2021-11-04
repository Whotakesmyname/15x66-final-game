#pragma once

#include <glm/glm.hpp>
#include <vector>


class Collider {
public:
    struct AABB {
        glm::vec2 upperleft;
        glm::vec2 lowerright;
        AABB(glm::vec2 upperleft, glm::vec2 lowerright): upperleft(upperleft), lowerright(lowerright) {}
    };

    std::vector<AABB> map_components;

    void add_component(glm::vec2 center, glm::vec2 size);

    std::pair<bool, glm::vec2> solve_collision(glm::vec2 center, glm::vec2 size);
};
