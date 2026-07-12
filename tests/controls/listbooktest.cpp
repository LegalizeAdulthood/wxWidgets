///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/listbooktest.cpp
// Purpose:     wxListbook unit test
// Author:      Steven Lamerton
// Created:     2010-07-02
// Copyright:   (c) 2010 Steven Lamerton
//              (c) 2026 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_LISTBOOK


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/listbook.h"
#include "wx/listctrl.h"
#include "bookctrlbasetest.h"

class ListbookTestCase : public BookCtrlBaseTestCase, public CppUnit::TestCase
{
public:
    ListbookTestCase() { }

    virtual void setUp() override;
    virtual void tearDown() override;

private:
    virtual wxBookCtrlBase *GetBase() const override { return m_listbook; }

    virtual wxEventType GetChangedEvent() const override
    { return wxEVT_LISTBOOK_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const override
    { return wxEVT_LISTBOOK_PAGE_CHANGING; }

    virtual bool HasBrokenMnemonics() const override { return true; }

    CPPUNIT_TEST_SUITE( ListbookTestCase );
        wxBOOK_CTRL_BASE_TESTS();
        CPPUNIT_TEST( ListView );
        CPPUNIT_TEST( NoImagesLabelSize );
    CPPUNIT_TEST_SUITE_END();

    void ListView();
    void NoImagesLabelSize();

    wxListbook *m_listbook;

    wxDECLARE_NO_COPY_CLASS(ListbookTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ListbookTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ListbookTestCase, "ListbookTestCase" );

void ListbookTestCase::setUp()
{
    m_listbook = new wxListbook(wxTheApp->GetTopWindow(), wxID_ANY,
                                wxDefaultPosition, wxSize(400, 300));
    AddPanels();
}

void ListbookTestCase::tearDown()
{
    wxDELETE(m_listbook);
}

void ListbookTestCase::ListView()
{
    wxListView* listview = m_listbook->GetListView();

    CPPUNIT_ASSERT(listview);
    CPPUNIT_ASSERT_EQUAL(3, listview->GetItemCount());
    CPPUNIT_ASSERT_EQUAL("Panel 1", listview->GetItemText(0));
}

void ListbookTestCase::NoImagesLabelSize()
{
#ifdef __WXMSW__
    wxDELETE(m_listbook);

    m_listbook = new wxListbook(wxTheApp->GetTopWindow(), wxID_ANY,
                                wxDefaultPosition, wxSize(400, 300));

    const wxString label("Long multi word label");

    m_listbook->AddPage(new wxPanel(m_listbook), label);
    m_listbook->SendSizeEvent();

    wxListView* listview = m_listbook->GetListView();

    CPPUNIT_ASSERT(listview->InReportView());

    const int labelWidth = listview->GetTextExtent(label).x;

    CPPUNIT_ASSERT(listview->GetColumnWidth(0) >= labelWidth);
    CPPUNIT_ASSERT(m_listbook->GetControllerSize().x >= labelWidth);
#endif // __WXMSW__
}

#endif //wxUSE_LISTBOOK
