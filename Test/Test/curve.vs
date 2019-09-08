#version 330 core

layout (location = 0) in float t;

uniform mat4 bezier;
uniform vec2 v0;
uniform vec2 v1;
uniform vec2 v2;
uniform vec2 v3;

void main()
{
	vec2 p = (1-t)*(1-t)*(1-t)*v0 + 3*t*(1-t)*(1-t)*v1 + 3*t*t*(1-t)*v2 + t*t*t*v3;
	gl_Position = vec4(p, 0.0, 1.0);
}