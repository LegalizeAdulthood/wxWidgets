///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/dirctrltest.cpp
// Purpose:     wxGenericDirCtrl unit test
// Author:      wxWidgets development team
// Created:     2026-07-09
// Copyright:   (c) 2026 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if (wxUSE_DIRDLG || wxUSE_FILEDLG) && wxUSE_TREECTRL && defined(__WINDOWS__)

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/filefn.h"
#include "wx/filename.h"
#include "wx/generic/dirctrlg.h"
#include "wx/msw/wrapwin.h"
#include "wx/msw/winundef.h"

class DirCtrlHiddenDirTestCase
{
public:
    DirCtrlHiddenDirTestCase()
    {
        m_root = wxFileName::CreateTempFileName("dirctrltest");
        REQUIRE( !m_root.empty() );
        REQUIRE( wxRemoveFile(m_root) );

        REQUIRE( wxMkdir(m_root) );

        m_hidden = m_root + wxString(wxFILE_SEP_PATH) + "hidden";
        REQUIRE( wxMkdir(m_hidden) );

        m_leaf = m_hidden + wxString(wxFILE_SEP_PATH) + "leaf";
        REQUIRE( wxMkdir(m_leaf) );

        const DWORD attrs = ::GetFileAttributes(m_hidden.t_str());
        REQUIRE( attrs != INVALID_FILE_ATTRIBUTES );
        REQUIRE( ::SetFileAttributes(m_hidden.t_str(),
                                     attrs | FILE_ATTRIBUTE_HIDDEN) != 0 );
    }

    ~DirCtrlHiddenDirTestCase()
    {
        wxTheApp->GetTopWindow()->DestroyChildren();

        const DWORD attrs = ::GetFileAttributes(m_hidden.t_str());
        if ( attrs != INVALID_FILE_ATTRIBUTES )
        {
            ::SetFileAttributes(m_hidden.t_str(),
                                attrs & ~FILE_ATTRIBUTE_HIDDEN);
        }

        wxFileName::Rmdir(m_root, wxPATH_RMDIR_RECURSIVE);
    }

protected:
    wxString m_root;
    wxString m_hidden;
    wxString m_leaf;

    wxDECLARE_NO_COPY_CLASS(DirCtrlHiddenDirTestCase);
};

TEST_CASE_METHOD(DirCtrlHiddenDirTestCase, "wxGenericDirCtrl::ShowHidden",
                 "[dirctrl]")
{
    wxGenericDirCtrl* const dirctrl = new wxGenericDirCtrl(
        wxTheApp->GetTopWindow(), wxID_ANY, m_leaf, wxDefaultPosition,
        wxDefaultSize, wxDIRCTRL_DIR_ONLY);

    CHECK_FALSE( wxFileName(dirctrl->GetPath()).SameAs(wxFileName(m_leaf)) );

    dirctrl->ShowHidden(true);

    CHECK( wxFileName(dirctrl->GetPath()).SameAs(wxFileName(m_leaf)) );
}

#endif // (wxUSE_DIRDLG || wxUSE_FILEDLG) && wxUSE_TREECTRL && __WINDOWS__
