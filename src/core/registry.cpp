#include "registry.h"

#include <iostream>

#include "../helpers/config.h"

Registry* Registry::init() noexcept {
	Registry* self = new Registry;

	self->keys.insert({"w", false});
	self->keys.insert({"a", false});
	self->keys.insert({"d", false});
	self->keys.insert({"q", false});
	self->keys.insert({"e", false});
	self->keys.insert({"space", false});
	self->keys.insert({"shift", false});
	self->keys.insert({"strg", false});
	self->keys.insert({"mouse_left", false});
	self->keys.insert({"mouse_right", false});
	self->keys.insert({"vertical_scroll_up", false});
	self->keys.insert({"vertical_scroll_down", false});
	self->keys.insert({"horizontal_scroll_up", false});
	self->keys.insert({"horizontal_scroll_down", false});



	self->m_viewport.m_origin = glm::uvec2(0);
	self->m_viewport.m_size   = glm::uvec2(0);

	self->debug_mode = false;

	return self;
}

void Registry::on_key_callback(int key, int action) noexcept {
	if (action != GLFW_PRESS && action != GLFW_RELEASE) return;
	bool active = action==GLFW_PRESS;
	if (active) {
		if (key == GLFW_KEY_F5) debug = !debug;
	}
	switch (key) {
		case GLFW_KEY_W:
			keys.insert_or_assign("w", active);
			break;
		case GLFW_KEY_A:
			keys.insert_or_assign("a", active);
			break;
		case GLFW_KEY_D:
			keys.insert_or_assign("d", active);
			break;
		case GLFW_KEY_S:
			keys.insert_or_assign("s", active);
			break;
		case GLFW_KEY_SPACE:
			keys.insert_or_assign("space", active);
			break;
		case GLFW_KEY_LEFT_SHIFT:
			keys.insert_or_assign("shift", active);
			break;
		case GLFW_KEY_LEFT_CONTROL:
			keys.insert_or_assign("strg", active);
			break;

	}
}

void Registry::on_mouse_callback(int key, int action) noexcept {
	if (action != GLFW_PRESS && action != GLFW_RELEASE) return;
	bool active = action==GLFW_PRESS;
	switch (key) {
		case GLFW_MOUSE_BUTTON_1:
			keys.insert_or_assign("mouse_left", active);
			break;
		case GLFW_MOUSE_BUTTON_2:
			keys.insert_or_assign("mouse_right", active);
			break;
	}
}

void Registry::on_scroll_callback(float xOff, float yOff) {
	if (yOff > 0) {
		keys["vertical_scroll_up"] = true;
	} else {
		keys["vertical_scroll_down"] = true;
	}
	if (xOff > 1) {
		keys["horizontal_scroll_up"] = true;
	} else {
		keys["horizontal_scroll_down"] = true;
	}
}


