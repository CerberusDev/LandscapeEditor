// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date: 23.03.2013
// --------------------------------------------------------------------

#include "LandGLCanvas.h"

enum
{
    SpinTimer = wxID_HIGHEST + 1
};

// --------------------------------------------------------------------
/** wxWidgets - Event table stuff */
BEGIN_EVENT_TABLE(LandGLCanvas, wxGLCanvas)
    EVT_SIZE(LandGLCanvas::OnResize)
    EVT_PAINT(LandGLCanvas::OnPaint)
    EVT_KEY_DOWN(LandGLCanvas::OnKeyDown)
    EVT_KEY_UP(LandGLCanvas::OnKeyUp)
    EVT_MOUSE_EVENTS(LandGLCanvas::OnMouse)
    EVT_TIMER(SpinTimer, LandGLCanvas::OnSpinTimer)
END_EVENT_TABLE()

// --------------------------------------------------------------------
LandGLCanvas::LandGLCanvas(wxWindow *parent): 
wxGLCanvas(parent, wxID_ANY, NULL, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE), m_spinTimer(this, SpinTimer), bOpenGLContextInitialized(false), framesCounter(0)
{
    m_spinTimer.Start(-1);
    usingHighFrequencyCounter = (QueryPerformanceFrequency(&frequency) != 0);   // Setting the flag, putting a frequnecy to the variable.

    if (usingHighFrequencyCounter) // Setting the programStartMoment value.
    {
        LARGE_INTEGER Ticks;
        QueryPerformanceCounter(&Ticks);
        programStartMoment =  (float)(Ticks.QuadPart / (double)frequency.QuadPart);
    }
    else
    {
        programStartMoment = GetTickCount() / 1000.0f;
    }
}

// --------------------------------------------------------------------
void LandGLCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // This is required even though dc is not used otherwise.
    wxPaintDC dc(this);

    // Set the OpenGL viewport according to the client size of this canvas.
    // This is done here rather than in a wxSizeEvent handler because our
    // OpenGL rendering context (and thus viewport setting) is used with
    // multiple canvases: If we updated the viewport in the wxSizeEvent
    // handler, changing the size of one canvas causes a viewport setting that
    // is wrong when next another canvas is repainted.

    LandGLContext& canvas = ((LandscapeEditor*)LandscapeEditor::GetInstance())->GetContext(this);
    canvas.ManageInput();

    // Render the graphics and swap the buffers.
    canvas.DrawScene();
    SwapBuffers();
}

// --------------------------------------------------------------------
void LandGLCanvas::OnResize(wxSizeEvent& WXUNUSED(event))
{
    const wxSize ClientSize = GetClientSize();
 
    LandGLContext& context = ((LandscapeEditor*)LandscapeEditor::GetInstance())->GetContext(this);
    context.OnResize(ClientSize);
    
}

// --------------------------------------------------------------------
void LandGLCanvas::OnKeyDown(wxKeyEvent& event)
{
    OpenGLContext->OnKey(true, event);
}

// --------------------------------------------------------------------
void LandGLCanvas::OnKeyUp(wxKeyEvent& event)
{
    OpenGLContext->OnKey(false, event);
}

// --------------------------------------------------------------------
void LandGLCanvas::OnMouse(wxMouseEvent& event)
{
    if (bOpenGLContextInitialized)
        OpenGLContext->OnMouse(event);
}

// --------------------------------------------------------------------
void LandGLCanvas::OnSpinTimer(wxTimerEvent& WXUNUSED(event))
{
    UpdateFPS();
    Refresh(false);
}

// --------------------------------------------------------------------
void LandGLCanvas::UpdateFPS()
{
    framesCounter++;
    if (GetSecond() - LastFPSUpdateTime > 1.0f)
    {
        FPS = framesCounter;
        //Log("FPS: %d\n", FPS);
        LastFPSUpdateTime = GetSecond();
        framesCounter = 0;
    }
}

// --------------------------------------------------------------------
float LandGLCanvas::GetSecond()
{
  float tmpCounter;   // Current time.

  if (usingHighFrequencyCounter) 
  {
    LARGE_INTEGER Ticks;
    QueryPerformanceCounter(&Ticks);
    tmpCounter = (float)(Ticks.QuadPart / (double)frequency.QuadPart);
  }
  else
  {
    tmpCounter = GetTickCount() / 1000.0f;
  }

  return tmpCounter - programStartMoment; // Returns: current time - program start moment = time since program start.
}