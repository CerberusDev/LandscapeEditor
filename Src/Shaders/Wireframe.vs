#version 330

layout (location = 0) in vec2 Position;

out vec2 UV;

uniform float LandscapeSizeX;
uniform float LandscapeVertexOffset;
uniform mat4 gWorld;
uniform samplerBuffer TBOSampler;

uniform float TestOffsetX;
uniform float TestOffsetY;

uniform vec2 ClipmapPartOffset;

float VertexHeight;
int VertexIndex;
float ConvertedX;
float ConvertedY;
float ModifiedX;
float ModifiedY;
float ClippedX;
float ClippedY;
float ClippedX1;
float ClippedY1;
float ClippedX2;
float ClippedY2;

int TestOffsetIntX;
float VertexOffsetX;

int TestOffsetIntY;
float VertexOffsetY;

void main()
{
	TestOffsetIntX = int(floor(TestOffsetX));
	VertexOffsetX = mod(TestOffsetX, 1.0);

	TestOffsetIntY = int(floor(TestOffsetY));
	VertexOffsetY = mod(TestOffsetY, 1.0);

	ConvertedX = Position.x / LandscapeVertexOffset + LandscapeSizeX / 2.0;
	ConvertedY = Position.y / LandscapeVertexOffset + LandscapeSizeX / 2.0;

	ModifiedX = ConvertedX + TestOffsetIntX;
	ModifiedY = ConvertedY + TestOffsetIntY;

	ClippedX = mod(ModifiedX, LandscapeSizeX);
	ClippedY = mod(ModifiedY, LandscapeSizeX);

	VertexIndex = int(ClippedX + LandscapeSizeX * ClippedY);
	VertexHeight = texelFetch(TBOSampler, VertexIndex).w;
	//VertexHeight = texelFetch(TBOSampler, VertexIndex).w * 0.00000001 + VertexIndex / 1000.0 - 5.0;

    gl_Position = gWorld * vec4(Position.x - VertexOffsetX * LandscapeVertexOffset, VertexHeight, Position.y - VertexOffsetY * LandscapeVertexOffset, 1.0);
	UV = vec2(Position.x, Position.y) + ClipmapPartOffset.x * 0.000001 ;
}