#include "menu.h"

#include "tft_ili9341/stm32g4_xpt2046.h"
#include "tft_ili9341/stm32g4_fonts.h"
#include "tft_ili9341/stm32g4_ili9341.h"
#include "flash_storage.h"

#include <stdio.h>

void show_score_screen(Game *game, GameDifficulty difficulty) {
	if (game == NULL) {
		return;
	}

	ILI9341_Rotate(ILI9341_Orientation_Portrait_1);
	ILI9341_Fill(0x0010);

	// Bande cyan en haut
	ILI9341_DrawFilledRectangle(0, 0, 240, 10, ILI9341_COLOR_CYAN);

	// GAME OVER
	char over[] = "GAME OVER";
	uint16_t ow, oh;
	ILI9341_GetStringSize(over, &Font_16x26, &ow, &oh);
	ILI9341_Puts((240 - ow) / 2, 40, over, &Font_16x26, ILI9341_COLOR_CYAN, 0x0010);

	// Ligne cyan
	ILI9341_DrawFilledRectangle(20, 80, 220, 83, ILI9341_COLOR_CYAN);

	// Difficulté
	char *diff_str = (difficulty == DIFFICULTY_EASY) ? "Facile" :
	                 (difficulty == DIFFICULTY_MEDIUM) ? "Moyen" : "Difficile";
	uint16_t dw, dh;
	ILI9341_GetStringSize(diff_str, &Font_11x18, &dw, &dh);
	ILI9341_Puts((240 - dw) / 2, 95, diff_str, &Font_11x18, ILI9341_COLOR_WHITE, 0x0010);

	// Score
	char buffer[32];
	snprintf(buffer, sizeof(buffer), "Score : %d", game->score);
	uint16_t text_w, text_h;
	ILI9341_GetStringSize(buffer, &Font_16x26, &text_w, &text_h);
	ILI9341_Puts((240 - text_w) / 2, 130, buffer, &Font_16x26, ILI9341_COLOR_WHITE, 0x0010);

	// Meilleur score
	uint32_t best = flash_read_best_score(difficulty);
	if ((uint32_t)game->score > best) {
		flash_write_best_score(difficulty, (uint32_t)game->score);
		best = (uint32_t)game->score;
	}

	char best_buf[32];
	snprintf(best_buf, sizeof(best_buf), "Best  : %lu", best);
	uint16_t bw, bh;
	ILI9341_GetStringSize(best_buf, &Font_16x26, &bw, &bh);
	ILI9341_Puts((240 - bw) / 2, 175, best_buf, &Font_16x26, ILI9341_COLOR_CYAN, 0x0010);

	// Tap to continue
	char tap[] = "Tap to continue";
	uint16_t tw, th;
	ILI9341_GetStringSize(tap, &Font_11x18, &tw, &th);
	ILI9341_Puts((240 - tw) / 2, 270, tap, &Font_11x18, ILI9341_COLOR_CYAN, 0x0010);

	// Bande cyan en bas
	ILI9341_DrawFilledRectangle(0, 310, 240, 320, ILI9341_COLOR_CYAN);

	int16_t tx, ty;
	while (!XPT2046_getMedianCoordinates(&tx, &ty, XPT2046_COORDINATE_SCREEN_RELATIVE));

	ILI9341_Rotate(ILI9341_Orientation_Landscape_2);
	ILI9341_Fill(0x0010);
}

GameDifficulty show_game_menu(void) {
    ILI9341_Rotate(ILI9341_Orientation_Portrait_1);
    ILI9341_Fill(0x0010);

    // Bande cyan en haut
    ILI9341_DrawFilledRectangle(0, 0, 240, 8, ILI9341_COLOR_CYAN);

    // Zone titre
    ILI9341_DrawFilledRectangle(0, 20, 240, 80, 0x600F);

    // Titre STACK
    char title[] = "STACK";
    uint16_t tw, th;
    ILI9341_GetStringSize(title, &Font_16x26, &tw, &th);
    ILI9341_Puts((240 - tw) / 2 + 2, 32, title, &Font_16x26, 0x0010, 0x600F);
    ILI9341_Puts((240 - tw) / 2, 30, title, &Font_16x26, ILI9341_COLOR_CYAN, 0x600F);

    // Ligne cyan sous titre
    ILI9341_DrawFilledRectangle(20, 82, 220, 86, ILI9341_COLOR_CYAN);

    // Blocs de sélection difficulté
    // Difficile : grand bloc en haut
    uint16_t bw_hard = 160, bh_block = 22;
    uint16_t bx_hard = (240 - bw_hard) / 2;
    uint16_t by_hard = 100;
    ILI9341_DrawFilledRectangle(bx_hard, by_hard, bx_hard + bw_hard, by_hard + bh_block, 0x051F);
    ILI9341_DrawRectangle(bx_hard, by_hard, bx_hard + bw_hard, by_hard + bh_block, ILI9341_COLOR_WHITE);
    char hard[] = "DIFFICILE";
    uint16_t hw, hh;
    ILI9341_GetStringSize(hard, &Font_11x18, &hw, &hh);
    ILI9341_Puts(bx_hard + (bw_hard - hw) / 2, by_hard + (bh_block - hh) / 2, hard, &Font_11x18, ILI9341_COLOR_WHITE, 0x051F);

    // Moyen : bloc moyen au milieu
    uint16_t bw_med = 120;
    uint16_t bx_med = (240 - bw_med) / 2;
    uint16_t by_med = 138;
    ILI9341_DrawFilledRectangle(bx_med, by_med, bx_med + bw_med, by_med + bh_block, ILI9341_COLOR_CYAN);
    ILI9341_DrawRectangle(bx_med, by_med, bx_med + bw_med, by_med + bh_block, ILI9341_COLOR_WHITE);
    char med[] = "MOYEN";
    uint16_t mw, mh;
    ILI9341_GetStringSize(med, &Font_11x18, &mw, &mh);
    ILI9341_Puts(bx_med + (bw_med - mw) / 2, by_med + (bh_block - mh) / 2, med, &Font_11x18, 0x0010, ILI9341_COLOR_CYAN);

    // Facile : petit bloc en bas
    uint16_t bw_easy = 80;
    uint16_t bx_easy = (240 - bw_easy) / 2;
    uint16_t by_easy = 176;
    ILI9341_DrawFilledRectangle(bx_easy, by_easy, bx_easy + bw_easy, by_easy + bh_block, 0x051F);
    ILI9341_DrawRectangle(bx_easy, by_easy, bx_easy + bw_easy, by_easy + bh_block, ILI9341_COLOR_WHITE);
    char easy[] = "FACILE";
    uint16_t ew, eh;
    ILI9341_GetStringSize(easy, &Font_11x18, &ew, &eh);
    ILI9341_Puts(bx_easy + (bw_easy - ew) / 2, by_easy + (bh_block - eh) / 2, easy, &Font_11x18, ILI9341_COLOR_WHITE, 0x051F);

    // Bouton PLAY
    uint16_t btn_w = 150;
    uint16_t btn_h = 50;
    uint16_t btn_x = (240 - btn_w) / 2;
    uint16_t btn_y = 240;

    ILI9341_DrawFilledRectangle(btn_x, btn_y, btn_x + btn_w, btn_y + btn_h, ILI9341_COLOR_CYAN);
    ILI9341_DrawRectangle(btn_x - 2, btn_y - 2, btn_x + btn_w + 2, btn_y + btn_h + 2, ILI9341_COLOR_WHITE);

    char play[] = "PLAY";
    uint16_t pw, ph;
    ILI9341_GetStringSize(play, &Font_16x26, &pw, &ph);
    ILI9341_Puts(
        btn_x + (btn_w - pw) / 2,
        btn_y + (btn_h - ph) / 2,
        play,
        &Font_16x26,
        0x0010,
        ILI9341_COLOR_CYAN
    );

    // Bande cyan en bas
    ILI9341_DrawFilledRectangle(0, 310, 240, 320, ILI9341_COLOR_CYAN);

    // Sélection difficulté par défaut
    GameDifficulty selected = DIFFICULTY_MEDIUM;

    // Indicateur de sélection initial
    ILI9341_DrawRectangle(bx_med - 3, by_med - 3, bx_med + bw_med + 3, by_med + bh_block + 3, ILI9341_COLOR_YELLOW);

    int16_t tx, ty;
    bool waiting = true;

    while (waiting) {
        if (XPT2046_getMedianCoordinates(&tx, &ty, XPT2046_COORDINATE_SCREEN_RELATIVE)) {
            // Touch sur DIFFICILE
            if (tx >= bx_hard && tx <= bx_hard + bw_hard && ty >= by_hard && ty <= by_hard + bh_block) {
                selected = DIFFICULTY_HARD;
                // Effacer anciens indicateurs
                ILI9341_DrawRectangle(bx_hard - 3, by_hard - 3, bx_hard + bw_hard + 3, by_hard + bh_block + 3, 0x0010);
                ILI9341_DrawRectangle(bx_med - 3, by_med - 3, bx_med + bw_med + 3, by_med + bh_block + 3, 0x0010);
                ILI9341_DrawRectangle(bx_easy - 3, by_easy - 3, bx_easy + bw_easy + 3, by_easy + bh_block + 3, 0x0010);
                ILI9341_DrawRectangle(bx_hard - 3, by_hard - 3, bx_hard + bw_hard + 3, by_hard + bh_block + 3, ILI9341_COLOR_YELLOW);
            }
            // Touch sur MOYEN
            else if (tx >= bx_med && tx <= bx_med + bw_med && ty >= by_med && ty <= by_med + bh_block) {
                selected = DIFFICULTY_MEDIUM;
                ILI9341_DrawRectangle(bx_hard - 3, by_hard - 3, bx_hard + bw_hard + 3, by_hard + bh_block + 3, 0x0010);
                ILI9341_DrawRectangle(bx_med - 3, by_med - 3, bx_med + bw_med + 3, by_med + bh_block + 3, 0x0010);
                ILI9341_DrawRectangle(bx_easy - 3, by_easy - 3, bx_easy + bw_easy + 3, by_easy + bh_block + 3, 0x0010);
                ILI9341_DrawRectangle(bx_med - 3, by_med - 3, bx_med + bw_med + 3, by_med + bh_block + 3, ILI9341_COLOR_YELLOW);
            }
            // Touch sur FACILE
            else if (tx >= bx_easy && tx <= bx_easy + bw_easy && ty >= by_easy && ty <= by_easy + bh_block) {
                selected = DIFFICULTY_EASY;
                ILI9341_DrawRectangle(bx_hard - 3, by_hard - 3, bx_hard + bw_hard + 3, by_hard + bh_block + 3, 0x0010);
                ILI9341_DrawRectangle(bx_med - 3, by_med - 3, bx_med + bw_med + 3, by_med + bh_block + 3, 0x0010);
                ILI9341_DrawRectangle(bx_easy - 3, by_easy - 3, bx_easy + bw_easy + 3, by_easy + bh_block + 3, 0x0010);
                ILI9341_DrawRectangle(bx_easy - 3, by_easy - 3, bx_easy + bw_easy + 3, by_easy + bh_block + 3, ILI9341_COLOR_YELLOW);
            }
            // Touch sur PLAY
            else if (tx >= btn_x && tx <= btn_x + btn_w && ty >= btn_y && ty <= btn_y + btn_h) {
                waiting = false;
            }
        }
    }

    ILI9341_Rotate(ILI9341_Orientation_Landscape_2);
    ILI9341_Fill(0x0010);

    return selected;
}
