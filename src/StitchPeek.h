#pragma once

#include <windows.h>
#include <shobjidl.h>
#include <thumbcache.h>

// {C6F4CCFA-8C37-4E64-A9D6-7EB76DC284FD}
extern const CLSID CLSID_StitchPeekProvider;

class StitchPeekProvider : public IInitializeWithItem, public IThumbnailProvider {
public:
    StitchPeekProvider();

    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IInitializeWithItem
    IFACEMETHODIMP Initialize(IShellItem *psi, DWORD grfMode);

    // IThumbnailProvider
    IFACEMETHODIMP GetThumbnail(UINT cx, HBITMAP *phbmp, WTS_ALPHATYPE *pdwAlpha);

private:
    ~StitchPeekProvider();
    
    long m_cRef;
    IShellItem *m_pItem;
    ULONG_PTR m_gdiplusToken;
};
