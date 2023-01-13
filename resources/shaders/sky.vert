in vec3 position;

uniform mat4 projectionView;

out vec3 TexCoords;

void main()
{
   TexCoords = position;
   gl_Position = (projectionView * vec4(position, 1.0)).xyww;
}
