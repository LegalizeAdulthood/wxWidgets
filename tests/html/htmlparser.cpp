///////////////////////////////////////////////////////////////////////////////
// Name:        tests/html/htmlparser.cpp
// Purpose:     wxHtmlParser tests
// Author:      Vadim Zeitlin
// Created:     2011-01-13
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Copyright:   (c) 2026 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_HTML


#ifndef WX_PRECOMP
    #include "wx/dcmemory.h"
#endif // WX_PRECOMP

#include "wx/html/htmlwin.h"
#include "wx/html/winpars.h"

#include <memory>

class TestHtmlWindowInterface : public wxHtmlWindowInterface
{
public:
    virtual void SetHTMLWindowTitle(const wxString& WXUNUSED(title)) override { }

    virtual void OnHTMLLinkClicked(const wxHtmlLinkInfo& WXUNUSED(link)) override { }

    virtual wxHtmlOpeningStatus OnHTMLOpeningURL(wxHtmlURLType type,
                                                 const wxString& url,
                                                 wxString *WXUNUSED(redirect)) const override
    {
        if ( type == wxHTML_URL_IMAGE )
            m_imageUrl = url;
        else if ( type == wxHTML_URL_PAGE )
            m_pageUrl = url;

        return wxHTML_BLOCK;
    }

    virtual wxPoint HTMLCoordsToWindow(wxHtmlCell *WXUNUSED(cell),
                                       const wxPoint& WXUNUSED(pos)) const override
        { return wxPoint(); }

    virtual wxWindow* GetHTMLWindow() override { return nullptr; }

    virtual wxColour GetHTMLBackgroundColour() const override { return *wxWHITE; }

    virtual void SetHTMLBackgroundColour(const wxColour& WXUNUSED(clr)) override { }

    virtual void SetHTMLBackgroundImage(const wxBitmapBundle& WXUNUSED(bmpBg)) override { }

    virtual void SetHTMLStatusText(const wxString& WXUNUSED(text)) override { }

    virtual wxCursor GetHTMLCursor(HTMLCursor WXUNUSED(type)) const override
        { return wxCursor(); }

    const wxString& GetImageUrl() const { return m_imageUrl; }
    const wxString& GetPageUrl() const { return m_pageUrl; }

private:
    mutable wxString m_imageUrl;
    mutable wxString m_pageUrl;
};

class TestHtmlParser : public wxHtmlWinParser
{
public:
    explicit TestHtmlParser(wxHtmlWindowInterface *windowInterface = nullptr)
        : wxHtmlWinParser(windowInterface)
    {
    }

protected:
    virtual void AddText(const wxString& WXUNUSED(txt)) override { }
};

// Test that parsing invalid HTML simply fails but doesn't crash for example.
TEST_CASE("wxHtmlParser::ParseInvalid", "[html][parser][error]")
{
    TestHtmlParser p;
    wxMemoryDC dc;
    p.SetDC(&dc);

    delete p.Parse("<");
    delete p.Parse("<foo");
    delete p.Parse("<!--");
    delete p.Parse("<!---");
}

TEST_CASE("wxHtmlParser::BaseHref", "[html][parser]")
{
    TestHtmlWindowInterface windowInterface;
    TestHtmlParser p(&windowInterface);
    wxFileSystem fs;
    wxMemoryDC dc;

    fs.ChangePathTo("http://example.com/original/page.html");
    p.SetDC(&dc);
    p.SetFS(&fs);

    delete p.Parse(
        "<html><head><base href=\"http://example.com/base/\"></head>"
        "<body><img src=\"image.png\"></body></html>");

    CHECK( windowInterface.GetImageUrl() == "http://example.com/base/image.png" );

    delete p.OpenURL(wxHTML_URL_PAGE, "linked.html");

    CHECK( windowInterface.GetPageUrl() == "http://example.com/base/linked.html" );
}

TEST_CASE("wxHtmlCell::Detach", "[html][cell]")
{
    wxMemoryDC dc;

    std::unique_ptr<wxHtmlContainerCell> const top(new wxHtmlContainerCell(nullptr));
    wxHtmlContainerCell* const cont = new wxHtmlContainerCell(nullptr);
    wxHtmlCell* const cell1 = new wxHtmlWordCell("Hello", dc);
    wxHtmlCell* const cell2 = new wxHtmlColourCell(*wxRED);
    wxHtmlCell* const cell3 = new wxHtmlWordCell("world", dc);

    cont->InsertCell(cell1);
    cont->InsertCell(cell2);
    cont->InsertCell(cell3);
    top->InsertCell(cont);

    SECTION("container")
    {
        top->Detach(cont);
        CHECK( top->GetFirstChild() == nullptr );

        delete cont;
    }

    SECTION("first-child")
    {
        cont->Detach(cell1);
        CHECK( cont->GetFirstChild() == cell2 );

        delete cell1;
    }

    SECTION("middle-child")
    {
        cont->Detach(cell2);
        CHECK( cont->GetFirstChild() == cell1 );
        CHECK( cell1->GetNext() == cell3 );

        delete cell2;
    }

    SECTION("last-child")
    {
        cont->Detach(cell3);
        CHECK( cont->GetFirstChild() == cell1 );
        CHECK( cell1->GetNext() == cell2 );
        CHECK( cell2->GetNext() == nullptr );

        delete cell3;
    }

    SECTION("invalid")
    {
        WX_ASSERT_FAILS_WITH_ASSERT_MESSAGE
        (
            "Expected assertion for detaching non-child",
            top->Detach(cell1);
        );
    }
}

#endif //wxUSE_HTML
