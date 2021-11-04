#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "GL.hpp"


class TileDrawer {
public:
    // render queue indicator
    enum RenderQueues : size_t {
        BACKGROUND = 0U,
        MAP,
        CHARACTER,
        FOREGROUND,
        RENDER_QUEUE_SIZE
    };
    
    enum ATTR_LOCS : GLuint {
        POSTION = 0U,
        TEXCOORD = 1U,
    };

    // a square with tex coord
    struct Square {
        glm::vec2 position;
        glm::vec2 size; // width, height
        glm::vec2 uv_upper_left;
        glm::vec2 uv_bottom_right;
    };

    std::vector<Square> components[RENDER_QUEUE_SIZE];
    std::vector<glm::vec4> vertices[RENDER_QUEUE_SIZE];
    GLuint vbos[RENDER_QUEUE_SIZE];
    GLuint vaos[RENDER_QUEUE_SIZE];
    GLuint program;

    GLuint PROJECTION_LOC = -1U;
    GLuint TEX_LOC = -1U;
    GLuint COLOR_LOC = -1U;

    glm::mat4 projection;

    TileDrawer();
    ~TileDrawer();

    size_t add_component(const Square &&square, RenderQueues queue);

    void clear_components(RenderQueues);

    void update_vertices(RenderQueues queue);

    void update_drawable_size(glm::uvec2 drawable_size);

    void draw();

    glm::uvec2 drawable_size;
};
