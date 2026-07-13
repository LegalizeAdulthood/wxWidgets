///////////////////////////////////////////////////////////////////////////////
// Name:        tests/geometry/rect.cpp
// Purpose:     wxRect unit test
// Author:      Vadim Zeitlin
// Created:     2004-12-11
// Copyright:   (c) 2004 wxWindows
//              (c) 2026 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/gdicmn.h"
#endif // WX_PRECOMP

#include "wx/geometry.h"

#include "wx/iosfwrap.h"

#include "asserthelper.h"

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

TEST_CASE("wxRect::CentreIn", "[rect]")
{
    typedef wxRect R;

    CHECK( R(0, 0, 10, 10).CentreIn(R(0, 0, 100, 100)) == R(45, 45, 10, 10) );
    CHECK( R(0, 0, 20, 20).CentreIn(R(0, 0, 10, 10)) == R(-5, -5, 20, 20) );

    R r(-10, -10, 20, 20);
    r.MakeCenteredIn(R(0, 0, 100, 100), wxHORIZONTAL);
    CHECK( r == R(40, -10, 20, 20) );
}

TEST_CASE("wxRect::InflateDeflate", "[rect]")
{
    // This is the rectangle from the example in the documentation of wxRect::Inflate().
    const wxRect r1(10, 10, 20, 40);

    CHECK(r1.Inflate( 10,  10)==wxRect(  0,   0, 40,  60));
    CHECK(r1.Inflate( 20,  30)==wxRect(-10, -20, 60, 100));
    CHECK(r1.Inflate(-10, -10)==wxRect( 20,  20,  0,  20));
    CHECK(r1.Inflate(-15, -15)==wxRect( 20,  25,  0,  10));

    CHECK(r1.Inflate( 10,  10)==r1.Deflate(-10, -10));
    CHECK(r1.Inflate( 20,  30)==r1.Deflate(-20, -30));
    CHECK(r1.Inflate(-10, -10)==r1.Deflate( 10,  10));
    CHECK(r1.Inflate(-15, -15)==r1.Deflate( 15,  15));
}

TEST_CASE("wxRect::Operators", "[rect]")
{
    // test + operator which works like Union but does not ignore empty rectangles
    static const struct RectData
    {
        int x1, y1, w1, h1;
        int x2, y2, w2, h2;
        int x, y, w, h;

        wxRect GetFirst() const { return wxRect(x1, y1, w1, h1); }
        wxRect GetSecond() const { return wxRect(x2, y2, w2, h2); }
        wxRect GetResult() const { return wxRect(x, y, w, h); }
    } s_rects[] =
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 2, 2 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 4, 4, 1, 1, 1, 1, 4, 4 },
        { 1, 1, 2, 2, 4, 4, 1, 1, 1, 1, 4, 4 },
        { 2, 2, 2, 2, 4, 4, 4, 4, 2, 2, 6, 6 },
        { 1, 1, 4, 4, 4, 4, 1, 1, 1, 1, 4, 4 }
    };

    for ( size_t n = 0; n < WXSIZEOF(s_rects); n++ )
    {
        const RectData& data = s_rects[n];

        CHECK( (data.GetFirst() + data.GetSecond()) == data.GetResult() );
        CHECK( (data.GetSecond() + data.GetFirst()) == data.GetResult() );
    }

    // test operator*() which returns the intersection of two rectangles
    wxRect r1 = wxRect(0, 2, 3, 4);
    wxRect r2 = wxRect(1, 2, 7, 8);
    r1 *= r2;
    CHECK(wxRect(1, 2, 2, 4) == r1);

    CHECK( (r1 * wxRect()).IsEmpty() );
}

TEST_CASE("wxRect::Union", "[rect]")
{
    static const struct RectData
    {
        int x1, y1, w1, h1;
        int x2, y2, w2, h2;
        int x, y, w, h;

        wxRect GetFirst() const { return wxRect(x1, y1, w1, h1); }
        wxRect GetSecond() const { return wxRect(x2, y2, w2, h2); }
        wxRect GetResult() const { return wxRect(x, y, w, h); }
    } s_rects[] =
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 4, 4, 1, 1, 1, 1, 4, 4 },
        { 1, 1, 2, 2, 4, 4, 1, 1, 1, 1, 4, 4 },
        { 2, 2, 2, 2, 4, 4, 4, 4, 2, 2, 6, 6 },
        { 1, 1, 4, 4, 4, 4, 1, 1, 1, 1, 4, 4 }
    };

    for ( size_t n = 0; n < WXSIZEOF(s_rects); n++ )
    {
        const RectData& data = s_rects[n];

        CHECK( data.GetFirst().Union(data.GetSecond()) == data.GetResult() );

        CHECK( data.GetSecond().Union(data.GetFirst()) == data.GetResult() );
    }
}

TEST_CASE("wxRect2D::UnionEmpty", "[rect]")
{
    const wxRect2DInt ri(100, 100, 50, 50);

    wxRect2DInt ri1 = ri;
    ri1.Union(wxRect2DInt(0, 0, 0, 0));
    CHECK( ri1 == ri );

    wxRect2DInt ri2(0, 0, 0, 0);
    ri2.Union(ri);
    CHECK( ri2 == ri );

    const wxRect2DDouble rd(100, 100, 50, 50);

    wxRect2DDouble rd1 = rd;
    rd1.Union(wxRect2DDouble(0, 0, 0, 0));
    CHECK( rd1 == rd );

    wxRect2DDouble rd2(0, 0, 0, 0);
    rd2.Union(rd);
    CHECK( rd2 == rd );
}

TEST_CASE("wxRect2D::ContainsEmpty", "[rect]")
{
    const wxRect2DInt ri(100, 100, 50, 50);

    CHECK( ri.Contains(wxRect2DInt(0, 0, 0, 0)) );
    CHECK( ri.Contains(wxRect2DInt(125, 125, 0, 0)) );
    CHECK( ri.Contains(wxRect2DInt(200, 200, 0, 0)) );

    const wxRect2DDouble rd(100, 100, 50, 50);

    CHECK( rd.Contains(wxRect2DDouble(0, 0, 0, 0)) );
    CHECK( rd.Contains(wxRect2DDouble(125, 125, 0, 0)) );
    CHECK( rd.Contains(wxRect2DDouble(200, 200, 0, 0)) );
}
