#include <iostream>
#include <algorithm>
#include <cmath>
extern "C" {
#include "embroidery.h"
}

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    EmbPattern *pattern = emb_pattern_create();
    if (!emb_pattern_readAuto(pattern, argv[1])) return 1;
    
    std::cout << "\n--- Spikes in " << argv[1] << " ---\n";
    EmbArray *stitches = pattern->stitch_list;
    if (stitches && stitches->count > 0) {
        for (int i = 0; i < stitches->count; i++) {
            EmbStitch st = stitches->stitch[i];
            if (st.flags == 0) { // NORMAL
                if (std::abs(st.x) > 500 || std::abs(st.y) > 500) {
                    std::cout << "Stitch " << i << ": X=" << st.x << " Y=" << st.y << " (NORMAL)\n";
                }
            } else if (st.flags == 1) { // JUMP
                if (std::abs(st.x) > 500 || std::abs(st.y) > 500) {
                    std::cout << "Stitch " << i << ": X=" << st.x << " Y=" << st.y << " (JUMP)\n";
                }
            }
        }
    }
    
    emb_pattern_free(pattern);
    return 0;
}
