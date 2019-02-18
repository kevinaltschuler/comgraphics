#version 330

layout(location=0) in vec4 vPosition;
layout(location=1) in vec4 vNormal;
layout(location=2) in vec2 vTexCoord;
uniform vec3 vColor;
uniform mat4 projection;
uniform mat4 modelview;
out vec4 outColor;

void main()
{
    gl_Position = projection * modelview * vPosition;
    outColor = vec4(vColor,1);
}
