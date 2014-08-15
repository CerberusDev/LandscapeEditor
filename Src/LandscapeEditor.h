// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date: 23.03.2013
// --------------------------------------------------------------------
#pragma once

#include <GL/glew.h>
#include "wx/wx.h"
#include "wx/glcanvas.h"
#include <string.h>
#include <iomanip>

#include "LandGLContext.h"
#include "LandscapeEditorFrame.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

#define LOG(X)																																		\
	(																																				\
		std::cout << "[" << std::fixed << std::setprecision(3) << (GetTickCount() - LandscapeEditor::InitTime) / 1000.0f << "] " << X << std::endl,	\
		(void)0																																		\
	)

#define WARN(X)																																		\
	(																																				\
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14),																				\
		std::cout << "[" << std::fixed << std::setprecision(3) << (GetTickCount() - LandscapeEditor::InitTime) / 1000.0f << "] " << X << std::endl,	\
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7),																				\
		(void)0																																		\
	)

#define ERR(X)																																		\
	(																																				\
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12),																				\
		std::cout << "[" << std::fixed << std::setprecision(3) << (GetTickCount() - LandscapeEditor::InitTime) / 1000.0f << "] " << X << std::endl,	\
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7),																				\
		(void)0																																		\
	)

#define CONF(X)																																		\
	(																																				\
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10),																				\
		std::cout << "[" << std::fixed << std::setprecision(3) << (GetTickCount() - LandscapeEditor::InitTime) / 1000.0f << "] " << X << std::endl,	\
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7),																				\
		(void)0																																		\
	)

/** Main application class, singleton */
class LandscapeEditor : public wxApp
{
protected:
    /// the GL context we use for all our windows
    LandGLContext *m_glContext;

public: 
	///
	static int InitTime;

	enum ELog {LOG, WARN, ERR};

    /// Pointer to the main application frame (window)
    LandscapeEditorFrame* Frame;

    /// Standard constructor
    LandscapeEditor() {m_glContext = NULL;}

    /// Returns the shared context used by all frames and sets it as current for the given canvas
    LandGLContext& GetContext(wxGLCanvas *canvas = 0);

    /// Function called on application init
    bool OnInit();

    /// Function called on application exit
    int OnExit();

    /// Read text from file
    static char* TextFileRead(const char *FilePath);

    /// Write text to file
    static int TextFileWrite(const char *FilePath, char *Content);

    /// Read data from file
    static void* FileRead(const char *FilePath, unsigned int &ByteSize);

    /// Write data to file
    static int FileWrite(const char *FilePath, void *Content32b, unsigned int Size);

    /// Accessor
    static LandscapeEditor* Inst() {return (LandscapeEditor*)ms_appInstance;}
};

