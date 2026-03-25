#pragma once
#include <windows.h>

namespace StitchPeek {
    // Shared rendering function used by both the COM DLL and the test tool
    HRESULT RenderEmbroideryToBitmap(const char* utf8path, UINT cx, HBITMAP* phbmp);
}
