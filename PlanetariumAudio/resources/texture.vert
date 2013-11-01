#version 150

uniform mat4	uModelViewProjection;

in vec4         vPosition;
in vec2         vTexCoord0;
out highp vec2	TexCoord;
in vec4         vColor;
out lowp vec4	Color;

void main( void )
{
	gl_Position	= uModelViewProjection * vPosition;
	Color = vColor;
	TexCoord	= vTexCoord0;
}
