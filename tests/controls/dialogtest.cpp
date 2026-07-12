///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/dialogtest.cpp
// Purpose:     wxWindow unit test
// Author:      Vaclav Slavik
// Created:     2012-08-30
// Copyright:   (c) 2012 Vaclav Slavik
//              (c) 2026 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"


#include "wx/testing.h"

#include "wx/filedlg.h"
#include "wx/intl.h"
#include "wx/msgdlg.h"
#include "wx/translation.h"

#include <string.h>

// This test suite tests helpers from wx/testing.h intended for testing of code
// that calls modal dialogs. It does not test the implementation of wxWidgets'
// dialogs.

#if wxUSE_INTL

namespace
{

const wxString& GetMessageDialogTestDomain()
{
    static const wxString s_domain("wx_test_msgdlg");
    return s_domain;
}

wxMsgCatalog *CreateMessageDialogTestCatalog(const wxString& domain)
{
    static const char header[] = "Content-Type: text/plain; charset=UTF-8\n";
    static const char msgid[] = "Message";
    static const char msgstr[] = "Translated Message";

    const size_t numStrings = 2;
    const size_t headerLen = sizeof(header) - 1;
    const size_t msgidLen = sizeof(msgid) - 1;
    const size_t msgstrLen = sizeof(msgstr) - 1;
    const size_t origTable = 28;
    const size_t transTable = origTable + 8 * numStrings;
    const size_t stringData = transTable + 8 * numStrings;
    const size_t origHeader = stringData;
    const size_t origMessage = origHeader + 1;
    const size_t transHeader = origMessage + msgidLen + 1;
    const size_t transMessage = transHeader + headerLen + 1;
    const size_t moLen = transMessage + msgstrLen + 1;

    unsigned char mo[256];
    memset(mo, 0, sizeof(mo));

    auto put32 = [](unsigned char* p, wxUint32 v)
    {
        p[0] = (unsigned char)(v & 0xff);
        p[1] = (unsigned char)((v >> 8) & 0xff);
        p[2] = (unsigned char)((v >> 16) & 0xff);
        p[3] = (unsigned char)((v >> 24) & 0xff);
    };

    put32(mo + 0, 0x950412de); // magic
    put32(mo + 8, (wxUint32)numStrings);
    put32(mo + 12, (wxUint32)origTable);
    put32(mo + 16, (wxUint32)transTable);

    put32(mo + origTable + 0, 0);
    put32(mo + origTable + 4, (wxUint32)origHeader);
    put32(mo + origTable + 8, (wxUint32)msgidLen);
    put32(mo + origTable + 12, (wxUint32)origMessage);

    put32(mo + transTable + 0, (wxUint32)headerLen);
    put32(mo + transTable + 4, (wxUint32)transHeader);
    put32(mo + transTable + 8, (wxUint32)msgstrLen);
    put32(mo + transTable + 12, (wxUint32)transMessage);

    memcpy(mo + origMessage, msgid, msgidLen);
    memcpy(mo + transHeader, header, headerLen);
    memcpy(mo + transMessage, msgstr, msgstrLen);

    wxCharTypeBuffer<char> data(reinterpret_cast<const char*>(mo), moLen);
    return wxMsgCatalog::CreateFromData(data, domain);
}

class MessageDialogTranslationsLoader : public wxTranslationsLoader
{
public:
    virtual wxMsgCatalog *LoadCatalog(const wxString& domain,
                                      const wxString& lang) override
    {
        if ( domain != GetMessageDialogTestDomain() || lang != "fr" )
            return nullptr;

        return CreateMessageDialogTestCatalog(domain);
    }

    virtual wxArrayString
    GetAvailableTranslations(const wxString& domain) const override
    {
        wxArrayString translations;

        if ( domain == GetMessageDialogTestDomain() )
            translations.Add("fr");

        return translations;
    }
};

} // anonymous namespace

TEST_CASE("MessageDialog::TranslatedDefaultCaption", "[msgdlg]")
{
    wxLocale locale;
    locale.Init(wxLANGUAGE_DEFAULT, wxLOCALE_DONT_LOAD_DEFAULT);

    wxTranslations * const trans = wxTranslations::Get();
    REQUIRE( trans );

    trans->SetLoader(new MessageDialogTranslationsLoader);
    trans->SetLanguage("fr");
    REQUIRE( trans->AddCatalog(GetMessageDialogTestDomain()) );

    CHECK( wxGetTranslation("Message") == "Translated Message" );

    wxMessageDialog translated(nullptr, "Text");
    CHECK( translated.GetCaption() == "Translated Message" );

    wxMessageDialog custom(nullptr, "Text", "Custom");
    CHECK( custom.GetCaption() == "Custom" );
}

#endif // wxUSE_INTL

TEST_CASE("Modal::MessageDialog", "[modal]")
{
    int rc;

#if wxUSE_FILEDLG
    #define FILE_DIALOG_TEST ,\
        wxExpectModal<wxFileDialog>(wxGetCwd() + "/test.txt").Optional()
#else
    #define FILE_DIALOG_TEST
#endif

    wxTEST_DIALOG
    (
        rc = wxMessageBox("Should I fail?", "Question", wxYES|wxNO),
        wxExpectModal<wxMessageDialog>(wxNO)
        FILE_DIALOG_TEST
    );

    CHECK( rc == wxNO );
}

#if wxUSE_FILEDLG
TEST_CASE("Modal::FileDialog", "[modal]")
{
#if defined(__WXQT__) && defined(__WINDOWS__)
    WARN("Skipping test known to fail under wxQt for Windows");
    return;
#else
    wxFileDialog dlg(nullptr);
    int rc;

    wxTEST_DIALOG
    (
        rc = dlg.ShowModal(),
        wxExpectModal<wxFileDialog>(wxGetCwd() + "/test.txt")
    );

    CHECK( rc == wxID_OK );

    CHECK( dlg.GetFilename() == "test.txt" );

#ifdef __WXGTK3__
    // The native file dialog in GTK+ 3 launches an async operation which tries
    // to dereference the already deleted dialog object if we don't let it to
    // complete before leaving this function.
    wxYield();
#endif
#endif
}
#endif

class MyDialog : public wxDialog
{
public:
    MyDialog(wxWindow *parent) : wxDialog(parent, wxID_ANY, "Entry"), m_value(-1)
    {
        // Dummy. Imagine it's a real dialog that shows some number-entry
        // controls.
    }

    int m_value;
};


template<>
class wxExpectModal<MyDialog> : public wxExpectModalBase<MyDialog>
{
public:
    wxExpectModal(int valueToSet) : m_valueToSet(valueToSet) {}

protected:
    virtual int OnInvoked(MyDialog *dlg) const override
    {
        // Simulate the user entering the expected number:
        dlg->m_value = m_valueToSet;
        return wxID_OK;
    }

    int m_valueToSet;
};

TEST_CASE("Modal::CustomDialog", "[modal]")
{
    MyDialog dlg(nullptr);

    wxTEST_DIALOG
    (
        dlg.ShowModal(),
        wxExpectModal<MyDialog>(42)
    );

    CHECK( dlg.m_value == 42 );
}


class MyModalDialog : public wxDialog
{
public:
    MyModalDialog() : wxDialog (nullptr, wxID_ANY, "Modal Dialog")
    {
        m_wasModal = false;
        Bind( wxEVT_INIT_DIALOG, &MyModalDialog::OnInit, this );
    }

    void OnInit(wxInitDialogEvent& WXUNUSED(event))
    {
        m_wasModal = IsModal();
        CallAfter( &MyModalDialog::EndModal, wxID_OK );
    }

    bool WasModal() const
    {
        return m_wasModal;
    }

private:
    bool m_wasModal;
};

TEST_CASE("Modal::InitDialog", "[modal]")
{
    MyModalDialog dlg;
    dlg.ShowModal();
    CHECK( dlg.WasModal() );
}
