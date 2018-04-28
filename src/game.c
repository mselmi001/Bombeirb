#include <assert.h>
#include <time.h>
#include <map.h>
#include <game.h>
#include <misc.h>
#include <window.h>
#include <sprite.h>
#include <player.h>
#include <bomb.h>
#include <monster.h>



struct game {
	struct map** maps;       // the game's map
	short levels;            // nb maps of the game
	short current;
	struct player* player;
	struct bomb_list* bomb_list;
	struct monster_list* monster_list[4];
	struct monster* monster;
	int key;
	int pause;
};

struct game* game_new(void) {
	sprite_load(); // load sprites into process memory

	struct game* game = malloc(sizeof(*game));
	game->maps = malloc(sizeof(struct game));
	game->maps[0]=map_get_default();
	game->maps[1]=load_map("data/map_1");
	game->maps[2]=load_map("data/map_2");
	game->maps[3]=load_map("data/map_3");
	game->maps[4]=load_map("data/map_finale");
	game->levels = 4;
	game->current = 0;
	game->key=0;
	game->pause=0;

	game->player = player_init(4);
	player_from_map(game->player, game->maps[0]); // get x,y of the player on the first map

	game->bomb_list = bomb_list_init();

	game->monster_list[0] = monster_list_init(game->maps[0]);
	game->monster_list[1] = monster_list_init(game->maps[1]);
	game->monster_list[2] = monster_list_init(game->maps[2]);
	game->monster_list[3] = monster_list_init(game->maps[3]);
	return game;
}

int game_set_nb_keys(struct game* game){
	assert(game);
	game->key = player_get_nb_keys(game_get_player(game));
	return game->key;
}

int game_get_current_level(struct game* game){
	assert(game);
	return game->current;
}

void game_set_level(struct game* game){
	assert(game);
	game->current = player_get_current_level(game_get_player(game));
}

void game_free(struct game* game) {
	assert(game);
	player_free(game->player);
	for (int i = 0; i < game->levels; i++)
		map_free(game->maps[i]);
}

struct map* game_get_current_map(struct game* game) {
	assert(game);
	return game->maps[game->current];
}

struct player* game_get_player(struct game* game) {
	assert(game);
	return game->player;
}

struct monster* game_get_current_monster(struct game* game) {
	assert(game);
	return game->monster;
}

struct monster_list* game_get_monster_list(struct game* game) {
	assert(game);
	return game->monster_list[game->current];
}

void game_set_monster(struct monster* monster, struct game* game){
	assert(game);
	game->monster = monster;
}

void game_set_monster_list(struct monster_list* monster_list, struct game* game){
	assert(game);
	game->monster_list[game->current] = monster_list;
}

void game_banner_display(struct game* game) {
	assert(game);

	struct map* map = game_get_current_map(game);

	int y = (map_get_height(map)) * SIZE_BLOC;
	for (int i = 0; i < map_get_width(map); i++)
		window_display_image(sprite_get_banner_line(), i * SIZE_BLOC, y);

	int white_bloc = ((map_get_width(map) * SIZE_BLOC) - 6 * SIZE_BLOC) / 4;
	int x = white_bloc;

	y = (map_get_height(map) * SIZE_BLOC) + LINE_HEIGHT;
	window_display_image(sprite_get_banner_life(), x, y);

	x = white_bloc + SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_nb_life(game_get_player(game))), x, y);

	x = 2 * white_bloc + 1 * SIZE_BLOC;
	window_display_image(sprite_get_banner_bomb(), x, y);

	x = 2 * white_bloc + 2 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_nb_bomb(game_get_player(game))), x, y);

	x = 3 * white_bloc + 2 * SIZE_BLOC;
	window_display_image(sprite_get_banner_range(), x, y);

	x = 3 * white_bloc + 3 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_range_bomb(game_get_player(game))), x, y);

	x = 4 * white_bloc + 3 * SIZE_BLOC;
	window_display_image(sprite_get_key(), x, y);

	x = 4 * white_bloc + 4 * SIZE_BLOC;
	window_display_image(sprite_get_number(game_set_nb_keys(game)), x, y);


}


void game_display(struct game* game) {
	assert(game);
	window_clear();
	game_banner_display(game);
	map_display(game_get_current_map(game));
	player_display(game->player);
	bomb_list_display(game->bomb_list,game);
	monster_list_display(game->monster_list[game->current]);
	window_refresh();
}

static short input_keyboard(struct game* game) {
	SDL_Event event;
	struct player* player = game_get_player(game);

	struct map* map = game_get_current_map(game);

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			return 1;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				return 1;
			case SDLK_UP:
				if (game->pause == 0){
					player_set_current_way(player, NORTH);
					player_move(player, map);
				}
				break;
			case SDLK_DOWN:
				if (game->pause == 0){
					player_set_current_way(player, SOUTH);
					player_move(player, map);
				}
				break;
			case SDLK_RIGHT:
				if (game->pause == 0){
					player_set_current_way(player, EAST);
					player_move(player, map);
				}
				break;
			case SDLK_LEFT:
				if (game->pause == 0){
					player_set_current_way(player, WEST);
					player_move(player, map);
				}
				break;
			case SDLK_SPACE:
				if (game->pause == 0){
					if(player_get_nb_bomb(player) > 0 ){
						game->bomb_list = bomb_list_add(game->bomb_list,game);
						player_dec_nb_bomb(player);
						map_set_cell_type(map,  player_get_x(game->player), player_get_y(game->player), CELL_BOMB);
					}
				}
				break;
			case SDLK_p:
				if (game->pause == 0){
					game->pause = 1;
				}
				else
					game->pause = 0;
				break;
			default:
				break;
				}
				break;
			}
		}

	return 0;
}

int game_update(struct game* game) {
	if (input_keyboard(game))
		return 1; // exit game

	game_set_level(game);

	if (game->pause == 0){
		game->bomb_list = bomb_list_update(game, game->bomb_list);
		game->monster_list[game->current] = monster_list_update(game);

		if(player_update(game->player))
			return 1;


	}
	return 0;
}






