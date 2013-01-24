#version 330

uniform vec3 color;

uniform vec3 ambient;
uniform vec3 diffuse;

in vec3 lightDir;
in vec3 normals;
in vec2 fragcoord;

out vec4 out_Color;

void main() {
	float dif = max(0.0,dot(normals, lightDir));
	vec3 light = dif*diffuse+ambient;
	
	float th=0.05;
	vec3 colorEdge=vec3(1,1,1);
	if (fragcoord.x<th || fragcoord.x>1-th || fragcoord.y<th || fragcoord.y>1-th)
	{
		float val=th;
		if (fragcoord.x<th && val > fragcoord.x) val=fragcoord.x;
		else if (fragcoord.x>1-th && val > 1-fragcoord.x) val=1-fragcoord.x;
		if (fragcoord.y<th && val > fragcoord.y) val=fragcoord.y;
		else if (fragcoord.y>1-th && val > 1-fragcoord.y) val=1-fragcoord.y;
		
		//out_Color = vec4((1-val/th)*(colorEdge.x-light.x)+light.x,(1-val/th)*(colorEdge.y-light.y)+light.y,(1-val/th)*(colorEdge.z-light.z)+light.z, 1.0);
		out_Color = vec4(((1-val/th)*(colorEdge.x-color.x)+color.x)*light.x,((1-val/th)*(colorEdge.y-color.y)+color.y)*light.y,((1-val/th)*(colorEdge.z-color.z)+color.z)*light.z, 1.0);
		//out_Color = vec4(((1-val/th)*(colorEdge.x-color.x)+color.x,(1-val/th)*(colorEdge.y-color.y)+color.y,(1-val/th)*(colorEdge.z-color.z)+color.z)*light, 1.0);
	}
	//else out_Color = vec4(color, 1.0);
	//else out_Color = vec4(light, 1.0);
	else out_Color = vec4(color*light, 1.0);
//	out_Color = vec4(1.0,1.0,1.0, 1.0);
}