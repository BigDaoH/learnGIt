#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
uniform sampler2D texture1;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    float specularStrength = 0.5;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 light = lightColor * (30.0-length(lightPos - FragPos))/30.0;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * light;

    float diff = max(dot(norm,lightDir),0.0f);
    vec3 diffuse = diff * light;

    FragColor = texture(texture1,TexCoord) * vec4(diffuse+specular, 1.0);
}
