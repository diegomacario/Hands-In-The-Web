in vec3 position;
in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

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

out vec3 color;

vec3 calculateContributionOfPointLight(PointLight light, vec3 fragPos, vec3 norm, vec3 viewDir);

void main()
{
   gl_Position  = projection * view * model * vec4(position, 1.0f);

   vec3 fragPos = vec3(model * vec4(position, 1.0f));
   // TODO: To support non-uniform scaling we will need to change the way we transform the normals
   vec3 norm    = normalize(vec3(model * vec4(normal, 0.0f)));
   vec3 viewDir = normalize(cameraPos - fragPos);

   vec3 shadedColor = vec3(0.0);
   for(int i = 0; i < numPointLightsInScene; i++)
   {
      shadedColor += calculateContributionOfPointLight(pointLights[i], fragPos, norm, viewDir);
   }

   color = shadedColor;
}

vec3 calculateContributionOfPointLight(PointLight light, vec3 fragPos, vec3 norm, vec3 viewDir)
{
   // Attenuation
   float distance    = length(light.worldPos - fragPos);
   float attenuation = 1.0 / (1.0 + (light.linearAtt * distance));

   // Diffuse
   vec3  lightDir    = normalize(light.worldPos - fragPos);
   vec3  diff        = max(dot(lightDir, norm), 0.0) * light.color * attenuation;
   vec3  diffuse     = diff * diffuseColor;

   return diffuse;
}
