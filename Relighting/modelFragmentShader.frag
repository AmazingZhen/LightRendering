#version 330 core

in vec3 view_space_pos;
in vec3 view_space_normal;

layout (location = 0) out vec4 color;

uniform int mode;
uniform mat4 model_view;
uniform samplerCube skybox;

void main() {
	vec3 outgoing_direction;
	if (mode == 1) {
		// Reflection
    	outgoing_direction = reflect(normalize(view_space_pos), normalize(view_space_normal));
    } else if (mode == 2) {
    	// Refraction
    	outgoing_direction = refract(normalize(view_space_pos), normalize(view_space_normal), 0.658);
    }

    // convert from eye to world space
    outgoing_direction = vec3(inverse(model_view) * vec4(outgoing_direction, 0.0));

    color = texture(skybox, outgoing_direction);
}