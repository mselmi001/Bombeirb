#ifndef MONSTER_H_
#define MONSTER_H_

#include <map.h>
#include <game.h>
#include <monster.h>

struct player* player;
struct game* game;
struct monster;
struct monster_list;

// Creates a new monster with a given position and a new list of monsters
struct monster* monster_init(int x,int y);
struct monster_list* monster_list_init(struct map* map);

//Returns the current position of the player
int monster_get_x(struct monster* monster);
int monster_get_y(struct monster* monster);

//Set the list of monsters
struct monster_list* monster_list_add(struct monster_list* monster_list,struct monster* monster);
struct monster_list* monster_list_remove(struct monster_list* monster_list, int x,int y);

// Set the direction of the next move of the monster
void monster_set_current_way(struct monster * monster, enum direction direction);

// Load the monster position from the map
void monster_from_map(struct monster* monster, struct map* map);

//move the monster randomly
int monster_move(struct monster* monster, struct game* game);
int monster_move_aux(struct monster* monster, struct game* game, int x, int y);

// Update the monsters and the list of monsters moves
struct monster* monster_update_move(struct game* game,struct monster* monster);
struct monster_list* monster_list_update(struct game* game);

//display the monster and the list of monsters on the screen
void monster_display(struct monster* monster);
void monster_list_display(struct monster_list* monster_list);

#endif /* MONSTER_H_ */





