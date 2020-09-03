#version 150

in vec3 pos;
in vec3 norm;
in vec2 tex_coord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 Normal;
out vec3 Frag_pos;
out vec2 Frag_tex_coord;

void main(void)
{
	gl_Position = projection * view * model * vec4(pos, 1.0);
	Normal = norm;
	Frag_pos = vec3(model * vec4(pos, 1.0));
	Frag_tex_coord = tex_coord;
}

