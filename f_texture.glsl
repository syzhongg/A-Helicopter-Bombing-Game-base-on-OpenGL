#version 330 core

in vec4 color;
in vec2 texCoord;

in vec3 N;
in vec3 V;

out vec4 fColor;
out vec4 fNormal;

uniform sampler2D texture;
uniform int is_shadow;
uniform vec3 lightPos;

void main()
{
	if (is_shadow!=0){
		fColor = vec4(0, 0, 0, 1);
		//fNormal = vec4(0, 1, 0, 1);
	}
	else{
	//  ������ά����Ĳ�������
	vec3 ambiColor = vec3(0.2, 0.2, 0.2);
	vec3 diffColor = vec3(0.5, 0.5, 0.5);
	vec3 specColor = vec3(0.3, 0.3, 0.3);
	//  ����N��L��V��R�ĸ���������һ��
	vec3 N_norm = normalize(N);
	vec3 L_norm = normalize(lightPos-V);
	vec3 V_norm = normalize(-V);
	vec3 R_norm = reflect(-L_norm,N_norm);
	//  ����������ϵ���;��淴��ϵ��
	float lambertian = clamp(dot(L_norm, N_norm), 0.0, 1.0);
	float specular = clamp(dot(R_norm, V_norm), 0.0, 1.0);
	float shininess = 10.0;
	//  ��������ÿ��ƬԪ�������ɫ
	fColor = vec4(ambiColor + diffColor * lambertian + specColor * pow(specular, shininess), 1.0);
    fColor = fColor * texture2D( texture, texCoord );
    fNormal = vec4(N,0);
	}
}