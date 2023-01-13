in vec3  position;
in vec3  normal;
in vec2  texCoord;
in vec4  weights;
in ivec4 joints;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Number of skin matrices of each character:
//    Woman  - 43
//    Man    - 43
//    Llama  - 48
//    Fox    - 53
//    Husky  - 51
//    Stag   - 40
//    Wolf   - 53
//    George - 49
//    Leela  - 19
//    Mike   - 45
//    Stan   - 45
//    Zombie - 43
// We use the maximum number here so that this shader works for all the characters
#define MAX_NUMBER_OF_SKIN_MATRICES 53
uniform mat4 animated[MAX_NUMBER_OF_SKIN_MATRICES];

out vec3 fragPos;
out vec3 norm;
out vec2 uv;

void main()
{
   mat4 skin = (animated[joints.x] * weights.x) +
               (animated[joints.y] * weights.y) +
               (animated[joints.z] * weights.z) +
               (animated[joints.w] * weights.w);

   gl_Position = projection * view * model * skin * vec4(position, 1.0f);

   fragPos = vec3(model * skin * vec4(position, 1.0f));
   norm    = normalize(vec3(model * skin * vec4(normal, 0.0f)));
   uv      = texCoord;
}
