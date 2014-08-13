#version 330

out vec4 FragColor;

in vec2 UV;
in vec3 Normal;
in float VertexHeight;

uniform sampler2D BrushTextureSampler;
uniform vec2 BrushPosition;
uniform float BrushScale;
uniform float DiffuseStrength;
uniform float AmbientStrength;
uniform float MaxHeight;

float DiffuseFactor;
float HeightRatioPlus;
float HeightRatioMinus;
vec4 BaseColor;
vec4 TextureColor;
vec4 BrushColor;
vec4 LandscapeColor;
vec4 BlendedColor;

vec3 LightDirection;

void main()
{
	LightDirection = normalize(vec3(0.5, 1.0, -0.5));
	HeightRatioPlus = clamp(VertexHeight / MaxHeight, 0.0, 1.0);
	HeightRatioMinus = clamp(VertexHeight / MaxHeight, -1.0, 0.0);
	BaseColor = vec4(HeightRatioPlus, 1.0 - HeightRatioPlus + HeightRatioMinus, -HeightRatioMinus, 1.0);
	
	BrushColor = texture2D(BrushTextureSampler, (UV - BrushPosition) / BrushScale).bgra;

	DiffuseFactor = clamp(dot(normalize(Normal) * DiffuseStrength, LightDirection), 0.0, 1.0);

	LandscapeColor = AmbientStrength * BaseColor + (1.0 - AmbientStrength) * BaseColor * DiffuseFactor;

	BlendedColor = vec4((1 - BrushColor.a) * LandscapeColor.rgb + BrushColor.a * BrushColor.rgb, 1.0);

	FragColor = clamp(BlendedColor, vec4(0.0, 0.0, 0.0, 0.0), vec4(1.0, 1.0, 1.0, 1.0));
}