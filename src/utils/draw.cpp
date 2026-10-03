//
// Created by luisb on 05.03.2026.
//

#include "draw.h"

#include <iostream>
#include <GL/gl3w.h>

#include "../helpers/config.h"
#include "glm/vec3.hpp"
#include "glm/ext/quaternion_geometric.hpp"

Draw *Draw::init(Window* window, Registry* registry) noexcept {
    Draw *self = new Draw;
    self->m_window = window;
    self->m_registry = registry;

    glm::uvec2 extent = self->m_window->windowExtent();
    self->m_intermediate_framebuffer = Framebuffer::init({ static_cast<GLsizei>(extent.x), static_cast<GLsizei>(extent.y), 1, true });

    self->m_registry->m_viewport.m_origin = glm::uvec2(0);
    self->m_registry->m_viewport.m_size = self->m_window->windowExtent();

    glGenVertexArrays(1, &self->m_quad_vao);
    glBindVertexArray(self->m_quad_vao);
    glGenBuffers(1, &self->m_quad_ebo);

    constexpr std::array<GLuint, 6> indices = { 0, 1, 2, 2, 3, 0 };
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->m_quad_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);


    self->m_player_shader = Shader::init(assets::Shader::player).value();
    self->m_line_shader = Shader::init(assets::Shader::line).value();
	self->m_polygon_shader = Shader::init(assets::Shader::polygon).value();
	self->m_platform_shader=Shader::init(assets::Shader::platform).value();
	self->m_text_shader = Shader::init(assets::Shader::text).value();
    self->m_sprite_shader = Shader::init(assets::Shader::sprite).value();
    self->m_circle_shader = Shader::init(assets::Shader::circle).value();
    self->m_round_rect_shader = Shader::init(assets::Shader::round_rect).value();

	// line VAO/VBO
    glGenVertexArrays(1, &self->m_line_vao);
    glBindVertexArray(self->m_line_vao);
    glGenBuffers(1, &self->m_line_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, self->m_line_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 2, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

	// triangle VAO/VBO
    glGenVertexArrays(1, &self->m_triangle_vao);
    glBindVertexArray(self->m_triangle_vao);
	glGenBuffers(1, &self->m_triangle_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, self->m_triangle_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 3, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

	glGenBuffers(1, &self->m_triangle_ebo);

	// platform VAO/VBO
    glGenVertexArrays(1, &self->m_rect_vao);
    glBindVertexArray(self->m_rect_vao);

    glGenBuffers(1, &self->m_rect_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, self->m_rect_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

    GLuint platform_indices[6] = { 0,2,1,1,2,3 };
    glGenBuffers(1, &self->m_rect_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->m_rect_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(platform_indices), platform_indices, GL_STATIC_DRAW);

	// Load Font
    if (FT_Init_FreeType(&self->ft_library)) {
        std::cout<<"Error initializing FreeType Library"<<std::endl;
    }
    if (FT_New_Face(self->ft_library, (getBasePath() + "/assets/fonts/arial.ttf").c_str(), 0, &self->ft_face)) {
        std::cout<<"Error loading Font"<<std::endl;
    }
    FT_Set_Pixel_Sizes(self->ft_face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(self->ft_face, c, FT_LOAD_RENDER)) {
            std::cout<<"Error loading Glyph for "<<c<<std::endl;
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RED,
            self->ft_face->glyph->bitmap.width, self->ft_face->glyph->bitmap.rows,
            0, GL_RED, GL_UNSIGNED_BYTE, self->ft_face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character new_char = {
            texture,
            glm::ivec2(self->ft_face->glyph->bitmap.width, self->ft_face->glyph->bitmap.rows),
            glm::ivec2(self->ft_face->glyph->bitmap_left, self->ft_face->glyph->bitmap_top),
            static_cast<unsigned int>(self->ft_face->glyph->advance.x)
        };

        self->Characters.insert(std::pair<char, Character>(c, new_char));

    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    FT_Done_Face(self->ft_face);
    FT_Done_FreeType(self->ft_library);

    glGenVertexArrays(1, &self->m_text_vao);
    glGenBuffers(1, &self->m_text_vbo);
    glBindVertexArray(self->m_text_vao);
    glBindBuffer(GL_ARRAY_BUFFER, self->m_text_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*6*4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,4,GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	glGenVertexArrays(1, &self->m_circle_vao);
	glBindVertexArray(self->m_circle_vao);
	glGenBuffers(1, &self->m_circle_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, self->m_circle_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*6, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(glm::vec2),nullptr);


    // Round rect
    // round rect VAO/VBO (pos.xy + localCoord.xy pro Vertex)
    glGenVertexArrays(1, &self->m_round_rect_vao);
    glBindVertexArray(self->m_round_rect_vao);

    glGenBuffers(1, &self->m_round_rect_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, self->m_round_rect_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)sizeof(glm::vec2));

    GLuint round_rect_indices[6] = { 0, 2, 1, 1, 2, 3 };
    glGenBuffers(1, &self->m_round_rect_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->m_round_rect_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(round_rect_indices), round_rect_indices, GL_STATIC_DRAW);


    // Fading Rect
    self->m_fade_rect_shader = Shader::init(assets::Shader::fade_rect).value();
    self->m_fade_rect_hollow_shader = Shader::init(assets::Shader::fade_rect_hollow).value();

    glGenVertexArrays(1, &self->m_fade_rect_vao);
    glBindVertexArray(self->m_fade_rect_vao);

    glGenBuffers(1, &self->m_fade_rect_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, self->m_fade_rect_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)sizeof(glm::vec2));

    GLuint fade_rect_indices[6] = { 0, 2, 1, 1, 2, 3 };
    glGenBuffers(1, &self->m_fade_rect_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->m_fade_rect_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(fade_rect_indices), fade_rect_indices, GL_STATIC_DRAW);


    // Hover Shimmer
    self->m_shimmer_shader = Shader::init(assets::Shader::shimmer).value();

    glGenVertexArrays(1, &self->m_shimmer_vao);
    glBindVertexArray(self->m_shimmer_vao);

    glGenBuffers(1, &self->m_shimmer_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, self->m_shimmer_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)sizeof(glm::vec2));

    GLuint shimmer_indices[6] = { 0, 2, 1, 1, 2, 3 };
    glGenBuffers(1, &self->m_shimmer_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->m_shimmer_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(shimmer_indices), shimmer_indices, GL_STATIC_DRAW);

    return self;
}

void Draw::deinit() noexcept {
    //m_player_mesh.deinit();
    m_player_shader.deinit();
    m_line_shader.deinit();
    m_polygon_shader.deinit();
    m_platform_shader.deinit();
    m_circle_shader.deinit();
    m_text_shader.deinit();
    m_sprite_shader.deinit();
    m_intermediate_framebuffer.deinit();
    m_round_rect_shader.deinit();
    m_fade_rect_shader.deinit();
    m_fade_rect_hollow_shader.deinit();
    m_shimmer_shader.deinit();
}

void Draw::start() noexcept {
    const auto& cam = m_registry->m_camera;
    glm::vec2 half = 0.5f * cam.viewSize;
    view = getViewMatrix();
    projection = glm::ortho(
        cam.center.x - half.x, cam.center.x + half.x,
        cam.center.y - half.y, cam.center.y + half.y,
        -1.f, 1.f
    );

    glm::uvec2 extent = m_intermediate_framebuffer.extent();
    glViewport(0, 0, extent.x, extent.y);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
}

void Draw::finish() noexcept {
    Viewport viewport = m_registry->m_viewport;
    glViewport(viewport.m_origin.x, viewport.m_origin.y,
               viewport.m_size.x, viewport.m_size.y);
}


void Draw::drawLine(glm::vec2 start, glm::vec2 end, glm::vec3 color, float thickness) noexcept {

    glm::vec2 direction = glm::normalize(end - start);
    glm::vec2 perp = glm::vec2(-direction.y, direction.x) * (thickness * 0.5f);

    fillRect(end + perp, end - perp, start + perp, start - perp, color, 1.0f);

    /*
    glBindVertexArray(m_line_vao);
    glm::vec2 points[2] = {start, end};

    glBindBuffer(GL_ARRAY_BUFFER, m_line_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);

    m_line_shader.use()
            .setMat4("view", view)
            .setMat4("projection", projection)
            .setVec3("color", color);

    glLineWidth(1);
    glDrawArrays(GL_LINES, 0, 2);
    */
}

void Draw::fillRect(glm::vec2 topleft, glm::vec2 topright,glm::vec2 bottomleft, glm::vec2 bottomright, glm::vec3 color, float opacity) noexcept {
    //draws a black quad
    glm::vec2 points[4] = { topleft, topright, bottomleft, bottomright };

    glBindVertexArray(m_rect_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_rect_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);

    m_platform_shader.use()
    .setMat4("view", view)
    .setMat4("projection", projection)
    .setVec3("fcolor", color)
    .setFloat("opacity", opacity);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Draw::fillRectBySize(glm::vec2 center, glm::vec2 size, glm::vec3 color, float opacity) noexcept {
    float w = size.x/2;
    float h = size.y/2;

    glm::vec2 tl = center + glm::vec2{-w, h};
    glm::vec2 tr = center + glm::vec2{w, h};
    glm::vec2 bl = center + glm::vec2{-w, -h};
    glm::vec2 br = center + glm::vec2{w, -h};

    fillRect(tl, tr, bl, br, color, opacity);
}

void Draw::fillFadeRect(glm::vec2 topleft, glm::vec2 topright, glm::vec2 bottomleft, glm::vec2 bottomright,
                         glm::vec3 outer_color, glm::vec3 inner_color, float fading_speed, float opacity) noexcept {
    glm::vec4 verts[4] = {
        glm::vec4(topleft,     -1.0f,  1.0f),
        glm::vec4(topright,     1.0f,  1.0f),
        glm::vec4(bottomleft,  -1.0f, -1.0f),
        glm::vec4(bottomright,  1.0f, -1.0f),
    };

    glm::vec2 halfSize = { glm::length(topright - topleft) * 0.5f, glm::length(topleft - bottomleft) * 0.5f };

    glBindVertexArray(m_fade_rect_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_fade_rect_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

    m_fade_rect_shader.use()
        .setMat4("view", view)
        .setMat4("projection", projection)
        .setVec2("halfSize", halfSize)
        .setVec3("outer_color", outer_color)
        .setVec3("inner_color", inner_color)
        .setFloat("fading_speed", fading_speed)
        .setFloat("opacity", opacity);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Draw::fillFadeRectBySize(glm::vec2 center, glm::vec2 size, glm::vec3 outer_color, glm::vec3 inner_color, float fading_speed, float opacity) noexcept {
    float w = size.x/2;
    float h = size.y/2;

    glm::vec2 tl = center + glm::vec2{-w, h};
    glm::vec2 tr = center + glm::vec2{w, h};
    glm::vec2 bl = center + glm::vec2{-w, -h};
    glm::vec2 br = center + glm::vec2{w, -h};

    fillFadeRect(tl, tr, bl, br, outer_color, inner_color, fading_speed, opacity);
}

void Draw::drawFadeRect(glm::vec2 topleft, glm::vec2 topright, glm::vec2 bottomleft, glm::vec2 bottomright,
                       glm::vec3 outer_color, float fading_speed, float opacity) noexcept {
    glm::vec4 verts[4] = {
        glm::vec4(topleft,     -1.0f,  1.0f),
        glm::vec4(topright,     1.0f,  1.0f),
        glm::vec4(bottomleft,  -1.0f, -1.0f),
        glm::vec4(bottomright,  1.0f, -1.0f),
    };

    glm::vec2 halfSize = { glm::length(topright - topleft) * 0.5f, glm::length(topleft - bottomleft) * 0.5f };

    glBindVertexArray(m_fade_rect_vao); // Nutzt dasselbe VAO/VBO[cite: 1]
    glBindBuffer(GL_ARRAY_BUFFER, m_fade_rect_vbo); //[cite: 1]
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts); //[cite: 1]

    m_fade_rect_hollow_shader.use() // Nutzt den NEUEN Shader
        .setMat4("view", view) //[cite: 1]
        .setMat4("projection", projection) //[cite: 1]
        .setVec2("halfSize", halfSize) //[cite: 1]
        .setVec3("outer_color", outer_color) //[cite: 1]
        .setFloat("fading_speed", fading_speed) //[cite: 1]
        .setFloat("opacity", opacity); //[cite: 1]

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //[cite: 1]
}

void Draw::drawFadeRectBySize(glm::vec2 center, glm::vec2 size, glm::vec3 outer_color, float fading_speed, float opacity) noexcept {
    float w = size.x / 2.0f;
    float h = size.y / 2.0f;

    glm::vec2 tl = center + glm::vec2{-w,  h};
    glm::vec2 tr = center + glm::vec2{ w,  h};
    glm::vec2 bl = center + glm::vec2{-w, -h};
    glm::vec2 br = center + glm::vec2{ w, -h};

    drawFadeRect(tl, tr, bl, br, outer_color, fading_speed, opacity);
}



void Draw::drawRect(glm::vec2 topleft, glm::vec2 topright,glm::vec2 bottomleft, glm::vec2 bottomright, glm::vec3 color) noexcept {
    drawLine(topleft, topright, color);
    drawLine(topleft, bottomleft, color);
    drawLine(bottomright, bottomleft, color);
    drawLine(bottomright, topright, color);
}

void Draw::drawRectBySize(glm::vec2 center, glm::vec2 size, glm::vec3 color) noexcept {
    float w = size.x/2;
    float h = size.y/2;

    glm::vec2 tl = center + glm::vec2{-w, h};
    glm::vec2 tr = center + glm::vec2{w, h};
    glm::vec2 bl = center + glm::vec2{-w, -h};
    glm::vec2 br = center + glm::vec2{w, -h};

    drawRect(tl, tr, bl, br, color);
}

void Draw::drawRoundRect(glm::vec2 center, glm::vec2 size, float radius, glm::vec3 color, float thickness) noexcept {
    glm::vec2 halfSize = size / 2.0f;
    radius = std::min({radius, halfSize.x, halfSize.y});

    glBindVertexArray(m_round_rect_vao);
    setupRoundRectQuad(m_round_rect_vbo, center, size);

    m_round_rect_shader.use()
        .setMat4("view", view)
        .setMat4("projection", projection)
        .setVec2("halfSize", halfSize)
        .setFloat("radius", radius)
        .setFloat("thickness", thickness)
        .setVec3("color", color)
        .setFloat("opacity", 1.0f);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Draw::fillRoundRect(glm::vec2 center, glm::vec2 size, float radius, glm::vec3 color, float opacity) noexcept {
    glm::vec2 halfSize = size / 2.0f;
    radius = std::min({radius, halfSize.x, halfSize.y});

    glBindVertexArray(m_round_rect_vao);
    setupRoundRectQuad(m_round_rect_vbo, center, size);

    m_round_rect_shader.use()
        .setMat4("view", view)
        .setMat4("projection", projection)
        .setVec2("halfSize", halfSize)
        .setFloat("radius", radius)
        .setFloat("thickness", 0.0f)
        .setVec3("color", color)
        .setFloat("opacity", opacity);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Draw::fillCircle(glm::vec2 position, float radius, glm::vec3 color, float opacity) noexcept {
    glm::vec2 tl = {-radius, radius};
    glm::vec2 tr = {radius, radius};
    glm::vec2 bl = {-radius, -radius};
    glm::vec2 br = {radius, -radius};
    glm::vec2 triangles[6] = {
        tl, bl, tr,
        tr, bl, br
    };

    glBindVertexArray(m_circle_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_circle_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(triangles), triangles);

    m_circle_shader.use()
        .setMat4("view", view)
        .setMat4("projection", projection)
        .setVec3("fcolor", color)
        .setFloat("opacity", opacity)
        .setVec2("center", position)
        .setFloat("radius", radius);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Draw::fillPolygon(glm::vec2 position, glm::vec2 scale, PolygonShape polygon, glm::vec3 color) noexcept {
    std::vector<glm::vec2> vertices = polygon.vertices;
    std::vector<int> indices = polygon.triangle_indices;
    if (vertices.size() < 3) {
        return; // Not enough vertices to form a polygon
    }

    glBindVertexArray(m_triangle_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_triangle_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_triangle_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(scale, 1.0f));

    m_polygon_shader.use()
        .setMat4("view", getViewMatrix())
        .setMat4("projection", projection)
        .setMat4("model", model)
        .setVec3("color", color);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
}



void Draw::drawString(std::string text, glm::vec2 pos, float scale, glm::vec3 color) noexcept {

    m_text_shader.use()
        .setMat4("view", view)
        .setMat4("projection", projection)
        .setVec3_f("textColor", color)
        .setSampler("text" , 0);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_text_vao);

    float text_width = getStringWidth(text, scale);

    float asc = 0.0f;
    float desc = 0.0f;
    for (char c : text) {
        Character ch = Characters[c];
        asc = std::max(asc, ch.bearing.y * scale);
        desc = std::max(desc, (ch.size.y - ch.bearing.y) * scale);
    }


    pos.x -= text_width/2.0f;
    pos.y -= (asc - desc) / 2.0f;

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        glm::vec2 position = {pos.x + ch.bearing.x*scale, pos.y - (ch.size.y - ch.bearing.y) * scale};

        drawCharacter(ch, position, scale);

        pos.x += (ch.advance >> 6) * scale;
    }
}

void Draw::drawStringInBox(std::string text, glm::vec2 pos, glm::vec2 size, glm::vec3 color, float max_size) noexcept {
    float scale = getOptimalScaling(text, size*0.8f);
    //float scale = getOptimalScaling(text, size*0.95f);
    if (max_size != -1) {
        scale = std::min(scale, max_size);
    }
    drawString(text, pos, scale, color);
}

void Draw::drawLinesInBox(std::vector<std::string> lines, glm::vec2 pos, glm::vec2 size, glm::vec3 color, float max_size) noexcept {
    if (lines.empty()) {
        return;
    }

    float avg_height = 0.0f;
    for (const auto& line : lines) {
        avg_height += getStringHeight(line, 1.0f);
    }
    avg_height /= static_cast<float>(lines.size());
    float line_gap = avg_height * 0.3f;

    float scale = getOptimalScalingForMultipleLines(lines, line_gap, size * 0.9f);

    if (max_size != -1) {
        scale = std::min(scale, max_size);
    }

    std::vector<float> heights;
    heights.reserve(lines.size());
    float total_height = 0.0f;
    for (const auto& line : lines) {
        float h = getStringHeight(line, scale);
        heights.push_back(h);
        total_height += h;
    }
    total_height += line_gap * scale * (static_cast<float>(lines.size()) - 1);

    float cursor_y = pos.y + total_height / 2.0f;

    for (size_t i = 0; i < lines.size(); i++) {
        cursor_y -= heights[i] / 2.0f;
        drawString(lines[i], {pos.x, cursor_y}, scale, color);
        cursor_y -= heights[i] / 2.0f;

        if (i < lines.size() - 1) {
            cursor_y -= line_gap * scale;
        }
    }
}

void Draw::drawRainbowString(std::string text, glm::vec2 pos, float scale, std::vector<glm::vec3> colors, std::vector<int> color_ids) noexcept {

    m_text_shader.use()
    .setMat4("view", view)
    .setMat4("projection", projection)
    .setSampler("text" , 0);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_text_vao);

    float text_width = getStringWidth(text, scale);
    pos.x -= text_width/2.0f;
    pos.y -= Characters[0].size.y*scale/2.0f;

    std::string::const_iterator c;
    int i = 0;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];
        m_text_shader.setVec3_f("textColor", colors[color_ids[i]]);

        glm::vec2 position = {pos.x + ch.bearing.x*scale, pos.y - (ch.size.y - ch.bearing.y) * scale};

        drawCharacter(ch, position, scale);

        pos.x += (ch.advance >> 6) * scale;
        i += 1;
    }
}

void Draw::drawCharacter(Character ch, glm::vec2 position, float scale) noexcept {
    float w = ch.size.x * scale;
    float h = ch.size.y * scale;
    float verts[6][4] = {
        {position.x, position.y+h, 0.0f, 0.0f},
        {position.x, position.y, 0.0f, 1.0f},
        {position.x + w, position.y, 1.0f, 1.0f},
        {position.x, position.y+h, 0.0f, 0.0f},
        {position.x + w, position.y, 1.0f, 1.0f},
        {position.x + w, position.y + h, 1.0f, 0.0f}
    };
    glBindTexture(GL_TEXTURE_2D, ch.id);
    glBindBuffer(GL_ARRAY_BUFFER, m_text_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

float Draw::getOptimalScaling(std::string text, glm::vec2 size) noexcept {
    float normalW = getStringWidth(text, 1.f);
    float normalH = getStringHeight(text, 1.f);
    return std::min(size.x / normalW, size.y / normalH);
}

float Draw::getOptimalScalingForMultipleLines(std::vector<std::string> lines, float line_gap, glm::vec2 size) noexcept {
    float maxWidth = 0.0f;
    float totalHeight = 0.0f;

    for (size_t i = 0; i < lines.size(); i++) {
        float w = getStringWidth(lines[i], 1.f);
        float h = getStringHeight(lines[i], 1.f);

        maxWidth = std::max(maxWidth, w);
        totalHeight += h;

        if (i < lines.size() - 1) {
            totalHeight += line_gap;
        }
    }

    return std::min(size.x / maxWidth, size.y / totalHeight);
}




float Draw::getStringWidth(std::string text, float scale) noexcept {
    std::string::const_iterator c;
    float whole_width = 0.0f;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];
        whole_width += (ch.advance >> 6) * scale;
    }
    return whole_width;
}

float Draw::getStringHeight(std::string text, float scale) noexcept {
    float maxH = 0.0f;
    float maxB = 0.0f;
    for (char c : text) {
        Character ch = Characters[c];
        float h = ch.bearing.y * scale;
        float b = (ch.size.y - ch.bearing.y) * scale;
        maxH = std::max(maxH, h);
        maxB = std::max(maxB, b);
    }
    return maxH + maxB;
}


void Draw::drawArrow(glm::vec2 start_point, glm::vec2 end_point, glm::vec3 color, float line_width) noexcept {
    glm::vec2 direction = glm::normalize(end_point - start_point);
    glm::vec2 perp = glm::vec2(-direction.y, direction.x);

    glm::vec2 arrow_1_direction = glm::normalize(-direction + perp);
    glm::vec2 arrow_1_perp = glm::vec2(-arrow_1_direction.y, arrow_1_direction.x);
    glm::vec2 arrow_2_direction = glm::normalize(-direction - perp);
    glm::vec2 arrow_2_perp = glm::vec2(-arrow_2_direction.y, arrow_2_direction.x);

    fillRect(end_point - direction * 0.003f + perp * line_width,
        end_point - direction * 0.003f - perp * line_width,
        start_point + perp * line_width,
        start_point - perp * line_width,
        color, 1.f);
    fillRect(end_point + arrow_1_direction * 0.1f + arrow_1_perp * line_width,
        end_point - line_width * perp + arrow_1_perp * line_width,
        end_point + arrow_1_direction * 0.1f - arrow_1_perp * line_width,
        end_point - line_width * perp - arrow_1_perp * line_width,
        color, 1.f);
    fillRect(end_point + arrow_2_direction * 0.1f + arrow_2_perp * line_width,
        end_point + arrow_2_perp * line_width,
        end_point + arrow_2_direction * 0.1f - arrow_2_perp * line_width,
        end_point - arrow_2_perp * line_width,
        color, 1.f);
}


void Draw::drawTexture(Texture& texture, glm::vec2 position, glm::vec2 scale) noexcept {

    glBindVertexArray(m_quad_vao);
    Shader& sprite_shader = m_sprite_shader;
    sprite_shader.use()
    .setMat4("view", view)
    .setMat4("projection", projection)
    .setSampler("sprite", 0)
    .setVec3("fcolor", {1,1,1});

    glm::mat4 model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(position, 0));
    model = glm::scale(model, glm::vec3(scale, 1));

    texture.bind(0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    sprite_shader.setMat4("model[0]", model);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 1);
}

void Draw::drawTextureByID(GLuint textureId, glm::vec2 position, glm::vec2 scale) noexcept {

    glBindVertexArray(m_quad_vao);
    Shader& sprite_shader = m_sprite_shader;
    sprite_shader.use()
    .setMat4("view", view)
    .setMat4("projection", projection)
    .setSampler("sprite", 0)
    .setVec3("fcolor", {1, 1, 1})
    .setFloat("alpha", 1.0);

    glm::mat4 model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(position, 0));
    model = glm::scale(model, glm::vec3(scale, 1));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    sprite_shader.setMat4("model[0]", model);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 1);
}

void Draw::drawPopup(std::string text, glm::vec2 pos, glm::vec2 size, glm::vec3 text_color) noexcept {
    float angle = 10.0;

    fillRoundRect(pos, size, angle, black, 0.75f);
    drawStringInBox(text, pos, size, text_color);
    drawRoundRect(pos, size, angle, black);
}

void Draw::drawPopupAtMouse(std::string text, glm::vec3 text_color, float text_size) noexcept {
    float w = getStringWidth(text, text_size)+0.1;
    float h = getStringHeight(text, text_size)+0.1;
    glm::vec2 size = {w, h};
    glm::vec2 pos = getPopupPositionForMouse(size);
    drawPopup(text, pos, size, text_color);
}

void Draw::drawMultilinePopupAtMouse(std::string title, std::vector<std::string> lines, bool with_title, glm::vec3 text_color, float text_size) noexcept {
    float title_scale = text_size;
    float line_scale = with_title?text_size*(2.0f/3.0f):text_size;

    std::vector<std::pair<std::string, float>> entries;
    if (with_title) {
        entries.emplace_back(title, title_scale);
    }
    for (const std::string& line : lines) {
        entries.emplace_back(line, line_scale);
    }

    if (entries.empty()) return;

    float avg_height = 0.0f;
    for (const auto& entry : entries) {
        avg_height += getStringHeight(entry.first, entry.second);
    }
    avg_height /= static_cast<float>(entries.size());
    float line_gap = avg_height * 0.3f;

    float max_width = 0.0f;
    float total_height = 0.0f;
    std::vector<float> heights;
    heights.reserve(entries.size());

    for (size_t i = 0; i < entries.size(); ++i) {
        float w = getStringWidth(entries[i].first, entries[i].second);
        float h = getStringHeight(entries[i].first, entries[i].second);

        max_width = std::max(w, max_width);
        heights.push_back(h);
        total_height += h;

        if (i < entries.size()-1) {
            total_height += line_gap;
        }
    }

    glm::vec2 size = {max_width + 0.1f, total_height+0.1f};
    glm::vec2 pos = getPopupPositionForMouse(size);

    float angle = std::min(size.x, size.y) * 0.15f;

    fillRoundRect(pos, size, angle, black, 0.75f);

    float cursor_y = pos.y + total_height / 2.0f;
    for (size_t i = 0; i < entries.size(); i++) {
        cursor_y -= heights[i] / 2.0f;
        drawString(entries[i].first, {pos.x, cursor_y}, entries[i].second, text_color);
        cursor_y -= heights[i] / 2.0f;

        if (i < entries.size() - 1) {
            cursor_y -= line_gap;
        }
    }

    drawRoundRect(pos, size, angle, black);
}



glm::mat4 Draw::getViewMatrix() noexcept {
    return glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

static std::pair<glm::uvec2, glm::uvec2> computeViewportConfig(const glm::uvec2 resolution) noexcept {
    float window_aspect_ratio = (float) resolution.x / resolution.y;
    if (window_aspect_ratio > config::GAME_ASPECT_RATIO) {
        glm::uvec2 viewport_size = {resolution.y * config::GAME_ASPECT_RATIO, resolution.y};
        uint32_t offset = (resolution.x - viewport_size.x) / 2;
        glm::uvec2 viewport_offset = {offset, 0};

        return {viewport_offset, viewport_size};
    }

    glm::uvec2 viewport_size = {resolution.x, resolution.x / config::GAME_ASPECT_RATIO};
    uint32_t offset = (resolution.y - viewport_size.y) / 2;
    glm::uvec2 viewport_offset = {0, offset};

    return {viewport_offset, viewport_size};
}


void Draw::onResizeCallback(GLFWwindow *, int width, int height) noexcept {
    auto [origin, size] = computeViewportConfig({width, height});
    m_registry->m_viewport.m_origin = origin;
    m_registry->m_viewport.m_size = size;

    // reinitialize off-screen framebuffer
    m_intermediate_framebuffer.deinit();
    m_intermediate_framebuffer = Framebuffer::init({(GLsizei) size.x, (GLsizei) size.y, 1, true});
}

void Draw::drawProgressBar(glm::vec2 center, glm::vec2 size, float progress, glm::vec3 bar_color, glm::vec3 background_color) noexcept {
    glm::vec2 top_left = center + glm:: vec2{-size.x, size.y}/2.0f;
    float progress_w = size.x * progress;
    float remaining_w = size.x - progress_w;

    fillRectBySize( glm::vec2{top_left.x + progress_w/2.0f, center.y}, glm::vec2{progress_w, size.y}, bar_color, 1.0f);
    fillRectBySize(glm::vec2{top_left.x + progress_w + remaining_w/2.0f, center.y}, glm::vec2{remaining_w, size.y}, background_color, 1.0f);
}

void Draw::drawShimmer(glm::vec2 center, glm::vec2 size, ShimmerShape shape, float intensity,
                             glm::vec3 glow_color, float height_factor) noexcept {
    if (intensity <= 0.001f) return; // unsichtbar -> Draw-Call sparen

    glBindVertexArray(m_shimmer_vao);
    setupHoverShimmerQuad(m_shimmer_vbo, center, size, height_factor);

    // additiv blenden fürs Leuchten, danach wieder normales Alpha-Blending herstellen
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    m_shimmer_shader.use()
        .setMat4("view", view)
        .setMat4("projection", projection)
        .setFloat("shapeMode", static_cast<float>(shape))
        .setFloat("intensity", intensity)
        .setFloat("time", static_cast<float>(glfwGetTime()))
        .setVec3("glowColor", glow_color);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

