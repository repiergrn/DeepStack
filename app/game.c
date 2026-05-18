#include "game.h"

#include "stm32g4_gpio.h"

#include "rectangle.h"
#include "tft_ili9341/stm32g4_xpt2046.h"

bool game_init(Game* game) {
    if (game == NULL) {
        return false;
    }

    game->score = 0;
    game->toright = true;

    game->rect_y = 0;
    game->rect_length = RECTANGLE_DEFAULT_SIZE;

    game->rect_x = RECTANGLE_DEFAULT_HEIGHT * RECTANGLES_DEFAULT_COUNT;
    game->height = RECTANGLE_DEFAULT_HEIGHT;

    if (!rectangles_init(&game->rects)) {
        return false;
    }

    return true;
}

void game_free(Game* game) {
    if (game == NULL) {
        return;
    }

    rectangles_free(&game->rects);

    game->score = 0;
    game->toright = false;

    game->rect_y = 0;
    game->rect_x = 0;
    game->rect_length = 0;
    game->height = 0;
}

void game_reset(Game* game) {
    if (game == NULL) {
        return;
    }

    game->score = 0;
    game->toright = true;

    game->rect_y = 0;
    game->rect_length = RECTANGLE_DEFAULT_SIZE;

    game->rect_x = RECTANGLE_DEFAULT_HEIGHT * RECTANGLES_DEFAULT_COUNT;
    game->height = RECTANGLE_DEFAULT_HEIGHT;

    rectangles_reset(&game->rects);
}

static bool handle_touch(Game *game, bool touched, bool *touched_before, uint32_t *last_touch_tick) {
    if (!game) {
        return false;
    }

    if (touched && !(*touched_before)) {
        uint32_t now = HAL_GetTick();

        if (now - *last_touch_tick < 300) {
            *touched_before = touched;
            return true;
        }

        *last_touch_tick = now;

        Rectangle moving = {
            game->rect_y,
            (uint16_t)(game->rect_y + game->rect_length)
        };

        RectAddResult res = rectangles_push_if_intersect(&game->rects, &moving);

        if (res != RECT_ADD_SUCCESS) {
            return false;
        }

        const Rectangle *top = rectangles_top(&game->rects);
        if (top != NULL) {
            game->rect_y = top->y_0;
            game->rect_length = (uint16_t)(top->y_1 - top->y_0);
        }

        if (game->rect_x < RECTANGLES_TOTAL_HEIGHT) {
            game->rect_x += game->height;
        }

        game->score++;

        ILI9341_Fill(0x0010);
        rectangles_draw_all(&game->rects);
    }

    *touched_before = touched;
    return true;
}

static void update_movement(Game *game) {
    if (game->toright) {
        if (game->rect_y + game->rect_length >= 240) {
            game->toright = false;
            game->rect_y--;
        } else {
            game->rect_y++;
        }
    } else {
        if (game->rect_y == 0) {
            game->toright = true;
            game->rect_y++;
        } else {
            game->rect_y--;
        }
    }
}

static void hide_rect(Game *game) {
    ILI9341_DrawFilledRectangle(
        game->rect_x,
        game->rect_y > 0 ? game->rect_y - 1 : 0,
        game->rect_x + game->height,
        game->rect_y + game->rect_length + 1,
        0x0010
    );
}

static void render_rect(Game *game) {
    ILI9341_DrawFilledRectangle(
        game->rect_x,
        game->rect_y,
        game->rect_x + game->height,
        game->rect_y + game->rect_length,
        ILI9341_COLOR_CYAN
    );
}

void game_play(Game *game, GameDifficulty difficulty) {
    if (game == NULL) {
        return;
    }

    ILI9341_Fill(0x0010);
    rectangles_draw_all(&game->rects);
    bool touched_before = false;
    uint32_t last_touch_tick = 0;

    uint32_t base_delay;
    if (difficulty == DIFFICULTY_EASY)        base_delay = 12;
    else if (difficulty == DIFFICULTY_MEDIUM) base_delay = 7;
    else                                       base_delay = 3;

    uint32_t min_delay;
    if (difficulty == DIFFICULTY_EASY)        min_delay = 4;
    else if (difficulty == DIFFICULTY_MEDIUM) min_delay = 1;
    else                                       min_delay = 0;

    for(;;) {
        int16_t tx, ty;
        bool touched = XPT2046_getMedianCoordinates(&tx, &ty, XPT2046_COORDINATE_SCREEN_RELATIVE);

        if (!handle_touch(game, touched, &touched_before, &last_touch_tick)) {
            return;
        }

        hide_rect(game);
        update_movement(game);
        render_rect(game);

        uint32_t reduction = game->score / 2;
        uint32_t delay = (base_delay > reduction + min_delay) ? base_delay - reduction : min_delay;

        if (delay > 0) HAL_Delay(delay);
    }
}
