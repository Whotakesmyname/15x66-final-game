#include "TileDrawer.hpp"

#include "gl_compile_program.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl_errors.hpp"


TileDrawer::TileDrawer() {
    glGenBuffers(RENDER_QUEUE_SIZE, vbos);
    glGenVertexArrays(RENDER_QUEUE_SIZE, vaos);

    program = gl_compile_program(
        "#version 330 core\n"
        "layout(location = 0) in vec2 position;\n"
        "layout(location = 1) in vec2 tex_coord;\n"
        "\n"
        "out vec2 TexCoord;\n"
        "\n"
        "uniform mat4 PROJECTION;\n"
        "\n"
        "void main() {\n"
        "  TexCoord = tex_coord;\n"
        "  gl_Position = PROJECTION * vec4(position, 0.0, 1.0);\n"
        "}\n"
        ,
        "#version 330 core\n"
        "in vec2 TexCoord;\n"
        "out vec4 color;\n"
        "\n"
        "uniform sampler2D TEX;\n"
        "uniform vec3 COLOR;\n"
        "\n"
        "void main() {\n"
        "  color = vec4(COLOR, 1.0);\n"
        "}\n"
    );

    // init VAOs
    for (size_t queue = 0; queue < RENDER_QUEUE_SIZE; ++queue) {
        glBindBuffer(GL_ARRAY_BUFFER, vbos[queue]);
        glBindVertexArray(vaos[queue]);
        glVertexAttribPointer(POSITION, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)0);
        glEnableVertexAttribArray(POSITION);
        glVertexAttribPointer(TEXCOORD, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
        glEnableVertexAttribArray(TEXCOORD);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // init uniform locations
    PROJECTION_LOC = glGetUniformLocation(program, "PROJECTION");
    TEX_LOC = glGetUniformLocation(program, "TEX");
    COLOR_LOC = glGetUniformLocation(program, "COLOR");
}

TileDrawer::~TileDrawer() {
    glDeleteBuffers(RENDER_QUEUE_SIZE, vbos);
    glDeleteVertexArrays(RENDER_QUEUE_SIZE, vaos);
    glDeleteProgram(program);
    program = 0;
}

size_t TileDrawer::add_component(const Square &&square, RenderQueues queue) {
    components[queue].emplace_back(square);
    return components[queue].size() - 1;
}

void TileDrawer::clear_components(RenderQueues queue) {
    components[queue].clear();
}

void TileDrawer::update_vertices(RenderQueues queue) {
    vertices[queue].clear();

    for (auto& square : components[queue]) {
        glm::vec2 upper_left = square.position - square.size / 2.f;
        glm::vec2 bottom_right = square.position + square.size / 2.f;
        // add 6 vertices
        // can be optimized
        vertices[queue].emplace_back(
            upper_left.x, upper_left.y, square.uv_upper_left.x, square.uv_upper_left.y
        );
        vertices[queue].emplace_back(
            upper_left.x, bottom_right.y, square.uv_upper_left.x, square.uv_bottom_right.y
        );
        vertices[queue].emplace_back(
            bottom_right.x, bottom_right.y, square.uv_bottom_right.x, square.uv_bottom_right.y
        );
        vertices[queue].emplace_back(
            upper_left.x, upper_left.y, square.uv_upper_left.x, square.uv_upper_left.y
        );
        vertices[queue].emplace_back(
            bottom_right.x, bottom_right.y, square.uv_bottom_right.x, square.uv_bottom_right.y
        );
        vertices[queue].emplace_back(
            bottom_right.x, upper_left.y, square.uv_bottom_right.x, square.uv_upper_left.y
        );
    }

    // update vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbos[queue]);
    const GLenum drawtypes[RENDER_QUEUE_SIZE] = {GL_STATIC_DRAW, GL_STATIC_DRAW, GL_STREAM_DRAW, GL_STREAM_DRAW};
    glBufferData(GL_ARRAY_BUFFER, vertices[queue].size() * sizeof(glm::vec4), vertices[queue].data(), drawtypes[queue]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TileDrawer::update_drawable_size(glm::uvec2 _drawable_size) {
    drawable_size = _drawable_size;
    projection = glm::ortho(0.f, static_cast<float>(drawable_size.x), static_cast<float>(drawable_size.y), 0.f, -10.f, 10.f);
}

void TileDrawer::draw() {
    glUseProgram(program);

    for (size_t queue = 0; queue < RENDER_QUEUE_SIZE; ++queue) {
        if (vertices[queue].empty()) {
            continue;
        }
        glUniformMatrix4fv(PROJECTION_LOC, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(COLOR_LOC, 1.f, 1.f, 1.f);
        glBindVertexArray(vaos[queue]);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices[queue].size()));
        glBindVertexArray(0);
    }
    glUseProgram(0);
	GL_ERRORS();
}
