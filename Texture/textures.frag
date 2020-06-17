#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
uniform sampler2D texture1;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 light = lightColor * (30.0-length(lightPos - FragPos))/30.0;

    float diff = max(dot(norm,lightDir),0.0f);
    vec3 diffuse = diff * light;

    FragColor = texture(texture1,TexCoord) * vec4(diffuse, 1.0);
}
