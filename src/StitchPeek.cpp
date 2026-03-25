#include "StitchPeek.h"
#include "StitchRenderer.h"
#include <shlwapi.h>
#include <gdiplus.h>
#include <new>
#include <vector>
#include <algorithm>
#include <algorithm>

extern "C" {
#include "embroidery.h"
}

using namespace Gdiplus;

// {C6F4CCFA-8C37-4E64-A9D6-7EB76DC284FD}
const CLSID CLSID_StitchPeekProvider = 
{ 0xc6f4ccfa, 0x8c37, 0x4e64, { 0xa9, 0xd6, 0x7e, 0xb7, 0x6d, 0xc2, 0x84, 0xfd } };

extern long g_cDllRef;

StitchPeekProvider::StitchPeekProvider() : m_cRef(1), m_pItem(NULL), m_gdiplusToken(0) {
    InterlockedIncrement(&g_cDllRef);
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

StitchPeekProvider::~StitchPeekProvider() {
    if (m_pItem) m_pItem->Release();
    GdiplusShutdown(m_gdiplusToken);
    InterlockedDecrement(&g_cDllRef);
}

IFACEMETHODIMP StitchPeekProvider::QueryInterface(REFIID riid, void **ppv) {
    static const QITAB qit[] = {
        QITABENT(StitchPeekProvider, IThumbnailProvider),
        QITABENT(StitchPeekProvider, IInitializeWithItem),
        { 0 },
    };
    return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) StitchPeekProvider::AddRef() {
    return InterlockedIncrement(&m_cRef);
}

IFACEMETHODIMP_(ULONG) StitchPeekProvider::Release() {
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef) delete this;
    return cRef;
}

IFACEMETHODIMP StitchPeekProvider::Initialize(IShellItem *psi, DWORD /*grfMode*/) {
    if (m_pItem) {
        m_pItem->Release();
        m_pItem = NULL;
    }
    if (psi) {
        psi->AddRef();
        m_pItem = psi;
    }
    return S_OK;
}

IFACEMETHODIMP StitchPeekProvider::GetThumbnail(UINT cx, HBITMAP *phbmp, WTS_ALPHATYPE *pdwAlpha) {
    if (!m_pItem) { return E_UNEXPECTED; }

    LPWSTR pszName = NULL;
    HRESULT hr = m_pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszName);
    if (FAILED(hr)) { return hr; }

    // Convert to ANSI for libembroidery
    int len = WideCharToMultiByte(CP_UTF8, 0, pszName, -1, NULL, 0, NULL, NULL);
    char *utf8path = new char[len];
    WideCharToMultiByte(CP_UTF8, 0, pszName, -1, utf8path, len, NULL, NULL);
    CoTaskMemFree(pszName);

    hr = StitchPeek::RenderEmbroideryToBitmap(utf8path, cx, phbmp);
    delete[] utf8path;
    
    if (SUCCEEDED(hr)) {
        *pdwAlpha = WTSAT_RGB; // Tell Explorer there is no alpha channel
    }

    return hr;
}
