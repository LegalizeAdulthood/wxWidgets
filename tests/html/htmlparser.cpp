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

#include <memory>

static wxHtmlCell *FindTextCell(wxHtmlCell *cell, const wxString& text)
{
    if ( cell->ConvertToText(nullptr) == text )
        return cell;

    for ( wxHtmlCell *child = cell->GetFirstChild();
          child;
          child = child->GetNext() )
    {
        wxHtmlCell *found = FindTextCell(child, text);
        if ( found )
            return found;
    }

    return nullptr;
}

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

TEST_CASE("wxHtmlParser::UnderlineKerning", "[html][parser]")
{
    wxMemoryDC dc;
    wxHtmlWinParser p;
    p.SetDC(&dc);

    std::unique_ptr<wxHtmlContainerCell> const top(
        static_cast<wxHtmlContainerCell*>(p.Parse("<u>A</u>dd")));
    REQUIRE( top );

    top->Layout(1000);

    wxHtmlCell *a = FindTextCell(top.get(), "A");
    wxHtmlCell *dd = FindTextCell(top.get(), "dd");

    REQUIRE( a );
    REQUIRE( dd );

    wxCoord ddWidth, combinedWidth, height;
    dc.GetTextExtent("dd", &ddWidth, &height);
    dc.GetTextExtent("Add", &combinedWidth, &height);

    const int expectedDdX = a->GetAbsPos(top.get()).x +
                            combinedWidth - ddWidth;

    CHECK( dd->GetAbsPos(top.get()).x == expectedDdX );
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
