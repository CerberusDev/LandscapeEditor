// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date: 23.03.2013
// --------------------------------------------------------------------

#include "Landscape.h"
#include "LandscapeEditor.h"

// --------------------------------------------------------------------
Landscape::Landscape(int ClipmapRimWidth, float VerticesInterval):
RestartIndex(0xFFFFFFFF), Offset(VerticesInterval), VBOSize(0), IBOSize(0), TBOSize(0), HeightData(0), HeightDataSize(0), StartIndexX(0), StartIndexY(0)
{
	ClipmapIBOsData = new unsigned int*[IBO_MODES_AMOUNT];

	IBOSize = new unsigned int[IBO_MODES_AMOUNT];

	ClipmapVBOWidth = ClipmapRimWidth * 4 + 4;
	TBOSize = ClipmapRimWidth * 4 + 5;

	CreateVBO();

	for (int i = 0; i < IBO_MODES_AMOUNT; ++i)
		CreateIBO((ClipmapIBOMode)i);


	HeightDataSize = 424;
	StartIndexX = StartIndexY = 210;

	HeightData = new float[HeightDataSize * HeightDataSize];

	LOG("Generating terrain data...");

	for (unsigned int i = 0; i < HeightDataSize; ++i)
	{
		for (unsigned int j = 0; j < HeightDataSize; ++j)
		{
			//HeightData[i + HeightDataSize * j] = 10.0f + i / 10.0f;
			//HeightData[i + HeightDataSize * j] = (i % 32) / 8.0f + 430.0f;
			//HeightData[i + HeightDataSize * j] = (j % 512 == 113 || i % 512 == 113) ? (20.0f) : (0.0f);
			//HeightData[i + HeightDataSize * j] = 93.8f;
			//HeightData[i + HeightDataSize * j] = 50.0f + sin(float(i) / 3.0f) * 1.0f + sin(float(j) / 5.6f) * 1.6f;
			//HeightData[i + HeightDataSize * j] = 20.0f + j / 11.0f + i / 4.36f;
			//HeightData[i + HeightDataSize * j] = sin(float(j) / 400.f) * 80.0f + 300.0f;
			HeightData[i + HeightDataSize * j] = 70.0f + sin(float(i) / 10.0f) * 2.0f + sin(float(j) / 25.6f) * 10.6f;

			//float a = sin(float(i) / (1.0 * 704.0f)) * 30.0f;
			//float b = sin(float(i) / (1.0 * 352.0f)) * 25.0f;
			//float c = (sin(float(j) / (1.0 * 469.4f)) - (cos(float(j) / (1.0 * 234.7f)) + 1.0f) / 4.5f) * 30.0f;
			//float d = sin(float(j) / (2.0 * 58.f)) * 3.0f + sin(float(i) / (2.0 * 122.f)) * 5.0f;
			//float e = sin(float(i) / (3.0 * 2.0f)) * 0.8f * cos(float(j) / (3.0 * 6.2f)) * 0.6f + sin(float(j) / (3.0 * 2.3f)) * 0.8f * cos(float(i) / (3.0 * 6.4f)) * 0.5f;
			//HeightData[i + HeightDataSize * j] = (a + b + c + d + e) * 5.0f;
		}

		if (HeightDataSize > 10 && i % (HeightDataSize / 10) == 0)
			LOG("Progress: " << i / (HeightDataSize / 10) * 10 << "%");
	}

	StartIndexX += TBOSize / 2;
	StartIndexY += TBOSize / 2;

	LOG("Terrain Ready!\n");
}

// --------------------------------------------------------------------
Landscape::Landscape(const char* FilePath):
RestartIndex(0xFFFFFFFF), Offset(0.25f)
{
    unsigned int DataByteSize;

    HeightData = (float*)LandscapeEditor::FileRead(FilePath, DataByteSize);
    ClipmapVBOWidth = sqrt((float)DataByteSize / 4.0);

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
	for (int i = 0; i < IBO_MODES_AMOUNT; ++i)
		delete [] ClipmapIBOsData[i];

	delete [] ClipmapIBOsData;
	delete [] ClipmapVBOData;

	delete [] HeightData;
}

// --------------------------------------------------------------------
void Landscape::CreateVBO()
{
	unsigned int CurrentIndex = 0;
	int Width = ClipmapVBOWidth;

	VBOSize = 2 * pow((float)Width, 2.0f);
	ClipmapVBOData = new float[VBOSize];
 
	for (int y = 0; y < Width; y++)
	{
		for (int x = 0; x < Width; x++)
		{
			ClipmapVBOData[CurrentIndex++] = (x - float(Width - 1) / 2.0f + 0.5f);
			ClipmapVBOData[CurrentIndex++] = (y - float(Width - 1) / 2.0f + 0.5f);
		}
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
		ClipmapIBOsData[Mode] = ConstructNiceIBOData(ClipmapVBOWidth, false, false, 0, IBOSize[Mode]);
		break;
	case IBO_CENTER_2:
		ClipmapIBOsData[Mode] = ConstructNiceIBOData(ClipmapVBOWidth, true, false, 0, IBOSize[Mode]);
		break;
	case IBO_CENTER_3:
		ClipmapIBOsData[Mode] = ConstructNiceIBOData(ClipmapVBOWidth, false, true, 0, IBOSize[Mode]);
		break;
	case IBO_CENTER_4:
		ClipmapIBOsData[Mode] = ConstructNiceIBOData(ClipmapVBOWidth, true, true, 0, IBOSize[Mode]);
		break;
	case IBO_CLIPMAP_1:
		ClipmapIBOsData[Mode] = ConstructNiceIBOData(ClipmapVBOWidth, false, false, ClipmapVBOWidth / 2 - 2, IBOSize[Mode]);
		break;
	case IBO_CLIPMAP_2:
		ClipmapIBOsData[Mode] = ConstructNiceIBOData(ClipmapVBOWidth, true, false, ClipmapVBOWidth / 2 - 2, IBOSize[Mode]);
		break;
	case IBO_CLIPMAP_3:
		ClipmapIBOsData[Mode] = ConstructNiceIBOData(ClipmapVBOWidth, false, true, ClipmapVBOWidth / 2 - 2, IBOSize[Mode]);
		break;
	case IBO_CLIPMAP_4:
		ClipmapIBOsData[Mode] = ConstructNiceIBOData(ClipmapVBOWidth, true, true, ClipmapVBOWidth / 2 - 2, IBOSize[Mode]);
		break;
	}
}

// --------------------------------------------------------------------
bool Landscape::SaveToFile(const char* FilePath)
{
    LandscapeEditor::FileWrite(FilePath, HeightData, ClipmapVBOWidth * ClipmapVBOWidth);

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
        for (unsigned int x = 0; x < ClipmapVBOWidth; x++)
        {
            for (unsigned int z = 0; z < ClipmapVBOWidth; z++)
            {
                CurrentVec.x = ClipmapVBOData[2 * (z * ClipmapVBOWidth + x)];
                CurrentVec.y = ClipmapVBOData[2 * (z * ClipmapVBOWidth + x) + 1];

                Distance = distance(CurrentVec, BrushPosition);
                
                if (Distance <= BrushRadius)
                {
                    Counter++;
                    HeightSum += HeightData[z * ClipmapVBOWidth + x];
                }
            }
        }

        HeightAverage = HeightSum / Counter;
    }

    for (unsigned int x = 0; x < ClipmapVBOWidth; x++)
    {
        for (unsigned int z = 0; z < ClipmapVBOWidth; z++)
        {
            CurrentVec.x = ClipmapVBOData[2 * (z * ClipmapVBOWidth + x)];
            CurrentVec.y = ClipmapVBOData[2 * (z * ClipmapVBOWidth + x) + 1];

            Distance = distance(CurrentVec, BrushPosition);

            if (Distance <= BrushRadius)
            {
                DistanceFactor = 1.0f - Distance / BrushRadius;

                switch (AffectingBrush.GetMode())
                {
                case 0:           
                    HeightData[z * ClipmapVBOWidth + x] += 0.15f * ((DistanceFactor < 0.5f) ? (pow(DistanceFactor, 2)) : (0.5f - pow(1.0f - DistanceFactor, 2))); 
                    break;
                case 1:
                    HeightData[z * ClipmapVBOWidth + x] -= 0.15f * ((DistanceFactor < 0.5f) ? (pow(DistanceFactor, 2)) : (0.5f - pow(1.0f - DistanceFactor, 2))); 
                    break;
                case 2:
                    HeightDifference = HeightAverage - HeightData[z * ClipmapVBOWidth + x];
                    HeightData[z * ClipmapVBOWidth + x] += 0.02f * HeightDifference;
                    break;
                case 3:
                    HeightData[z * ClipmapVBOWidth + x] += 0.15f * ((DistanceFactor < 0.5f) ? (pow(DistanceFactor, 2)) : (pow(1.0f - DistanceFactor, 2))); 
                    break;
                }
            }   
        }
    }
}

// --------------------------------------------------------------------
float * Landscape::GetClipmapVBOData(int &outDataAmount)
{
	outDataAmount = VBOSize;
    return ClipmapVBOData;
}

// --------------------------------------------------------------------
unsigned int * Landscape::GetClipmapIBOData(ClipmapIBOMode Mode, int &outDataAmount)
{
	outDataAmount = IBOSize[Mode];
    return ClipmapIBOsData[Mode];
}