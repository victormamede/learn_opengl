#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;
in vec3 FragPos;  
in vec3 Normal;

uniform vec3 viewPos;

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;  
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
}; 

struct SpotLight {
    vec3  position;
    vec3  direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};    


struct Material {
    vec3 ambient;

    vec4 diffuse;
    sampler2D diffuseMap;

    float shininess;
    vec3 specular;
    sampler2D specularMap;

    vec3 emission;
    sampler2D emissionMap;
};

uniform Material material;

#define MAX_DIRECTIONAL_LIGHTS 4
uniform int directionalLightsCount;
uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];

#define MAX_SPOT_LIGHTS 16
uniform int spotLightsCount;
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

#define MAX_POINT_LIGHTS 16
uniform int pointLightsCount;
uniform PointLight pointLights[MAX_POINT_LIGHTS];


vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir);

void main()
{
    vec3 outColor = vec3(0.0);
    vec4 textureDiffuse = texture(material.diffuseMap, TexCoord);
    if(textureDiffuse.a <= 0.1) {
        discard;
    }

    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    for(int i = 0; i < directionalLightsCount; i++)
         result += CalcDirectionalLight(directionalLights[i], norm, viewDir);
    for(int i = 0; i < pointLightsCount; i++)
        result += CalcPointLight(pointLights[i], norm, viewDir);
    for(int i = 0; i < spotLightsCount; i++)
        result += CalcSpotLight(spotLights[i], norm, viewDir);

    vec3 emission = material.emission * vec3(texture(material.emissionMap, TexCoord));
    FragColor = vec4(result + emission, textureDiffuse.a);
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    vec3 albedo = material.diffuse.rgb * texture(material.diffuseMap, TexCoord).rgb;
    vec3 specularFactor = material.specular * vec3(texture(material.specularMap, TexCoord));

    // ambient shading
    vec3 ambient = light.ambient * albedo;

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * albedo;

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128.0);
    vec3 specular = light.specular * spec * specularFactor;

    // combine results
    return (ambient + diffuse + specular);
}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 albedo = material.diffuse.rgb * texture(material.diffuseMap, TexCoord).rgb;
    vec3 specularFactor = material.specular * vec3(texture(material.specularMap, TexCoord));

    // attenuation
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));   

    // ambient shading
    vec3 ambient = light.ambient * albedo;

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * albedo;

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128.0);
    vec3 specular = light.specular * spec * specularFactor;

    // combine results
    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - FragPos);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    if(theta < light.outerCutOff) {
        return vec3(0.0);
    }

    vec3 albedo = material.diffuse.rgb * texture(material.diffuseMap, TexCoord).rgb;
    vec3 specularFactor = material.specular * vec3(texture(material.specularMap, TexCoord));

    // attenuation
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));   

    // ambient shading
    vec3 ambient = light.ambient * albedo;

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * albedo;

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128.0);
    vec3 specular = light.specular * spec * specularFactor;

    // combine results
    return (ambient + diffuse + specular) * intensity * attenuation;
}