///////////////////////////////////////////////////////////////////////////////
// Name:        tests/drawing/basictest.cpp
// Purpose:     Basic tests for wxGraphicsContext
// Author:      Armel Asselin
// Created:     2014-02-28
// Copyright:   (c) 2014 Ellié Computing <opensource@elliecomputing.com>
// Copyright:   (c) 2026 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/dcmemory.h"
    #include "wx/font.h"
    #include "wx/image.h"
//    #include "wx/window.h"
#endif // WX_PRECOMP

#define _WX_CURSOR_H_BASE_


#include "drawing.h"

#ifdef __WXMSW__
namespace
{

int FindFirstNonWhitePixelX(const wxBitmap& bmp)
{
    const wxImage img = bmp.ConvertToImage();
    const int width = img.GetWidth();
    const int height = img.GetHeight();

    for ( int x = 0; x < width; ++x )
    {
        for ( int y = 0; y < height; ++y )
        {
            if ( img.GetRed(x, y) < 250 ||
                 img.GetGreen(x, y) < 250 ||
                 img.GetBlue(x, y) < 250 )
                return x;
        }
    }

    return width;
}

enum DrawTextWithTabsMode
{
    DrawTextDirectly,
    DrawTextAsLabel
};

int DrawTextWithTabAndFindFirstInk(DrawTextWithTabsMode mode, int* charWidth)
{
    wxBitmap bmp(200, 50, 24);
    wxMemoryDC dc(bmp);

    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();
    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.SetTextForeground(*wxBLACK);
    dc.SetFont(wxFont(wxFontInfo(12).Family(wxFONTFAMILY_MODERN)));

    *charWidth = dc.GetCharWidth();

    const wxString text("\tX");
    if ( mode == DrawTextAsLabel )
        dc.DrawLabel(text, wxRect(0, 0, bmp.GetWidth(), bmp.GetHeight()));
    else
        dc.DrawText(text, 0, 0);

    dc.SelectObject(wxNullBitmap);

    return FindFirstNonWhitePixelX(bmp);
}

} // anonymous namespace
#endif // __WXMSW__

TEST_CASE("wxDC::DrawTextWithTabs", "[dc][text][msw]")
{
#ifdef __WXMSW__
    int charWidth = 0;
    int firstInkX =
        DrawTextWithTabAndFindFirstInk(DrawTextDirectly, &charWidth);

    REQUIRE( firstInkX < 200 );
    CHECK( firstInkX > 2*charWidth );

    firstInkX = DrawTextWithTabAndFindFirstInk(DrawTextAsLabel, &charWidth);

    REQUIRE( firstInkX < 200 );
    CHECK( firstInkX > 2*charWidth );
#endif // __WXMSW__
}

#if wxUSE_TEST_GC_DRAWING

const GraphicsContextDrawingTestCase::DrawingTestCase
GraphicsContextDrawingTestCase::ms_drawingBasicTc = {
    1, &GraphicsContextDrawingTestCase::DoBasicDrawings, 800, 600, 72., true
};

void GraphicsContextDrawingTestCase::DoBasicDrawings (wxGraphicsContext *gc)
{
    // this test is expected to be portable, on any platform, in order to keep
    //  that property, it should contain only axis aligned/integer drawings so
    //  that the anti-aliasing method does not cause troubles.

    wxGraphicsBrush gbBackground =
        gc->CreateBrush (wxBrush (wxColour (255, 255, 255)));

    gc->SetBrush (gbBackground);
    gc->DrawRectangle (0, 0, 800, 600);
}

#endif // wxUSE_TEST_GC_DRAWING
