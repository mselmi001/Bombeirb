#include <SDL/SDL_image.h>
#include <assert.h>
#include <player.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <constant.h>
#include <monster.h>

struct monster {
	int x;
	int y;
	enum direction direction;
	unsigned int time_of_the_last_move;
};

struct monster_list{
	struct monster* monster;
	struct monster_list* next_monster;
};

struct monster* monster_init (int x, int y){

	struct monster* monster=malloc(sizeof(*monster));
	if(!monster)
		error("Memory error");

	monster->direction = SOUTH;
	monster->x=x;
	monster->y=y;
	monster->time_of_the_last_move = SDL_GetTicks();

	return monster;
}

struct monster_list* monster_list_init(struct map* map){
	assert(map);
	struct monster_list* monster_list = NULL;
	int h = map_get_height(map);
	int w = map_get_width(map);
	int i, j;
	for (i = 0; i < w; i++) {
		for (j = 0; j < h; j++) {
			if (map_get_cell_type(map, i, j) == CELL_MONSTER){
				struct monster* monster = monster_init(i,j);
				monster_list = monster_list_add(monster_list,monster);
			}
		}
	}
	return monster_list;
}

struct monster_list* monster_list_add(struct monster_list* monster_list,struct monster* monster){
	struct monster_list* new_list = malloc(sizeof(*new_list));
	new_list->monster = monster;
	new_list->next_monster = monster_list;
	return new_list;
}

struct monster_list* monster_list_remove(struct monster_list* monster_list, int x,int y){
	if(monster_list == NULL)
		return NULL;
	if(monster_get_x(monster_list->monster)==x && monster_get_y(monster_list->monster)==y){
		struct monster_list* list_finale = monster_list->next_monster;
		free(monster_list);
		list_finale = monster_list_remove(list_finale, x, y);
		return list_finale;
	}
	else{
		monster_list->next_monster = monster_list_remove(monster_list->next_monster,x,y);
		return monster_list;
	}
}



void monster_set_direction(struct monster* monster, enum direction way) {
	assert(monster);
	monster->direction = way;
}

int monster_get_x(struct monster* monster) {
assert( monster != NULL);
return monster->x;
}

int monster_get_y(struct monster* monster) {
assert(monster != NULL);
return monster->y;
}

void monster_from_map(struct monster* monster, struct map* map) {
	assert(monster);
	assert(map);

	int i, j;
	for (i = 0; i < map_get_width(map); i++) {
		for (j = 0; j < map_get_height(map); j++) {
			if (map_get_cell_type(map, i, j) == CELL_MONSTER) {
				monster->x = i;
				monster->y = j;
			}
		}
	}
}

int monster_move_aux(struct monster* monster, struct game* game, int x, int y) {

struct map* map = game_get_current_map(game);
struct player* player = game_get_player(game);

	if (!map_is_inside(map, x, y))
		return 0;

	switch (map_get_cell_type(map, x, y)) {
	case CELL_SCENERY:
		return 0;
		break;

    case CELL_BOX:
    	return 0;
    	break;

	case CELL_DOOR:
		return 0;
		break ;

	case CELL_PLAYER:
		player_dec_nb_life(player);
		return 0;
		break;
	case CELL_KEY:
		return 0;
		break;

	case CELL_BOMB:
		switch( map_get_compose_type(map, x, y)){
		case CELL_BOMB_EXPLOSION_EN_COURS:
			map_set_cell_type(map, x, y, CELL_EMPTY);
			break;
		default:
			break;
				}
		break;

	case CELL_MONSTER:
		return 0;
		break;
	default:
		break;
	}

	// Monster has moved
	return 1;
}

int monster_move(struct monster* monster, struct game* game) {
	int x = monster->x;
	int y = monster->y;
	int move = 0;
	struct map* map = game_get_current_map(game);
	switch (monster->direction) {
	case NORTH:
		if (monster->y!=0){
			if (monster_move_aux(monster, game, x, y - 1) && map_get_cell_type(map,x,y-2) != CELL_DOOR){
				monster->y--;
				move = 1;}
		}
		break;

	case SOUTH:
		if (monster->y!=map_get_height(map)-1){
			if (monster_move_aux(monster, game, x, y + 1) && map_get_cell_type(map,x,y+2) != CELL_DOOR) {
				monster->y++;
				move = 1;}
		}
		break;

	case WEST:
		if (monster->x!=0){
			if (monster_move_aux(monster, game, x - 1, y) && map_get_cell_type(map,x-2,y) != CELL_DOOR) {
				monster->x--;
				move = 1;}
		}
		break;


	case EAST:
		if (monster->x!=map_get_width(map)-1){
			if (monster_move_aux(monster, game, x + 1, y) && map_get_cell_type(map,x+2,y) != CELL_DOOR) {
				monster->x++;
				move = 1;}
		}
		break;
	}

	if (move) {
		map_set_cell_type(map, x, y, CELL_EMPTY);
		map_set_cell_type(map, monster->x, monster->y, CELL_MONSTER);
	}
	return move;
}


void monster_display(struct monster* monster) {
	if (monster != NULL){
	window_display_image(sprite_get_monster(monster->direction),monster->x * SIZE_BLOC, monster->y * SIZE_BLOC);
	}
}

void monster_list_display(struct monster_list* monster_list){
	struct monster_list* disp = monster_list;
	while(disp != NULL){
		monster_display(disp->monster);
		disp = disp->next_monster;
	}
}

struct monster* monster_update_move(struct game* game,struct monster* monster) {
	if (monster != NULL){
	unsigned int time_of_the_last_move = monster->time_of_the_last_move;
	struct player* player = game_get_player(game);
	unsigned int time = SDL_GetTicks();
	if ((time - time_of_the_last_move) > player_get_vitesse(player) ) {
		monster_set_direction(monster, rand() % 4);
		monster_move(monster, game);
		monster->time_of_the_last_move = SDL_GetTicks();
	}
	}
		return monster;
}

struct monster_list* monster_list_update(struct game* game){

	struct monster_list* monster_list = game_get_monster_list(game);
	struct monster_list* list = monster_list;
	while(list != NULL){
		list->monster = monster_update_move(game,list->monster);
		list = list->next_monster;
	}

	return monster_list;
}








