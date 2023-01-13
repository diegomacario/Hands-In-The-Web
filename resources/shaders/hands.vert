in vec3 position;
//in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//out vec3 fragPos;
//out vec3 norm;

void main()
{
   gl_Position = projection * view * model * vec4(position, 1.0f);

   //fragPos = vec3(model * vec4(position, 1.0f));
   //norm    = normalize(vec3(model * vec4(normal, 0.0f)));
}
