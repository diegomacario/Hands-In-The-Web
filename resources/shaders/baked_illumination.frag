in vec2 uv;

uniform sampler2D diffuseTex;

out vec4 fragColor;

void main()
{
   fragColor = texture(diffuseTex, uv);
}
