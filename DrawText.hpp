#pragma once

/**
 * @brief Render text at runtime on-the-fly
 * 
 * This approach is inspired by https://learnopengl.com/In-Practice/Text-Rendering, but I
 * takes another routine which is totally rendered on-the-fly. The performance issue is
 * mitigated by adding a cache.
 * 
 * The main advantage for this runtime rendering is its flexibility to adapt to any
 * changes happening in runtime to the font.
 * 
 * I also introduced Harfbuzz for better unicode and ligatures handling as well as kerning.
 */


#include <unordered_map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>

#include "GL.hpp"


// cache element
struct Bitmap {
    GLuint texture_id;
    unsigned int w, h;
    int left, top;
    Bitmap(GLuint id, unsigned int w, unsigned int h, int l, int t): 
        texture_id(id), w(w), h(h), left(l), top(t) {}
    ~Bitmap() {
        glDeleteTextures(1, &texture_id);
    }
};

class DrawText {
public:
    DrawText()=delete;
    DrawText(const char *font_file);
    DrawText(const DrawText&)=delete;
    ~DrawText();

    // This will clear texture cache!
    void set_font_size(int pixels) const;

    void set_font_color(float r, float g, float b) const;

    void set_window_size(float w, float h) const;
    
    void draw(const char *text, float x, float y) const;

private:
    GLuint add_texture(FT_ULong codepoint) const;

    void delete_cache() const;

    FT_Library library;  // handle to library
    FT_Face face_ft;  // face object handle
    GLuint VAO;  // character vertex buffer
    GLuint VBO;
    GLuint program;
    mutable glm::vec3 text_color;
    mutable glm::mat4 projection;
    GLuint projection_loc, text_color_loc;
    mutable std::unordered_map<FT_ULong, Bitmap> texture_cache;
};