#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 vertex_uv0;

out vec2 uv0;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	uv0 = vertex_uv0;
}