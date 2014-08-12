// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date: 23.03.2013
// --------------------------------------------------------------------
#pragma once

#include "Brush.h"

enum ClipmapModule		{CLIPMAP_CENTER, 
						CLIPMAP_MODULES_AMOUNT};

enum ClipmapStripPair	{CLIPMAP_STRIP_1, 
						CLIPMAP_STRIP_2, 
						CLIPMAP_STRIP_3, 
						CLIPMAP_STRIP_4, 
						CLIPMAP_STRIP_PAIRS_AMOUNT};

/** the main terrain class */
class Landscape
{
public:
    /// Index used for primitive restart when drawing
    const unsigned int RestartIndex;

protected:
    /// Distance between two adjacent vertices
    float Offset;

    /// Array with heigh vertices data
    float *TrueHeightmap;

	float **ClipmapVBOsData;
	unsigned int **ClipmapIBOsData;
	unsigned int *ClipmapVBOsSizeX;
	unsigned int *ClipmapVBOsSizeY;
	unsigned int TBOSize;

	unsigned int VBOSize;
	unsigned int IBOSize;

public:
    /// Standard constructors and destructor
    Landscape(int VerticesAmountX);
    Landscape(const char* FilePath);
    ~Landscape();

    /// Save heightmap to file, return true if succeeded
    bool SaveToFile(const char* FilePath);

    /// Change landscape height data
    void UpdateHeightmap(Brush &AffectingBrush);

    /// Getters
	float * GetClipmapVBOData(ClipmapModule Module, int &outDataAmount);
	unsigned int * GetClipmapIBOData(ClipmapModule Module, int &outDataAmount);
	unsigned int GetClipmapVBOSizeX(ClipmapModule Module) {return ClipmapVBOsSizeX[Module];};
	unsigned int GetClipmapVBOSizeY(ClipmapModule Module) {return ClipmapVBOsSizeY[Module];};
	unsigned int GetTBOSize() {return TBOSize;};
    float * GetHeightmap(int &VerticesAmountX);
    float GetOffset() {return Offset;};

	// Not working assignment operator
	Landscape & operator= (Landscape & other) {return other;};
   
protected: 
	void CreateVBO(ClipmapModule Module);
	void CreateIBO(ClipmapModule Module);
};