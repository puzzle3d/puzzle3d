#version 330

in vec2 coord;
in vec4 position;
in vec3 norm;

uniform mat4 lookAt, projMatrix, pivot, pivotObjet, translObjet, scaleMat;
uniform vec3 lightPos;
 
out vec2 fragcoord;
out vec3 normals;
out vec3 lightDir;

void main()
{
	fragcoord=coord;
	vec4 normals4 = pivot * pivotObjet * vec4(norm,1.0);
	normals = normals4.xyz / normals4.w;
	vec4 vPosition4 = projMatrix * lookAt * pivot * pivotObjet * translObjet* scaleMat * position;
	vec3 vPosition3 = vPosition4.xyz / vPosition4.w;
	lightDir = normalize(lightPos - vPosition3);
	gl_Position = vPosition4;
}
