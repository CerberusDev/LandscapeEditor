#version 330

layout (location = 0) in vec2 Position;

out vec2 UV;
out vec3 Normal;

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
	int iCameraOffsetX = int(floor(TestOffsetX / ClipmapScale));
	int iCameraOffsetY = int(floor(TestOffsetY / ClipmapScale));

	float VertexOffsetX = mod(TestOffsetX, ClipmapScale);
	float VertexOffsetY = mod(TestOffsetY, ClipmapScale);

	float BaseX = Position.x * ClipmapScale;
	float BaseY = Position.y * ClipmapScale;

	int TBOIndex = CalculateTBOIndex(Position.x, Position.y, iCameraOffsetX, iCameraOffsetY);
	float VertexHeight = texelFetch(TBOSampler, TBOIndex).w;
		
    gl_Position = gWorld * vec4((BaseX - VertexOffsetX) * LandscapeVertexOffset, VertexHeight + ClipmapPartOffset.x * 0.0000001, (BaseY - VertexOffsetY) * LandscapeVertexOffset, 1.0);
	UV = vec2(BaseY, BaseX);


	TBOIndex = CalculateTBOIndex(Position.x + 1.0f, Position.y, iCameraOffsetX, iCameraOffsetY);
	float VH1 = texelFetch(TBOSampler, TBOIndex).w;

	TBOIndex = CalculateTBOIndex(Position.x - 1.0f, Position.y, iCameraOffsetX, iCameraOffsetY);
	float VH2 = texelFetch(TBOSampler, TBOIndex).w;

	TBOIndex = CalculateTBOIndex(Position.x, Position.y + 1.0f, iCameraOffsetX, iCameraOffsetY);
	float VH3 = texelFetch(TBOSampler, TBOIndex).w;

	TBOIndex = CalculateTBOIndex(Position.x, Position.y - 1.0f, iCameraOffsetX, iCameraOffsetY);
	float VH4 = texelFetch(TBOSampler, TBOIndex).w;

	vec3 V1 = normalize(vec3(0.0, VH1 - VH2, 2.0 * LandscapeVertexOffset * ClipmapScale));
	vec3 V2 = normalize(vec3(2.0 * LandscapeVertexOffset * ClipmapScale, VH3 - VH4, 0.0));

	Normal = normalize(cross(V1, V2));
}