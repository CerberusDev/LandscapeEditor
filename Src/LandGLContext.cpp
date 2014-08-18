// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date: 23.03.2013
// --------------------------------------------------------------------

#include "LandGLContext.h"
#include "LandscapeEditor.h"


// --------------------------------------------------------------------
static void CheckGLError()
{
    GLenum errLast = GL_NO_ERROR;

    for ( ;; )
    {
        GLenum err = glGetError();
        if ( err == GL_NO_ERROR )
            return;

        // normally the error is reset by the call to glGetError() but if
        // glGetError() itself returns an error, we risk looping forever here
        // so check that we get a different error than the last time
        if ( err == errLast )
        {
            //wxLogError(wxT("OpenGL error state couldn't be reset."));
            return;
        }

        errLast = err;
        ERR("OpenGL error " << err);
    }
}

// --------------------------------------------------------------------
float LandGLContext::getSecond()
{
	return timeGetTime() / 1000.0f - programStartMoment; // Returns: current time - program start moment = time since program start.
}

// --------------------------------------------------------------------
LandGLContext::LandGLContext(wxGLCanvas *canvas):
wxGLContext(canvas), MouseIntensity(350.0f), CurrentLandscape(0), LandscapeTexture(0), BrushTexture(1), SoilTexture(3), CameraSpeed(0.2f),
OffsetX(0.0001f), OffsetY(0.0001f), ClipmapsAmount(0), VBOs(0), IBOs(0), TBOs(0), TBO(0), IBOLengths(0), VisibleCenterClipmapStrip(CLIPMAP_STRIP_1), 
VisibleClipmapStrips(0), CurrentDisplayMode(WIREFRAME), NORMALS_0(0), NORMALS_1(0), NORMALS_2(0), DATA(0), CurrentMovementMode(ATTACHED_TO_TERRAIN), 
MovementModifier(10.0f), bNewLandscape(false)
{
	programStartMoment = timeGetTime() / 1000.0f;
	DataSize = 424;
	DATA = new float[DataSize * DataSize];

	LOG("Generating terrain data...");

	for (int i = 0; i < DataSize; ++i)
	{
		for (int j = 0; j < DataSize; ++j)
		{
			//DATA[i + DataSize * j] = j / 10.0f;
			//DATA[i + DataSize * j] = (i % 32) / 8.0f + 430.0f;
			//DATA[i + DataSize * j] = (j % 512 == 113 || i % 512 == 113) ? (20.0f) : (0.0f);
			DATA[i + DataSize * j] = 0.0f;
			//DATA[i + DataSize * j] = sin(float(i) / 4.0f) / 2.4f + sin(float(j) / 11.f) / 1.6f;

			//DATA[i + DataSize * j] = sin(float(j) / 400.f) * 80.0f + 300.0f;

			//float a = sin(float(i) / (1.0 * 704.0f)) * 20.0f;
			//float b = sin(float(i) / (1.0 * 352.0f)) * 20.0f;
			//float c = (sin(float(j) / (1.0 * 469.4f)) - (cos(float(j) / (1.0 * 234.7f)) + 1.0f) / 4.5f) * 30.0f;
			//float d = sin(float(j) / (2.0 * 58.f)) * 3.0f + sin(float(i) / (2.0 * 122.f)) * 5.0f;
			//float e = sin(float(i) / (3.0 * 2.0f)) * 0.8f * cos(float(j) / (3.0 * 6.2f)) * 0.6f + sin(float(j) / (3.0 * 2.3f)) * 0.8f * cos(float(i) / (3.0 * 6.4f)) * 0.5f;
			//float f = (rand() % 100) / 1500.0f;
			//DATA[i + DataSize * j] = a + b + c + d + e + f;
		}

		//if (DataSize > 10 && i % (DataSize / 10) == 0)
		//	LOG("%d%% ", i / (DataSize / 10) * 10);
	}


	LOG("Terrain Ready!\n");

	StartIndexX = 200.0f;
	StartIndexY = 200.0f;

	VisibleClipmapStrips = new ClipmapStripPair[ClipmapsAmount];

	for (int i = 0; i < ClipmapsAmount; ++i)
		VisibleClipmapStrips[i] = CLIPMAP_STRIP_1;

	VBOs = new GLuint[CLIPMAP_MODULES_AMOUNT];
	IBOs = new GLuint[CLIPMAP_MODULES_AMOUNT];
	IBOLengths = new int[CLIPMAP_MODULES_AMOUNT];

	for (int i = 0; i < CLIPMAP_MODULES_AMOUNT; ++i)
	{
		VBOs[i] = 0;
		IBOs[i] = 0;
		IBOLengths[i] = 0;
	}

    for (int i = 0; i < sizeof(Keys); ++i)
        Keys[i] = false;

    SetCurrent(*canvas);
    ((LandGLCanvas*)canvas)->SetOpenGLContext(this);
    
    if (glewInit() == GLEW_OK)
		LOG("GLEW initialized");
	else 
		ERR("Failed to initialize GLEW!");

    CurrentLandscape = new Landscape(3);
    LOG("Initial Landscape created");

	LOG("Generating normals...");
	NORMALS_0 = new float[DataSize * DataSize * 3];
	NORMALS_1 = new float[DataSize * DataSize * 3];
	NORMALS_2 = new float[DataSize * DataSize * 3];

	for (int i = 0; i < DataSize * DataSize * 3; ++i)
		NORMALS_0[i] = NORMALS_1[i] = NORMALS_2[i] = 0.0f;

	for (int i = 1; i < DataSize - 1; ++i)
		for (int j = 1; j < DataSize - 1; ++j)
		{ 
			vec3 v1 = vec3(0.0f, CurrentLandscape->GetOffset() * 2.0f, DATA[i * DataSize + j - 1] - DATA[i * DataSize + j + 1]);
			vec3 v2 = vec3(CurrentLandscape->GetOffset() * 2.0f, 0.0f, DATA[(i - 1) * DataSize + j] - DATA[(i + 1) * DataSize + j]);

			vec3 n = normalize(cross(v2, v1));

			NORMALS_0[i * DataSize * 3 + j * 3] = n.x;
			NORMALS_0[i * DataSize * 3 + j * 3 + 1] = n.y;
			NORMALS_0[i * DataSize * 3 + j * 3 + 2] = n.z;
		}

	for (int i = 8; i < DataSize - 2; ++i)
		for (int j = 8; j < DataSize - 2; ++j)
		{ 

			vec3 v1 = vec3(0.0f, CurrentLandscape->GetOffset() * 4.0f, DATA[i * DataSize + j - 2] - DATA[i * DataSize + j + 2]);
			vec3 v2 = vec3(CurrentLandscape->GetOffset() * 4.0f, 0.0f, DATA[(i - 2) * DataSize + j] - DATA[(i + 2) * DataSize + j]);

			vec3 n = normalize(cross(v2, v1));

			NORMALS_1[i * DataSize * 3 + j * 3] = n.x;
			NORMALS_1[i * DataSize * 3 + j * 3 + 1] = n.y;
			NORMALS_1[i * DataSize * 3 + j * 3 + 2] = n.z;
		}

	for (int i = 16; i < DataSize - 16; ++i)
		for (int j = 16; j < DataSize - 16; ++j)
		{ 
			vec3 v1 = vec3(0.0f, CurrentLandscape->GetOffset() * 32.0f, DATA[i * DataSize + j - 16] - DATA[i * DataSize + j + 16]);
			vec3 v2 = vec3(CurrentLandscape->GetOffset() * 32.0f, 0.0f, DATA[(i - 16) * DataSize + j] - DATA[(i + 16) * DataSize + j]);

			vec3 n = normalize(cross(v2, v1));

			NORMALS_2[i * DataSize * 3 + j * 3] = n.x;
			NORMALS_2[i * DataSize * 3 + j * 3 + 1] = n.y;
			NORMALS_2[i * DataSize * 3 + j * 3 + 2] = n.z;
		}

	LOG("Normals Ready!");

    glClearColor(0.6f, 0.85f, 0.9f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(CurrentLandscape->RestartIndex);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    mat4 Scale = scale(1.0f, 1.0f, 1.0f);
    mat4 Rotate = rotate(0.0f, vec3(0.0f, 1.0f, 0.0f));
    mat4 Translate = translate(0.0f, 0.0f, 0.0f);
	Model = Translate * Rotate * Scale;

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);

	Projection = perspective(90.0f, ((float)viewport[2] / (float)viewport[3]), 0.1f, 100000.0f);
    LOG("Matrices calculated");

    ResetCamera();
    ResetAllVBOIBO();

    if (LandscapeShad.Initialize("Landscape") == false)
        FatalError("Landscape Shader init failed");
    if (LightningOnlyShad.Initialize("LightningOnly") == false)
        FatalError("Lightning Only Shader init failed");
    if (HeightShad.Initialize("Height") == false)
        FatalError("Height Shader init failed");
    if (WireframeShad.Initialize("Wireframe") == false)
        FatalError("Wireframe Shader init failed");
	if (ClipmapWireframeShad.Initialize("ClipmapWireframe") == false)
        FatalError("Clipmap Wireframe Shader init failed");
	if (ClipmapLandscapeShad.Initialize("ClipmapLandscape") == false)
        FatalError("Clipmap Landscape Shader init failed");
    
	SetShadersInitialUniforms();

    LandscapeShad.Use();

    LOG("Loading textures...");

    // ----------------------------- Landscape Texture --------------------------------
    glActiveTexture(GL_TEXTURE0);
    if (TextureManager::Inst()->LoadTexture("Content/Textures/grass.tga", LandscapeTexture))
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
		glGenerateMipmap(GL_TEXTURE_2D);

		LandscapeShad.SetTextureSampler(0);
	}
	else
	{
		WARN("Can't load grass.tga texture!");
	}

    // ----------------------------- Brush Texture --------------------------------
    glActiveTexture(GL_TEXTURE1);
    if (TextureManager::Inst()->LoadTexture("Content/Textures/Brush2a.png", BrushTexture, GL_RGBA, GL_RGBA))
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
		glGenerateMipmap(GL_TEXTURE_2D);

		LandscapeShad.SetBrushTextureSampler(1);
	}
	else
	{
		WARN("Can't load Brush2a.png texture!");
	}


    // ----------------------------- Soil Texture --------------------------------
    glActiveTexture(GL_TEXTURE3);
    if (TextureManager::Inst()->LoadTexture("Content/Textures/smallrocks.tga", SoilTexture))
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
		glGenerateMipmap(GL_TEXTURE_2D);

		//LandscapeShad.SetSoilTexture(3);
	}
	else
	{
		WARN("Can't load smallrocks.tga texture!");
	}

    LOG("Textures loaded");

    CurrentBrush.SetPosition(vec3(0.0f));

	// ----------------------------- Texture Buffer Objects (TBOs) --------------------------------

    glGenBuffers(1, &TBO);
	InitTBO(TBO, true);

	TBOs = new GLuint[ClipmapsAmount];
	glGenBuffers(ClipmapsAmount, TBOs);

	int ClipmapScale = 1;

	for (int i = 0; i < ClipmapsAmount; ++i)
	{
		ClipmapScale *= 2;
		InitTBO(TBOs[i], false, ClipmapScale);
	}

    // ---------------------------------- Uniforms -------------------------------------------
    //LandscapeShad.SetBrushPosition(CurrentBrush.GetRenderPosition());
    //LandscapeShad.SetBrushScale(CurrentBrush.GetRadius() * 2.0f);
    //LandscapeShad.SetLandscapeSizeX(CurrentLandscape->GetCenterSize());
    //LandscapeShad.SetLandscapeVertexOffset(CurrentLandscape->GetOffset());
    //LandscapeShad.SetAmbientStrength(0.15f);
    //LandscapeShad.SetDiffuseStrength(1.0f);
    
    CheckGLError();

    ((LandGLCanvas*)canvas)->SetOpenGLContextInitialized(true);

	SetVSync(false);
	CONF("==== Initialization completed! ====");
}

// --------------------------------------------------------------------
LandGLContext::~LandGLContext(void)
{
	glDeleteBuffers(CLIPMAP_MODULES_AMOUNT, VBOs);
	glDeleteBuffers(CLIPMAP_MODULES_AMOUNT, IBOs);
	glDeleteBuffers(1, &TBO);
	glDeleteBuffers(ClipmapsAmount, TBOs);

	glDeleteTextures(1, &LandscapeTexture);
    glDeleteTextures(1, &SoilTexture);
    glDeleteTextures(1, &BrushTexture);

	delete[] DATA;
	delete[] NORMALS_0;
	delete[] VBOs;
	delete[] IBOs;
	delete[] IBOLengths;
}

// --------------------------------------------------------------------
void LandGLContext::SetShadersInitialUniforms()
{
	WireframeShad.Use();
    WireframeShad.SetBrushTextureSampler(1);
    WireframeShad.SetTBOSampler(2);
    WireframeShad.SetBrushPosition(vec2(0.0f, 0.0f));
    WireframeShad.SetBrushScale(1.0f);
    WireframeShad.SetLandscapeSizeX(CurrentLandscape->GetClipmapVBOSizeX(CLIPMAP_CENTER) + 1);
    WireframeShad.SetLandscapeVertexOffset(CurrentLandscape->GetOffset());
    WireframeShad.SetWireframeColor(vec3(0.0f, 0.0f, 0.0f));
    WireframeShad.SetBrushColor(vec3(1.0f, 1.0f, 1.0f));
	WireframeShad.SetTestOffsetX(0.0f);
	WireframeShad.SetTestOffsetY(0.0f);
	WireframeShad.SetgWorld(mat4(0.0f));
	WireframeShad.SetClipmapPartOffset(vec2(0.0f, 0.0f));

	ClipmapWireframeShad.Use();
	ClipmapWireframeShad.SetBrushTextureSampler(1);
	ClipmapWireframeShad.SetTBOSampler(2);
    ClipmapWireframeShad.SetBrushPosition(CurrentBrush.GetRenderPosition());
    ClipmapWireframeShad.SetBrushScale(CurrentBrush.GetRadius() * 2.0f);
    ClipmapWireframeShad.SetLandscapeVertexOffset(CurrentLandscape->GetOffset());
    ClipmapWireframeShad.SetBrushColor(vec3(1.0f, 1.0f, 1.0f));
	ClipmapWireframeShad.SetWireframeColor(vec3(0.6f, 0.0f, 0.0f));
	ClipmapWireframeShad.SetTestOffsetX(0.0f);
	ClipmapWireframeShad.SetTestOffsetY(0.0f);
	ClipmapWireframeShad.SetgWorld(mat4(0.0f));
	ClipmapWireframeShad.SetClipmapScale(1.0f);
	ClipmapWireframeShad.SetClipmapSizeX(CurrentLandscape->GetTBOSize());
	ClipmapWireframeShad.SetClipmapSizeY(CurrentLandscape->GetTBOSize());
	ClipmapWireframeShad.SetClipmapPartOffset(vec2(0.0f, 0.0f));

	LandscapeShad.Use();
    LandscapeShad.SetBrushTextureSampler(1);
    LandscapeShad.SetTBOSampler(2);
    LandscapeShad.SetBrushPosition(vec2(0.0f, 0.0f));
    LandscapeShad.SetBrushScale(1.0f);
    LandscapeShad.SetLandscapeSizeX(CurrentLandscape->GetClipmapVBOSizeX(CLIPMAP_CENTER) + 1);
    LandscapeShad.SetLandscapeVertexOffset(CurrentLandscape->GetOffset());
    LandscapeShad.SetWireframeColor(vec3(0.0f, 0.0f, 0.0f));
    LandscapeShad.SetBrushColor(vec3(1.0f, 1.0f, 1.0f));
	LandscapeShad.SetTestOffsetX(0.0f);
	LandscapeShad.SetTestOffsetY(0.0f);
	LandscapeShad.SetgWorld(mat4(0.0f));
	LandscapeShad.SetClipmapPartOffset(vec2(0.0f, 0.0f));
	LandscapeShad.SetTextureSampler(0);

	ClipmapLandscapeShad.Use();
    ClipmapLandscapeShad.SetBrushTextureSampler(1);
    ClipmapLandscapeShad.SetTBOSampler(2);
    ClipmapLandscapeShad.SetBrushPosition(CurrentBrush.GetRenderPosition());
    ClipmapLandscapeShad.SetBrushScale(CurrentBrush.GetRadius() * 2.0f);
    ClipmapLandscapeShad.SetLandscapeVertexOffset(CurrentLandscape->GetOffset());
    ClipmapLandscapeShad.SetBrushColor(vec3(1.0f, 1.0f, 1.0f));
	ClipmapLandscapeShad.SetWireframeColor(vec3(0.6f, 0.0f, 0.0f));
	ClipmapLandscapeShad.SetTestOffsetX(0.0f);
	ClipmapLandscapeShad.SetTestOffsetY(0.0f);
	ClipmapLandscapeShad.SetgWorld(mat4(0.0f));
	ClipmapLandscapeShad.SetClipmapScale(1.0f);
	ClipmapLandscapeShad.SetClipmapSizeX(CurrentLandscape->GetTBOSize());
	ClipmapLandscapeShad.SetClipmapSizeY(CurrentLandscape->GetTBOSize());
	ClipmapLandscapeShad.SetClipmapPartOffset(vec2(0.0f, 0.0f));
	ClipmapLandscapeShad.SetTextureSampler(0);
}

// --------------------------------------------------------------------
void LandGLContext::InitTBO(GLuint TBOID, const bool bClipmapCenter, int ClipmapScale)
{
	int TBOSize = CurrentLandscape->GetTBOSize();

	glActiveTexture(GL_TEXTURE2);
	float *Data = new float[TBOSize * TBOSize* 4];

	float *NORMALS = 0;

	if (ClipmapScale > 128)
		NORMALS = NORMALS_2;
	else if (ClipmapScale > 32)
		NORMALS = NORMALS_1;
	else
		NORMALS = NORMALS_0;

	for (int x = 0; x < TBOSize; ++x)
		for (int y = 0; y < TBOSize; ++y)
			if (bClipmapCenter)
			{
				int IndexX = int(mod(float(x + StartIndexX + 1 - TBOSize), float(DataSize)));
				int IndexY = int(mod(float(y + StartIndexY + 1 - TBOSize), float(DataSize))); 

				Data[4 * (y * TBOSize + x)] = NORMALS[3 * (IndexY * DataSize + IndexX)];
				Data[4 * (y * TBOSize + x) + 1] = NORMALS[3 * (IndexY * DataSize + IndexX) + 1];
				Data[4 * (y * TBOSize + x) + 2] = NORMALS[3 * (IndexY * DataSize + IndexX) + 2];
				Data[4 * (y * TBOSize + x) + 3] = DATA[(IndexY * DataSize + IndexX)];
			}
			else
			{
				int IndexX = int(mod(float(StartIndexX + ClipmapScale + (32 * (ClipmapScale - 1)) + (x - TBOSize) * ClipmapScale), float(DataSize)));
				int IndexY = int(mod(float(StartIndexY + ClipmapScale + (32 * (ClipmapScale - 1)) + (y - TBOSize) * ClipmapScale), float(DataSize)));

				Data[4 * (y * TBOSize + x)] = NORMALS[3 * (IndexY * DataSize + IndexX)];
				Data[4 * (y * TBOSize + x) + 1] = NORMALS[3 * (IndexY * DataSize + IndexX) + 1];
				Data[4 * (y * TBOSize + x) + 2] = NORMALS[3 * (IndexY * DataSize + IndexX) + 2];
				Data[4 * (y * TBOSize + x) + 3] = DATA[IndexY * DataSize + IndexX];
			}
			
	glBindBuffer(GL_TEXTURE_BUFFER, TBOID);
	glBufferData(GL_TEXTURE_BUFFER, 4 * TBOSize * TBOSize * sizeof(float), Data, GL_STATIC_DRAW);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, TBOID);

	delete[] Data;
}

// --------------------------------------------------------------------
void LandGLContext::DrawScene()
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 MVP = Projection * View * Model;

    CheckGLError();

    glActiveTexture(GL_TEXTURE2);
    glEnableVertexAttribArray(0);

	// ------------------ Render Center Clipmap module ------------------

	//switch (CurrentDisplayMode)
	//{
	//case LANDSCAPE:
	//	LandscapeShad.Use();
	//	LandscapeShad.SetWireframeColor(vec3(0.0f, 0.0f, 0.0f));
	//	LandscapeShad.SetTestOffsetX(OffsetX);
	//	LandscapeShad.SetTestOffsetY(OffsetY);
	//	LandscapeShad.SetMVP(MVP);
	//	LandscapeShad.SetClipmapPartOffset(vec2((mod(OffsetX, 2.0f) < 1.0f) ? (0.0f) : (1.0f), (mod(OffsetY, 2.0f) < 1.0f) ? (0.0f) : (1.0f)));
	//	break;
	//case WIREFRAME:
	//	WireframeShad.Use();
	//	WireframeShad.SetWireframeColor(vec3(0.0f, 0.0f, 0.0f));
	//	WireframeShad.SetTestOffsetX(OffsetX);
	//	WireframeShad.SetTestOffsetY(OffsetY);
	//	WireframeShad.SetMVP(MVP);
	//	WireframeShad.SetClipmapPartOffset(vec2((mod(OffsetX, 2.0f) < 1.0f) ? (0.0f) : (1.0f), (mod(OffsetY, 2.0f) < 1.0f) ? (0.0f) : (1.0f)));
	//	break;
	//}

	ClipmapWireframeShad.Use();
	ClipmapWireframeShad.SetgWorld(MVP);
	ClipmapWireframeShad.SetWireframeColor(vec3(0.0f, 0.0f, 0.0f));

	ClipmapWireframeShad.SetClipmapScale(1.0f);
	RenderLandscapeModule(CLIPMAP_CENTER, TBO);

	ClipmapWireframeShad.SetClipmapScale(2.0f);
	RenderLandscapeModule(CLIPMAP_CENTER, TBO);

	ClipmapWireframeShad.SetClipmapScale(4.0f);
	RenderLandscapeModule(CLIPMAP_CENTER, TBO);

	ClipmapWireframeShad.SetClipmapScale(8.0f);
	RenderLandscapeModule(CLIPMAP_CENTER, TBO);

    glDisableVertexAttribArray(0);

    glFlush();
    CheckGLError();
}

// --------------------------------------------------------------------
void LandGLContext::RenderLandscapeModule(const ClipmapModule Module, GLuint TBOID)
{
	glBindBuffer(GL_TEXTURE_BUFFER, TBOID);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, TBOID);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[Module]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOs[Module]);

	glDrawElements(GL_TRIANGLE_STRIP, IBOLengths[Module], GL_UNSIGNED_INT, 0);
}

// --------------------------------------------------------------------
void LandGLContext::SetVSync(bool sync)
{	
	// Function pointer for the wgl extention function we need to enable/disable
	// vsync
	typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALPROC)( int );
	PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;

	const char *extensions = (char*)glGetString( GL_EXTENSIONS );

	if( strstr( extensions, "WGL_EXT_swap_control" ) == 0 )
	{
		return;
	}
	else
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

		if( wglSwapIntervalEXT )
			wglSwapIntervalEXT(sync);
	}
}

// --------------------------------------------------------------------
void LandGLContext::OnKey(bool bKeyIsDown, wxKeyEvent& event)
{
	static float MovementSpeed = 0.0f;

    switch (event.GetKeyCode())
    {
        case WXK_UP:
            Keys[4] = bKeyIsDown;
			//if (bKeyIsDown)
			//{
			//	OffsetY += 200.0f;
			//	UpdateTBO();
			//}
            break;

        case WXK_DOWN:
            Keys[5] = bKeyIsDown;
			//if (bKeyIsDown)
			//{
			//	OffsetY -= 8.0f;
			//	UpdateTBO();
			//}
            break;

        case WXK_RIGHT:
            Keys[6] = bKeyIsDown;
			//if (bKeyIsDown)
			//{
			//	OffsetX -= MovementSpeed;
			//	UpdateTBO();
			//}
            break;

		case WXK_LEFT:
            Keys[7] = bKeyIsDown;
			//if (bKeyIsDown)
			//{
			//	OffsetX += MovementSpeed;
			//	UpdateTBO();
			//}
            break;

		case (16 * wxMOD_SHIFT + WXK_CONTROL_W):
            Keys[0] = bKeyIsDown;
            break;


        case (16 * wxMOD_SHIFT + WXK_CONTROL_S):
            Keys[1] = bKeyIsDown;
            break;


        case (16 * wxMOD_SHIFT + WXK_CONTROL_D):
            Keys[2] = bKeyIsDown;
            break;

        case (16 * wxMOD_SHIFT + WXK_CONTROL_A):
            Keys[3] = bKeyIsDown;
            break;
       
        case WXK_F1:
            if (bKeyIsDown)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                CurrentDisplayMode = LANDSCAPE;
            }
            break;
        case WXK_F2:
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                CurrentDisplayMode = WIREFRAME;
            }
            break;
        case WXK_SPACE:
            Keys[8] = bKeyIsDown;
            break;
    }
}

// --------------------------------------------------------------------
void LandGLContext::OnResize(wxSize NewSize)
{
    glViewport(0, 0, NewSize.x, NewSize.y);
	Projection = perspective(90.0f, ( (float)NewSize.x / (float)NewSize.y), 0.1f, 100000.0f);
}

// --------------------------------------------------------------------
void LandGLContext::OnMouse(wxMouseEvent& event)
{
    static float StartX = 0.0f, StartY = 0.0f;

    if (event.RightIsDown())
    {
        if (event.RightDown())
        {
            LandscapeEditor::Inst()->Frame->SetCursor(wxCursor(wxCURSOR_BLANK));

            StartX = event.GetX();
            StartY = event.GetY();
        }

        CameraHorizontalAngle -= (event.GetX() - StartX) / MouseIntensity;
        CameraVerticalAngle -= (event.GetY() - StartY) / MouseIntensity;

        LandscapeEditor::Inst()->Frame->WarpPointer(StartX, StartY);

        vec3 Direction(cos(CameraVerticalAngle) * sin(CameraHorizontalAngle), sin(CameraVerticalAngle), cos(CameraVerticalAngle) * cos(CameraHorizontalAngle));
        vec3 Right = vec3(sin(CameraHorizontalAngle - 3.14f/2.0f), 0, cos(CameraHorizontalAngle - 3.14f/2.0f));
        vec3 Up = cross(Right, Direction);
        View = lookAt(CameraPosition, CameraPosition + Direction, Up);
    }
    else
    {
        // Calculate new brush position
        GLfloat winZ;
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        glReadPixels(event.GetX(), viewport[3] - event.GetY(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
        vec3 screenPos = vec3(event.GetX(), viewport[3] - event.GetY(), winZ);
        vec3 worldPos = unProject(screenPos, View, Projection, vec4(0.0f, 0.0f, viewport[2], viewport[3]));

        CurrentBrush.SetPosition(worldPos);

        //if ((*CurrentShader) == LandscapeShad)
        //    LandscapeShad.SetBrushPosition(CurrentBrush.GetRenderPosition());
        //else if ((*CurrentShader) == LightningOnlyShad)
        //    LightningOnlyShad.SetBrushPosition(CurrentBrush.GetRenderPosition());
        //else if ((*CurrentShader) == HeightShad)
        //    HeightShad.SetBrushPosition(CurrentBrush.GetRenderPosition());
        //else if ((*CurrentShader) == WireframeShad)
        //    WireframeShad.SetBrushPosition(CurrentBrush.GetRenderPosition());
    }

    //if (event.GetWheelRotation() != 0)
    //{
    //    (event.GetWheelRotation() < 0) ? (CurrentBrush.ModifyRadius(1.1f)) : (CurrentBrush.ModifyRadius(0.9f));

    //    if ((*CurrentShader) == LandscapeShad)
    //    {
    //        LandscapeShad.SetBrushScale(CurrentBrush.GetRadius() * 2.0f);
    //        LandscapeShad.SetBrushPosition(CurrentBrush.GetRenderPosition());
    //    }
    //    else if ((*CurrentShader) == LightningOnlyShad)
    //    {
    //        LightningOnlyShad.SetBrushScale(CurrentBrush.GetRadius() * 2.0f);
    //        LightningOnlyShad.SetBrushPosition(CurrentBrush.GetRenderPosition());
    //    }
    //    else if ((*CurrentShader) == HeightShad)
    //    {
    //        HeightShad.SetBrushScale(CurrentBrush.GetRadius() * 2.0f);
    //        HeightShad.SetBrushPosition(CurrentBrush.GetRenderPosition());
    //    }
    //    else if ((*CurrentShader) == WireframeShad)
    //    {
    //        WireframeShad.SetBrushScale(CurrentBrush.GetRadius() * 2.0f);
    //        WireframeShad.SetBrushPosition(CurrentBrush.GetRenderPosition());
    //    }
    //}

    if (event.RightUp())
        LandscapeEditor::Inst()->Frame->SetCursor(wxNullCursor);

    if (event.LeftIsDown()) Keys[9] = true;
    else  Keys[9] = false;
}

// --------------------------------------------------------------------
void LandGLContext::ManageInput()
{
	static float MovementSpeed = 0.1f;

    if (Keys[0] || Keys[1] || Keys[2] || Keys[3])
    {
        vec3 Direction(cos(CameraVerticalAngle) * sin(CameraHorizontalAngle), sin(CameraVerticalAngle), cos(CameraVerticalAngle) * cos(CameraHorizontalAngle));
        vec3 Right = vec3(sin(CameraHorizontalAngle - 3.14f/2.0f), 0, cos(CameraHorizontalAngle - 3.14f/2.0f));
        vec3 Up = cross(Right, Direction);

		if (Keys[0])
		{
			OffsetX += ((Keys[8]) ? (MovementModifier) : (1.0f)) * MovementSpeed * sin(CameraHorizontalAngle);
			OffsetY += ((Keys[8]) ? (MovementModifier) : (1.0f)) * MovementSpeed * cos(CameraHorizontalAngle);
			CameraPosition.y += (Direction * (CameraSpeed * ((Keys[8]) ? (MovementModifier) : (1.0f)))).y;
		}
		if (Keys[1])
		{
			OffsetX -= ((Keys[8]) ? (MovementModifier) : (1.0f)) * MovementSpeed * sin(CameraHorizontalAngle);
			OffsetY -= ((Keys[8]) ? (MovementModifier) : (1.0f)) * MovementSpeed * cos(CameraHorizontalAngle);
			CameraPosition.y -= (Direction * (CameraSpeed * ((Keys[8]) ? (MovementModifier) : (1.0f)))).y;
		}
		if (Keys[2])
		{
			OffsetX += ((Keys[8]) ? (MovementModifier) : (1.0f)) * MovementSpeed * sin(CameraHorizontalAngle - 3.14f/2.0f);
			OffsetY += ((Keys[8]) ? (MovementModifier) : (1.0f)) * MovementSpeed * cos(CameraHorizontalAngle - 3.14f/2.0f);
			CameraPosition.y += (Right * (CameraSpeed * ((Keys[8]) ? (MovementModifier) : (1.0f)))).y;
		}
		if (Keys[3])
		{
			OffsetX -= ((Keys[8]) ? (MovementModifier) : (1.0f)) * MovementSpeed * sin(CameraHorizontalAngle - 3.14f/2.0f);
			OffsetY -= ((Keys[8]) ? (MovementModifier) : (1.0f)) * MovementSpeed * cos(CameraHorizontalAngle - 3.14f/2.0f);
			CameraPosition.y -= (Right * (CameraSpeed * ((Keys[8]) ? (MovementModifier) : (1.0f)))).y;
		}

		ClipmapWireframeShad.SetTestOffsetX(OffsetX);
		ClipmapWireframeShad.SetTestOffsetY(OffsetY);
		UpdateTBO();

		View = lookAt(CameraPosition, CameraPosition + Direction, Up);
    }

	if (Keys[4])
	{
		OffsetY += (MovementSpeed * ((Keys[8]) ? (MovementModifier) : (1.0f))) / CurrentLandscape->GetOffset();
		ClipmapWireframeShad.SetTestOffsetY(OffsetY);
		UpdateTBO();
	}
	if (Keys[5])
	{
		OffsetY -= (MovementSpeed * ((Keys[8]) ? (MovementModifier) : (1.0f))) / CurrentLandscape->GetOffset();
		ClipmapWireframeShad.SetTestOffsetY(OffsetY);
		UpdateTBO();
	}
	if (Keys[6])
	{
		OffsetX -= (MovementSpeed * ((Keys[8]) ? (MovementModifier) : (1.0f))) / CurrentLandscape->GetOffset();
		ClipmapWireframeShad.SetTestOffsetX(OffsetX);
		UpdateTBO();
	}
	if (Keys[7])
	{
		OffsetX += (MovementSpeed * ((Keys[8]) ? (MovementModifier) : (1.0f))) / CurrentLandscape->GetOffset();
		ClipmapWireframeShad.SetTestOffsetX(OffsetX);
		UpdateTBO();
	}

    //if (Keys[8] || Keys[9])
    //{
    //    CurrentLandscape->UpdateHeightmap(CurrentBrush);
    //    ResetTBO();
    //}
}

// --------------------------------------------------------------------
void LandGLContext::CreateNewLandscape(int Size)
{
    if (CurrentLandscape != 0)
        delete CurrentLandscape;

    CurrentLandscape = new Landscape(Size);

    ResetAllVBOIBO();
    ResetCamera();

    //if ((*CurrentShader) == LandscapeShad)
    //{
    //    LandscapeShad.SetLandscapeSizeX(CurrentLandscape->GetTBOSize());
    //    LandscapeShad.SetLandscapeVertexOffset(CurrentLandscape->GetOffset());
    //}
    //else if ((*CurrentShader) == LightningOnlyShad)
    //{
    //    LightningOnlyShad.SetLandscapeSizeX(CurrentLandscape->GetTBOSize());
    //    LightningOnlyShad.SetLandscapeVertexOffset(CurrentLandscape->GetOffset());
    //}
    //else if ((*CurrentShader) == HeightShad)
    //{
    //    HeightShad.SetLandscapeSizeX(CurrentLandscape->GetTBOSize());
    //    HeightShad.SetLandscapeVertexOffset(CurrentLandscape->GetOffset());
    //    HeightShad.SetMaxHeight(CurrentLandscape->GetTBOSize() * CurrentLandscape->GetOffset() * 0.15f);
    //}
    //else if ((*CurrentShader) == WireframeShad)
    //{
    //    WireframeShad.SetLandscapeSizeX(CurrentLandscape->GetTBOSize());
    //    WireframeShad.SetLandscapeVertexOffset(CurrentLandscape->GetOffset());
    //}

    CheckGLError();
}

// --------------------------------------------------------------------
void LandGLContext::SaveLandscape(const char* FilePath)
{
    //if (CurrentLandscape != 0)
    //    CurrentLandscape->SaveToFile(FilePath);

	LOG("Saving...");

	LandscapeEditor::FileWrite(FilePath, DATA, DataSize * DataSize);

	LOG("Completed!");
}

// --------------------------------------------------------------------
void LandGLContext::OpenFromFile(const char* FilePath)
{
	LOG("Opening...");

	delete[] DATA;
	delete[] NORMALS_0;
	delete[] NORMALS_1;
	delete[] NORMALS_2;

	DATA = new float[DataSize * DataSize];
	unsigned int DataByteSize;

	DATA = (float*)LandscapeEditor::FileRead(FilePath, DataByteSize);
	DataSize = sqrt((float)DataByteSize / 4.0);
	StartIndexY = StartIndexX = DataSize / 2.0f;

	NORMALS_0 = new float[DataSize * DataSize * 3];
	NORMALS_1 = new float[DataSize * DataSize * 3];
	NORMALS_2 = new float[DataSize * DataSize * 3];

	for (int i = 0; i < DataSize * DataSize * 3; ++i)
		NORMALS_0[i] = NORMALS_1[i] = NORMALS_2[i] = 0.0f;

	LOG("Generating normals...");

	for (int i = 1; i < DataSize - 1; ++i)
		for (int j = 1; j < DataSize - 1; ++j)
		{ 
			vec3 v1 = vec3(0.0f, CurrentLandscape->GetOffset() * 2.0f, DATA[i * DataSize + j - 1] - DATA[i * DataSize + j + 1]);
			vec3 v2 = vec3(CurrentLandscape->GetOffset() * 2.0f, 0.0f, DATA[(i - 1) * DataSize + j] - DATA[(i + 1) * DataSize + j]);

			vec3 n = normalize(cross(v2, v1));

			NORMALS_0[i * DataSize * 3 + j * 3] = n.x;
			NORMALS_0[i * DataSize * 3 + j * 3 + 1] = n.y;
			NORMALS_0[i * DataSize * 3 + j * 3 + 2] = n.z;
		}

	for (int i = 8; i < DataSize - 2; ++i)
		for (int j = 8; j < DataSize - 2; ++j)
		{ 

			vec3 v1 = vec3(0.0f, CurrentLandscape->GetOffset() * 4.0f, DATA[i * DataSize + j - 2] - DATA[i * DataSize + j + 2]);
			vec3 v2 = vec3(CurrentLandscape->GetOffset() * 4.0f, 0.0f, DATA[(i - 2) * DataSize + j] - DATA[(i + 2) * DataSize + j]);

			vec3 n = normalize(cross(v2, v1));

			NORMALS_1[i * DataSize * 3 + j * 3] = n.x;
			NORMALS_1[i * DataSize * 3 + j * 3 + 1] = n.y;
			NORMALS_1[i * DataSize * 3 + j * 3 + 2] = n.z;
		}

	for (int i = 16; i < DataSize - 16; ++i)
		for (int j = 16; j < DataSize - 16; ++j)
		{ 
			vec3 v1 = vec3(0.0f, CurrentLandscape->GetOffset() * 32.0f, DATA[i * DataSize + j - 16] - DATA[i * DataSize + j + 16]);
			vec3 v2 = vec3(CurrentLandscape->GetOffset() * 32.0f, 0.0f, DATA[(i - 16) * DataSize + j] - DATA[(i + 16) * DataSize + j]);

			vec3 n = normalize(cross(v2, v1));

			NORMALS_2[i * DataSize * 3 + j * 3] = n.x;
			NORMALS_2[i * DataSize * 3 + j * 3 + 1] = n.y;
			NORMALS_2[i * DataSize * 3 + j * 3 + 2] = n.z;
		}

	InitTBO(TBO, true);
	int ClipmapScale = 1;
	for (int i = 0; i < ClipmapsAmount; ++i)
	{
		ClipmapScale *= 2;
		InitTBO(TBOs[i], false, ClipmapScale);
	}

	ResetCamera();
	SetShadersInitialUniforms();

	VisibleCenterClipmapStrip = CLIPMAP_STRIP_1;
	for (int i = 0; i < ClipmapsAmount; ++i)
		VisibleClipmapStrips[i] = CLIPMAP_STRIP_1;

	bNewLandscape = true;
	LOG("Completed!");

    CheckGLError();
}

// --------------------------------------------------------------------
void LandGLContext::FatalError(char* text)
{
    ERR("Fatal error ocured!");
    ERR(text);
    system("pause");
    exit(1);
}

// --------------------------------------------------------------------
void LandGLContext::ResetCamera()
{
	OffsetX = 0.0001f;
	OffsetY = 0.0001f;

	CameraPosition = vec3(0.0f, 10.0f, 0.0f);
	CameraVerticalAngle = -1.62f;
    CameraHorizontalAngle = 0.0f;

    vec3 Direction(cos(CameraVerticalAngle) * sin(CameraHorizontalAngle), sin(CameraVerticalAngle), cos(CameraVerticalAngle) * cos(CameraHorizontalAngle));
    vec3 Right = vec3(sin(CameraHorizontalAngle - 3.14f/2.0f), 0, cos(CameraHorizontalAngle - 3.14f/2.0f));
    vec3 Up = cross(Right, Direction);
    View = lookAt(CameraPosition, CameraPosition + Direction, Up);
}

// --------------------------------------------------------------------
void LandGLContext::ResetVBO(GLuint &BufferID, float *NewData, int DataSize)
{
	if (BufferID != 0)
        glDeleteBuffers(1, &BufferID);

 	glGenBuffers(1, &BufferID);
	glBindBuffer(GL_ARRAY_BUFFER, BufferID);
	glBufferData(GL_ARRAY_BUFFER, DataSize * sizeof(float), NewData, GL_STATIC_DRAW);
}

// --------------------------------------------------------------------
void LandGLContext::ResetIBO(GLuint &BufferID, unsigned int *NewData, int DataSize)
{
	if (BufferID != 0)
		glDeleteBuffers(1, &BufferID);

    glGenBuffers(1, &BufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, DataSize * sizeof(unsigned int), NewData, GL_STATIC_DRAW);
}

// --------------------------------------------------------------------
void LandGLContext::ResetAllVBOIBO()
{
	float **ClipmapVBOsData = new float*[CLIPMAP_MODULES_AMOUNT];
	unsigned int **ClipmapIBOsData = new unsigned int*[CLIPMAP_MODULES_AMOUNT];
	int *ClipmapVBOsSize = new int[CLIPMAP_MODULES_AMOUNT];

	for (int i = 0; i < CLIPMAP_MODULES_AMOUNT; ++i)
	{
		ClipmapVBOsData[i] = CurrentLandscape->GetClipmapVBOData((ClipmapModule)i, ClipmapVBOsSize[i]);
		ClipmapIBOsData[i] = CurrentLandscape->GetClipmapIBOData((ClipmapModule)i, IBOLengths[i]);
		ResetVBO(VBOs[i], ClipmapVBOsData[i], ClipmapVBOsSize[i]);
		ResetIBO(IBOs[i], ClipmapIBOsData[i], IBOLengths[i]);
	}

	delete [] ClipmapVBOsData;
	delete [] ClipmapIBOsData;
	delete [] ClipmapVBOsSize;
}

// --------------------------------------------------------------------
void LandGLContext::UpdateTBO()
{
	static float LastUpdateOffsetX = 1.0f;
	static float LastUpdateOffsetY = 1.0f;

	static float *ClipmapLastUpdateOffsetX = 0;
	static float *ClipmapLastUpdateOffsetY = 0;

	if (bNewLandscape)
	{
		delete [] ClipmapLastUpdateOffsetX;
		delete [] ClipmapLastUpdateOffsetY;

		ClipmapLastUpdateOffsetX = 0;
		ClipmapLastUpdateOffsetY = 0;

		LastUpdateOffsetX = 1.0f;
		LastUpdateOffsetY = 1.0f;

		bNewLandscape = false;
	}

	if (ClipmapLastUpdateOffsetX == 0 && ClipmapLastUpdateOffsetY == 0)
	{
		ClipmapLastUpdateOffsetX = new float[ClipmapsAmount];
		ClipmapLastUpdateOffsetY = new float[ClipmapsAmount];

		float scale11 = 1.0f;

		for (int i = 0; i < ClipmapsAmount; ++i)
		{
			scale11 *= 2.0f;
			ClipmapLastUpdateOffsetX[i] = scale11;
			ClipmapLastUpdateOffsetY[i] = scale11;
		}
	}

	float *BufferData32 = NULL;
	int TBOSize = CurrentLandscape->GetTBOSize();

	float fDiffX = OffsetX - LastUpdateOffsetX;
	float fDiffY = OffsetY - LastUpdateOffsetY;

	int DiffX = floor((abs(fDiffX) + 1.0f) / 2.0f) * sign(fDiffX) * 2.0f;
	int DiffY = floor((abs(fDiffY) + 1.0f) / 2.0f) * sign(fDiffY) * 2.0f;
	
	//// Hack :(
	if (sign(fDiffX) < 0 && fDiffX - 1.0f == DiffX)
		DiffX += 2;
	if (sign(fDiffY) < 0 && fDiffY - 1.0f == DiffY)
		DiffY += 2;

	if (abs(DiffX) > TBOSize)
		LastUpdateOffsetX += DiffX + ((DiffX > 0) ? (-TBOSize) : (TBOSize));

	if (abs(DiffY) > TBOSize)
		LastUpdateOffsetY += DiffY + ((DiffY > 0) ? (-TBOSize) : (TBOSize));

	if (DiffX != 0 || DiffY != 0)
	{
		glActiveTexture(GL_TEXTURE2);	

		int SignX = (DiffX > 0) ? (1) : (-1);
		int SignY = (DiffY > 0) ? (1) : (-1);

		glBindBuffer(GL_TEXTURE_BUFFER, TBO);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, TBO);
		BufferData32 = (float*)glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);

		for (int j = 0; j < min(abs(DiffX), TBOSize); j++)
		{
			for (int i = max(0, DiffY); i < TBOSize + min(0, DiffY); i++)
			{
				int xTBO = int(mod(LastUpdateOffsetX - ((SignX > 0) ? (2.0f) : (1.0f)) + SignX * (j + 1), float(TBOSize)));
				int yTBO = int(mod(LastUpdateOffsetY + i - 1.0f, float(TBOSize)));
				int x = int(mod(StartIndexX + LastUpdateOffsetX - 1.0f + SignX * (j + 1) - ((SignX < 0) ? (TBOSize - 1.0f) : (0.0f)), float(DataSize)));
				int y = int(mod(StartIndexY - (TBOSize - 1.0f) + LastUpdateOffsetY - 1.0f + i, float(DataSize)));

				BufferData32[4 * (yTBO * TBOSize + xTBO)] = NORMALS_0[3 * (y * DataSize + x)];
				BufferData32[4 * (yTBO * TBOSize + xTBO) + 1] = NORMALS_0[3 * (y * DataSize + x) + 1];
				BufferData32[4 * (yTBO * TBOSize + xTBO) + 2] = NORMALS_0[3 * (y * DataSize + x) + 2];
				BufferData32[4 * (yTBO * TBOSize + xTBO) + 3] = DATA[y * DataSize + x];
			}
		}

		for (int j = 0; j < min(abs(DiffY), TBOSize); j++)
		{
			for (int i = 0; i < TBOSize; i++)
			{
				int xTBO = int(mod(LastUpdateOffsetX + i - 1.0f, float(TBOSize)));
				int yTBO = int(mod(LastUpdateOffsetY - ((SignY > 0) ? (2.0f) : (1.0f)) + SignY * (j + 1), float(TBOSize)));
				int x = int(mod(StartIndexX - (TBOSize - 1.0f) + LastUpdateOffsetX - 1.0f + i, float(DataSize)));
				int y = int(mod(StartIndexY + LastUpdateOffsetY - 1.0f + SignY * (j + 1) - ((SignY < 0) ? (TBOSize - 1.0f) : (0.0f)), float(DataSize)));
				
				BufferData32[4 * (yTBO * TBOSize + xTBO)] = NORMALS_0[3 * (y * DataSize + x)];
				BufferData32[4 * (yTBO * TBOSize + xTBO) + 1] = NORMALS_0[3 * (y * DataSize + x) + 1];
				BufferData32[4 * (yTBO * TBOSize + xTBO) + 2] = NORMALS_0[3 * (y * DataSize + x) + 2];
				BufferData32[4 * (yTBO * TBOSize + xTBO) + 3] = DATA[y * DataSize + x];
			}
		}

		glUnmapBuffer(GL_TEXTURE_BUFFER);

		float LastOffsetX = LastUpdateOffsetX;
		float LastOffsetY = LastUpdateOffsetY;
		int ClipmapScale = 1;

		LastUpdateOffsetX += min(abs(DiffX), TBOSize) * SignX;
		LastUpdateOffsetY += min(abs(DiffY), TBOSize) * SignY;

		// -------------------------------- TBOs update loop ---------------------------------
		for (int i = 0; i < ClipmapsAmount; i++)
		{
			ClipmapScale *= 2;

			float *NORMALS = 0;

			if (ClipmapScale > 128)
				NORMALS = NORMALS_2;
			else if (ClipmapScale > 32)
				NORMALS = NORMALS_1;
			else
				NORMALS = NORMALS_0;

			fDiffX = OffsetX - ClipmapLastUpdateOffsetX[i];
			fDiffY = OffsetY - ClipmapLastUpdateOffsetY[i];

			SignX = sign(fDiffX);
			SignY = sign(fDiffY);

			DiffX = floor((abs(fDiffX) + ClipmapScale) / (2.0f * ClipmapScale)) * SignX * 2.0f;
			DiffY = floor((abs(fDiffY) + ClipmapScale) / (2.0f * ClipmapScale)) * SignY * 2.0f;


			// Update VisibleClipmapStrip value
			if (mod(OffsetX, 2.0f * ClipmapScale) < ClipmapScale)
			{
				if (mod(OffsetY, 2.0f * ClipmapScale) < ClipmapScale)
					VisibleClipmapStrips[i] = CLIPMAP_STRIP_1;
				else
					VisibleClipmapStrips[i] = CLIPMAP_STRIP_2;
			}
			else
			{
				if (mod(OffsetY, 2.0f * ClipmapScale) < ClipmapScale)
					VisibleClipmapStrips[i] = CLIPMAP_STRIP_3;
				else
					VisibleClipmapStrips[i] = CLIPMAP_STRIP_4;
			}

			//// Hack :(
			//if (sign(fDiffX) < 0 && fDiffX - 2.0f == DiffX)
			//	DiffX += 4;
			//if (sign(fDiffY) < 0 && fDiffY - 2.0f == DiffY)
			//	DiffY += 4;

			if (DiffX != 0 || DiffY != 0)
			{
				LastOffsetX = ClipmapLastUpdateOffsetX[i];
				LastOffsetY = ClipmapLastUpdateOffsetY[i];

				ClipmapLastUpdateOffsetX[i] += min(abs(DiffX), TBOSize) * sign(DiffX) * ClipmapScale;
				ClipmapLastUpdateOffsetY[i] += min(abs(DiffY), TBOSize) * sign(DiffY) * ClipmapScale;

				glBindBuffer(GL_TEXTURE_BUFFER, TBOs[i]);
				glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, TBOs[i]);
				BufferData32 = (float*)glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);

				//if (abs(DiffX) > TBOSize)
				//	LastOffsetX += DiffX + 0.5 * ((DiffX > 0) ? (-TBOSize) : (TBOSize));

				if (abs(DiffY) > TBOSize)
					LastOffsetY += DiffY + 1.0f / ClipmapScale * ((DiffY > 0) ? (-TBOSize) : (TBOSize)) + 4.0f;

				for (int j = 0; j < min(abs(DiffX), TBOSize); j++)
				{
					for (int k = max(0, DiffY); k < TBOSize + min(0, DiffY); k++)
					{
						int xTBO = int(mod((LastOffsetX / ClipmapScale - ((SignX > 0) ? (2.0f) : (1.0f))) + SignX * (j + 1), float(TBOSize)));
						int yTBO = int(mod(LastOffsetY / ClipmapScale + k - 1.0f, float(TBOSize)));
						int x = int(mod(StartIndexX + (33 * (ClipmapScale - 1)) + LastOffsetX - ClipmapScale + SignX * (j * ClipmapScale + 1) - ((SignX < 0) ? ((TBOSize + 1) * ClipmapScale - 2.0f) : (0.0f)), float(DataSize)));
						int y = int(mod(StartIndexY - (32 * (ClipmapScale - 1)) - (TBOSize - 1.0f) + LastOffsetY - ClipmapScale + k * ClipmapScale, float(DataSize)));
						BufferData32[4 * (yTBO * TBOSize + xTBO)] = NORMALS[3 * (y * DataSize + x)];
						BufferData32[4 * (yTBO * TBOSize + xTBO) + 1] = NORMALS[3 * (y * DataSize + x) + 1];
						BufferData32[4 * (yTBO * TBOSize + xTBO) + 2] = NORMALS[3 * (y * DataSize + x) + 2];
						BufferData32[4 * (yTBO * TBOSize + xTBO) + 3] = DATA[y * DataSize + x];
					}
				}

				for (int j = 0; j < min(abs(DiffY), TBOSize); j++)
				//for (int j = 0; j < abs(DiffY); j++)
				{
					for (int k = 0; k < TBOSize; k++)
					{
						int xTBO = int(mod(LastOffsetX / ClipmapScale + k - 1.0f, float(TBOSize)));
						int yTBO = int(mod((LastOffsetY / ClipmapScale - ((SignY > 0) ? (2.0f) : (1.0f))) + SignY * (j + 1), float(TBOSize)));
						int x = int(mod(StartIndexX - (32 * (ClipmapScale - 1)) - (TBOSize - 1.0f) + LastOffsetX - ClipmapScale + k * ClipmapScale, float(DataSize)));
						int y = int(mod(StartIndexY + (33 * (ClipmapScale - 1)) + LastOffsetY - ClipmapScale + SignY * (j * ClipmapScale + 1) - ((SignY < 0) ? (TBOSize * ClipmapScale + (ClipmapScale - 2.0f)) : (0.0f)), float(DataSize)));
						BufferData32[4 * (yTBO * TBOSize + xTBO)] = NORMALS[3 * (y * DataSize + x)];
						BufferData32[4 * (yTBO * TBOSize + xTBO) + 1] = NORMALS[3 * (y * DataSize + x) + 1];
						BufferData32[4 * (yTBO * TBOSize + xTBO) + 2] = NORMALS[3 * (y * DataSize + x) + 2];
						BufferData32[4 * (yTBO * TBOSize + xTBO) + 3] = DATA[y * DataSize + x];
					}
					LOG(int(mod((LastOffsetY / ClipmapScale - ((SignY > 0) ? (2.0f) : (1.0f))) + SignY * (j + 1), float(TBOSize))));
				}

				glUnmapBuffer(GL_TEXTURE_BUFFER);			
			}
			else
			{
				break;
			}
		}

		fDiffX = OffsetX - LastUpdateOffsetX;
		fDiffY = OffsetY - LastUpdateOffsetY;
	}

	// Update VisibleCenterClipmapStrip value
	if (fDiffX < 0.0f)
	{
		if (fDiffY < 0.0f)
			VisibleCenterClipmapStrip = CLIPMAP_STRIP_1;
		else
			VisibleCenterClipmapStrip = CLIPMAP_STRIP_2;
	}
	else
	{
		if (fDiffY < 0.0f)
			VisibleCenterClipmapStrip = CLIPMAP_STRIP_3;
		else
			VisibleCenterClipmapStrip = CLIPMAP_STRIP_4;
	}
}