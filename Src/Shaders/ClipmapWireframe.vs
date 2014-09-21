#version 330

layout (location = 0) in vec2 Position;

out vec2 UV;

uniform int ClipmapWidth;
uniform float LandscapeVertexOffset;
uniform mat4 gWorld;
uniform samplerBuffer TBOSampler;
uniform float TestOffsetX;
uniform float TestOffsetY;
uniform float ClipmapScale;


int CalculateTBOIndex(const in int PosX, const in int PosY, const in int CameraOffsetX, const in int CameraOffsetY)
{
	int ConvertedX = PosX + (ClipmapWidth - 3) / 2;				
	int ConvertedY = PosY + (ClipmapWidth - 3) / 2;

	int ModifiedX = ConvertedX + CameraOffsetX;
	int ModifiedY = ConvertedY + CameraOffsetY;

	int ClippedY = ModifiedY % ClipmapWidth;
	int ClippedX = ModifiedX % ClipmapWidth;

	return int(ClippedY * ClipmapWidth + ClippedX);
}

void main()
{
	float VertexOffsetX = mod(TestOffsetX, ClipmapScale);
	float VertexOffsetY = mod(TestOffsetY, ClipmapScale);

	float BaseX = Position.x * ClipmapScale;
	float BaseY = Position.y * ClipmapScale;

	int iCameraOffsetX = int(floor(TestOffsetX / ClipmapScale));
	int iCameraOffsetY = int(floor(TestOffsetY / ClipmapScale));

	int PosX = int(Position.x);
	int PosY = int(Position.y);

	int TBOIndex = CalculateTBOIndex(PosX, PosY, iCameraOffsetX, iCameraOffsetY);
	float VertexHeight = texelFetch(TBOSampler, TBOIndex).w;
		
    gl_Position = gWorld * vec4((BaseX - VertexOffsetX) * LandscapeVertexOffset, VertexHeight, (BaseY - VertexOffsetY) * LandscapeVertexOffset, 1.0);
	UV = vec2(Position.x, Position.y);
}