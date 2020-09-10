#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit;
out vec4 out_Color;

void main(void)
{
    float offset = 1.0f / 32;
    vec4 c = texture(texUnit, outTexCoord);
    vec4 l = texture(texUnit, outTexCoord + vec2(offset, 0.0));
    vec4 r = texture(texUnit, outTexCoord + vec2(-offset, 0.0));
    vec4 d = texture(texUnit, outTexCoord + vec2(0.0, offset));
    vec4 u = texture(texUnit, outTexCoord + vec2(0.0, -offset));
    out_Color = ((4 * c) + l + r + d + u) / 8;
}
