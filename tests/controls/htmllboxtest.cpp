///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/htmllboxtest.cpp
// Purpose:     wxSimpleHtmlListBoxNameStr unit test
// Author:      Vadim Zeitlin
// Created:     2010-11-27
// Copyright:   (c) 2010 Vadim Zeitlin <vadim@wxwidgets.org>
// Copyright:   (c) 2026 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_HTML


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/htmllbox.h"
#include "itemcontainertest.h"

class HtmlListBoxTestCase : public ItemContainerTestCase,
                            public CppUnit::TestCase
{
public:
    HtmlListBoxTestCase() { }

    virtual void setUp() override;
    virtual void tearDown() override;

private:
    virtual wxItemContainer *GetContainer() const override { return m_htmllbox; }
    virtual wxWindow *GetContainerWindow() const override { return m_htmllbox; }

    CPPUNIT_TEST_SUITE( HtmlListBoxTestCase );
        wxITEM_CONTAINER_TESTS();
        CPPUNIT_TEST( EmptyPaging );
    CPPUNIT_TEST_SUITE_END();

    wxSimpleHtmlListBox* m_htmllbox;

    void EmptyPaging();

    wxDECLARE_NO_COPY_CLASS(HtmlListBoxTestCase);
};

wxREGISTER_UNIT_TEST_WITH_TAGS(HtmlListBoxTestCase,
                               "[HtmlListBoxTestCase][item-container]");

void HtmlListBoxTestCase::setUp()
{
    m_htmllbox = new wxSimpleHtmlListBox(wxTheApp->GetTopWindow(), wxID_ANY);
}

void HtmlListBoxTestCase::tearDown()
{
    wxDELETE(m_htmllbox);
}

void HtmlListBoxTestCase::EmptyPaging()
{
    wxKeyEvent eventPageDown(wxEVT_KEY_DOWN);
    eventPageDown.m_keyCode = WXK_PAGEDOWN;
    CPPUNIT_ASSERT( m_htmllbox->GetEventHandler()->ProcessEvent(eventPageDown) );

    wxKeyEvent eventPageUp(wxEVT_KEY_DOWN);
    eventPageUp.m_keyCode = WXK_PAGEUP;
    CPPUNIT_ASSERT( m_htmllbox->GetEventHandler()->ProcessEvent(eventPageUp) );

    CPPUNIT_ASSERT_EQUAL( wxNOT_FOUND, m_htmllbox->GetSelection() );
}

#endif //wxUSE_HTML
