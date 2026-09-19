//
// Created by luisb on 19.09.2026.
//

#include "cursor.h"

#include "helpers/sizes.h"

CursorSystem CursorSystem::init(Window *window, Registry *registry, UISystem *ui) noexcept {
    CursorSystem self;
    self.m_window = window;
    self.m_registry = registry;
    self.m_ui = ui;

    self.init_cursor_texture(self.axe, "axe");
    self.init_cursor_texture(self.basket, "basket");
    self.init_cursor_texture(self.crane, "crane");
    self.init_cursor_texture(self.hoe, "hoe");
    self.init_cursor_texture(self.sickle, "sickle");
    self.init_cursor_texture(self.truck, "truck");
    self.init_cursor_texture(self.wrecking_ball, "wrecking_ball");

    return self;
}

void CursorSystem::deinit() noexcept {

}

void CursorSystem::reset() noexcept {

}

void CursorSystem::step(const float delta) noexcept {
    (void) delta;

    if (m_registry->window() != FIELDS) {
        if (!normal_cursor_active) {
            normal_cursor_active = true;
            current_tool_name = "";
            reset_cursor();
        }
        return;
    }

    glm::vec2 mouse_pos = m_window->getMousePosition();
    bool inside_fields = point_in_rect(mouse_pos, full_field_center, field_tool_cursor_size);
    bool valid_tile = m_registry->current_hover_field_tile != entt::null;

    if (inside_fields && valid_tile) {
        normal_cursor_active = false;
        FarmField tile = m_registry->ecs.get<FarmField>(m_registry->current_hover_field_tile);
        Field field = m_registry->ecs.get<Field>(tile.parent_field);

        std::string tool = "Hoe";
        if (field.plant_type == NORMAL_PLANT) {
            if (tile.harvestable || m_registry->keys["shift"]) {
                tool = "Sickle";
                switch_cursor(sickle);
            } else {
                tool = "Hoe";
                switch_cursor(hoe);
            }
        } else if (field.plant_type == COLLECT_PLANT) {
            if (m_registry->keys["shift"]) {
                tool = "Axe";
                switch_cursor(axe);
            } else if (tile.harvestable) {
                tool = "Basket";
                switch_cursor(basket);
            } else {
                tool = "Hoe";
                switch_cursor(hoe);
            }
        } else if (field.plant_type == INVENTORY_PLANT) {
            if (m_registry->keys["shift"]) {
                tool = "Wrecking Ball";
                switch_cursor(wrecking_ball);
            } else if (tile.plant != entt::null) {
                tool = "Truck";
                switch_cursor(truck);
            } else {
                tool = "Crane";
                switch_cursor(crane);
            }
        }
        if (current_tool_name != tool) {
            current_tool_name = tool;
        }
    } else if (!normal_cursor_active) {
        normal_cursor_active = true;
        current_tool_name = "";
        reset_cursor();
    }
}

void CursorSystem::switch_cursor(const Texture &text) noexcept {
    GLuint handle = static_cast<GLuint>(static_cast<int>(text));

    auto it = m_cursor_cache.find(handle);
    GLFWcursor* cursor = nullptr;

    if (it != m_cursor_cache.end()) {
        cursor = it->second;
    } else {
        glBindTexture(GL_TEXTURE_2D, handle);

        GLint width = 0, height = 0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

        if (width <= 0 || height <= 0) {
            std::cerr << "switch_cursor: ungueltige Texturgroesse" << std::endl;
            return;
        }

        std::vector<unsigned char> pixels(static_cast<size_t>(width) * height * 4);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());


        std::vector<unsigned char> flipped(pixels.size());
        for (int y = 0; y < height; ++y) {
            memcpy(
                &flipped[(height - 1 - y) * width * 4],
                &pixels[y * width * 4],
                width * 4
            );
        }

        GLFWimage image;
        image.width = width;
        image.height = height;
        image.pixels = flipped.data();

        cursor = glfwCreateCursor(&image, width/2, height/2);
        m_cursor_cache[handle] = cursor;
    }

    if (m_active_cursor != cursor) {
        glfwSetCursor(m_window->handle(), cursor);
        m_active_cursor = cursor;
    }
}

void CursorSystem::reset_cursor() noexcept {
    if (m_active_cursor != nullptr) {
        glfwSetCursor(m_window->handle(), nullptr);
        m_active_cursor = nullptr;
    }
}

