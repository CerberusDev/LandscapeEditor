#version 330

layout (location = 0) in vec2 Position;

out vec2 UV;
out vec2 UVBrush;
out vec3 Normal;

uniform int ClipmapWidth;
uniform float LandscapeVertexOffset;
uniform mat4 gWorld;
uniform samplerBuffer TBOSampler;
uniform float CameraOffsetX;
uniform float CameraOffsetY;
uniform int ClipmapScale;


int imod(in int x, in int y)
{
	if (x < 0)
	{
		int d = abs(x) / ClipmapWidth;
		x += (d+1) * ClipmapWidth;
	}

	return x % y;	
}

int CalculateTBOIndex(const in int PosX, const in int PosY, const in int CameraOffsetX, const in int CameraOffsetY)
{
	int ConvertedX = PosX + (ClipmapWidth - 3) / 2;				
	int ConvertedY = PosY + (ClipmapWidth - 3) / 2;

	int ModifiedX = ConvertedX + CameraOffsetX;
	int ModifiedY = ConvertedY + CameraOffsetY;

	int ClippedY = imod(int(ModifiedY), ClipmapWidth);
	int ClippedX = imod(int(ModifiedX), ClipmapWidth);

	return int(ClippedY * ClipmapWidth + ClippedX);
}

void main()
{
	int iCameraOffsetX = int(floor(CameraOffsetX / ClipmapScale));
	int iCameraOffsetY = int(floor(CameraOffsetY / ClipmapScale));

	float VertexOffsetX = mod(CameraOffsetX, ClipmapScale);
	float VertexOffsetY = mod(CameraOffsetY, ClipmapScale);

	float BaseX = Position.x * ClipmapScale;
	float BaseY = Position.y * ClipmapScale;

	int PosX = int(Position.x);
	int PosY = int(Position.y);

	int TBOIndex = CalculateTBOIndex(PosX, PosY, iCameraOffsetX, iCameraOffsetY);
	float VertexHeight = texelFetch(TBOSampler, TBOIndex).r;
		
    gl_Position = gWorld * vec4((BaseX - VertexOffsetX) * LandscapeVertexOffset, VertexHeight, (BaseY - VertexOffsetY) * LandscapeVertexOffset, 1.0);

	UVBrush = vec2(BaseY - VertexOffsetY, BaseX - VertexOffsetX);
	UV = vec2(BaseY, BaseX);

	TBOIndex = CalculateTBOIndex(PosX + 1, PosY, iCameraOffsetX, iCameraOffsetY);
	float VH1 = texelFetch(TBOSampler, TBOIndex).r;

	TBOIndex = CalculateTBOIndex(PosX - 1, PosY, iCameraOffsetX, iCameraOffsetY);
	float VH2 = texelFetch(TBOSampler, TBOIndex).r;

	TBOIndex = CalculateTBOIndex(PosX, PosY + 1, iCameraOffsetX, iCameraOffsetY);
	float VH3 = texelFetch(TBOSampler, TBOIndex).r;

	TBOIndex = CalculateTBOIndex(PosX, PosY - 1, iCameraOffsetX, iCameraOffsetY);
	float VH4 = texelFetch(TBOSampler, TBOIndex).r;

	vec3 V1 = normalize(vec3(0.0, VH1 - VH2, 2.0 * LandscapeVertexOffset * ClipmapScale));
	vec3 V2 = normalize(vec3(2.0 * LandscapeVertexOffset * ClipmapScale, VH3 - VH4, 0.0));

	Normal = normalize(cross(V1, V2));
}