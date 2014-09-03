// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date: 23.03.2013
// --------------------------------------------------------------------

#include "Landscape.h"
#include "LandscapeEditor.h"

// --------------------------------------------------------------------
Landscape::Landscape(int VerticesAmountX):
RestartIndex(0xFFFFFFFF), Offset(1.0f), TBOSize(VerticesAmountX), VBOSize(0), IBOSize(0)
{
	ClipmapVBOsData = new float*[VBO_MODES_AMOUNT];
	ClipmapIBOsData = new unsigned int*[IBO_MODES_AMOUNT];
	ClipmapVBOsSizeX = new unsigned int[VBO_MODES_AMOUNT];
	ClipmapVBOsSizeY = new unsigned int[VBO_MODES_AMOUNT];

	VBOSize = new unsigned int[VBO_MODES_AMOUNT];
	IBOSize = new unsigned int[IBO_MODES_AMOUNT];

	ClipmapVBOsSizeX[VBO_CLIPMAP] = ClipmapVBOsSizeY[VBO_CLIPMAP] = 22;
	ClipmapVBOsSizeX[VBO_STRIPS] = ClipmapVBOsSizeY[VBO_STRIPS] = 24;

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
    ClipmapVBOsSizeX[0] = sqrt((float)DataByteSize / 4.0);

    //CenterVBOData = new float[ClipmapVBOsSizeX[0] * ClipmapVBOsSizeX[0] * 2];

    //for (unsigned int x = 0; x < ClipmapVBOsSizeX[0]; x++)
    //{
    //    for (unsigned int z = 0; z < ClipmapVBOsSizeX[0]; z++)
    //    {
    //        CenterVBOData[2 * (z * ClipmapVBOsSizeX[0] + x)] = Offset * x;
    //        CenterVBOData[2  * (z * ClipmapVBOsSizeX[0] + x) + 1] = Offset * z;
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
	delete [] ClipmapVBOsSizeX;
	delete [] ClipmapVBOsSizeY;
}

// --------------------------------------------------------------------
void Landscape::CreateVBO(ClipmapVBOMode Mode)
{
	unsigned int CurrentIndex = 0;
	int SizeX = ClipmapVBOsSizeX[Mode];
	int SizeY = ClipmapVBOsSizeY[Mode];

	int SquaresNum = SizeX - 1;
	int MissingVerticesX = SizeX / 2 - 1;
	int RimVerticesX = (SizeX - MissingVerticesX) / 2;

	switch (Mode)
	{
	case VBO_CLIPMAP:

		if ((SquaresNum - 1) % 4 != 0 || SquaresNum < 9)
			ERR("!!! ASSERT FAIL !!!   Bad landscape size!\n");

		VBOSize[Mode] = 2 * pow((float)SizeX, 2.0f);
		ClipmapVBOsData[Mode] = new float[VBOSize[Mode]];
 
		for (int y = 0; y < SizeY; y++)
		{
			for (int x = 0; x < SizeX; x++)
			{
				ClipmapVBOsData[Mode][CurrentIndex++] = Offset * (x - float(SizeX - 1) / 2.0f + 0.5f);
				ClipmapVBOsData[Mode][CurrentIndex++] = Offset * (y - float(SizeY - 1) / 2.0f + 0.5f);
			}
		}

		break;
	case VBO_STRIPS:

		MissingVerticesX = SizeX - 4;
		RimVerticesX = 2;

		VBOSize[Mode] = 2 * (pow((float)SizeX, 2.0f) - pow(MissingVerticesX, 2.0f));
		ClipmapVBOsData[Mode] = new float[VBOSize[Mode]];
 
		for (int y = 0; y < SizeY; y++)
		{
			for (int x = 0; x < SizeX; x++)
			{
				if (x < RimVerticesX || x >= RimVerticesX + MissingVerticesX || y < RimVerticesX || y >= RimVerticesX + MissingVerticesX)
				{
					ClipmapVBOsData[Mode][CurrentIndex++] = Offset * (x - float(SizeX - 1) / 2.0f + 0.5f) + 0.0f;
					ClipmapVBOsData[Mode][CurrentIndex++] = Offset * (y - float(SizeY - 1) / 2.0f + 0.5f);
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
	int SizeX = 0;
	int SizeY = 0;

	if (Mode == IBO_CENTER || Mode == IBO_CLIPMAP)
	{
		SizeX = ClipmapVBOsSizeX[VBO_CLIPMAP];
		SizeY = ClipmapVBOsSizeX[VBO_CLIPMAP];
	}
	else
	{
		SizeX = ClipmapVBOsSizeX[VBO_STRIPS];
		SizeY = ClipmapVBOsSizeX[VBO_STRIPS];
	}

	//int SquaresNum = SizeX - 1;
	int MissingVerticesX = SizeX / 2 - 1;
	int RimVerticesX = (SizeX - MissingVerticesX) / 2;

	// --- Working version for homogenous VBO, displays full quad without any hole ---
	//IBOSize = (SizeY * 2) * (SizeX - 1) + (SizeX - 1);

 //   ClipmapIBOsData[Mode] = new unsigned int[IBOSize];

	//for (int x = 0; x < SquaresNum; x++)
 //   {
 //       for (int y = 0; y < SizeY; y++)
 //       {
 //           ClipmapIBOsData[Mode][CurrentIndex++] = x * SizeX + y;
 //           ClipmapIBOsData[Mode][CurrentIndex++] = (x+1) * SizeX + y;
 //       }
 //       ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
 //   }


	//IBOSize[Mode] = (2 * ((SizeY * 2) + 1) + 2 * (((MissingVerticesX + 2) * 2) + 1)) * (RimVerticesX - 1);
	//IBOSize = ((SizeY * 2) + 1) * (RimVerticesX - 1) +
	//		  (((MissingVerticesX + 2) * 2) + 1) * (RimVerticesX - 1) +
	//		  (((MissingVerticesX + 2) * 2) + 1) * (RimVerticesX - 1) +
	//		  ((SizeY * 2) + 1) * (RimVerticesX - 1);


	switch (Mode)
	{
	case IBO_CENTER:
	{
		IBOSize[Mode] = (SizeY * 2) * (SizeX - 1) + (SizeX - 1);
		ClipmapIBOsData[Mode] = new unsigned int[IBOSize[Mode]];

		for (int x = 0; x < SizeX - 1; x++)
		{
			for (int y = 0; y < SizeY; y++)
			{
				ClipmapIBOsData[Mode][CurrentIndex++] = x * SizeX + y;
				ClipmapIBOsData[Mode][CurrentIndex++] = (x+1) * SizeX + y;
			}
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}
		break;
	}
	case IBO_CLIPMAP:
	{
		IBOSize[Mode] = ((SizeY * 2) + 1) * (RimVerticesX - 1) +
						  ((MissingVerticesX + 2) * 2) * RimVerticesX +
						  ((MissingVerticesX + 2) * 2) * RimVerticesX +
						  ((SizeY * 2) + 1) * (RimVerticesX - 1);

		ClipmapIBOsData[Mode] = new unsigned int[IBOSize[Mode]];

		for (int x = 0; x < RimVerticesX-1; x++)
		{
			for (int y = 0; y < SizeY; y++)
			{
				ClipmapIBOsData[Mode][CurrentIndex++] = x * SizeX + y;
				ClipmapIBOsData[Mode][CurrentIndex++] = (x+1) * SizeX + y;
			}
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}

		for (int x = RimVerticesX-1; x < RimVerticesX + MissingVerticesX; x++)
		{
			for (int y = 0; y < RimVerticesX; y++)
			{
				ClipmapIBOsData[Mode][CurrentIndex++] = x * SizeX + y;
				ClipmapIBOsData[Mode][CurrentIndex++] = (x+1) * SizeX + y;
			}
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}

		for (int x = RimVerticesX-1; x < RimVerticesX + MissingVerticesX; x++)
		{
			for (int y = RimVerticesX + MissingVerticesX; y < SizeX; y++)
			{
				ClipmapIBOsData[Mode][CurrentIndex++] = x * SizeX + y;
				ClipmapIBOsData[Mode][CurrentIndex++] = (x+1) * SizeX + y;
			}
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}

		for (int x = RimVerticesX + MissingVerticesX; x < SizeX-1; x++)
		{
			for (int y = 0; y < SizeY; y++)
			{
				ClipmapIBOsData[Mode][CurrentIndex++] = x * SizeX + y;
				ClipmapIBOsData[Mode][CurrentIndex++] = (x+1) * SizeX + y;
			}
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}
		break;
	}
	case IBO_STRIP_1:
	{
		IBOSize[Mode] = (((SizeX - 1) * 2) + 1) + ((SizeX - 3) * 5);
		ClipmapIBOsData[Mode] = new unsigned int[IBOSize[Mode]];

		for (int y = 0; y < SizeX - 1; y++)
		{
			ClipmapIBOsData[Mode][CurrentIndex++] = y;
			ClipmapIBOsData[Mode][CurrentIndex++] = SizeX + y;
		}
		ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;

		ClipmapIBOsData[Mode][CurrentIndex++] = SizeX;
		ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX;
		ClipmapIBOsData[Mode][CurrentIndex++] = SizeX + 1;
		ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + 1;
		ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;

		for (int x = 2; x < SizeX - 2; x++)
		{
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (x - 2) * 4;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (x - 1) * 4;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (x - 2) * 4 + 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (x - 1) * 4 + 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}

		break;
	}
	case IBO_STRIP_2:
	{
		IBOSize[Mode] = (((SizeX - 1) * 2) + 1) + ((SizeX - 3) * 5);
		ClipmapIBOsData[Mode] = new unsigned int[IBOSize[Mode]];

		for (int y = 1; y < SizeX; y++)
		{
			ClipmapIBOsData[Mode][CurrentIndex++] = y;
			ClipmapIBOsData[Mode][CurrentIndex++] = SizeX + y;
		}
		ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;

		for (int x = 1; x < SizeX - 3; x++)
		{
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (x - 1) * 4 - 2;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + x * 4 - 2;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (x - 1) * 4 - 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + x * 4 - 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}

		ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (SizeX - 4) * 4 - 2;
		ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (SizeX - 3) * 4 - 2 + (SizeX - 4);
		ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (SizeX - 4) * 4 - 1;
		ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (SizeX - 3) * 4 - 1 + (SizeX - 4);
		ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;

		break;
	}
	case IBO_STRIP_3:
	{
		IBOSize[Mode] = (((SizeX - 1) * 2) + 1) + ((SizeX - 3) * 5);
		ClipmapIBOsData[Mode] = new unsigned int[IBOSize[Mode]];

		ClipmapIBOsData[Mode][CurrentIndex++] = SizeX;
		ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX;
		ClipmapIBOsData[Mode][CurrentIndex++] = SizeX + 1;
		ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + 1;
		ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;

		for (int x = 2; x < SizeX - 2; x++)
		{
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (x - 2) * 4;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (x - 1) * 4;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (x - 2) * 4 + 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (x - 1) * 4 + 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}

		int Offset = pow(SizeX - 4, 2.0f);

		for (int y = 0; y < SizeX - 1; y++)
		{
			ClipmapIBOsData[Mode][CurrentIndex++] = (SizeX - 2) * SizeX + y - Offset;
			ClipmapIBOsData[Mode][CurrentIndex++] = (SizeX - 1) * SizeX + y - Offset;
		}
		ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;

		break;
	}
	case IBO_STRIP_4:
	{
		IBOSize[Mode] = (((SizeX - 1) * 2) + 1) + ((SizeX - 3) * 5);
		ClipmapIBOsData[Mode] = new unsigned int[IBOSize[Mode]];

		for (int x = 1; x < SizeX - 3; x++)
		{
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (x - 1) * 4 - 2;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + x * 4 - 2;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (x - 1) * 4 - 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + x * 4 - 1;
			ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;
		}

		ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (SizeX - 4) * 4 - 2;
		ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (SizeX - 3) * 4 - 2 + (SizeX - 4);
		ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (SizeX - 4) * 4 - 1;
		ClipmapIBOsData[Mode][CurrentIndex++] = 2 * SizeX + (SizeX - 3) * 4 - 1 + (SizeX - 4);
		ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;

		int Offset = pow(SizeX - 4, 2.0f);

		for (int y = 1; y < SizeX; y++)
		{
			ClipmapIBOsData[Mode][CurrentIndex++] = (SizeX - 2) * SizeX + y - Offset;
			ClipmapIBOsData[Mode][CurrentIndex++] = (SizeX - 1) * SizeX + y - Offset;
		}
		ClipmapIBOsData[Mode][CurrentIndex++] = RestartIndex;

		break;
	}
	}
}

// --------------------------------------------------------------------
bool Landscape::SaveToFile(const char* FilePath)
{
    LandscapeEditor::FileWrite(FilePath, TrueHeightmap, ClipmapVBOsSizeX[0] * ClipmapVBOsSizeX[0]);

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
        for (unsigned int x = 0; x < ClipmapVBOsSizeX[0]; x++)
        {
            for (unsigned int z = 0; z < ClipmapVBOsSizeX[0]; z++)
            {
                CurrentVec.x = ClipmapVBOsData[0][2 * (z * ClipmapVBOsSizeX[0] + x)];
                CurrentVec.y = ClipmapVBOsData[0][2 * (z * ClipmapVBOsSizeX[0] + x) + 1];

                Distance = distance(CurrentVec, BrushPosition);
                
                if (Distance <= BrushRadius)
                {
                    Counter++;
                    HeightSum += TrueHeightmap[z * ClipmapVBOsSizeX[0] + x];
                }
            }
        }

        HeightAverage = HeightSum / Counter;
    }

    for (unsigned int x = 0; x < ClipmapVBOsSizeX[0]; x++)
    {
        for (unsigned int z = 0; z < ClipmapVBOsSizeX[0]; z++)
        {
            CurrentVec.x = ClipmapVBOsData[0][2 * (z * ClipmapVBOsSizeX[0] + x)];
            CurrentVec.y = ClipmapVBOsData[0][2 * (z * ClipmapVBOsSizeX[0] + x) + 1];

            Distance = distance(CurrentVec, BrushPosition);

            if (Distance <= BrushRadius)
            {
                DistanceFactor = 1.0f - Distance / BrushRadius;

                switch (AffectingBrush.GetMode())
                {
                case 0:           
                    TrueHeightmap[z * ClipmapVBOsSizeX[0] + x] += 0.15f * ((DistanceFactor < 0.5f) ? (pow(DistanceFactor, 2)) : (0.5f - pow(1.0f - DistanceFactor, 2))); 
                    break;
                case 1:
                    TrueHeightmap[z * ClipmapVBOsSizeX[0] + x] -= 0.15f * ((DistanceFactor < 0.5f) ? (pow(DistanceFactor, 2)) : (0.5f - pow(1.0f - DistanceFactor, 2))); 
                    break;
                case 2:
                    HeightDifference = HeightAverage - TrueHeightmap[z * ClipmapVBOsSizeX[0] + x];
                    TrueHeightmap[z * ClipmapVBOsSizeX[0] + x] += 0.02f * HeightDifference;
                    break;
                case 3:
                    TrueHeightmap[z * ClipmapVBOsSizeX[0] + x] += 0.15f * ((DistanceFactor < 0.5f) ? (pow(DistanceFactor, 2)) : (pow(1.0f - DistanceFactor, 2))); 
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
    VerticesAmountX = ClipmapVBOsSizeX[0];

    float *Data = new float[ClipmapVBOsSizeX[0] * ClipmapVBOsSizeX[0]];

    for (unsigned int i = 0; i < ClipmapVBOsSizeX[0] * ClipmapVBOsSizeX[0]; i++)
        Data[i] = TrueHeightmap[i];

    return Data;
}