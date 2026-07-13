///////////////////////////////////////////////////////////////////////////////
// Name:        tests/debugrpt/debugrpttest.cpp
// Purpose:     wxDebugReport tests
// Author:      wxWidgets development team
// Created:     2026-07-13
// Copyright:   (c) 2026 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_DEBUGREPORT && wxUSE_XML

#include "wx/debugrpt.h"
#include "wx/filename.h"

namespace
{

class NamedDebugReport : public wxDebugReport
{
public:
    wxString GetReportName() const override
    {
        return wxT("derived_debug_report");
    }
};

} // anonymous namespace

TEST_CASE("wxDebugReport::DerivedReportName", "[debugrpt]")
{
    NamedDebugReport report;
    REQUIRE( report.IsOk() );

    const wxString dirName = wxFileName(report.GetDirectory()).GetFullName();

    CHECK( dirName.StartsWith(wxT("derived_debug_report_dbgrpt-")) );
}

#endif // wxUSE_DEBUGREPORT && wxUSE_XML
