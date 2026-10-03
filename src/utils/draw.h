//
// Created by luisb on 05.03.2026.
//
#pragma once

#include <utils/mesh.h>
#include <utils/shader.h>
#include <utils/framebuffer.h>
#include <core/registry.h>

#include "window.h"
#include "freetype/freetype.h"
#include "glm/vec3.hpp"
#include "helpers/colors.h"
#include "helpers/sizes.h"


struct Character {
    unsigned int id;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
};

enum class ShimmerShape {
    Circle = 0,
    Square = 1,
};


class Draw {
public:
    Draw() = default;
    ~Draw() = default;

    static Draw *init(Window* window, Registry* registry) noexcept;
    void deinit() noexcept;

    void start() noexcept;
    void finish() noexcept;

    /*
     * Geometry
     */
    void drawLine(glm::vec2 start, glm::vec2 end, glm::vec3 color, float thickness=0.02f) noexcept;

    void drawRectBySize(glm::vec2 center, glm::vec2 size, glm::vec3 color) noexcept;
    void drawRect(glm::vec2 topleft, glm::vec2 topright,glm::vec2 bottomleft, glm::vec2 bottomright, glm::vec3 color) noexcept;
    void fillRectBySize(glm::vec2 center, glm::vec2 size, glm::vec3 color, float opacity) noexcept;
    void fillRect(glm::vec2 topleft, glm::vec2 topright,glm::vec2 bottomleft, glm::vec2 bottomright, glm::vec3 color, float opacity) noexcept;
    void drawRoundRect(glm::vec2 center, glm::vec2 size, float radius, glm::vec3 color, float thickness = 0.03f) noexcept;
    void fillRoundRect(glm::vec2 center, glm::vec2 size, float radius, glm::vec3 color, float opacity) noexcept;
    void fillFadeRectBySize(glm::vec2 center, glm::vec2 size, glm::vec3 outer_color, glm::vec3 inner_color, float fading_speed, float opacity = 1.0f) noexcept;
    void fillFadeRect(glm::vec2 topleft, glm::vec2 topright, glm::vec2 bottomleft, glm::vec2 bottomright,
                       glm::vec3 outer_color, glm::vec3 inner_color, float fading_speed, float opacity = 1.0f) noexcept;
    void drawFadeRectBySize(glm::vec2 center, glm::vec2 size, glm::vec3 outer_color, float fading_speed, float opacity = 1.0f) noexcept;
    void drawFadeRect(glm::vec2 topleft, glm::vec2 topright, glm::vec2 bottomleft, glm::vec2 bottomright,
                       glm::vec3 outer_color, float fading_speed, float opacity = 1.0f) noexcept;

    //TODO void drawCircle(glm::vec2 position, float radius, glm::vec3 color, float opacity, glm::mat4 view, glm::mat4 projection) noexcept;
    void fillCircle(glm::vec2 position, float radius, glm::vec3 color, float opacity) noexcept;

    //TODO void drawPolygon(glm::vec2 position, glm::vec2 scale, PolygonShape polygon, glm::vec3 color) noexcept;
    void fillPolygon(glm::vec2 position, glm::vec2 scale, PolygonShape polygon, glm::vec3 color) noexcept;

    void drawProgressBar(glm::vec2 center, glm::vec2 size, float progress, glm::vec3 bar_color, glm::vec3 background_color) noexcept;

    /*
     * Forms
     */

    /**
     * Draws an Arrow. Made by Floke
     * @param start_point arrow start
     * @param end_point arrow dir
     * @param color arrow color
     * @param line_width arrow dicke
     */
    void drawArrow(glm::vec2 start_point, glm::vec2 end_point, glm::vec3 color, float line_width) noexcept;

    /*
     * Text
     */
    void drawString(std::string text, glm::vec2 pos, float scale, glm::vec3 color) noexcept;
    void drawStringInBox(std::string text, glm::vec2 pos, glm::vec2 size, glm::vec3 color, float max_size = -1.0f) noexcept;
    void drawLinesInBox(std::vector<std::string> lines, glm::vec2 pos, glm::vec2 size, glm::vec3 color, float max_size = -1.0f) noexcept;
    void drawRainbowString(std::string text, glm::vec2 pos, float scale, std::vector<glm::vec3> colors, std::vector<int> color_ids) noexcept;
    void drawCharacter(Character ch, glm::vec2 position, float scale) noexcept;
    void drawPopup(std::string text, glm::vec2 pos, glm::vec2 size, glm::vec3 text_color=white) noexcept;
    void drawPopupAtMouse(std::string text, glm::vec3 text_color=white, float text_size=normal_popup_text_size) noexcept;
    void drawMultilinePopupAtMouse(std::string title, std::vector<std::string> lines, bool with_title, glm::vec3 text_color=white, float text_size=normal_popup_text_size) noexcept;

    void drawShimmer(glm::vec2 center, glm::vec2 size, ShimmerShape shape, float intensity,
                                 glm::vec3 glow_color, float height_factor) noexcept;

    float getOptimalScalingForMultipleLines(std::vector<std::string> lines, float line_gap, glm::vec2 size) noexcept;
    float getOptimalScaling(std::string text, glm::vec2 size) noexcept;
    float getStringWidth(std::string text, float scale) noexcept;
    float getStringHeight(std::string text, float scale) noexcept;




    inline void drawButton(std::string text, glm::vec2 center, glm::vec2 size, glm::vec3 outer_color, glm::vec3 inner_color, glm::vec3 text_color, bool with_max_size) noexcept {
        float b_size = with_max_size?max_button_text_size:-1;
        fillFadeRectBySize(center, size, outer_color, inner_color, button_fading_speed);
        drawStringInBox(text, center, size, text_color, b_size);
    }

    /**
     *Texture
     */
    void drawTexture(Texture& texture, glm::vec2 position, glm::vec2 scale) noexcept;
    void drawTextureByID(GLuint textureId, glm::vec2 position, glm::vec2 scale) noexcept;
    /*
     * Helpers
     */
    inline glm::vec2 getPopupPositionForMouse(glm::vec2 popup_size) noexcept {
        glm::vec2 mouse_pos = m_window->getMousePosition();

        glm::vec2 result = {0, 0};

        if (mouse_pos.x + popup_size.x > config::CAMERA_VIEW_WIDTH) {
            result.x = mouse_pos.x - popup_size.x / 2.0f;
        } else {
            result.x = mouse_pos.x + popup_size.x / 2.0f;
        }

        if (mouse_pos.y + popup_size.y > config::CAMERA_VIEW_HEIGHT) {
            result.y = mouse_pos.y - popup_size.y / 2.0f;
        } else {
            result.y = mouse_pos.y + popup_size.y / 2.0f;
        }

        return result;
    }

    inline void drawOutOfSquare(glm::vec2 top_left, glm::vec2 size, glm::vec3 color, float opacity=1.0f) noexcept {
        glm::vec2 top_right = top_left + glm::vec2{size.x, 0};
        glm::vec2 bot_left = top_left + glm::vec2{0, -size.y};
        glm::vec2 bot_right = top_right + glm::vec2{0, -size.y};

        fillRect(screen_top_left, screen_top_right, {screen_top_left.x, top_left.y}, {screen_top_right.x, top_right.y}, color, opacity);
        fillRect({screen_bot_left.x, bot_left.y}, {screen_bot_right.x, bot_left.y}, screen_bot_left, screen_bot_right, color, opacity);
        fillRect(screen_top_left, {top_left.x, screen_top_left.y}, screen_bot_left, {bot_left.x, screen_bot_left.y}, color, opacity);
        fillRect({top_right.x, screen_top_right.y}, screen_top_right, {bot_right.x, screen_bot_right.y}, screen_bot_right, color, opacity);
    }

    glm::mat4 getViewMatrix() noexcept;

    void onResizeCallback(GLFWwindow* window, int width, int height) noexcept;


private:
    Window* m_window;
    Registry* m_registry;

    glm::mat4 view;
    glm::mat4 projection;

    Framebuffer m_intermediate_framebuffer;

    Shader m_player_shader;
    Shader m_line_shader;
    Shader m_platform_shader;
    Shader m_text_shader;
    Shader m_sprite_shader;

    Shader m_circle_shader;

    Shader m_polygon_shader;


    Shader m_round_rect_shader;

    Shader m_fade_rect_shader;
    Shader m_fade_rect_hollow_shader;
    GLuint m_fade_rect_vao, m_fade_rect_vbo, m_fade_rect_ebo;


    GLuint m_quad_vao;
    GLuint m_quad_ebo;

    Shader m_shimmer_shader;
    GLuint m_shimmer_vao, m_shimmer_vbo, m_shimmer_ebo;


    struct Line { glm::vec2 position; };
    GLuint m_line_vao;
    GLuint m_line_vbo;

    GLuint m_triangle_vao;
    GLuint m_triangle_vbo;
    GLuint m_triangle_ebo;

    struct platform {
        glm::vec2 m_position;
        glm::vec2 m_size;
    };
    GLuint m_rect_vao;
    GLuint m_rect_vbo;
    GLuint m_rect_ebo;

    FT_Library ft_library;
    FT_Face ft_face;
    std::map<char, Character> Characters;
    GLuint m_text_vao;
    GLuint m_text_vbo;

    GLuint m_circle_vao;
    GLuint m_circle_vbo;

    GLuint m_round_rect_vao;
    GLuint m_round_rect_vbo;
    GLuint m_round_rect_ebo;


    static void setupRoundRectQuad(GLuint vbo, glm::vec2 center, glm::vec2 size) noexcept {
        float w = size.x / 2.0f;
        float h = size.y / 2.0f;

        // vec4: worldPos.xy, localPos.xy (-1..1)
        glm::vec4 verts[4] = {
            { center.x - w, center.y + h, -1.0f,  1.0f }, // top-left
            { center.x + w, center.y + h,  1.0f,  1.0f }, // top-right
            { center.x - w, center.y - h, -1.0f, -1.0f }, // bottom-left
            { center.x + w, center.y - h,  1.0f, -1.0f }, // bottom-right
        };

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
    }


    static void setupHoverShimmerQuad(GLuint vbo, glm::vec2 center, glm::vec2 size, float height_factor) noexcept {
        float w = size.x / 2.0f;
        float h = size.y / 2.0f;
        float top_extra = size.y * height_factor;

        float bottom = center.y - h;
        float top = center.y + h + top_extra;

        // vec4: worldPos.xy, localCoord.xy (-1..1, y=-1 an der Basis, y=1 an der Spitze)
        glm::vec4 verts[4] = {
            { center.x - w, top,    -1.0f,  1.0f }, // top-left
            { center.x + w, top,     1.0f,  1.0f }, // top-right
            { center.x - w, bottom, -1.0f, -1.0f }, // bottom-left
            { center.x + w, bottom,  1.0f, -1.0f }, // bottom-right
        };

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
    }

};


