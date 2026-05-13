#include "rectangle.h"

#include <stdio.h>
#include <stdlib.h>

static uint16_t rectangles_index(const Rectangles* rects, uint16_t logical_index) {
    return (uint16_t)((rects->begin + logical_index) % rects->capacity);
}

bool rectangles_init(Rectangles* rects) {
    if (rects == NULL) {
        return false;
    }

    Rectangle* data = malloc(RECTANGLES_DEFAULT_CAPACITY * sizeof(*data));
    if (!data) {
        return false;
    }

    *rects = (Rectangles){
        .capacity = RECTANGLES_DEFAULT_CAPACITY,
        .size = 0,
        .begin = 0,
        .data = data
    };

    for (uint16_t i = 0; i < RECTANGLES_DEFAULT_COUNT; i++) {
        rectangles_push(rects, (Rectangle){
            RECTANGLE_DEFAULT_Y0,
			RECTANGLE_DEFAULT_Y0 + RECTANGLE_DEFAULT_SIZE
        });
    }

    return true;
}

void rectangles_free(Rectangles* rects) {
    if (rects == NULL) {
        return;
    }

    free(rects->data);

    rects->data = NULL;
    rects->capacity = 0;
    rects->size = 0;
    rects->begin = 0;
}

void rectangles_reset(Rectangles* rects) {
    if (rects == NULL) {
        return;
    }

    rects->size = 0;
    rects->begin = 0;

    for (uint16_t i = 0; i < RECTANGLES_DEFAULT_COUNT; i++) {
		rectangles_push(rects, (Rectangle){
			RECTANGLE_DEFAULT_Y0,
			RECTANGLE_DEFAULT_Y0 + RECTANGLE_DEFAULT_SIZE
		});
	}
}

bool rectangles_push(Rectangles* rects, Rectangle rect) {
    if (rects == NULL || rects->data == NULL || rects->capacity == 0) {
        return false;
    }

    if (rects->size < rects->capacity) {
        const uint16_t idx = rectangles_index(rects, rects->size);

        rects->data[idx] = rect;
        rects->size++;

        return true;
    }

    rects->data[rects->begin] = rect;
    rects->begin = (uint16_t)((rects->begin + 1) % rects->capacity);

    return true;
}

const Rectangle* rectangles_top(const Rectangles* rects) {
    if (rects == NULL || rects->data == NULL || rects->size == 0) {
        return NULL;
    }

    const uint16_t idx = rectangles_index(rects, (uint16_t)(rects->size - 1));

    return &rects->data[idx];
}

bool rectangle_intersection(const Rectangle* a, const Rectangle* b, Rectangle* out) {
    if (a == NULL || b == NULL || out == NULL) {
        return false;
    }

    out->y_0 = (a->y_0 > b->y_0) ? a->y_0 : b->y_0;
    out->y_1 = (a->y_1 < b->y_1) ? a->y_1 : b->y_1;

    return out->y_0 < out->y_1;
}

bool rectangles_top_intersection(const Rectangles* rects, const Rectangle* other, Rectangle* out) {
    const Rectangle* top = rectangles_top(rects);

    if (top == NULL) {
        return false;
    }

    return rectangle_intersection(top, other, out);
}

RectAddResult rectangles_push_if_intersect(Rectangles* rects, const Rectangle* rect) {
    if (rects == NULL || rect == NULL) {
        return RECT_ADD_ERROR;
    }

    Rectangle inter;

    if (!rectangles_top_intersection(rects, rect, &inter)) {
        return RECT_ADD_NONE;
    }

    if (!rectangles_push(rects, inter)) {
        return RECT_ADD_ERROR;
    }

    return RECT_ADD_SUCCESS;
}

void rectangles_draw_all(const Rectangles* rects) {
    if (rects == NULL || rects->data == NULL || RECTANGLE_DEFAULT_HEIGHT == 0) {
        return;
    }

    for (uint16_t i = 0; i < rects->size; i++) {
    	printf("Rendered");
        uint16_t idx = rectangles_index(rects, i);
        const Rectangle* rect = &rects->data[idx];

        uint16_t x0 = i * RECTANGLE_DEFAULT_HEIGHT;
        uint16_t x1 = x0 + RECTANGLE_DEFAULT_HEIGHT - 1;
        ILI9341_DrawFilledRectangle(
            x0,
            rect->y_0,
            x1,
            rect->y_1,
            RECTANGLE_DEFAULT_COLOR
        );
    }
}
