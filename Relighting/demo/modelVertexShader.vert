#version 410 core
layout (location = 0) in vec3 world_position;
layout (location = 1) in vec3 world_normal;

out vec3 view_space_pos;
out vec3 view_space_normal;
out vec3 view_space_tangent;
out vec3 view_space_bitangent;

uniform mat4 MV;
uniform mat4 P;

void calculateBasis(out vec3 tangent, out vec3 bitangent, in vec3 normal) {
	bitangent = vec3(0.0, 1.0, 0.0);

	float normalDotUp = dot(normal, bitangent);

	if (normalDotUp == 1.0)
	{
		bitangent = vec3(0.0, 0.0, -1.0);
	}
	else if (normalDotUp == -1.0)
	{
		bitangent = vec3(0.0, 0.0, 1.0);
	}
	
	tangent = cross(bitangent, normal);	
	bitangent = cross(normal, tangent);
}

void main() {
    gl_Position = P * MV * vec4(world_position, 1.0);

    view_space_pos = vec3(MV * vec4(world_position, 1.0));
    view_space_normal = vec3(MV * vec4(world_normal, 0.0));

    calculateBasis(view_space_tangent, view_space_bitangent, view_space_normal);
}