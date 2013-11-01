#version 150 core
#extension all : warn

out vec4            oColor;
in vec4             Color;

void main()
{
	oColor = Color;
}