#include <iostream>
#include <algorithm>
extern "C" {
#include "embroidery.h"
}

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    EmbPattern *pattern = emb_pattern_create();
    if (!emb_pattern_readAuto(pattern, argv[1])) {
        std::cout << "Read failed\n";
        return 1;
    }
    
    std::cout << "\n--- Analyzing " << argv[1] << " ---\n";
    std::cout << "Threads: " << (pattern->thread_list ? pattern->thread_list->count : 0) << "\n";
    std::cout << "Stitches: " << (pattern->stitch_list ? pattern->stitch_list->count : 0) << "\n";
    
    EmbArray *stitches = pattern->stitch_list;
    int numThreads = pattern->thread_list ? pattern->thread_list->count : 0;
    EmbThread *threads = pattern->thread_list ? pattern->thread_list->thread : NULL;
    if (threads) {
        for (int i = 0; i < 10 && i < numThreads; i++) {
            EmbColor ec = threads[i].color;
            std::cout << "Thread " << i << ": R=" << (int)ec.r << " G=" << (int)ec.g << " B=" << (int)ec.b << "\n";
        }
    }

    double min_x = 999999.0, min_y = 999999.0;
    double max_x = -999999.0, max_y = -999999.0;
    
    if (stitches && stitches->count > 0) {
        for (int i = 0; i < 10 && i < stitches->count; i++) {
            EmbStitch st = stitches->stitch[i];
            std::cout << "Stitch " << i << ": X=" << st.x << " Y=" << st.y << " Flags=" << st.flags << "\n";
        }
        for (int i = 0; i < stitches->count; i++) {
            EmbStitch st = stitches->stitch[i];
            if (st.flags == 0) { // 0 is NORMAL
                if (st.x < min_x) min_x = st.x;
                if (st.x > max_x) max_x = st.x;
                if (st.y < min_y) min_y = st.y;
                if (st.y > max_y) max_y = st.y;
            }
        }
    }
    
    double width = max_x - min_x;
    double height = max_y - min_y;
    double scale = 256.0 / std::max(width, height);
    
    std::cout << "Bounds X: [" << min_x << ", " << max_x << "] Width: " << width << "\n";
    std::cout << "Bounds Y: [" << min_y << ", " << max_y << "] Height: " << height << "\n";
    std::cout << "Scale: " << scale << "\n";
    
    emb_pattern_free(pattern);
    return 0;
}
