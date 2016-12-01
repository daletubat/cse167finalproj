#version 330 core

out vec4 color;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform sampler2D tex;

void main()
{    
    // Properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = Normal;
	//color = vec4(TexCoords, 0.0, 1.0);
	color = 0.4 * texture(tex, TexCoords);
}