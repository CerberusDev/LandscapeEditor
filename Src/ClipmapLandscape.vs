#version 330

layout (location = 0) in vec2 Position;

out vec2 UV;
out vec3 Normal;

uniform float ClipmapSizeX;
uniform float ClipmapSizeY;
uniform float LandscapeVertexOffset;
uniform mat4 gWorld;
uniform samplerBuffer TBOSampler;

uniform float TestOffsetX;
uniform float TestOffsetY;

uniform float ClipmapScale;
uniform vec2 ClipmapPartOffset;

int VertexIndex;
int VertexIndex1;
int VertexIndex2;
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
	iCameraOffsetX  = int(floor(TestOffsetX / ClipmapScale));
	iCameraOffsetY = int(floor(TestOffsetY / ClipmapScale));

	VertexOffsetX = mod(TestOffsetX, ClipmapScale);
	VertexOffsetY = mod(TestOffsetY, ClipmapScale);

	BaseX = Position.x * ClipmapScale;
	BaseY = Position.y * ClipmapScale;

	ConvertedX = Position.x / LandscapeVertexOffset + (ClipmapSizeX - 1) / 2.0;				
	ConvertedY = Position.y / LandscapeVertexOffset + (ClipmapSizeY - 1) / 2.0;

	ModifiedX = ConvertedX + iCameraOffsetX;
	ModifiedY = ConvertedY + iCameraOffsetY;

	ClippedY = mod(ModifiedY, ClipmapSizeY);
	ClippedX = mod(ModifiedX, ClipmapSizeX);

	VertexIndex = int(ClippedY * ClipmapSizeX + ClippedX);

	//VertexHeight = (texelFetch(TBOSampler, VertexIndex).w + ClippedX - ClipmapSizeX + ClipmapSizeY - ClipmapPartOffset) * 0.000000001 + VertexIndex / 1000.0 + 435.0;
	VertexHeight = texelFetch(TBOSampler, VertexIndex).w;

	if (((Position.x == 8.0 - 0.25 * ClipmapPartOffset.x) || (Position.x == -8.0 - 0.25 * ClipmapPartOffset.x)) && mod(Position.y - 0.25 * ClipmapPartOffset.y, 0.5) != 0) 
	{
		ClippedY1 = mod(ModifiedY + 1, ClipmapSizeX);
		ClippedY2 = mod(ModifiedY - 1, ClipmapSizeX);
	
		VertexIndex1 = int(ClippedY1 * ClipmapSizeX + ClippedX);
		VertexIndex2 = int(ClippedY2 * ClipmapSizeX + ClippedX);

		VertexHeight = (texelFetch(TBOSampler, VertexIndex1).w + texelFetch(TBOSampler, VertexIndex2).w) / 2.0; 
		Normal = normalize((texelFetch(TBOSampler, VertexIndex1).xyz + texelFetch(TBOSampler, VertexIndex2).xyz) / 2.0);
	}
	else if (((Position.y == 8.0 - 0.25 * ClipmapPartOffset.y) || (Position.y == -8.0 - 0.25 * ClipmapPartOffset.y)) && mod(Position.x - 0.25 * ClipmapPartOffset.x, 0.5) != 0)
	{
		ClippedX1 = mod(ModifiedX + 1, ClipmapSizeX);
		ClippedX2 = mod(ModifiedX - 1, ClipmapSizeX);

		VertexIndex1 = int(ClippedY * ClipmapSizeX + ClippedX1);
		VertexIndex2 = int(ClippedY * ClipmapSizeX + ClippedX2);

		VertexHeight = (texelFetch(TBOSampler, VertexIndex1).w + texelFetch(TBOSampler, VertexIndex2).w) / 2.0; 
		Normal = normalize((texelFetch(TBOSampler, VertexIndex1).xyz + texelFetch(TBOSampler, VertexIndex2).xyz) / 2.0);
	}
	else
	{
		Normal = texelFetch(TBOSampler, VertexIndex).xyz;
	}

    gl_Position = gWorld * vec4(BaseX - VertexOffsetX * LandscapeVertexOffset, VertexHeight, BaseY - VertexOffsetY * LandscapeVertexOffset, 1.0);
	UV = vec2(Position.x, Position.y) * ClipmapScale;
}