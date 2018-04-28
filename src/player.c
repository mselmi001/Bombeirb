#include <SDL/SDL_image.h>
#include <assert.h>
#include <string.h>
#include <player.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <constant.h>
#include <game.h>
#include <map.h>
#include <bomb.h>



struct player {
	int x, y;
	enum direction current_direction;
	int nb_bombs;
	int bomb;
	int current_level;
	int key;
	int range;
	int nb_life;
	int invulnerable;
	int inv_time;
	int vitesse;
	int disable_inv;
	int princess;
};

struct player* player_init(int bomb_number) {
	struct player* player = malloc(sizeof(*player));
	if (!player)
		error("Memory error");

	player->current_direction = SOUTH;
	player->nb_bombs = bomb_number;
	player->bomb = 0;
	player->current_level = 0;
	player->key = 0;
	player->nb_life = 2;
	player->range = 1;
	player->invulnerable = 0;
	player->princess = 0;
	player->vitesse = 1000;
	return player;
}

void player_free(struct player* player) {
	assert(player);
	free(player);
}

int player_get_x(struct player* player) {
	assert(player != NULL);
	return player->x;
}

int player_get_y(struct player* player) {
	assert(player != NULL);
	return player->y;
}

void player_dec_vitesse(struct player* player){
	assert(player);
	player->vitesse -= 250;
}

int player_get_vitesse(struct player* player){
	assert(player);
	return player->vitesse;
}
void player_set_current_way(struct player* player, enum direction way) {
	assert(player);
	player->current_direction = way;
}

int player_get_current_level(struct player* player) {
	assert(player);
	return player->current_level;
}

int player_get_nb_keys(struct player* player) {
	assert(player);
	return player->key;
}

int player_get_nb_life(struct player* player){
	assert(player);
	return player->nb_life;
}

void player_inc_nb_life(struct player* player){
	assert(player);
	player->nb_life += 1 ;
}

void player_dec_nb_life(struct player* player){
	assert(player);
	if (player->nb_life > 0)
	player->nb_life -= 1 ;
}

void player_inc_current_level(struct player* player) {
	assert(player);
	player->current_level += 1 ;
}

int player_get_range_bomb(struct player* player){
	assert(player != NULL);
	return player->range;
}

void player_inc_range_bomb(struct player* player){
	assert(player);
	player->range += 1;
}

void player_dec_range_bomb(struct player* player){
	assert(player);
	if (player->range > 0)
	player->range -= 1;
}

int player_get_nb_bomb(struct player* player) {
	assert(player);
	return player->nb_bombs;
}

void player_inc_nb_bomb(struct player* player) {
	assert(player);
	player->nb_bombs += 1;
}

void player_dec_nb_bomb(struct player* player) {
	assert(player);
	if (player->nb_bombs > 0)
	player->nb_bombs -= 1;
}

int player_invulnerable(struct player* player) {
	assert(player);
	return player->invulnerable;
}

void player_set_invulnerable(struct player* player) {
	assert(player);
	player->invulnerable = 1;
	player->inv_time = SDL_GetTicks();
	player->disable_inv = SDL_GetTicks();
}

int player_get_princess(struct player* player){
	assert(player);
	return player->princess;
}

void player_from_map(struct player* player, struct map* map) {
	assert(player);
	assert(map);

	int i, j;
	for (i = 0; i < map_get_width(map); i++) {
	  for (j = 0; j < map_get_height(map); j++) {	
	    if (map_get_cell_type(map, i, j) == CELL_PLAYER) {
	      player->x = i;
	      player->y = j;
	    }
	  }
	}
}

static int player_move_aux(struct player* player, struct map* map, int x, int y) {

	if (!map_is_inside(map, x, y))
		return 0;

	switch (map_get_cell_type(map, x, y)) {
	case CELL_SCENERY:
		switch( map_get_compose_type(map, x, y)){
		case CELL_TREE:
			return 0;
			break;
		case CELL_STONE:
			return 0;
			break;
		case CELL_PRINCESS:
			player->current_level=4;
			break;
		default:
			break;
		}
		break;

	case CELL_BOX:
		return 0; /* 0 à la place de 1 pour ne pas faire diparaître les box */
		break;

	case CELL_BONUS:
		switch( map_get_compose_type(map, x, y)){
		case CELL_BONUS_RANGEINC:
			player_inc_range_bomb(player);
			break;
		case CELL_BONUS_RANGEDEC:
			player_dec_range_bomb(player);
			break;
		case CELL_BONUS_BOMBINC:
			player_inc_nb_bomb(player);
			break;
		case CELL_BONUS_BOMBDEC:
			player_dec_nb_bomb(player);
			break;
		case CELL_BONUS_LIFE:
			player_inc_nb_life(player);
			break;
		default:
			break;
		}
		break;

	case CELL_MONSTER:
		if(player->nb_life > 0 && player->invulnerable == 0 ){
			player_dec_nb_life(player);
			player_set_invulnerable(player);
				return 0;
		}
		break;

	case CELL_PLAYER:
		break;

	case CELL_DOOR:
		switch( map_get_compose_type(map, x, y)){
		case CELL_DOOR_OPEN:
			player_inc_current_level(player);
			player_dec_vitesse(player);
		break;
		case CELL_DOOR_CLOSED:
			if (player->key >= 1){
				player->key -= 1 ;
				map_set_cell_type(map,x,y, CELL_DOOR_OPEN);
			}
			return 0;
		default:
			break;
		}

		break;

	case CELL_KEY:
		player->key += 1 ;
		break;

	case CELL_BOMB:
		switch( map_get_compose_type(map, x, y)){
		case CELL_BOMB_EXPLOSION_EN_COURS:
			player_inc_nb_life(player);
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}

	// Player has moved
	return 1;
}

int player_move(struct player* player, struct map* map) {
	int x = player->x;
	int y = player->y;
	int move = 0;

	switch (player->current_direction) {
	case NORTH:
		if (map_get_cell_type(map , player->x , player->y - 1) == CELL_BOX ){
			if (map_get_cell_type(map , player->x , player->y -2) == CELL_EMPTY){
				if (player->y - 1 != 0){
					map_set_cell_type(map , player->x , player->y - 2 , CELL_BOX ) ;
					map_set_cell_type(map , player->x , player->y - 1 , CELL_EMPTY ) ;}}}
		if (player->y!=0){
			if (player_move_aux(player, map, x, y - 1)){
				player->y--;
				move = 1;}
			}
		break;

	case SOUTH:
		if (map_get_cell_type(map , player->x , player->y + 1 ) == CELL_BOX ){
					if (map_get_cell_type(map , player->x , player->y + 2) == CELL_EMPTY){
						if (player->y + 1 != map_get_height(map)-1 ){
							map_set_cell_type(map , player->x , player->y + 2  , CELL_BOX ) ;
							map_set_cell_type(map , player->x , player->y + 1  , CELL_EMPTY ) ;}}}
		if (player->y!=map_get_height(map)-1){
			if (player_move_aux(player, map, x, y + 1)) {
				player->y++;
				move = 1;}
		}
		break;

	case WEST:
		if (map_get_cell_type(map , player->x - 1, player->y ) == CELL_BOX ){
							if (map_get_cell_type(map , player->x - 2 , player->y) == CELL_EMPTY){
								if (player->x - 1 != 0){
									map_set_cell_type(map , player->x - 2, player->y , CELL_BOX ) ;
									map_set_cell_type(map , player->x - 1 , player->y  , CELL_EMPTY ) ;}}}
		if (player->x!=0){
			if (player_move_aux(player, map, x - 1, y)) {
				player->x--;
				move = 1;}
		}
		break;

	case EAST:
		if (map_get_cell_type(map , player->x + 1, player->y ) == CELL_BOX ){
			if (map_get_cell_type(map , player->x + 2 , player->y) == CELL_EMPTY){
				if (player->x + 1 != 0){
					map_set_cell_type(map , player->x + 2, player->y , CELL_BOX ) ;
					map_set_cell_type(map , player->x + 1 , player->y  , CELL_EMPTY ) ;}}}
		if (player->x!=map_get_width(map)-1){
			if (player_move_aux(player, map, x + 1, y)) {
				player->x++;
				move = 1;}
		}
		break;
	}

	if  (move && map_get_cell_type(map, x, y)==CELL_BOMB) {
		map_set_cell_type(map, player->x, player->y, CELL_PLAYER);
		move = 0;
	}

	if (move) {
		map_set_cell_type(map, x, y, CELL_EMPTY);
		map_set_cell_type(map, player->x, player->y, CELL_PLAYER);
	}
	return move;
}

void player_display(struct player* player) {
	assert(player);
	window_display_image(sprite_get_player(player->current_direction),	player->x * SIZE_BLOC, player->y * SIZE_BLOC);
}

int player_update(struct player* player){
	int time = SDL_GetTicks();
	if(player->invulnerable == 1){
		if((time - player->inv_time) > 1000)
			player->invulnerable = 0;
	}
	if(player->nb_life == 0){
		return 1;
	}
	return 0;
}


