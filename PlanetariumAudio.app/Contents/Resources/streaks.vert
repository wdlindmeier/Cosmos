#version 150

uniform mat4        uModelViewProjection;
uniform sampler2D   uTex0;

in vec4         vPosition;
in vec2         vTexCoord0;
in vec4         vColor;
out lowp vec4	Color;

void main( void )
{
	gl_Position	= uModelViewProjection * vPosition;
	Color = vColor;//vec4(1.0, 1.0, 1.0, 1.0);
}
