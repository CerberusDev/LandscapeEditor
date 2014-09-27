#version 330

out vec4 FragColor;

in vec2 UV;
in vec2 UVBrush;
in vec3 Normal;

uniform sampler2D TextureSampler;
uniform sampler2D BrushTextureSampler;

uniform vec2 BrushPosition;
uniform float BrushScale;

float AmbientLightningStrength;
vec3 LightDirection;


void main()
{
	LightDirection = normalize(vec3(0.0, 1.0, 0.0));
	AmbientLightningStrength = 0.2;

	vec4 BrushTextureData = texture2D(BrushTextureSampler, (UV - BrushPosition) / BrushScale).bgra;

	float DiffuseFactor = clamp(pow(dot(normalize(Normal), LightDirection),4.0), 0.0, 1.0);
	float TotalLightFactor = AmbientLightningStrength + (1.0 - AmbientLightningStrength) * DiffuseFactor;

	vec4 LandscapeColor = texture2D(TextureSampler, UV).bgra * TotalLightFactor;
	vec4 BrushColor = texture2D(BrushTextureSampler, (UVBrush.yx - BrushPosition) / BrushScale).bgra;
	vec4 BlendedColor = vec4((1 - BrushColor.a) * LandscapeColor.rgb + BrushColor.a * BrushColor.rgb, 1.0);

	FragColor = BlendedColor;
	

	//float ShitVariable = 0.00000001 * ((texture2D(BrushTextureSampler, (UV - BrushPosition) / (BrushScale + BrushColor.r + WireframeColor.r)).bgra) + texture2D(TextureSampler, UV).bgra);
	//FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f) * TotalLightFactor + vec4(1.0f) * ShitVariable;
}