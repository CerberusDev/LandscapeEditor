#version 330

out vec4 FragColor;

in vec2 UV;

uniform sampler2D BrushTextureSampler;
uniform vec2 BrushPosition;
uniform float BrushScale;
uniform vec3 WireframeColor;
uniform vec3 BrushColor;

vec4 BrushTextureData;
vec4 BlendedColor;

void main()
{
	BrushTextureData = texture2D(BrushTextureSampler, (UV - BrushPosition) / BrushScale).bgra;

	//BlendedColor = vec4((1 - BrushTextureData.a) * WireframeColor + BrushTextureData.a * BrushColor, 1.0);
	BlendedColor = vec4(WireframeColor + BrushTextureData.a * BrushColor * 0.00001, 1.0);

	FragColor = clamp(BlendedColor, vec4(0.0, 0.0, 0.0, 0.0), vec4(1.0, 1.0, 1.0, 1.0));
}