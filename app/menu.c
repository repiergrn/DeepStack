#include "menu.h"

#include "tft_ili9341/stm32g4_fonts.h"
#include "tft_ili9341/stm32g4_ili9341.h"

#include <stdio.h>

void show_score_screen(Game *game) {
	if (game == NULL) {
		return;
	}

	ILI9341_Rotate(ILI9341_Orientation_Portrait_1);
	ILI9341_Fill(ILI9341_COLOR_RED);

	char buffer[32];
	snprintf(buffer, sizeof(buffer), "Score : %d", game->score);

	uint16_t text_w;
	uint16_t text_h;

	ILI9341_GetStringSize(buffer, &Font_16x26, &text_w, &text_h);

	uint16_t x = (ILI9341_getOptions().width - text_w) / 2;
	uint16_t y = (ILI9341_getOptions().height - text_h) / 2;

	ILI9341_Puts(x, y, buffer, &Font_16x26, ILI9341_COLOR_BLACK, ILI9341_COLOR_RED);

	do {} while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5));

	ILI9341_Rotate(ILI9341_Orientation_Landscape_2);
	ILI9341_Fill(ILI9341_COLOR_WHITE);
}

void show_game_menu(void) {
    ILI9341_Rotate(ILI9341_Orientation_Portrait_1);
    ILI9341_Fill(ILI9341_COLOR_WHITE);

    uint16_t cx = ILI9341_getOptions().width / 2;

    for (int i = 0; i < 10; i++) {
        uint16_t w = i * 10;

        ILI9341_DrawFilledRectangle(
            cx - w,
            20 + i * 20,
            cx + w,
            20 + (i + 1) * 20,
            ILI9341_COLOR_CYAN
        );
    }

    char title[] = "STACK";

    uint16_t tw, th;
    ILI9341_GetStringSize(title, &Font_16x26, &tw, &th);

    uint16_t tx = (ILI9341_getOptions().width - tw) / 2;

    ILI9341_Puts(
        tx,
        10,
        title,
        &Font_16x26,
        ILI9341_COLOR_BLACK,
        ILI9341_COLOR_WHITE
    );

    uint16_t bw = 120;
    uint16_t bh = 50;

    uint16_t bx = (ILI9341_getOptions().width - bw) / 2;
    uint16_t by = (ILI9341_getOptions().height / 2);

    ILI9341_DrawFilledRectangle(
        bx, by,
        bx + bw, by + bh,
        ILI9341_COLOR_BLACK
    );

    char play[] = "PLAY";
    uint16_t pw, ph;

    ILI9341_GetStringSize(play, &Font_11x18, &pw, &ph);

    ILI9341_Puts(
        bx + (bw - pw) / 2,
        by + (bh - ph) / 2,
        play,
        &Font_11x18,
        ILI9341_COLOR_WHITE,
        ILI9341_COLOR_BLACK
    );

    do {} while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5));

    ILI9341_Rotate(ILI9341_Orientation_Landscape_2);
    ILI9341_Fill(ILI9341_COLOR_WHITE);
}
