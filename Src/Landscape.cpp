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
	ClipmapVBOsData = new float*[CLIPMAP_MODULES_AMOUNT];
	ClipmapIBOsData = new unsigned int*[CLIPMAP_MODULES_AMOUNT];
	ClipmapVBOsSizeX = new unsigned int[CLIPMAP_MODULES_AMOUNT];
	ClipmapVBOsSizeY = new unsigned int[CLIPMAP_MODULES_AMOUNT];

	VBOSize = new unsigned int[CLIPMAP_MODULES_AMOUNT];
	IBOSize = new unsigned int[CLIPMAP_MODULES_AMOUNT];

	ClipmapVBOsSizeX[CLIPMAP_CENTER] = ClipmapVBOsSizeY[CLIPMAP_CENTER] = 18;
	
	ClipmapVBOsSizeX[CLIPMAP_STRIP_10] = ClipmapVBOsSizeY[CLIPMAP_STRIP_10] = 22;

	CreateVBO(CLIPMAP_CENTER);
	CreateIBO(CLIPMAP_CENTER);

	CreateVBO(CLIPMAP_STRIP_10);
	CreateIBO(CLIPMAP_STRIP_10);
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
	for (int i = 0; i < CLIPMAP_MODULES_AMOUNT; ++i)
	{
		delete [] ClipmapVBOsData[i];
		delete [] ClipmapIBOsData[i];
	}

	delete [] ClipmapVBOsData;
	delete [] ClipmapIBOsData;
	delete [] ClipmapVBOsSizeX;
	delete [] ClipmapVBOsSizeY;
}

// --------------------------------------------------------------------
void Landscape::CreateVBO(ClipmapModule Module)
{
	unsigned int CurrentIndex = 0;
	int SizeX = ClipmapVBOsSizeX[Module];
	int SizeY = ClipmapVBOsSizeY[Module];

	int SquaresNum = SizeX - 1;
	int MissingVerticesX = SizeX / 2 - 1;
	int RimVerticesX = (SizeX - MissingVerticesX) / 2;

	if ((SquaresNum - 1) % 4 != 0 || SquaresNum < 9)
		ERR("!!! ASSERT FAIL !!!   Bad landscape size!\n");

	switch (Module)
	{
	case CLIPMAP_CENTER:

		VBOSize[Module] = 2 * (pow((float)SizeX, 2.0f) - pow(MissingVerticesX, 2.0f));
		//VBOSize = 2 * pow((float)SizeX, 2.0f);

		ClipmapVBOsData[Module] = new float[VBOSize[Module]];
 
		for (int y = 0; y < SizeY; y++)
		{
			for (int x = 0; x < SizeX; x++)
			{
				if (x < RimVerticesX || x >= RimVerticesX + MissingVerticesX || y < RimVerticesX || y >= RimVerticesX + MissingVerticesX)
				{
					ClipmapVBOsData[Module][CurrentIndex++] = Offset * (x - float(SizeX - 1) / 2.0f + 0.5f);
					ClipmapVBOsData[Module][CurrentIndex++] = Offset * (y - float(SizeY - 1) / 2.0f + 0.5f);
				}
			}
		}

		break;
	case CLIPMAP_STRIP_10:
		VBOSize[Module] = 2 * (pow((float)SizeX, 2.0f) - pow(MissingVerticesX, 2.0f));
		//VBOSize = 2 * pow((float)SizeX, 2.0f);

		ClipmapVBOsData[Module] = new float[VBOSize[Module]];
 
		for (int y = 0; y < SizeY; y++)
		{
			for (int x = 0; x < SizeX; x++)
			{
				if (x < RimVerticesX || x >= RimVerticesX + MissingVerticesX || y < RimVerticesX || y >= RimVerticesX + MissingVerticesX)
				{
					ClipmapVBOsData[Module][CurrentIndex++] = Offset * (x - float(SizeX - 1) / 2.0f + 0.5f) + 0.0f;
					ClipmapVBOsData[Module][CurrentIndex++] = Offset * (y - float(SizeY - 1) / 2.0f + 0.5f);
				}
			}
		}

		break;
	}

}

// --------------------------------------------------------------------
void Landscape::CreateIBO(ClipmapModule Module)
{
	unsigned int CurrentIndex = 0;
	int SizeX = ClipmapVBOsSizeX[Module];
	int SizeY = ClipmapVBOsSizeY[Module];

	//int SquaresNum = SizeX - 1;
	int MissingVerticesX = SizeX / 2 - 1;
	int RimVerticesX = (SizeX - MissingVerticesX) / 2;2;

	// --- Working version for homogenous VBO, displays full quad without any hole ---
	//IBOSize = (SizeY * 2) * (SizeX - 1) + (SizeX - 1);

 //   ClipmapIBOsData[Module] = new unsigned int[IBOSize];

	//for (int x = 0; x < SquaresNum; x++)
 //   {
 //       for (int y = 0; y < SizeY; y++)
 //       {
 //           ClipmapIBOsData[Module][CurrentIndex++] = x * SizeX + y;
 //           ClipmapIBOsData[Module][CurrentIndex++] = (x+1) * SizeX + y;
 //       }
 //       ClipmapIBOsData[Module][CurrentIndex++] = RestartIndex;
 //   }




	IBOSize[Module] = (2 * ((SizeY * 2) + 1) + 2 * (((MissingVerticesX + 2) * 2) + 1)) * (RimVerticesX - 1);

	//IBOSize = ((SizeY * 2) + 1) * (RimVerticesX - 1) +
	//		  (((MissingVerticesX + 2) * 2) + 1) * (RimVerticesX - 1) +
	//		  (((MissingVerticesX + 2) * 2) + 1) * (RimVerticesX - 1) +
	//		  ((SizeY * 2) + 1) * (RimVerticesX - 1);



	switch (Module)
	{
	case CLIPMAP_CENTER:
	{
		ClipmapIBOsData[Module] = new unsigned int[IBOSize[Module]];

		for (int x = 0; x < RimVerticesX-1; x++)
		{
			for (int y = 0; y < SizeY; y++)
			{
				ClipmapIBOsData[Module][CurrentIndex++] = x * SizeX + y;
				ClipmapIBOsData[Module][CurrentIndex++] = (x+1) * SizeX + y;
			}
			ClipmapIBOsData[Module][CurrentIndex++] = RestartIndex;
		}

		for (int y = 0; y < RimVerticesX-1; y++)
		{
			int Offset = 0;

			for (int x = RimVerticesX-1; x < RimVerticesX + MissingVerticesX + 1; x++)
			{
				if (x > RimVerticesX)
					Offset += MissingVerticesX;

				ClipmapIBOsData[Module][CurrentIndex++] = x * SizeX + y + 1 - Offset;
				ClipmapIBOsData[Module][CurrentIndex++] = x * SizeX + y - Offset;
			}
			ClipmapIBOsData[Module][CurrentIndex++] = RestartIndex;
		}

		for (int y = RimVerticesX + MissingVerticesX; y < SizeX-1; y++)
		{
			int Offset = 0;

			for (int x = RimVerticesX-1; x < RimVerticesX + MissingVerticesX + 1; x++)
			{
				if (x > RimVerticesX-1 && x != RimVerticesX + MissingVerticesX)
					Offset += MissingVerticesX;

				ClipmapIBOsData[Module][CurrentIndex++] = x * SizeX + y + 1 - Offset;
				ClipmapIBOsData[Module][CurrentIndex++] = x * SizeX + y - Offset;
			}
			ClipmapIBOsData[Module][CurrentIndex++] = RestartIndex;
		}

		int Offset = pow(MissingVerticesX, 2.0f);

		for (int x = RimVerticesX + MissingVerticesX; x < SizeX-1; x++)
		{
			for (int y = 0; y < SizeY; y++)
			{
				ClipmapIBOsData[Module][CurrentIndex++] = x * SizeX + y - Offset;
				ClipmapIBOsData[Module][CurrentIndex++] = (x+1) * SizeX + y - Offset;
			}
			ClipmapIBOsData[Module][CurrentIndex++] = RestartIndex;
		}


		break;
	}
	case CLIPMAP_STRIP_10:
	{

		ClipmapIBOsData[Module] = new unsigned int[IBOSize[Module]];

		for (int x = 0; x < RimVerticesX-1; x++)
		{
			for (int y = 0; y < SizeY; y++)
			{
				ClipmapIBOsData[Module][CurrentIndex++] = x * SizeX + y;
				ClipmapIBOsData[Module][CurrentIndex++] = (x+1) * SizeX + y;
			}
			ClipmapIBOsData[Module][CurrentIndex++] = RestartIndex;
		}

		for (int y = 0; y < RimVerticesX-1; y++)
		{
			int Offset = 0;

			for (int x = RimVerticesX-1; x < RimVerticesX + MissingVerticesX + 1; x++)
			{
				if (x > RimVerticesX)
					Offset += MissingVerticesX;

				ClipmapIBOsData[Module][CurrentIndex++] = x * SizeX + y + 1 - Offset;
				ClipmapIBOsData[Module][CurrentIndex++] = x * SizeX + y - Offset;
			}
			ClipmapIBOsData[Module][CurrentIndex++] = RestartIndex;
		}

		for (int y = RimVerticesX + MissingVerticesX; y < SizeX-1; y++)
		{
			int Offset = 0;

			for (int x = RimVerticesX-1; x < RimVerticesX + MissingVerticesX + 1; x++)
			{
				if (x > RimVerticesX-1 && x != RimVerticesX + MissingVerticesX)
					Offset += MissingVerticesX;

				ClipmapIBOsData[Module][CurrentIndex++] = x * SizeX + y + 1 - Offset;
				ClipmapIBOsData[Module][CurrentIndex++] = x * SizeX + y - Offset;
			}
			ClipmapIBOsData[Module][CurrentIndex++] = RestartIndex;
		}

		int Offset = pow(MissingVerticesX, 2.0f);

		for (int x = RimVerticesX + MissingVerticesX; x < SizeX-1; x++)
		{
			for (int y = 0; y < SizeY; y++)
			{
				ClipmapIBOsData[Module][CurrentIndex++] = x * SizeX + y - Offset;
				ClipmapIBOsData[Module][CurrentIndex++] = (x+1) * SizeX + y - Offset;
			}
			ClipmapIBOsData[Module][CurrentIndex++] = RestartIndex;
		}



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
float * Landscape::GetClipmapVBOData(ClipmapModule Module, int &outDataAmount)
{
	outDataAmount = VBOSize[Module];
    return ClipmapVBOsData[Module];
}

// --------------------------------------------------------------------
unsigned int * Landscape::GetClipmapIBOData(ClipmapModule Module, int &outDataAmount)
{
	outDataAmount = IBOSize[Module];
    return ClipmapIBOsData[Module];
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