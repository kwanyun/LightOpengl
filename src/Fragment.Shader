#version 460 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;

    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

void main()
{
    //emmision
    vec3 emission = vec3(1, 1, 1) * texture(material.emission, TexCoords).rgb;
    emission = vec3(texture(material.specular, TexCoords)) == vec3(0,0,0) ? emission : vec3(0, 0, 0);
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    //it will not make object texture visible in backward
    //vec3 ambient = light.ambient * vec3(0.1, 0.1, 0.1)*2;

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * (vec3(texture(material.specular, TexCoords)));

    vec3 result = ambient + diffuse + specular+ emission;
    FragColor = vec4(result, 1.0);
}