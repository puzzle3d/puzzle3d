#version 140

in vec3 in_Vertex;
in vec3 in_Color;

out vec3 color;

uniform mat4 projection;
uniform mat4 modelview;

void main()
{
    gl_Position = projection * modelview * vec4(in_Vertex, 1.0);
    color = in_Color;
}
