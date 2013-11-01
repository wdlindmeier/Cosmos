#version 150

out vec4            oColor;
in vec4             Color;
uniform sampler2D   uTex0;
in vec2             TexCoord;

void main(void)
{
    oColor = texture( uTex0, TexCoord ) * Color;
}