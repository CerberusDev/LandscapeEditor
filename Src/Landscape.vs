#version 330

layout (location = 0) in vec2 Position;

out vec2 UV;
out vec3 Normal;

uniform float LandscapeSizeX;
uniform float LandscapeVertexOffset;
uniform mat4 gWorld;
uniform samplerBuffer TBOSampler;

uniform float TestOffsetX;
uniform float TestOffsetY;

uniform vec2 ClipmapPartOffset;

float VertexHeight;
int VertexIndex;
int VertexIndex1;
int VertexIndex2;
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
	//VertexHeight = texelFetch(TBOSampler, VertexIndex).w * 0.00000001 + VertexIndex / 1000.0 + 435.0;


	if (((Position.x == 7.875 - 0.25 * ClipmapPartOffset.x) || (Position.x == -8.125 - 0.25 * ClipmapPartOffset.x)) && mod(Position.y + 0.125 - 0.25 * ClipmapPartOffset.y, 0.5) != 0) 
	{
		ClippedY1 = mod(ModifiedY + 1, LandscapeSizeX);
		ClippedY2 = mod(ModifiedY - 1, LandscapeSizeX);
	
		VertexIndex1 = int(ClippedY1 * LandscapeSizeX + ClippedX);
		VertexIndex2 = int(ClippedY2 * LandscapeSizeX + ClippedX);

		VertexHeight = (texelFetch(TBOSampler, VertexIndex1).w + texelFetch(TBOSampler, VertexIndex2).w) / 2.0; 
		Normal = normalize((texelFetch(TBOSampler, VertexIndex1).xyz + texelFetch(TBOSampler, VertexIndex2).xyz) / 2.0);
	}
	else if (((Position.y == 7.875 - 0.25 * ClipmapPartOffset.y) || (Position.y == -8.125 - 0.25 * ClipmapPartOffset.y)) && mod(Position.x + 0.125 - 0.25 * ClipmapPartOffset.x, 0.5) != 0)
	{
		ClippedX1 = mod(ModifiedX + 1, LandscapeSizeX);
		ClippedX2 = mod(ModifiedX - 1, LandscapeSizeX);

		VertexIndex1 = int(ClippedY * LandscapeSizeX + ClippedX1);
		VertexIndex2 = int(ClippedY * LandscapeSizeX + ClippedX2);

		VertexHeight = (texelFetch(TBOSampler, VertexIndex1).w + texelFetch(TBOSampler, VertexIndex2).w) / 2.0; 
		Normal = normalize((texelFetch(TBOSampler, VertexIndex1).xyz + texelFetch(TBOSampler, VertexIndex2).xyz) / 2.0);
	}
	else
	{
		Normal = texelFetch(TBOSampler, VertexIndex).xyz;
	}

    gl_Position = gWorld * vec4(Position.x + 0.125 - VertexOffsetX * LandscapeVertexOffset, VertexHeight, Position.y + 0.125 - VertexOffsetY * LandscapeVertexOffset, 1.0);
	UV = vec2(Position.x + 0.125 + 0.25 * ClipmapPartOffset.x, Position.y + 0.125 + 0.25 * ClipmapPartOffset.y);
}