#include "board.h"
#include "game.h"
#include "menu.h"
#include "rectangle.h"

#include "stm32g4_gpio.h"

#include <stdbool.h>

int main(void) {
	GameState state = STATE_INIT;
	Game game;

	for(;;) {
		switch(state) {
			case STATE_INIT:
				board_init();
				game_init(&game);

				if (!game_init(&game)) {
					while (1);
				}

				state = STATE_MENU;
				break;
			case STATE_GAME:
				game_play(&game);
			case STATE_SCORE:
				show_score_screen(&game);
				game_reset(&game);
			case STATE_MENU:
				show_game_menu();
				state = STATE_GAME;
		 }
	}
}
