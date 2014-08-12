// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date: 23.03.2013
// --------------------------------------------------------------------
#pragma once

#include "wx/wx.h"

#include "LandscapeEditorFrame.h"
#include "LandGLCanvas.h"

/// Window class, handles with all GUI
class LandscapeEditorFrame: public wxFrame
{
protected:
    /// Toolbar ID, for internal usage only
    static const int ID_TOOLBAR = 500;

    /// Pointer to the top toolbar
    wxToolBar *toolbarTop;

    /// Function creating the toolbar
    void CreateToolbar();

public:
    /// Constructs the window
    LandscapeEditorFrame(wxFrame *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style);

    /// GUI handles
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnBrush1(wxCommandEvent& event);
    void OnBrush2(wxCommandEvent& event);
    void OnBrush3(wxCommandEvent& event);
    void OnBrush4(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

