#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;

uniform mat4 projection;
uniform mat4 view;

out vec3 color;

void main()
{
  color = col;
  gl_Position = projection * view * vec4(pos, 1.0f); 
}