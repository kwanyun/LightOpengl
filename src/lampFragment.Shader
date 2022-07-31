#version 460 core

in vec3 fColor;

out vec4 FragColor;

void main()
{
    
    FragColor = vec4(fColor,1.0); // 4 개의 모든 벡터의 값을 1.0으로 설정합니다.
}