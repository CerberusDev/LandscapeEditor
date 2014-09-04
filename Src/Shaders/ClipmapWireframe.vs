#version 330

layout (location = 0) in vec2 Position;

out vec2 UV;

uniform float ClipmapWidth;
uniform float LandscapeVertexOffset;
uniform mat4 gWorld;
uniform samplerBuffer TBOSampler;

uniform float TestOffsetX;
uniform float TestOffsetY;

uniform float ClipmapScale;
uniform vec2 ClipmapPartOffset;

highp int VertexIndex;
float VertexHeight;
float BaseX;
float BaseY;
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

int iCameraOffsetX;
int iCameraOffsetY;

float VertexOffsetX;
float VertexOffsetY;

void main()
{
	iCameraOffsetX = int(floor(TestOffsetX / ClipmapScale));
	iCameraOffsetY = int(floor(TestOffsetY / ClipmapScale));

	VertexOffsetX = mod(TestOffsetX, ClipmapScale);
	VertexOffsetY = mod(TestOffsetY, ClipmapScale);

	BaseX = Position.x * ClipmapScale;
	BaseY = Position.y * ClipmapScale;

	ConvertedX = Position.x / LandscapeVertexOffset + (ClipmapWidth - 1) / 2.0;				
	ConvertedY = Position.y / LandscapeVertexOffset + (ClipmapWidth - 1) / 2.0;

	ModifiedX = ConvertedX + iCameraOffsetX;
	ModifiedY = ConvertedY + iCameraOffsetY;

	ClippedY = mod(ModifiedY, ClipmapWidth);
	ClippedX = mod(ModifiedX, ClipmapWidth);

	VertexIndex = int(ClippedY * ClipmapWidth + ClippedX);

	VertexHeight = texelFetch(TBOSampler, VertexIndex).w;
		
    gl_Position = gWorld * vec4(BaseX - VertexOffsetX * LandscapeVertexOffset, VertexHeight + ClipmapPartOffset.x * 0.0000001, BaseY - VertexOffsetY * LandscapeVertexOffset, 1.0);
	UV = vec2(Position.x, Position.y);
}