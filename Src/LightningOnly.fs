#version 330

out vec4 FragColor;

in vec2 UV;
in vec3 Normal;

uniform sampler2D BrushTextureSampler;
uniform vec2 BrushPosition;
uniform float BrushScale;
uniform float DiffuseStrength;
uniform float AmbientStrength;

float DiffuseFactor;
vec4 WhiteColor;
vec4 TextureColor;
vec4 BrushColor;
vec4 LandscapeColor;
vec4 BlendedColor;
vec3 LightDirection;

void main()
{
	LightDirection = normalize(vec3(-0.5, 1.0, -0.5));
	WhiteColor = vec4(1.0, 1.0, 1.0, 1.0);
	
	BrushColor = texture2D(BrushTextureSampler, (UV - BrushPosition) / BrushScale).bgra;

	DiffuseFactor = clamp(dot(normalize(Normal) * DiffuseStrength, LightDirection), 0.0, 1.0);

	LandscapeColor = AmbientStrength * WhiteColor + (1.0 - AmbientStrength) * WhiteColor * DiffuseFactor;

	BlendedColor = vec4((1 - BrushColor.a) * LandscapeColor.rgb + BrushColor.a * BrushColor.rgb, 1.0);

	FragColor = clamp(BlendedColor, vec4(0.0, 0.0, 0.0, 0.0), vec4(1.0, 1.0, 1.0, 1.0));
}