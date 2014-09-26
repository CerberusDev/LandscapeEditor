// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date: 23.03.2013
// --------------------------------------------------------------------
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "wx/wx.h"
#include "wx/glcanvas.h"

#include "Landscape.h"
#include "TextureManager.h"
#include "Brush.h"
#include "LandscapeShader.h"
#include "LightningOnlyShader.h"
#include "HeightShader.h"
#include "WireframeShader.h"
#include "ClipmapWireframeShader.h"
#include "ClipmapLandscapeShader.h"

using namespace glm;

enum DisplayMode {LANDSCAPE, WIREFRAME};
enum MovementMode {FREE_CAMERA, ATTACHED_TO_TERRAIN};

/** the rendering context used by all GL canvases */
class LandGLContext : public wxGLContext
{
protected:
    /// Shaders!
    LightningOnlyShader LightningOnlyShad;
    HeightShader HeightShad;
    WireframeShader WireframeShad;
    LandscapeShader LandscapeShad;
	ClipmapWireframeShader ClipmapWireframeShad;
	ClipmapLandscapeShader ClipmapLandscapeShad;

	DisplayMode CurrentDisplayMode;
	MovementMode CurrentMovementMode;

	int ClipmapsAmount;

    /// Buffer objects
	GLuint VBO;
	GLuint *IBOs;
	GLuint *TBOs;

    /// Textures
    GLuint LandscapeTexture, BrushTexture, SoilTexture;

    /// Pipelinie transformation matrices
    mat4 Model, View, Projection;

    /// Amount of all landscape vertices
	int *IBOLengths;

    /// Brush object
    Brush CurrentBrush;

    /// Camera Position
    vec3 CameraPosition;

    /// Camera angles
    float CameraVerticalAngle, CameraHorizontalAngle;

    /// Camera moving speed
    float CameraSpeed;
	float MovementModifier;

    /// Mouse intensity
    float MouseIntensity;

    /// Key table, true = key is down, false = key is up, [0]=W [1]=S [2]=D [3]=A [4]=up [5]=down [6]=right [7]=left [8]=space [9]=LMB
    bool Keys[10];

    /// Pointer to current landscape object
    Landscape *CurrentLandscape;

	float OffsetX;
	float OffsetY;

	int StartIndexX;
	int StartIndexY;

	float *DATA;
	int DataSize;

	ClipmapStripPair *VisibleClipmapStrips;

	LARGE_INTEGER frequency;
	float programStartMoment;
	bool usingHighFrequencyCounter;   

	bool bNewLandscape;

public:
    /// Standard constructor/destructor
    LandGLContext(wxGLCanvas *canvas);
    ~LandGLContext(void);

    /// Draw debug line
    void DrawDebugLine();

    /// Render the scene
    void DrawScene();

    /// Called from LandGLCanvas when key action occurs
    void OnKey(bool bKeyIsDown, wxKeyEvent& event);

    /// Called from LandGLCanvas when mouse event comes
    void OnMouse(wxMouseEvent& event);

    /// Called from LandGLCanvas when mouse size event comes
    void OnResize(wxSize NewSize);

    /// Call this function regulary, calculate the input and moves camera
    void ManageInput();

    /// Create new landscape
    void CreateNewLandscape(int Size);

    /// Save landscape
    void SaveLandscape(const char* FilePath);

    /// Open landscape from file
    void OpenFromFile(const char* FilePath);

    /// Manage fatal errors
    void FatalError(char* text = 0);

    /// Call when you want to change current brush mode
    void ChangeBrushMode(int NewMode) {CurrentBrush.SetMode(NewMode);};

protected:
    /// Reset camera to default position
    void ResetCamera();

    /// Reset buffers, needed when new terrain is setting up
    void ResetAllVBOIBO();

    /// Reset TBO
	void UpdateTBO();

	/// Set vertical synchronization status
	void SetVSync(bool sync);

	void InitTBO(GLuint TBOID, int ClipmapScale = 1);
	void SetShadersInitialUniforms();
	void RenderLandscapeModule(const ClipmapIBOMode IBOMode, GLuint TBOID);
	void ResetVBO(GLuint &BufferID, float *NewData, int DataSize);
	void ResetIBO(GLuint &BufferID, unsigned int *NewData, int DataSize);
	float getSecond();
};