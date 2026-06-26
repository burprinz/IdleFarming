//basicly like line.vert with added worldPos.

#version 410 core
layout (location = 0) in vec2 in_pos;

uniform mat4 view;
uniform mat4 projection;

out vec2 v_worldPos;

void main() {
    v_worldPos = in_pos; //before Transform
    gl_Position = projection * view * vec4(in_pos, 0.0, 1.0);
}


