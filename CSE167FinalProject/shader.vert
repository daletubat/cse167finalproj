#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 texture;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0);
	FragPos = vec3(model * vec4(position, 1.0f));
	Normal = mat3(transpose(inverse(model))) * normal;
	TexCoords = vec2(texture.x, texture.y);
}
