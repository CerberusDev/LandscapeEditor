// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date: 23.03.2013
// --------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LandscapeEditor.h"

IMPLEMENT_APP_CONSOLE(LandscapeEditor)

int LandscapeEditor::InitTime;

// --------------------------------------------------------------------
LandGLContext& LandscapeEditor::GetContext(wxGLCanvas *canvas)
{
    if (canvas != 0)
    {
        if (!m_glContext)
            m_glContext = new LandGLContext(canvas);

        m_glContext->SetCurrent(*canvas);
    }

    return *m_glContext;
}

// --------------------------------------------------------------------
bool LandscapeEditor::OnInit()
{
	InitTime = GetTickCount();

    if (!wxApp::OnInit())
        return false;

    Frame = new LandscapeEditorFrame((wxFrame *) NULL, wxID_ANY, wxT("Landscape Editor"), wxPoint(100, 100), wxSize(WINDOW_WIDTH, WINDOW_HEIGHT), 
                                 wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN | wxNO_FULL_REPAINT_ON_RESIZE);
    Frame->Show(true);
    Frame->SetStatusText(wxT("Ready for action"));

    wxInitAllImageHandlers();

    return true;
}

// --------------------------------------------------------------------
int LandscapeEditor::OnExit()
{
    delete m_glContext;

    return wxApp::OnExit();
}

// --------------------------------------------------------------------
char* LandscapeEditor::TextFileRead(const char* FilePath) 
{
	FILE* File;
	char* Content = NULL;
	int Count = 0;

	if (FilePath != NULL) 
    {
		File = fopen(FilePath, "rt");

		if (File != NULL) 
        {
            fseek(File, 0, SEEK_END);
            Count = ftell(File);
            rewind(File);

			if (Count > 0) 
            {
				Content = (char*)malloc(sizeof(char) * (Count + 1));
				Count = fread(Content, sizeof(char), Count, File);
				Content[Count] = '\0';
			}
			fclose(File);
		}
	}
	return Content;
}

// --------------------------------------------------------------------
int LandscapeEditor::TextFileWrite(const char* FilePath, char* Content) 
{
	FILE* File;
	int status = 0;
    
	if (FilePath != NULL) 
    {
		File = fopen(FilePath, "w");

		if (File != NULL) 
        {
			if (fwrite(Content, sizeof(char), strlen(Content), File) == strlen(Content))
				status = 1;

			fclose(File);
		}
	}
	return(status);
}

// --------------------------------------------------------------------
void* LandscapeEditor::FileRead(const char* FilePath, unsigned int &ByteSize) 
{
	FILE* File;
	void* Content = NULL;
	ByteSize = 0;

	if (FilePath != NULL) 
    {
		File = fopen(FilePath, "rb");
        
		if (File != NULL) 
        {
            fseek(File, 0, SEEK_END);
            ByteSize = ftell(File);
            rewind(File);

			if (ByteSize > 0) 
            {
				Content = malloc(sizeof(int) * ByteSize);
				fread(Content, sizeof(int), ByteSize, File);
			}
			fclose(File);
		}
	}
	return Content;
}

// --------------------------------------------------------------------
int LandscapeEditor::FileWrite(const char* FilePath, void* Content32b, unsigned int Size) 
{
	FILE* File;
	int status = 0;
    
	if (FilePath != NULL) 
    {
		File = fopen(FilePath, "wb");

		if (File != NULL) 
        {
			if (fwrite(Content32b, sizeof(int), Size, File) == Size)
				status = 1;

			fclose(File);
		}
	}
	return(status);
}
