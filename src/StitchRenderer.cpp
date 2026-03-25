#include "StitchRenderer.h"
#include <gdiplus.h>

extern "C" {
#include "embroidery.h"
}

using namespace Gdiplus;

namespace StitchPeek {

HRESULT RenderEmbroideryToBitmap(const char* utf8path, UINT cx, HBITMAP* phbmp) {
    EmbPattern *pattern = emb_pattern_create();
    if (!pattern) return E_OUTOFMEMORY;

    if (!emb_pattern_readAuto(pattern, utf8path)) {
        emb_pattern_free(pattern);
        return E_FAIL;
    }

    double min_x = 999999.0, min_y = 999999.0;
    double max_x = -999999.0, max_y = -999999.0;
    EmbArray *stitches = pattern->stitch_list;
    if (stitches && stitches->count > 0) {
        for (int i = 0; i < stitches->count; i++) {
            EmbStitch st = stitches->stitch[i];
            if (!(st.flags & TRIM)) {
                if (st.x < min_x) min_x = st.x;
                if (st.x > max_x) max_x = st.x;
                if (st.y < min_y) min_y = st.y;
                if (st.y > max_y) max_y = st.y;
            }
        }
    } else {
        min_x = 0; min_y = 0; max_x = 0; max_y = 0;
    }

    double width = max_x - min_x;
    double height = max_y - min_y;
    double bounds_x = min_x;
    double bounds_y = min_y;

    if (width <= 0 || height <= 0) {
        emb_pattern_free(pattern);
        return E_FAIL;
    }

    double scale = (double)cx / max(width, height);
    
    Bitmap *bitmap = new Bitmap(cx, cx, PixelFormat32bppARGB);
    Graphics *g = Graphics::FromImage(bitmap);
    
    g->SetSmoothingMode(SmoothingModeAntiAlias);
    g->Clear(Color(255, 255, 255, 255)); // Solid white background

    // Increased pen width from 1.0f to 1.5f to prevent faint/dotted appearances on high-res monitors
    Pen *pen = new Pen(Color(255, 0, 0, 0), 1.5f);
    
    int numThreads = pattern->thread_list ? pattern->thread_list->count : 0;
    EmbThread *threads = pattern->thread_list ? pattern->thread_list->thread : NULL;

    // Synthetic palette for industrial formats (DST, EXP) which lack internal color data
    EmbColor defaultPalette[] = {
        {220, 70, 70},   // Red
        {70, 170, 80},   // Green
        {70, 110, 220},  // Blue
        {230, 160, 60},  // Orange
        {170, 80, 210},  // Purple
        {60, 170, 170},  // Teal
        {210, 90, 140},  // Rose
        {140, 150, 70},  // Olive
        {80, 130, 190},  // Steel blue
        {200, 120, 70},  // Copper
        {120, 90, 190},  // Indigo
        {80, 180, 120},  // Mint green
        {190, 80, 90},   // Crimson
        {90, 150, 90},   // Moss green
        {150, 100, 180}  // Violet
    };
    int paletteSize = sizeof(defaultPalette) / sizeof(EmbColor);
    bool useSyntheticPalette = (numThreads <= 1);
    int currentColorIndex = 0;

    float prevX = 0, prevY = 0;
    bool hasPrev = false;

    // Center in thumbnail
    double offsetX = (cx - width * scale) / 2.0 - bounds_x * scale;
    double offsetY = (cx - height * scale) / 2.0 - bounds_y * scale;

    if (stitches) {
        for (int i = 0; i < stitches->count; i++) {
            EmbStitch st = stitches->stitch[i];

            if (st.flags == STOP) {
                currentColorIndex++;
            }

            EmbColor ec;
            if (useSyntheticPalette) {
                ec = defaultPalette[currentColorIndex % paletteSize];
            } else if (st.color >= 0 && st.color < numThreads && threads) {
                ec = threads[st.color].color;
            } else {
                ec = {0, 0, 0}; // Fallback black
            }
            pen->SetColor(Color(255, ec.r, ec.g, ec.b));

            float sx = (float)(st.x * scale + offsetX);
            float sy = (float)(cx - (st.y * scale + offsetY)); // Flipped Y

            if (st.flags == NORMAL) {
                if (hasPrev) {
                    g->DrawLine(pen, prevX, prevY, sx, sy);
                }
            } else if (st.flags == END) {
                break;
            }

            prevX = sx;
            prevY = sy;
            hasPrev = true;
        }
    }

    delete pen;
    delete g;

    Color bg(255, 255, 255); // Solid white background for GetHBITMAP
    HBITMAP hbmp = NULL;
    bitmap->GetHBITMAP(bg, &hbmp);
    delete bitmap;

    emb_pattern_free(pattern);

    if (hbmp) {
        *phbmp = hbmp;
        return S_OK;
    }

    return E_FAIL;
}

} // namespace StitchPeek
