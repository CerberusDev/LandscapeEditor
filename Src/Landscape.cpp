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

	ClipmapVBOsWidth[VBO_CLIPMAP] = ClipmapRimWidth * 4 + 2;
	ClipmapVBOsWidth[VBO_STRIPS] = ClipmapRimWidth * 4 + 4;
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
	int Width = ClipmapVBOsWidth[Mode];

	int SquaresNum = Width - 1;
	int MissingVerticesX = Width / 2 - 1;
	int RimVerticesX = (Width - MissingVerticesX) / 2;

	switch (Mode)
	{
	case VBO_CLIPMAP:

		if ((SquaresNum - 1) % 4 != 0 || SquaresNum < 9)
			ERR("!!! ASSERT FAIL !!!   Bad landscape size!\n");

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
	case VBO_STRIPS:

		MissingVerticesX = Width - 4;
		RimVerticesX = 2;

		VBOSize[Mode] = 2 * (pow((float)Width, 2.0f) - pow(MissingVerticesX, 2.0f));
		ClipmapVBOsData[Mode] = new float[VBOSize[Mode]];
 
		for (int y = 0; y < Width; y++)
		{
			for (int x = 0; x < Width; x++)
			{
				if (x < RimVerticesX || x >= RimVerticesX + MissingVerticesX || y < RimVerticesX || y >= RimVerticesX + MissingVerticesX)
				{
					ClipmapVBOsData[Mode][CurrentIndex++] = (x - float(Width - 1) / 2.0f + 0.5f) + 0.0f;
					ClipmapVBOsData[Mode][CurrentIndex++] = (y - float(Width - 1) / 2.0f + 0.5f);
				}
			}
		}

		break;
	}
}

// --------------------------------------------------------------------
void Landscape::CreateIBO(ClipmapIBOMode Mode)
{
	unsigned int CurrentIndex = 0;
	int Width = 0;

	if (Mode == IBO_CENTER || Mode == IBO_CLIPMAP)
		Width = ClipmapVBOsWidth[VBO_CLIPMAP];
	else
		Width = ClipmapVBOsWidth[VBO_STRIPS];

	int MissingVerticesX = Width / 2 - 1;
	int RimVerticesX = (Width - MissingVerticesX) / 2;

	switch (Mode)
	{
	case IBO_CENTER:
	{
		IBOSize[Mode] = (Width * 2) * (Width - 1) + (Width - 1);
		ClipmapIBOsData[Mode] = new unsigned int[IBOSize[Mode]];

		for (int x = 0; x < Width - 1; x++)
		{
			for (int y = 0; y < Width; y++)
			{
				ClipmapIBOsData[Mode][CurrentIndex++] = x * Width + y;
				ClipmapIBOsData[Mode][CurrentIndex++] = (x+1) * Width + y;
			}
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}
		break;
	}
	case IBO_CLIPMAP:
	{
		IBOSize[Mode] = ((Width * 2) + 1) * (RimVerticesX - 1) +
						  ((MissingVerticesX + 2) * 2) * RimVerticesX +
						  ((MissingVerticesX + 2) * 2) * RimVerticesX +
						  ((Width * 2) + 1) * (RimVerticesX - 1);

		ClipmapIBOsData[Mode] = new unsigned int[IBOSize[Mode]];

		for (int x = 0; x < RimVerticesX-1; x++)
		{
			for (int y = 0; y < Width; y++)
			{
				ClipmapIBOsData[Mode][CurrentIndex++] = x * Width + y;
				ClipmapIBOsData[Mode][CurrentIndex++] = (x+1) * Width + y;
			}
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}

		for (int x = RimVerticesX-1; x < RimVerticesX + MissingVerticesX; x++)
		{
			for (int y = 0; y < RimVerticesX; y++)
			{
				ClipmapIBOsData[Mode][CurrentIndex++] = x * Width + y;
				ClipmapIBOsData[Mode][CurrentIndex++] = (x+1) * Width + y;
			}
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}

		for (int x = RimVerticesX-1; x < RimVerticesX + MissingVerticesX; x++)
		{
			for (int y = RimVerticesX + MissingVerticesX; y < Width; y++)
			{
				ClipmapIBOsData[Mode][CurrentIndex++] = x * Width + y;
				ClipmapIBOsData[Mode][CurrentIndex++] = (x+1) * Width + y;
			}
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}

		for (int x = RimVerticesX + MissingVerticesX; x < Width-1; x++)
		{
			for (int y = 0; y < Width; y++)
			{
				ClipmapIBOsData[Mode][CurrentIndex++] = x * Width + y;
				ClipmapIBOsData[Mode][CurrentIndex++] = (x+1) * Width + y;
			}
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}
		break;
	}
	case IBO_STRIP_1:	// bottom, right
	{
		IBOSize[Mode] = ((Width / 2) - 1) * 12 - 2;
		ClipmapIBOsData[Mode] = new unsigned int[IBOSize[Mode]];

		for (int y = 1; y < Width - 1; y += 2)
		{
			ClipmapIBOsData[Mode][CurrentIndex++] = Width + y + 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = y + 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = Width + y;
			ClipmapIBOsData[Mode][CurrentIndex++] = y - 1;
			if (y > 2)
				ClipmapIBOsData[Mode][CurrentIndex++] = Width + y - 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}

		for (int x = 3; x < Width - 1; x += 2)
		{
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (x - 3) * 4 + 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (x - 3) * 4;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (x - 2) * 4 + 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (x - 1) * 4;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (x - 1) * 4 + 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}

		ClipmapIBOsData[Mode][CurrentIndex++] = 0;
		ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width;
		ClipmapIBOsData[Mode][CurrentIndex++] = Width + 1;
		ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + 1;
		ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;

		break;
	}
	case IBO_STRIP_2:	// bottom, left
	{
		IBOSize[Mode] = ((Width / 2) - 1) * 12 - 2;
		ClipmapIBOsData[Mode] = new unsigned int[IBOSize[Mode]];

		for (int y = 2; y < Width - 2; y += 2)
		{
			ClipmapIBOsData[Mode][CurrentIndex++] = Width + y + 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = y + 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = Width + y;
			ClipmapIBOsData[Mode][CurrentIndex++] = y - 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = Width + y - 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}
		ClipmapIBOsData[Mode][CurrentIndex++] = Width - 1;
		ClipmapIBOsData[Mode][CurrentIndex++] = Width - 3;
		ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width - 2;
		ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width - 3;
		ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;

		for (int x = 0; x < Width - 3; x += 2)
		{
			if (x > Width - 5)
			{
				ClipmapIBOsData[Mode][CurrentIndex++] = 3 * Width + (x + 0) * 4 - 2;
				ClipmapIBOsData[Mode][CurrentIndex++] = 3 * Width + (x + 0) * 4 - 1;
			}
			else
			{
				ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (x + 1) * 4 - 2;
				ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (x + 1) * 4 - 1;
			}
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + x * 4 - 2;
			if (x < 1)
			{
				ClipmapIBOsData[Mode][CurrentIndex++] = Width + (x - 0) * 4 - 1;
			}
			else
			{			
				ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (x - 1) * 4 - 1;
				ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (x - 1) * 4 - 2;
			}
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}

		break;
	}
	case IBO_STRIP_3:	// top, right
	{
		IBOSize[Mode] = ((Width / 2) - 1) * 12 - 2;
		ClipmapIBOsData[Mode] = new unsigned int[IBOSize[Mode]];

		int Offset = pow(Width - 4, 2.0f);

		ClipmapIBOsData[Mode][CurrentIndex++] = (Width - 1) * Width - Offset;
		ClipmapIBOsData[Mode][CurrentIndex++] = (Width - 1) * Width + 2 - Offset;
		ClipmapIBOsData[Mode][CurrentIndex++] = (Width - 2) * Width + 1 - Offset;
		ClipmapIBOsData[Mode][CurrentIndex++] = (Width - 2) * Width + 2 - Offset;
		ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;

		for (int y = 2; y < Width - 2; y += 2)
		{
			ClipmapIBOsData[Mode][CurrentIndex++] = (Width - 2) * Width + y - Offset;
			ClipmapIBOsData[Mode][CurrentIndex++] = (Width - 1) * Width + y - Offset;
			ClipmapIBOsData[Mode][CurrentIndex++] = (Width - 2) * Width + y + 1 - Offset;
			ClipmapIBOsData[Mode][CurrentIndex++] = (Width - 1) * Width + y + 2 - Offset;
			ClipmapIBOsData[Mode][CurrentIndex++] = (Width - 2) * Width + y + 2 - Offset;
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}

		for (int x = 2; x < Width; x += 2)
		{
			if (x < 3)
			{
				ClipmapIBOsData[Mode][CurrentIndex++] = Width + (x - 2) * 4 + 1;
				ClipmapIBOsData[Mode][CurrentIndex++] = Width + (x - 2) * 4;
			}
			else
			{
				ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (x - 3) * 4 + 1;
				ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (x - 3) * 4;
			}
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (x - 2) * 4 + 1;
			if (x > Width - 3)
			{
				ClipmapIBOsData[Mode][CurrentIndex++] = 3 * Width + (x - 2) * 4;
			}
			else
			{
				ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (x - 1) * 4;
				ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (x - 1) * 4 + 1;
			}
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}

		break;
	}
	case IBO_STRIP_4:	// top, left
	{
		IBOSize[Mode] = ((Width / 2) - 1) * 12 - 2;
		ClipmapIBOsData[Mode] = new unsigned int[IBOSize[Mode]];

		int Offset = pow(Width - 4, 2.0f);

		for (int y = 1; y < Width - 1; y += 2)
		{
			ClipmapIBOsData[Mode][CurrentIndex++] = (Width - 2) * Width + y - Offset;
			ClipmapIBOsData[Mode][CurrentIndex++] = (Width - 1) * Width + y - Offset;
			ClipmapIBOsData[Mode][CurrentIndex++] = (Width - 2) * Width + y + 1 - Offset;
			ClipmapIBOsData[Mode][CurrentIndex++] = (Width - 1) * Width + y + 2 - Offset;
			if (y < Width - 4)
				ClipmapIBOsData[Mode][CurrentIndex++] = (Width - 2) * Width + y + 2 - Offset;
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}

		for (int x = 1; x < Width - 4; x += 2)
		{
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (x + 1) * 4 - 2;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (x + 1) * 4 - 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + x * 4 - 2;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (x - 1) * 4 - 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (x - 1) * 4 - 2;
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}

		ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (Width - 4) * 4 - 2;
		ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (Width - 3) * 4 - 2 + (Width - 4);
		ClipmapIBOsData[Mode][CurrentIndex++] = 2 * Width + (Width - 4) * 4 - 1;
		ClipmapIBOsData[Mode][CurrentIndex++] = 3 * Width + (Width - 3) * 4 - 1 + (Width - 4);
		ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;

		break;
	}
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