#version 440 core
layout (location = 0) in vec3 aPos;   // λ�ñ���������λ��ֵΪ 0 
layout (location = 1) in vec3 aColor; //  ��ɫ����������λ��ֵΪ 1

//uniform mat4 transform;
uniform mat4 trans;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 ourColor; // ��Ƭ����ɫ�����һ����ɫ

void main()
{
    //gl_Position = transform * vec4(aPos, 1.0);
	gl_Position =   projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;  // ��ourColor����Ϊ���ǴӶ�����������õ���������ɫ
}