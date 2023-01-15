in vec3 fragPos;
in vec3 norm;

struct PointLight
{
   vec3  worldPos;
   vec3  color;
   float linearAtt;
};

#define MAX_NUMBER_OF_POINT_LIGHTS 4
uniform PointLight pointLights[MAX_NUMBER_OF_POINT_LIGHTS];
uniform int numPointLightsInScene;

uniform vec3 cameraPos;

uniform vec3 diffuseColor;

out vec4 fragColor;

vec3 calculateContributionOfPointLight(PointLight light, vec3 viewDir);

void main()
{
   vec3 viewDir = normalize(cameraPos - fragPos);

   vec3 color = vec3(0.0);
   for(int i = 0; i < numPointLightsInScene; i++)
   {
      color += calculateContributionOfPointLight(pointLights[i], viewDir);
   }

   fragColor = vec4(color, 1.0);
}

vec3 calculateContributionOfPointLight(PointLight light, vec3 viewDir)
{
   // Ambient
   vec3 ambient = 0.05 * diffuseColor;

   // Diffuse
   vec3 lightDir = normalize(light.worldPos - fragPos);
   float diff    = max(dot(lightDir, norm), 0.0);
   vec3 diffuse  = diff * diffuseColor;

   // specular
   vec3 reflectDir = reflect(-lightDir, norm);
   vec3 halfwayDir = normalize(lightDir + viewDir);
   float spec      = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
   vec3 specular   = vec3(0.3) * spec;

   return (ambient + diffuse + specular);
}
