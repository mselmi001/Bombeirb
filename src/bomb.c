#include <SDL/SDL_image.h>
#include <assert.h>
#include <game.h>
#include <player.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <constant.h>
#include <bomb.h>
#include <monster.h>


struct bomb {
	int x, y;
	int time;
	int range;
	enum bomb_state state; // l'état de la bombe
	int bomb_sense[4]; // les sens de l'explosion de la bombe
};

struct bomb_list{
	struct bomb* bomb;
	struct bomb_list* next_bomb;
};

struct bomb* bomb_init(int x, int y, int range){
	struct bomb* bomb = malloc(sizeof(*bomb));
	bomb->x=x;
	bomb->y=y;
	bomb->range = range;
	bomb->time = SDL_GetTicks();
	bomb->state = BOMB_4_SEC;
	for(int i=0; i < 4; i++){
		bomb->bomb_sense[i] =range;
	}
	return bomb;
}

struct bomb_list* bomb_list_init(){
	struct bomb_list* bomb_list = NULL;
	return bomb_list;
}

int bomb_get_x(struct bomb* bomb){
	assert(bomb != NULL);
	return bomb->x;
}

int bomb_get_y(struct bomb* bomb){
	assert(bomb != NULL);
	return bomb->y;
}

int bomb_get_range(struct bomb* bomb){
	assert(bomb != NULL);
	return bomb->range;
}

void bomb_inc_range(struct bomb* bomb){
	assert(bomb != NULL);
	bomb->range = +1;
}

void bomb_dec_range(struct bomb* bomb){
	assert(bomb != NULL);
	bomb->range = -1;
}

int bomb_get_state(struct bomb* bomb){
	assert(bomb != NULL);
	return bomb->state;
}

struct bomb_list* bomb_list_add(struct bomb_list* list, struct game* game){
	struct player* player = game_get_player(game);
	struct bomb_list* new_list = malloc(sizeof(*new_list));
	new_list->bomb = bomb_init(player_get_x(player),player_get_y(player),player_get_range_bomb(player));
	new_list->next_bomb = list;
	return new_list;
}

struct bomb_list* bomb_list_remove(struct bomb_list* list,struct player* player, struct map* map){
	struct bomb_list* bomb_list = list;
	if(bomb_list == NULL)
		return NULL;
	if(bomb_get_state(bomb_list->bomb) == FIN){
		struct bomb_list* list_finale = bomb_list->next_bomb;
		map_set_cell_type(map, bomb_get_x(bomb_list->bomb), bomb_get_y(bomb_list->bomb), CELL_EMPTY);
		player_inc_nb_bomb(player);
		list_finale = bomb_list_remove(list_finale, player, map);
		return list_finale;
	}
	else{
		bomb_list->next_bomb = bomb_list_remove(bomb_list->next_bomb,player, map);
	}
	return bomb_list;
}

void bomb_state( struct bomb* bomb, struct game* game){

	enum bomb_state state = bomb->state;

	switch ( state ) {
	case BOMB_4_SEC:
		bomb->state = BOMB_3_SEC;
		break;
	case BOMB_3_SEC:
		bomb->state = BOMB_2_SEC;
		break;
	case BOMB_2_SEC:
		bomb->state = BOMB_1_SEC;
		break;
	case BOMB_1_SEC:
		bomb->state = EXPLOSION;
		bomb->bomb_sense[NORTH] = explosion(game,bomb, NORTH);
		bomb->bomb_sense[SOUTH] = explosion(game,bomb, SOUTH);
		bomb->bomb_sense[WEST] = explosion(game,bomb, WEST);
		bomb->bomb_sense[EAST] = explosion(game,bomb, EAST);
		break ;
	case EXPLOSION:
		bomb->state = EXPLOSION_EN_COURS;
		break ;
	case EXPLOSION_EN_COURS:
		bomb->state = FIN;
		break;
	default:
		break;
	}
}

struct bomb* bomb_update_move(struct bomb* bomb,struct game* game){
	if (bomb!= NULL){
		unsigned int time_actual = SDL_GetTicks();
		unsigned int timer = bomb->time;
		if(bomb->state != FIN){
			if((time_actual - timer) > 1000){
				bomb_state(bomb,game);
				bomb->time = SDL_GetTicks();
			}
			if(bomb->state == EXPLOSION ){
				bomb->state = 5;
			}
		}
	}
	return bomb;
}

struct bomb_list* bomb_list_update(struct game* game, struct bomb_list* list){
	struct player* player = game_get_player(game);
	struct map* map = game_get_current_map(game);
	struct bomb_list* list_finale = list;
	while(list_finale != NULL){
		bomb_update_move(list_finale->bomb, game);
		list_finale = list_finale->next_bomb;
	}
	list = bomb_list_remove(list, player, map);
	return list;
}

int bomb_explosion(struct map* map, int x, int y){
	if(!map_is_inside(map, x, y))
			return 0;


	switch (map_get_cell_type(map,x,y)) {
	case CELL_SCENERY :
		return 0;
		break;
	case CELL_DOOR:
		return 0;
		break;
	case CELL_KEY:
		return 0;
		break;
	case CELL_BONUS:
		return 5;
		break;
	case CELL_MONSTER:
		return 3;
		break;
	case CELL_PLAYER:
		return 4;
		break;
	case CELL_BOX:
		return 2;
		break;
	default:
		break;
	}
return 1;
}

int explosion(struct game* game, struct bomb* bomb, enum direction direction){
	int i=0;
	struct player* player = game_get_player(game);
	struct map* map=game_get_current_map(game);
	struct monster_list* monster_list = game_get_monster_list(game);
	int x = bomb->x;
	int y = bomb->y;
	switch (direction){
	case NORTH:
		while (i<= bomb->range && bomb_explosion(map,x,y-i)){
			if(bomb_explosion(map, x, y-i) == 2){
				switch(map_get_compose_type(map,x,y-i)){

				case CELL_BOX_RANGEINC:
					map_set_cell_type(map, x, y-i, CELL_BONUS_RANGEINC);
					break;
				case CELL_BOX_RANGEDEC:
					map_set_cell_type(map, x, y-i, CELL_BONUS_RANGEDEC);
					break;
				case CELL_BOX_BOMBINC:
					map_set_cell_type(map, x, y-i, CELL_BONUS_BOMBINC);
					break;
				case CELL_BOX_BOMBDEC:
					map_set_cell_type(map, x, y-i, CELL_BONUS_BOMBDEC);
					break;
				case CELL_BOX_LIFE:
					map_set_cell_type(map, x, y-i, CELL_BONUS_LIFE);
					break;
				case CELL_BOX_MONSTER:
					map_set_cell_type(map, x, y-i, CELL_MONSTER);

					break;
				default:
					break;
				}
				i = i+1;
				break;
			}
			if (bomb_explosion(map, x,y-i) == 3){
				map_set_cell_type(map,x,y-i,CELL_EMPTY);
				game_set_monster_list(monster_list_remove(monster_list,x,y-i),game);

				break;
			}
			if (bomb_explosion(map, x, y-i) == 4){
				player_dec_nb_life(player);
			}
			if (bomb_explosion(map, x,y-i) == 5)
				map_set_cell_type(map,x,y-i,CELL_EMPTY);
			i++;

		}
		break;
	case SOUTH:
		while (i<= bomb->range && bomb_explosion(map,x,y+i)){
			if(bomb_explosion(map, x, y+i) == 2){
				switch(map_get_compose_type(map,x,y+i)){

				case CELL_BOX_RANGEINC:
					map_set_cell_type(map, x, y+i, CELL_BONUS_RANGEINC);
					break;
				case CELL_BOX_RANGEDEC:
					map_set_cell_type(map, x, y+i, CELL_BONUS_RANGEDEC);
					break;
				case CELL_BOX_BOMBINC:
					map_set_cell_type(map, x, y+i, CELL_BONUS_BOMBINC);
					break;
				case CELL_BOX_BOMBDEC:
					map_set_cell_type(map, x, y+i, CELL_BONUS_BOMBDEC);
					break;
				case CELL_BOX_LIFE:
					map_set_cell_type(map, x, y+i, CELL_BONUS_LIFE);
					break;
				case CELL_BOX_MONSTER:
					map_set_cell_type(map, x, y+i, CELL_MONSTER);

					break;
				default:
					break;
				}
				i = i+1;
				break;
			}
			if (bomb_explosion(map, x,y+i) == 3){
				map_set_cell_type(map,x,y+i,CELL_EMPTY);
				game_set_monster_list(monster_list_remove(monster_list,x,y+i),game);

				break;
			}
			if (bomb_explosion(map, x, y+i) == 4){
				player_dec_nb_life(player);
			}
			if (bomb_explosion(map, x,y+i) == 5)
				map_set_cell_type(map,x,y+i,CELL_EMPTY);
			i++;
		}
		break;

	case EAST:
		while (i<= bomb->range && bomb_explosion(map,x+i,y)){
			if(bomb_explosion(map, x+i, y) == 2){
				switch(map_get_compose_type(map,x+i,y)){

				case CELL_BOX_RANGEINC:
					map_set_cell_type(map, x+i, y, CELL_BONUS_RANGEINC);
					break;
				case CELL_BOX_RANGEDEC:
					map_set_cell_type(map, x+i, y, CELL_BONUS_RANGEDEC);
					break;
				case CELL_BOX_BOMBINC:
					map_set_cell_type(map, x+i, y, CELL_BONUS_BOMBINC);
					break;
				case CELL_BOX_BOMBDEC:
					map_set_cell_type(map, x+i, y, CELL_BONUS_BOMBDEC);
					break;
				case CELL_BOX_LIFE:
					map_set_cell_type(map, x+i, y, CELL_BONUS_LIFE);
					break;
				case CELL_BOX_MONSTER:
					map_set_cell_type(map, x+i, y, CELL_MONSTER);
					break;
				default:
					break;
				}
				i = i+1;
				break;
			}
			if (bomb_explosion(map, x+i,y) == 3){
				map_set_cell_type(map,x+i,y,CELL_EMPTY);
				game_set_monster_list(monster_list_remove(monster_list,x+i,y),game);
				break;
			}
			if (bomb_explosion(map, x+i, y) == 4){
				player_dec_nb_life(player);
			}
			if (bomb_explosion(map, x+i,y) == 5)
				map_set_cell_type(map,x+i,y,CELL_EMPTY);
			i++;
		}
		break;

	case WEST:
		while (i<= bomb->range && bomb_explosion(map,x-i,y)){
			if(bomb_explosion(map, x-i, y) == 2){
				switch(map_get_compose_type(map,x-i,y)){

				case CELL_BOX_RANGEINC:
					map_set_cell_type(map, x-i, y, CELL_BONUS_RANGEINC);
					break;
				case CELL_BOX_RANGEDEC:
					map_set_cell_type(map, x-i, y, CELL_BONUS_RANGEDEC);
					break;
				case CELL_BOX_BOMBINC:
					map_set_cell_type(map, x-i, y, CELL_BONUS_BOMBINC);
					break;
				case CELL_BOX_BOMBDEC:
					map_set_cell_type(map, x-i, y, CELL_BONUS_BOMBDEC);
					break;
				case CELL_BOX_LIFE:
					map_set_cell_type(map, x-i, y, CELL_BONUS_LIFE);
					break;
				case CELL_BOX_MONSTER:
					map_set_cell_type(map, x-i, y, CELL_MONSTER);

					break;
				default:
					break;
				}
				i = i+1;
				break;
			}
			if (bomb_explosion(map, x-i,y) == 3){
				map_set_cell_type(map,x-i,y,CELL_EMPTY);
				game_set_monster_list(monster_list_remove(monster_list,x-i,y),game);
				break;
			}
			if (bomb_explosion(map, x-i, y) == 4){
				player_dec_nb_life(player);
			}
			if (bomb_explosion(map, x-i,y) == 5)
				map_set_cell_type(map,x-i,y,CELL_EMPTY);
			i++;
		}
		break;
	}
	return (i-1);
}

void bomb_display(struct bomb* bomb){
	if( bomb != NULL){
		if (bomb->state != FIN){
			if (bomb->state == EXPLOSION_EN_COURS){
				int i = 0;
				for(i=0; i <= bomb->bomb_sense[NORTH]; i++){
					window_display_image(sprite_get_bomb(EXPLOSION_EN_COURS), bomb->x * SIZE_BLOC, (bomb->y - i) * SIZE_BLOC);
				}
				for(i=0; i <= bomb->bomb_sense[SOUTH]; i++){
					window_display_image(sprite_get_bomb(EXPLOSION_EN_COURS), bomb->x * SIZE_BLOC, (bomb->y + i) * SIZE_BLOC);
				}
				for(i=0; i <= bomb->bomb_sense[WEST]; i++){
						window_display_image(sprite_get_bomb(EXPLOSION_EN_COURS), (bomb->x-i) * SIZE_BLOC, bomb->y * SIZE_BLOC);
				}
				for(i=0; i <= bomb->bomb_sense[EAST]; i++){
					window_display_image(sprite_get_bomb(EXPLOSION_EN_COURS), (bomb->x+i) * SIZE_BLOC, bomb->y * SIZE_BLOC);
				}
			}
			window_display_image(sprite_get_bomb(bomb->state),bomb->x * SIZE_BLOC, bomb->y * SIZE_BLOC);
		}
	}
}

void  bomb_list_display(struct bomb_list* list,struct game* game){
	struct bomb_list* list_finale = list;
	while(list_finale != NULL){
		bomb_display(list_finale->bomb);
		list_finale = list_finale->next_bomb;
	}
}


