#version 330 core

in  vec3 vPosition;
in  vec3 vColor;
in  vec3 vNormal;
in  vec2 vTexCoord;

out vec4 color;
out vec2 texCoord;
out vec3 N;
out vec3 V;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform int is_shadow;

void main()
{
	color = vec4(vColor, 0);
    texCoord = vTexCoord;
    vec4 v1 = modelMatrix * vec4(vPosition, 1);
	if (is_shadow != 0) {
        v1 = vec4(v1.xyz / v1.w, 1);
    }
    gl_Position = projMatrix * viewMatrix * v1;
	vec4 vertPos_cameraspace = viewMatrix * modelMatrix * vec4(vPosition, 1);
	N = (modelMatrix * vec4(vNormal, 0.0)).xyz;
	// 对顶点坐标做透视投影
	// V = v1.xyz / v1.w;
	V = vertPos_cameraspace.xyz / vertPos_cameraspace.w;
}