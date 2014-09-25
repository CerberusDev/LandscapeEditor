// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date: 23.03.2013
// --------------------------------------------------------------------

#include "Landscape.h"
#include "LandscapeEditor.h"

// --------------------------------------------------------------------
Landscape::Landscape(int ClipmapRimWidth, float VerticesInterval):
RestartIndex(0xFFFFFFFF), Offset(VerticesInterval), VBOSize(0), IBOSize(0)
{
	ClipmapVBOsData = new float*[VBO_MODES_AMOUNT];
	ClipmapIBOsData = new unsigned int*[IBO_MODES_AMOUNT];
	ClipmapVBOsWidth = new unsigned int[VBO_MODES_AMOUNT];

	VBOSize = new unsigned int[VBO_MODES_AMOUNT];
	IBOSize = new unsigned int[IBO_MODES_AMOUNT];

	ClipmapVBOsWidth[VBO_CLIPMAP] = ClipmapRimWidth * 4 + 4;
	TBOSize = ClipmapRimWidth * 4 + 5;

	for (int i = 0; i < VBO_MODES_AMOUNT; ++i)
		CreateVBO((ClipmapVBOMode)i);

	for (int i = 0; i < IBO_MODES_AMOUNT; ++i)
		CreateIBO((ClipmapIBOMode)i);
}

// --------------------------------------------------------------------
Landscape::Landscape(const char* FilePath):
RestartIndex(0xFFFFFFFF), Offset(0.25f)
{
    unsigned int DataByteSize;

    TrueHeightmap = (float*)LandscapeEditor::FileRead(FilePath, DataByteSize);
    ClipmapVBOsWidth[0] = sqrt((float)DataByteSize / 4.0);

    //CenterVBOData = new float[ClipmapVBOsWidth[0] * ClipmapVBOsWidth[0] * 2];

    //for (unsigned int x = 0; x < ClipmapVBOsWidth[0]; x++)
    //{
    //    for (unsigned int z = 0; z < ClipmapVBOsWidth[0]; z++)
    //    {
    //        CenterVBOData[2 * (z * ClipmapVBOsWidth[0] + x)] = Offset * x;
    //        CenterVBOData[2  * (z * ClipmapVBOsWidth[0] + x) + 1] = Offset * z;
    //    }
    //}

    //CreateIndexBuffers();
}

// --------------------------------------------------------------------
Landscape::~Landscape()
{
	for (int i = 0; i < VBO_MODES_AMOUNT; ++i)
		delete [] ClipmapVBOsData[i];

	for (int i = 0; i < IBO_MODES_AMOUNT; ++i)
		delete [] ClipmapIBOsData[i];

	delete [] ClipmapVBOsData;
	delete [] ClipmapIBOsData;
	delete [] ClipmapVBOsWidth;
}

// --------------------------------------------------------------------
void Landscape::CreateVBO(ClipmapVBOMode Mode)
{
	unsigned int CurrentIndex = 0;
	int Width = ClipmapVBOsWidth[VBO_CLIPMAP];

	switch (Mode)
	{
	case VBO_CLIPMAP:
		VBOSize[Mode] = 2 * pow((float)Width, 2.0f);
		ClipmapVBOsData[Mode] = new float[VBOSize[Mode]];
 
		for (int y = 0; y < Width; y++)
		{
			for (int x = 0; x < Width; x++)
			{
				ClipmapVBOsData[Mode][CurrentIndex++] = (x - float(Width - 1) / 2.0f + 0.5f);
				ClipmapVBOsData[Mode][CurrentIndex++] = (y - float(Width - 1) / 2.0f + 0.5f);
			}
		}

		break;
	}
}

// --------------------------------------------------------------------
unsigned int * Landscape::ConstructNiceIBOData(unsigned int Width, bool bOffsetX, bool bOffsetY, unsigned int CenterHoleWidth, unsigned int &DataSize)
{
	unsigned int OffsetX = bOffsetX ? 1 : 0;
	unsigned int OffsetY = bOffsetY ? 1 : 0;
	unsigned int CurrentIndex = 0;
	unsigned int Limit = 0;

	if (CenterHoleWidth > 0)
	{
		Limit = (Width - CenterHoleWidth) / 2;
		DataSize = 4 * ((Width - 2) / 2 * 6 - 2) + (Width - 4) * (Width - 3) * 2 + Width - 4 - (CenterHoleWidth) * (CenterHoleWidth + 1) * 2 + CenterHoleWidth + 1;
	}
	else
	{
		DataSize = 4 * ((Width - 2) / 2 * 6 - 2) + (Width - 4) * (Width - 3) * 2 + Width - 4;
	}
	
	unsigned int * IBOData = new unsigned int[DataSize];

	// ======================= Central part ===========================

	if (CenterHoleWidth == 0)
	{
		for (unsigned int x = 2 - OffsetX; x < Width - 2 - OffsetX; x++)
		{
			for (unsigned int y = 2 - OffsetY; y < Width - 1 - OffsetY; y++)
			{
				IBOData[CurrentIndex++] = x * Width + y;
				IBOData[CurrentIndex++] = (x + 1) * Width + y;
			}
			IBOData[CurrentIndex++] = RestartIndex;
		}
	}
	else
	{
		for (unsigned int x = 2 - OffsetX; x < Limit - 1; x++)
		{
			for (unsigned int y = 2 - OffsetY; y < Width - 1 - OffsetY; y++)
			{
				IBOData[CurrentIndex++] = x * Width + y;
				IBOData[CurrentIndex++] = (x + 1) * Width + y;
			}
			IBOData[CurrentIndex++] = RestartIndex;
		}

		for (unsigned int x = Limit + CenterHoleWidth; x < Width - 2 - OffsetX; x++)
		{
			for (unsigned int y = 2 - OffsetY; y < Width - 1 - OffsetY; y++)
			{
				IBOData[CurrentIndex++] = x * Width + y;
				IBOData[CurrentIndex++] = (x + 1) * Width + y;
			}
			IBOData[CurrentIndex++] = RestartIndex;
		}

		for (unsigned int x = Limit - 1; x < Limit + CenterHoleWidth; x++)
		{
			for (unsigned int y = 2 - OffsetY; y < Limit; y++)
			{
				IBOData[CurrentIndex++] = x * Width + y;
				IBOData[CurrentIndex++] = (x + 1) * Width + y;
			}
			IBOData[CurrentIndex++] = RestartIndex;
		}

		for (unsigned int x = Limit - 1; x < Limit + CenterHoleWidth; x++)
		{
			for (unsigned int y = Limit + CenterHoleWidth; y < Width - 1 - OffsetY; y++)
			{
				IBOData[CurrentIndex++] = x * Width + y;
				IBOData[CurrentIndex++] = (x + 1) * Width + y;
			}
			IBOData[CurrentIndex++] = RestartIndex;
		}
	}
	// ======================= Bottom part ===========================

	for (unsigned int y = 2 - OffsetY; y < Width - 2 - OffsetY; y += 2)
	{
		IBOData[CurrentIndex++] = (2 - OffsetX) * Width + y + 1;
		IBOData[CurrentIndex++] = (1 - OffsetX) * Width + y + 1;
		IBOData[CurrentIndex++] = (2 - OffsetX) * Width + y;
		IBOData[CurrentIndex++] = (1 - OffsetX) * Width + y - 1;
		if (y > 3 - OffsetY)
			IBOData[CurrentIndex++] = (2 - OffsetX) * Width + y - 1;
		IBOData[CurrentIndex++] = RestartIndex;
	}

	IBOData[CurrentIndex++] = (3 - OffsetX) * Width - 3 - OffsetY;
	IBOData[CurrentIndex++] = (3 - OffsetX) * Width - 2 - OffsetY;
	IBOData[CurrentIndex++] = (2 - OffsetX) * Width - 3 - OffsetY;
	IBOData[CurrentIndex++] = (2 - OffsetX) * Width - 1 - OffsetY;
	IBOData[CurrentIndex++] = RestartIndex;

	// ======================= Top part ===========================

	IBOData[CurrentIndex++] = (Width - 2 - OffsetX) * Width + 3 - OffsetY;
	IBOData[CurrentIndex++] = (Width - 2 - OffsetX) * Width + 2 - OffsetY;
	IBOData[CurrentIndex++] = (Width - 1 - OffsetX) * Width + 3 - OffsetY;
	IBOData[CurrentIndex++] = (Width - 1 - OffsetX) * Width + 1 - OffsetY;
	IBOData[CurrentIndex++] = RestartIndex;

	for (unsigned int y = 4 - OffsetY; y < Width - OffsetY; y += 2)
	{
		IBOData[CurrentIndex++] = (Width - 2 - OffsetX) * Width + y - 1;
		IBOData[CurrentIndex++] = (Width - 1 - OffsetX) * Width + y - 1;
		IBOData[CurrentIndex++] = (Width - 2 - OffsetX) * Width + y;
		IBOData[CurrentIndex++] = (Width - 1 - OffsetX) * Width + y + 1;
		if (y < Width - 3 - OffsetY)
			IBOData[CurrentIndex++] = (Width - 2 - OffsetX) * Width + y + 1;
		IBOData[CurrentIndex++] = RestartIndex;
	}

	// ======================= Right part ===========================

	IBOData[CurrentIndex++] = (1 - OffsetX) * Width + 1 - OffsetY;
	IBOData[CurrentIndex++] = (3 - OffsetX) * Width + 1 - OffsetY;
	IBOData[CurrentIndex++] = (2 - OffsetX) * Width + 2 - OffsetY;
	IBOData[CurrentIndex++] = (3 - OffsetX) * Width + 2 - OffsetY;
	IBOData[CurrentIndex++] = RestartIndex;
		
	for (unsigned int x = 3 - OffsetX; x < Width - 1 - OffsetX; x += 2)
	{
		IBOData[CurrentIndex++] = x * Width + 2 - OffsetY;
		IBOData[CurrentIndex++] = x * Width + 1 - OffsetY;
		IBOData[CurrentIndex++] = (x + 1) * Width + 2 - OffsetY;
		IBOData[CurrentIndex++] = (x + 2) * Width + 1 - OffsetY;
		if (x < Width - 4 - OffsetX)
			IBOData[CurrentIndex++] = (x + 2) * Width + 2 - OffsetY;
		IBOData[CurrentIndex++] = RestartIndex;
	}

	// ======================= Left part ===========================

	for (unsigned int x = 1 - OffsetX; x < Width - 3 - OffsetX; x += 2)
	{
		IBOData[CurrentIndex++] = (x + 3) * Width - 2 - OffsetY;
		IBOData[CurrentIndex++] = (x + 3) * Width - 1 - OffsetY;
		IBOData[CurrentIndex++] = (x + 2) * Width - 2 - OffsetY;
		IBOData[CurrentIndex++] = (x + 1) * Width - 1 - OffsetY;
		if (x > 2 - OffsetX)
			IBOData[CurrentIndex++] = (x + 1) * Width - 2 - OffsetY;
		IBOData[CurrentIndex++] = RestartIndex;
	}

	IBOData[CurrentIndex++] = (Width - OffsetX) * Width - 1 - OffsetY;
	IBOData[CurrentIndex++] = (Width - 2 - OffsetX) * Width - 1 - OffsetY;
	IBOData[CurrentIndex++] = (Width - 1 - OffsetX) * Width - 2 - OffsetY;
	IBOData[CurrentIndex++] = (Width - 2 - OffsetX) * Width - 2 - OffsetY;
	IBOData[CurrentIndex++] = RestartIndex;

	return IBOData;
}

// --------------------------------------------------------------------
void Landscape::CreateIBO(ClipmapIBOMode Mode)
{
	switch (Mode)
	{
	case IBO_CENTER_1:
		ClipmapIBOsData[Mode] = ConstructNiceIBOData(ClipmapVBOsWidth[VBO_CLIPMAP], false, false, 0, IBOSize[Mode]);
		break;
	case IBO_CENTER_2:
		ClipmapIBOsData[Mode] = ConstructNiceIBOData(ClipmapVBOsWidth[VBO_CLIPMAP], true, false, 0, IBOSize[Mode]);
		break;
	case IBO_CENTER_3:
		ClipmapIBOsData[Mode] = ConstructNiceIBOData(ClipmapVBOsWidth[VBO_CLIPMAP], false, true, 0, IBOSize[Mode]);
		break;
	case IBO_CENTER_4:
		ClipmapIBOsData[Mode] = ConstructNiceIBOData(ClipmapVBOsWidth[VBO_CLIPMAP], true, true, 0, IBOSize[Mode]);
		break;
	case IBO_CLIPMAP_1:
		ClipmapIBOsData[Mode] = ConstructNiceIBOData(ClipmapVBOsWidth[VBO_CLIPMAP], false, false, ClipmapVBOsWidth[VBO_CLIPMAP] / 2 - 2, IBOSize[Mode]);
		break;
	case IBO_CLIPMAP_2:
		ClipmapIBOsData[Mode] = ConstructNiceIBOData(ClipmapVBOsWidth[VBO_CLIPMAP], true, false, ClipmapVBOsWidth[VBO_CLIPMAP] / 2 - 2, IBOSize[Mode]);
		break;
	case IBO_CLIPMAP_3:
		ClipmapIBOsData[Mode] = ConstructNiceIBOData(ClipmapVBOsWidth[VBO_CLIPMAP], false, true, ClipmapVBOsWidth[VBO_CLIPMAP] / 2 - 2, IBOSize[Mode]);
		break;
	case IBO_CLIPMAP_4:
		ClipmapIBOsData[Mode] = ConstructNiceIBOData(ClipmapVBOsWidth[VBO_CLIPMAP], true, true, ClipmapVBOsWidth[VBO_CLIPMAP] / 2 - 2, IBOSize[Mode]);
		break;
	}
}

// --------------------------------------------------------------------
bool Landscape::SaveToFile(const char* FilePath)
{
    LandscapeEditor::FileWrite(FilePath, TrueHeightmap, ClipmapVBOsWidth[0] * ClipmapVBOsWidth[0]);

    return true;
}

// --------------------------------------------------------------------
void Landscape::UpdateHeightmap(Brush &AffectingBrush)
{
    vec2 CurrentVec;
    vec2 BrushPosition = AffectingBrush.GetPosition();
    float BrushRadius = AffectingBrush.GetRadius();
    float Distance, DistanceFactor;
    float HeightDifference;
    float HeightAverage = 0, HeightSum = 0;
    int Counter = 0;

    if (AffectingBrush.GetMode())
    {
        for (unsigned int x = 0; x < ClipmapVBOsWidth[0]; x++)
        {
            for (unsigned int z = 0; z < ClipmapVBOsWidth[0]; z++)
            {
                CurrentVec.x = ClipmapVBOsData[0][2 * (z * ClipmapVBOsWidth[0] + x)];
                CurrentVec.y = ClipmapVBOsData[0][2 * (z * ClipmapVBOsWidth[0] + x) + 1];

                Distance = distance(CurrentVec, BrushPosition);
                
                if (Distance <= BrushRadius)
                {
                    Counter++;
                    HeightSum += TrueHeightmap[z * ClipmapVBOsWidth[0] + x];
                }
            }
        }

        HeightAverage = HeightSum / Counter;
    }

    for (unsigned int x = 0; x < ClipmapVBOsWidth[0]; x++)
    {
        for (unsigned int z = 0; z < ClipmapVBOsWidth[0]; z++)
        {
            CurrentVec.x = ClipmapVBOsData[0][2 * (z * ClipmapVBOsWidth[0] + x)];
            CurrentVec.y = ClipmapVBOsData[0][2 * (z * ClipmapVBOsWidth[0] + x) + 1];

            Distance = distance(CurrentVec, BrushPosition);

            if (Distance <= BrushRadius)
            {
                DistanceFactor = 1.0f - Distance / BrushRadius;

                switch (AffectingBrush.GetMode())
                {
                case 0:           
                    TrueHeightmap[z * ClipmapVBOsWidth[0] + x] += 0.15f * ((DistanceFactor < 0.5f) ? (pow(DistanceFactor, 2)) : (0.5f - pow(1.0f - DistanceFactor, 2))); 
                    break;
                case 1:
                    TrueHeightmap[z * ClipmapVBOsWidth[0] + x] -= 0.15f * ((DistanceFactor < 0.5f) ? (pow(DistanceFactor, 2)) : (0.5f - pow(1.0f - DistanceFactor, 2))); 
                    break;
                case 2:
                    HeightDifference = HeightAverage - TrueHeightmap[z * ClipmapVBOsWidth[0] + x];
                    TrueHeightmap[z * ClipmapVBOsWidth[0] + x] += 0.02f * HeightDifference;
                    break;
                case 3:
                    TrueHeightmap[z * ClipmapVBOsWidth[0] + x] += 0.15f * ((DistanceFactor < 0.5f) ? (pow(DistanceFactor, 2)) : (pow(1.0f - DistanceFactor, 2))); 
                    break;
                }
            }   
        }
    }
}

// --------------------------------------------------------------------
float * Landscape::GetClipmapVBOData(ClipmapVBOMode Mode, int &outDataAmount)
{
	outDataAmount = VBOSize[Mode];
    return ClipmapVBOsData[Mode];
}

// --------------------------------------------------------------------
unsigned int * Landscape::GetClipmapIBOData(ClipmapIBOMode Mode, int &outDataAmount)
{
	outDataAmount = IBOSize[Mode];
    return ClipmapIBOsData[Mode];
}

// --------------------------------------------------------------------
float * Landscape::GetHeightmap(int &VerticesAmountX)
{
    VerticesAmountX = ClipmapVBOsWidth[0];

    float *Data = new float[ClipmapVBOsWidth[0] * ClipmapVBOsWidth[0]];

    for (unsigned int i = 0; i < ClipmapVBOsWidth[0] * ClipmapVBOsWidth[0]; i++)
        Data[i] = TrueHeightmap[i];

    return Data;
}