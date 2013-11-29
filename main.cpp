#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <assert.h>
#include <algorithm>

#include "debug.h"
#include "timer.h"

#include "main.h"

cell *grid, *single_acc;
ACC *accumulator;
test_case *cases;

int W = 512, H = 512;
int cnt = 10;
size_t seed = 42;

#define SQRLUT 1
int *sqrt_lut;
const int lut_size = 8192;

#define PRESORT 1

/* img r g b */
void write_img3d(const char *filename) {
    int i;
    FILE *f = fopen(filename, "w");
    fprintf(f, "P6 %d %d %d\n", W, W, 255);
    /* the data */
    for(i = 0; i <  W * H ; i++) {
        /* double iv = img[i]; */
        color c = grid[i].value();
        // color c = onEnter[i].value();
        putc((unsigned char) (255 * c.r) ,f);
        putc((unsigned char) (255 * c.g) ,f);
        putc((unsigned char) (255 * c.b) ,f);
    }
    fclose(f);
}

void init_tests() {
    Timer timer;
    timer.start();
    cases = new test_case[cnt];
    for (int i = 0; i < cnt; i++)
        cases[i].init();
    timer.stop();
    printf("initialized %d tests in %d ms\n", cnt, timer.getms());
#if PRESORT
    timer.start();
    std::sort(cases, cases + cnt);
    timer.stop();
    printf("Timer to sort points %d ms\n", timer.getms());
#endif
}

void init_lut() {
    Timer timer;
    timer.start();
    sqrt_lut = new int[lut_size];
    for (int i = 0; i < lut_size; i++) {
        sqrt_lut[i] = (int) sqrt(i);
        if ( sqrt_lut[i] * sqrt_lut[i] > i
             || ( (1 + sqrt_lut[i]) * (1+sqrt_lut[i]) < i)) {
            PL("dfodof");
            PD(sqrt_lut[i]);
            PD(i);
            PF(std::sqrt(i));
            PF(sqrt(i));
            exit(42);
        }
    }
    timer.stop();
    printf("initialized lut in %d ms\n", cnt, timer.getms());
}

void draw_circle(const test_case& test) {
    const float& x = test.x;
    const float& y = test.y;
    const float& r = test.r; 
    const color& c = test.c;
    // a test case where values are floats [0:1]
    // so things are scaled into a W * W square
    int radius = W * r;
    for (int i = -radius; i <= radius ; i++) {
        for (int j = -radius; j <= radius ; j++) {
            int ix = j + x * W;
            int iy = i + y * W;
            // clamping
            ix = std::max(std::min((int)(W-1), ix), 0);
            iy = std::max(std::min((int)(W-1), iy), 0);
            // only draw if we are inside the circle
            float d2 = (ix - x *(W-1) ) * (ix - x *(W-1) ) + (iy - y *(W-1) ) * (iy - y *(W-1) );
            if (d2 > radius*radius) continue;
            int idx = ix + iy * W;
            grid[idx].addColor(test.c);
        }
    }
}

void draw_circle_dual(const test_case& test) {
    const float& x = test.x;
    const float& y = test.y;
    const float& r = test.r; 
    const color& c = test.c;

    const int radius = W * r;
    const int x0 = W *x;
    const int y0 = W *y;
    const int start = std::max(0,   (int)(y0 - radius));
    const int end   = std::min(W-1, (int)(y0 + radius));
    int idx;
    for (int i = start; i <= end ; i++) {
        int yp = i - y0;
        // PD(yp);
        int diff = radius * radius - yp *yp;
        if (diff < 1) continue;
#if SQRLUT
        assert(diff < lut_size);
        diff = sqrt_lut[diff];
#else
        diff = (int) std::sqrt(diff);
#endif        
        // start
        int ix = x0 - diff;
        if (ix < 0) ix = 0;
        idx = ix + i * W;
        accumulator[idx].enter.addColor(c);

        // end
        ix = (int)(x0 + diff);
        if (ix < W) {
            idx = ix + i* W;
            accumulator[idx].exit.addColor(c);
        }
    }
}

void scanit_dual() {
    cell acc;
    for(int i = 0; i < W; i++) {
        acc.reset();
        for(int j = 0; j < W; j++) {
            // add
            int idx = i * W + j;
            acc += accumulator[idx].enter;
            // set
            grid[idx].c = acc.c;
            grid[idx].cnt = acc.cnt;
            // onExit
            acc -= accumulator[idx].exit;
        }
    }
}
void draw_circle_single(const test_case& test) {
    const float& x = test.x;
    const float& y = test.y;
    const float& r = test.r; 
    const color& c = test.c;
    const int radius = W * r;
    const int x0 = W *x;
    const int y0 = W *y;
    const int start = std::max(0,   (int)(y0 - radius));
    const int end   = std::min(W-1, (int)(y0 + radius));
    int idx;
    for (int i = start; i <= end ; i++) {
        int yp = i - y0;
        int diff = radius * radius - yp *yp;
        if (diff < 1) continue;
#if SQRLUT
        diff = sqrt_lut[diff];
#else
        diff = (int) std::sqrt(diff);
#endif        

        // start
        int ix = x0 - diff;
        if (ix < 0) ix = 0;
        idx = ix + i * W;
        single_acc[idx].addColor(c);

        // end
        ix = (int)(x0 + diff+1);
        if (ix < W)  {
            idx = ix + i* W;
            single_acc[idx].subColor(c);
        }
    }
}

void scanit_single() {
    cell acc;
    for(int i = 0; i < W; i++) {
        acc.reset();
        for(int j = 0; j < W; j++) {
            // add
            int idx = i * W + j;
            acc += single_acc[idx];
            // set
            grid[idx].c = acc.c;
            grid[idx].cnt = acc.cnt;
        }
    }
}

void test_naive() {
    PL("Test naive\n");
    Timer timer;
    timer.start();
    printf("Init data structure %dms\n", timer.getLap());
    test_case test;
    for(int i = 0; i < cnt; i++) {
        draw_circle(cases[i]);
    }
    timer.stop();
    printf("Total time for naive %dms\n", timer.getms());
    write_img3d("naive.ppm");
    
}
void test_dual() {
    PL("Test dual buffer\n");
    Timer timer;
    timer.start();
    accumulator= new ACC[W*W];
    for(int i = 0; i < W*W; i++) {
        accumulator[i].reset();
    }
    int init_time = timer.getLap();

    for(int i = 0; i < cnt; i++) {
        draw_circle_dual(cases[i]);
    }
    int draw_lap = timer.getLap();
    scanit_dual();
    timer.stop();
    printf("Init data structure   %dms\n", init_time);
    printf("Setting acc buffer    %dms\n", draw_lap - init_time);
    printf("Scaning buffer        %dms\n", timer.getms() - draw_lap);
    printf("Total time for double %dms\n", timer.getms());
    write_img3d("dual_buffer.ppm");

}
void test_single() {
    PL("Test single buffer\n");
    Timer timer;
    timer.start();
    single_acc= new cell[W*W];
    for(int i = 0; i < W*W; i++) {
        single_acc[i].reset();
    }
    int init_time = timer.getLap();
    for(int i = 0; i < cnt; i++) {
        draw_circle_single(cases[i]);
    }
    int draw_lap = timer.getLap();
    scanit_single();
    timer.stop();
    printf("Init data structure   %dms\n", init_time);
    printf("Setting acc buffer    %dms\n", draw_lap - init_time);
    printf("Scaning buffer        %dms\n", timer.getms() - draw_lap);
    printf("Total time for single %dms\n", timer.getms());

    // write the image on out
    write_img3d("single_buffer.ppm");
}

int main(int argc, char **args) {
    int test = 0;
    if (argc > 1)
        cnt = atoi(args[1]);
    if (argc > 2)
        test = atoi(args[2]);
    if (argc > 3)
        seed = atol(args[3]);
    
    init_tests();
    init_lut();
    
    // grid init
    grid = new cell[W*W];
    for (int i = 0; i < cnt; i++)
        grid[i].reset();
    
    if (0 == test)
        test_naive();
    else if (1 == test)
        test_dual();
    else if (2 == test)
        test_single();
    else {
        // test_naive();
        test_dual();
        test_single();
    }

    return 0;
}




// void read_cases() {
//     PL("read_cases");
//     float r,g,b,x,y,rr;
//     while ( 6 == scanf("%f %f %f %f %f %f",&r,&g,&b,&x,&y,&rr) ) {
//         color c(r,g,b);
//         draw_circle(c, x,y,rr);
// //        draw_circles(c, x,y,rr);
//     }
// //    scanit();
// }
