#version 150

//in vec3 in_Color;
in vec3 in_Position;
in vec3 in_Normal;
in vec2 in_TexCoord;
uniform mat4 matrix;

out vec4 g_color;
const vec3 lightDir = normalize(vec3(0.3, 0.5, 1.0));

// Uppgift 3: Soft-skinning p� GPU
//
// Flytta �ver din implementation av soft skinning fr�n CPU-sidan
// till vertexshadern. Mer info finns p� hemsidan.
uniform mat4 bone_0_rot;
uniform mat4 bone_0_trans;
uniform mat4 bone_1_rot;
uniform mat4 bone_1_trans;

void main(void)
{
	// transformera resultatet med ModelView- och Projection-matriserna
	mat4 Mbone1_prime = bone_0_trans*bone_0_rot; 
	mat4 Mbone2_prime = bone_1_trans*bone_1_rot;

	mat4 MInvbone1 = inverse(bone_0_trans);
	mat4 MInvbone2 = inverse(bone_1_trans);

	mat4 M1 = Mbone1_prime * MInvbone1;
	mat4 M2 = Mbone1_prime * Mbone2_prime * MInvbone2 * MInvbone1;
	vec3 g_vertsResRow = vec3(in_TexCoord.x * M1 * vec4(in_Position, 1));
	vec3 g_vertsResCol = vec3(in_TexCoord.y * M2 * vec4(in_Position, 1));
	gl_Position = matrix * vec4(g_vertsResRow + g_vertsResCol, 1.0);

	// s�tt r�d+gr�n f�rgkanal till vertex Weights
	vec4 color = vec4(in_TexCoord.x, in_TexCoord.y, 0.0, 1.0);

	// L�gg p� en enkel ljuss�ttning p� vertexarna 	
	float intensity = dot(in_Normal, lightDir);
	color.xyz *= intensity;

	g_color = color;
}

