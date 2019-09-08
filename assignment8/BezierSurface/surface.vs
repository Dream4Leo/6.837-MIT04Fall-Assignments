#version 450 core

layout (location = 0) in vec3 ws;

out vec3 tcs_ws;

void main()
{
	tcs_ws = ws;
}