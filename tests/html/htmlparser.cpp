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

#include "wx/html/winpars.h"
#include "wx/sstream.h"

#include <memory>

// Test that parsing invalid HTML simply fails but doesn't crash for example.
TEST_CASE("wxHtmlParser::ParseInvalid", "[html][parser][error]")
{
    class NullParser : public wxHtmlWinParser
    {
    protected:
        virtual void AddText(const wxString& WXUNUSED(txt)) override { }
    };

    NullParser p;
    wxMemoryDC dc;
    p.SetDC(&dc);

    delete p.Parse("<");
    delete p.Parse("<foo");
    delete p.Parse("<!--");
    delete p.Parse("<!---");
}

TEST_CASE("wxHtmlWinParser::OpenURLAbsoluteArchivePath",
          "[html][parser][url]")
{
    class RecordingFSHandler : public wxFileSystemHandler
    {
    public:
        bool CanOpen(const wxString& location) override
        {
            return !location.empty();
        }

        wxFSFile *OpenFile(wxFileSystem& WXUNUSED(fs),
                           const wxString& location) override
        {
            m_location = location;
            return new wxFSFile(new wxStringInputStream("data"),
                                location,
                                wxEmptyString,
                                wxEmptyString
#if wxUSE_DATETIME
                                , wxDateTime::Now()
#endif // wxUSE_DATETIME
                                );
        }

        const wxString& GetLocation() const { return m_location; }

    private:
        wxString m_location;
    };

    class AutoFSHandler
    {
    public:
        AutoFSHandler(RecordingFSHandler *handler) : m_handler(handler)
            { wxFileSystem::AddHandler(m_handler); }
        ~AutoFSHandler()
            { wxFileSystem::RemoveHandler(m_handler); }

    private:
        RecordingFSHandler *m_handler;
    };

    RecordingFSHandler handler;
    AutoFSHandler autoFSHandler(&handler);

    wxFileSystem fs;
    fs.ChangePathTo("file:doc.chm#xchm:/d/e/", true);

    wxHtmlWinParser parser;
    parser.SetFS(&fs);

    const wxString expected("file:doc.chm#xchm:/a/b/c.jpg");
    std::unique_ptr<wxFSFile> file(
        parser.OpenURL(wxHTML_URL_IMAGE, "/a/b/c.jpg"));

    REQUIRE(file);
    CHECK(handler.GetLocation() == expected);
    CHECK(file->GetLocation() == expected);
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
