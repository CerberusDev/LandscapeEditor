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


int CalculateTBOIndex(const in float PosX, const in float PosY, const in int CameraOffsetX, const in int CameraOffsetY)
{
	float ConvertedX = PosX + (ClipmapWidth - 3) / 2.0;				
	float ConvertedY = PosY + (ClipmapWidth - 3) / 2.0;

	float ModifiedX = ConvertedX + CameraOffsetX;
	float ModifiedY = ConvertedY + CameraOffsetY;

	float ClippedY = mod(ModifiedY, ClipmapWidth);
	float ClippedX = mod(ModifiedX, ClipmapWidth);

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

	int TBOIndex = CalculateTBOIndex(Position.x, Position.y, iCameraOffsetX, iCameraOffsetY);
	float VertexHeight = texelFetch(TBOSampler, TBOIndex).w;
		
    gl_Position = gWorld * vec4((BaseX - VertexOffsetX) * LandscapeVertexOffset, VertexHeight + ClipmapPartOffset.x * 0.0000001, (BaseY - VertexOffsetY) * LandscapeVertexOffset, 1.0);
	UV = vec2(Position.x, Position.y);
}