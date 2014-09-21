#version 330

out vec4 FragColor;

in vec2 UV;
in vec3 Normal;

uniform sampler2D TextureSampler;
uniform sampler2D BrushTextureSampler;

uniform vec2 BrushPosition;
uniform float BrushScale;
uniform vec3 WireframeColor;
uniform vec3 BrushColor;

vec4 BrushTextureData;
vec4 BlendedColor;
float DiffuseFactor;
float AmbientLightningStrength;
float TotalLightFactor;
vec3 LightDirection;


float ShitVariable;

void main()
{
	LightDirection = normalize(vec3(0.0, 1.0, 0.0));
	AmbientLightningStrength = 0.2;

	BrushTextureData = texture2D(BrushTextureSampler, (UV - BrushPosition) / BrushScale).bgra;

	BlendedColor = texture2D(TextureSampler, UV).bgra + vec4((WireframeColor + BrushTextureData.a - BrushColor) * 0.000001, 1.0);

	DiffuseFactor = clamp(pow(dot(normalize(Normal), LightDirection),4.0), 0.0, 1.0);
	TotalLightFactor = AmbientLightningStrength + (1.0 - AmbientLightningStrength) * DiffuseFactor;

	FragColor = BlendedColor * TotalLightFactor;
	

	//ShitVariable = 0.00000001 * ((texture2D(BrushTextureSampler, (UV - BrushPosition) / (BrushScale + BrushColor.r + WireframeColor.r)).bgra) + texture2D(TextureSampler, UV).bgra);
	//FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f) * TotalLightFactor + vec4(1.0f) * ShitVariable;
}