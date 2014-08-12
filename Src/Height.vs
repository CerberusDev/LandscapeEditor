#version 330

layout (location = 0) in vec2 Position;

out vec2 UV;
out vec3 Normal;
out float VertexHeight;

uniform float LandscapeSizeX;
uniform float LandscapeVertexOffset;
uniform mat4 gWorld;
uniform samplerBuffer TBOSampler;

float Reductor;
float AdjacentVertexHeight1;
float AdjacentVertexHeight2;
float AdjacentVertexHeight3;
float AdjacentVertexHeight4;
float AdjacentVertexHeight5;
float AdjacentVertexHeight6;
float BrushRadius;
vec3 Component1;
vec3 Component2;
vec3 Component3;

void main()
{
	Reductor = 1.0 / LandscapeVertexOffset;

	VertexHeight = texelFetch(TBOSampler, int(Position.x * Reductor + LandscapeSizeX * Position.y * Reductor)).r;

	if (Position.x * Reductor + 1.0 < LandscapeSizeX)
		AdjacentVertexHeight1 = texelFetch(TBOSampler, int(Position.x * Reductor + 1.0 + LandscapeSizeX * Position.y * Reductor)).r;
	else
		AdjacentVertexHeight1 = VertexHeight;

	if (Position.x * Reductor - 1.0 >= 0.0)
		AdjacentVertexHeight2 = texelFetch(TBOSampler, int(Position.x * Reductor - 1.0 + LandscapeSizeX * Position.y * Reductor)).r;
	else
		AdjacentVertexHeight2 = VertexHeight;

	if (Position.y * Reductor + 1.0 < LandscapeSizeX)
		AdjacentVertexHeight3 = texelFetch(TBOSampler, int(Position.x * Reductor + LandscapeSizeX * (Position.y * Reductor + 1.0))).r;
	else
		AdjacentVertexHeight3 = VertexHeight;

	if (Position.y * Reductor - 1.0 >= 0.0)
		AdjacentVertexHeight4 = texelFetch(TBOSampler, int(Position.x * Reductor + LandscapeSizeX * (Position.y * Reductor - 1.0))).r;
	else
		AdjacentVertexHeight4 = VertexHeight;

	if (Position.x * Reductor - 1.0 >= 0.0 && Position.y * Reductor - 1.0 >= 0.0)
		AdjacentVertexHeight5 = texelFetch(TBOSampler, int(Position.x * Reductor - 1.0 + LandscapeSizeX * (Position.y * Reductor - 1.0))).r;
	else
		AdjacentVertexHeight5 = VertexHeight;

	if (Position.x * Reductor + 1.0 < LandscapeSizeX && Position.y * Reductor + 1.0 < LandscapeSizeX)
		AdjacentVertexHeight6 = texelFetch(TBOSampler, int(Position.x * Reductor + 1.0 + LandscapeSizeX * (Position.y * Reductor + 1.0))).r;
	else
		AdjacentVertexHeight6 = VertexHeight;

	Component1 = vec3(LandscapeVertexOffset * 2.0, (AdjacentVertexHeight1 - AdjacentVertexHeight2), 0.0);
	Component2 = vec3(0.0, (AdjacentVertexHeight3 - AdjacentVertexHeight4), LandscapeVertexOffset * 2.0);
	Component3 = vec3(LandscapeVertexOffset * 2.0, (AdjacentVertexHeight5 - AdjacentVertexHeight6) * 2.0, LandscapeVertexOffset * 2.0);

    Normal = vec3(-Component1.y, Component1.x, 0.0);
    Normal += vec3(0.0, Component2.z, -Component2.y);
    Normal += cross(Component3, vec3(Component3.z, 0.0, -Component3.x));

	Normal = normalize(Normal);

    gl_Position = gWorld * vec4(Position.x, VertexHeight, Position.y, 1.0);
	UV = vec2(Position.x, Position.y);
}