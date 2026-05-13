#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <stdbool.h>

#include "tft_ili9341/stm32g4_ili9341.h"

#define RECTANGLES_DEFAULT_CAPACITY 18

#define RECTANGLE_DEFAULT_Y0 70
#define RECTANGLE_DEFAULT_SIZE 100

#define RECTANGLE_DEFAULT_COLOR ILI9341_COLOR_BLACK

#define RECTANGLE_DEFAULT_HEIGHT 10
#define RECTANGLES_DEFAULT_COUNT 4
#define RECTANGLES_TOTAL_HEIGHT (RECTANGLE_DEFAULT_HEIGHT * RECTANGLES_DEFAULT_CAPACITY)

typedef struct {
    uint16_t y_0;
    uint16_t y_1;
} Rectangle;

typedef struct {
    uint16_t capacity;
    uint16_t size;
    uint16_t begin;
    Rectangle* data;
} Rectangles;

typedef enum {
    RECT_ADD_NONE,
    RECT_ADD_SUCCESS,
    RECT_ADD_ERROR
} RectAddResult;

bool rectangles_init(Rectangles* rects);
void rectangles_free(Rectangles* rects);
void rectangles_reset(Rectangles* rects);

bool rectangles_push(Rectangles* rects, Rectangle rect);
const Rectangle* rectangles_top(const Rectangles* rects);

bool rectangle_intersection(const Rectangle* a, const Rectangle* b, Rectangle* out);
bool rectangles_top_intersection(const Rectangles* rects, const Rectangle* other, Rectangle* out);

RectAddResult rectangles_push_if_intersect(Rectangles* rects, const Rectangle* rect);

void rectangles_draw_all(const Rectangles* rects);

#endif
