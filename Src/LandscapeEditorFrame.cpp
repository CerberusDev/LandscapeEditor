// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date: 23.03.2013
// --------------------------------------------------------------------

#include <wx/stdpaths.h>
#include <wx/numdlg.h>

#include "LandscapeEditor.h"

// --------------------------------------------------------------------
/** wxWidgets - Event table stuff */
enum
{
    ID_NEW = 11,
    ID_OPEN = 12,
    ID_SAVE = 13,
    ID_BRUSH_1 = 100,
    ID_BRUSH_2 = 101,
    ID_BRUSH_3 = 102,
    ID_BRUSH_4 = 103,
    ID_COMBO = 1000
};

BEGIN_EVENT_TABLE(LandscapeEditorFrame, wxFrame)
    EVT_MENU(wxID_EXIT, LandscapeEditorFrame::OnQuit)
    EVT_MENU(wxID_HELP, LandscapeEditorFrame::OnAbout)
    EVT_MENU(ID_NEW, LandscapeEditorFrame::OnNew)
    EVT_MENU(ID_OPEN, LandscapeEditorFrame::OnOpen)
    EVT_MENU(ID_SAVE, LandscapeEditorFrame::OnSave)
    EVT_MENU(ID_BRUSH_1, LandscapeEditorFrame::OnBrush1)
    EVT_MENU(ID_BRUSH_2, LandscapeEditorFrame::OnBrush2)
    EVT_MENU(ID_BRUSH_3, LandscapeEditorFrame::OnBrush3)
    EVT_MENU(ID_BRUSH_4, LandscapeEditorFrame::OnBrush4)
END_EVENT_TABLE()

// --------------------------------------------------------------------
LandscapeEditorFrame::LandscapeEditorFrame(wxFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
wxFrame(parent, id, title, pos, size, style)
{
    SetIcon(wxICON(appicon));
    CreateStatusBar();

    new LandGLCanvas(this);

    wxMenuBar* menuBar = new wxMenuBar(wxMB_DOCKABLE);

    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, wxT("E&xit\tAlt-X"), wxT("Quit Editor") );

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_HELP, wxT("&About"), wxT("About Edtior"));

    menuBar->Append(fileMenu, wxT("&File"));
    menuBar->Append(helpMenu, wxT("&Help"));

    SetMenuBar(menuBar);

    // test IsDisplaySupported() function:
    static const int attribs[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };
    //wxLogStatus("Double-buffered display %s supported",
	//            wxGLCanvas::IsDisplaySupported(attribs) ? "is" : "not");

    CreateToolbar();
}

// --------------------------------------------------------------------
void LandscapeEditorFrame::CreateToolbar()
{
    enum
    {
        Tool_new,
        Tool_open,
        Tool_save,
        Tool_help,
        Tool_brush1,
        Tool_brush2,
        Tool_brush3,
        Tool_brush4,
        Tool_Max
    };

    toolbarTop = CreateToolBar(wxTB_FLAT | wxTB_DOCKABLE, ID_TOOLBAR);

    wxBitmap toolBarBitmaps[Tool_Max];

    #define INIT_TOOL_BMP(bmp) \
        toolBarBitmaps[Tool_##bmp] = wxBITMAP(bmp)

    INIT_TOOL_BMP(new);
    INIT_TOOL_BMP(open);
    INIT_TOOL_BMP(save);
    INIT_TOOL_BMP(help);
    INIT_TOOL_BMP(brush1);
    INIT_TOOL_BMP(brush2);
    INIT_TOOL_BMP(brush3);
    INIT_TOOL_BMP(brush4);

    toolbarTop->AddSeparator();

    toolbarTop->AddTool(ID_NEW, wxT("New"), toolBarBitmaps[Tool_new], wxNullBitmap, wxITEM_NORMAL,
                     wxT("New file"), wxT("This is 'new map' tool"));
    toolbarTop->AddTool(ID_OPEN, wxT("Open"), toolBarBitmaps[Tool_open], wxNullBitmap, wxITEM_NORMAL,
                     wxT("Open file"), wxT("This is 'open map file' tool"));
    toolbarTop->AddTool(ID_SAVE, wxT("Save"), toolBarBitmaps[Tool_save], wxNullBitmap, wxITEM_NORMAL,
                     wxT("Save map"), wxT("This is 'save map' tool"));

    toolbarTop->AddSeparator();

    //toolbarTop->AddTool(ID_BRUSH_1, wxT("New"), toolBarBitmaps[Tool_brush1], wxNullBitmap, wxITEM_NORMAL,
    //                 wxT("Brush 1"), wxT("Click for select first brush"));
    //toolbarTop->AddTool(ID_BRUSH_2, wxT("New"), toolBarBitmaps[Tool_brush2], wxNullBitmap, wxITEM_NORMAL,
    //                 wxT("Brush 2"), wxT("Click for select second brush"));
    //toolbarTop->AddTool(ID_BRUSH_3, wxT("New"), toolBarBitmaps[Tool_brush3], wxNullBitmap, wxITEM_NORMAL,
    //                 wxT("Brush 3"), wxT("Click for select third brush"));
    //toolbarTop->AddTool(ID_BRUSH_4, wxT("New"), toolBarBitmaps[Tool_brush4], wxNullBitmap, wxITEM_NORMAL,
    //                 wxT("Brush 4"), wxT("Click for select fourth brush"));

    //toolbarTop->AddSeparator();

    toolbarTop->AddTool(wxID_HELP, wxT("Help"), toolBarBitmaps[Tool_help], wxT("Help button"), wxITEM_CHECK);

    toolbarTop->Realize();
}

// --------------------------------------------------------------------
void LandscapeEditorFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

// --------------------------------------------------------------------
void LandscapeEditorFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    (void)wxMessageBox(wxT("Created by Maciek Pryc"), wxT("Landscape Editor info"));
}

// --------------------------------------------------------------------
void LandscapeEditorFrame::OnNew(wxCommandEvent& WXUNUSED(event)) 
{
    long Result = wxGetNumberFromUser(wxT("How big should be your terrain?"), wxT("Enter a number:"), wxT("Create new landscape"), 50, 2, 1000);

    if (Result != -1 )
        LandscapeEditor::Inst()->GetContext().CreateNewLandscape(Result);
}

// --------------------------------------------------------------------
void LandscapeEditorFrame::OnOpen(wxCommandEvent& WXUNUSED(event)) 
{
    wxFileDialog dialog(this, wxT("Open existing map"), wxEmptyString, wxEmptyString, wxT("Data files (*.raw)|*.raw"));

    dialog.SetDirectory(wxStandardPaths::Get().GetDataDir());

    if (dialog.ShowModal() == wxID_OK)
        LandscapeEditor::Inst()->GetContext().OpenFromFile(dialog.GetPath().c_str().AsChar());
}

// --------------------------------------------------------------------
void LandscapeEditorFrame::OnSave(wxCommandEvent& WXUNUSED(event)) 
{
    wxFileDialog dialog(this, wxT("Save map"), wxEmptyString, wxT("MyTerrain.raw"),
                        wxT("Data files (*.raw)|*.raw"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    dialog.SetDirectory(wxStandardPaths::Get().GetDataDir());

    if (dialog.ShowModal() == wxID_OK)
        LandscapeEditor::Inst()->GetContext().SaveLandscape(dialog.GetPath().c_str().AsChar());
}

// --------------------------------------------------------------------
void LandscapeEditorFrame::OnBrush1(wxCommandEvent& WXUNUSED(event)) 
{
    LandscapeEditor::Inst()->GetContext().ChangeBrushMode(0);
}

// --------------------------------------------------------------------
void LandscapeEditorFrame::OnBrush2(wxCommandEvent& WXUNUSED(event)) 
{
    LandscapeEditor::Inst()->GetContext().ChangeBrushMode(1);
}

// --------------------------------------------------------------------
void LandscapeEditorFrame::OnBrush3(wxCommandEvent& WXUNUSED(event)) 
{
    LandscapeEditor::Inst()->GetContext().ChangeBrushMode(2);
}

// --------------------------------------------------------------------
void LandscapeEditorFrame::OnBrush4(wxCommandEvent& WXUNUSED(event)) 
{
    LandscapeEditor::Inst()->GetContext().ChangeBrushMode(3);
}