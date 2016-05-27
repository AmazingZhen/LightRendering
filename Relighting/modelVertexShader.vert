#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 view_space_pos;
out vec3 view_space_normal;

uniform mat4 model_view;
uniform mat4 projection;

void main() {
    gl_Position = projection * model_view * vec4(position, 1.0);

    view_space_pos = vec3(model_view * vec4(position, 1.0));
    view_space_normal = vec3(model_view * vec4(normal, 0.0));
}