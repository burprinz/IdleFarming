#version 410 core
uniform vec3 fcolor;
uniform float opacity;

out vec4 FragColor;

void main() {
    FragColor = vec4(fcolor, opacity);
}