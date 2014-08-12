// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date: 23.03.2013
// --------------------------------------------------------------------
#pragma once

#include <GL/glew.h>
#include "wx/wx.h"
#include "wx/glcanvas.h"

#include "LandGLContext.h"
#include "LandscapeEditor.h"

/** OpenGL canvas class */
class LandGLCanvas : public wxGLCanvas
{
protected:
    /// Spin timer
    wxTimer m_spinTimer;

    /// Reference to OpenGL drawing context
    LandGLContext* OpenGLContext;

    /// True if OpenGL context already initialized
    bool bOpenGLContextInitialized;

    // Flag: Is high frequency counter available?
    bool usingHighFrequencyCounter;      

    // Stores the information about time when the program was started
    float programStartMoment;        

    // Frequency of high frequency counter.
    LARGE_INTEGER frequency; 

    // How many frames we have in curret second?
    int framesCounter;  

    // Time when we actualized our FPS last time.
    float LastFPSUpdateTime;  

    // Frames Per Second.
    int FPS;                            

public:
    /// Standard constructor
    LandGLCanvas(wxWindow *parent);

protected:
    /// Function called on paint action
    void OnPaint(wxPaintEvent& event);
    
    /// Function called on resize
    void OnResize(wxSizeEvent& event);

    /// Called when key is down
    void OnKeyDown(wxKeyEvent& event);

    /// Called when key is up
    void OnKeyUp(wxKeyEvent& event);

    /// Called when mouse event comes
    void OnMouse(wxMouseEvent& event);

    /// Called every time when cube position should be updated cause of spinning
    void OnSpinTimer(wxTimerEvent& WXUNUSED(event));

    /// Call this function in every frame to proper FPS counting
    void UpdateFPS();

public:
    /// Setters
    void SetOpenGLContext(LandGLContext* newOpenGLContext) {OpenGLContext = newOpenGLContext;};
    void SetOpenGLContextInitialized(bool newValue) {bOpenGLContextInitialized = newValue;};

    // Get amount of seconds which has passed since begining of the program.
    float GetSecond();
  

    DECLARE_EVENT_TABLE()
};