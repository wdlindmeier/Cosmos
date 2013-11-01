#version 150 core
#extension all : warn

uniform mat4        uModelViewProjection;
uniform vec4        uColor;
in vec4             vPosition;
out lowp vec4       Color;

void main()
{
	gl_Position	= uModelViewProjection * vPosition;
    Color = uColor;
}