#include <iostream>
#include <cmath>
extern "C" {
#include "embroidery.h"
}

void analyze(const char* file) {
    EmbPattern *pattern = emb_pattern_create();
    if (!emb_pattern_readAuto(pattern, file)) return;
    
    std::cout << "\n--- " << file << " ---\n";
    EmbArray *stitches = pattern->stitch_list;
    if (!stitches || stitches->count == 0) return;
    
    double min_x = 999999, min_y = 999999, max_x = -999999, max_y = -999999;
    double max_dx = 0, max_dy = 0;
    
    for (int i = 0; i < stitches->count; i++) {
        EmbStitch st = stitches->stitch[i];
        if (i < 20) {
            std::cout << "S" << i << ": X=" << st.x << " Y=" << st.y << " \n";
        }
        if (st.flags == 0) {
            if (st.x < min_x) min_x = st.x;
            if (st.x > max_x) max_x = st.x;
            if (st.y < min_y) min_y = st.y;
            if (st.y > max_y) max_y = st.y;
        }
        
        if (i > 0) {
            EmbStitch prev = stitches->stitch[i-1];
            double dx = std::abs(st.x - prev.x);
            double dy = std::abs(st.y - prev.y);
            if (dx > max_dx && st.flags == 0) max_dx = dx;
            if (dy > max_dy && st.flags == 0) max_dy = dy;
        }
    }
    
    std::cout << "Vertices: " << stitches->count << "\n";
    std::cout << "Bounds Box: [" << min_x << ", " << min_y << "] to [" << max_x << ", " << max_y << "]\n";
    std::cout << "Width: " << (max_x - min_x) << "  Height: " << (max_y - min_y) << "\n";
    std::cout << "Max NORMAL Delta X: " << max_dx << "  Max NORMAL Delta Y: " << max_dy << "\n";
    
    // Print the last stitch
    EmbStitch last = stitches->stitch[stitches->count - 1];
    std::cout << "Final Coordinates: X=" << last.x << " Y=" << last.y << "\n";
    emb_pattern_free(pattern);
}

int main(int argc, char* argv[]) {
    analyze("test\\sample.vp3");
    analyze("test\\sample.pec");
    return 0;
}
