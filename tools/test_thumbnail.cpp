#include <windows.h>
#include <gdiplus.h>
#include <iostream>
#include <string>
#include "StitchRenderer.h"

using namespace Gdiplus;

#pragma comment(lib, "gdiplus.lib")

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0, size = 0;
    GetImageEncodersSize(&num, &size);
    if(size == 0) return -1;
    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    GetImageEncoders(num, size, pImageCodecInfo);
    for(UINT j = 0; j < num; ++j) {
        if(wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }
    free(pImageCodecInfo);
    return -1;
}

int wmain(int argc, wchar_t *argv[]) {
    if (argc < 2) return 1;

    char utf8path[512];
    WideCharToMultiByte(CP_UTF8, 0, argv[1], -1, utf8path, 512, NULL, NULL);

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    HBITMAP hbmp = NULL;
    HRESULT hr = StitchPeek::RenderEmbroideryToBitmap(utf8path, 256, &hbmp);
    if (FAILED(hr)) {
        std::cout << "Failed to render embroidery to bitmap. E_FAIL or E_OUTOFMEMORY\n";
        GdiplusShutdown(gdiplusToken);
        return 1;
    }

    Bitmap *bitmap = Bitmap::FromHBITMAP(hbmp, NULL);
    CLSID pngClsid;
    GetEncoderClsid(L"image/png", &pngClsid);
    std::wstring outputPath = argv[1];
    outputPath += L".png";
    bitmap->Save(outputPath.c_str(), &pngClsid, NULL);

    std::wcout << L"Saved " << outputPath << L"\n";

    delete bitmap;
    DeleteObject(hbmp);
    GdiplusShutdown(gdiplusToken);
    
    return 0;
}
