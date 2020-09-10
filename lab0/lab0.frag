#version 150

in vec3 Normal;
in vec3 Frag_pos;
in vec2 Frag_tex_coord;

uniform sampler2D example_texture;
uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 object_color;

out vec4 Frag_color;

void main(void)
{
	vec3 norm = normalize(Normal);
	vec3 light_dir = normalize(light_pos - Frag_pos); 

	float ambient_strength = 0.1;
    vec3 ambient = ambient_strength * light_color;

	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = diff * light_color;

	float specular_strength = 0.5;
	vec3 view_dir = normalize(vec3(0,0,0) - Frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular = specular_strength * spec * light_color;  

	vec3 result = (ambient + diffuse + specular) * object_color;
	Frag_color = vec4(result, 1.0) + texture(example_texture, Frag_tex_coord);
}

