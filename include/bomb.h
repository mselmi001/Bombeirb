
#ifndef BOMB_H_
#define BOMB_H_

#include<game.h>
struct bomb;
struct bomb_list;

// Creates a new bomb and a new list of bombs
struct bomb* bomb_init(int x, int y, int range);
struct bomb_list* bomb_list_init();

// Get, set the bombs attributs
int bomb_get_x(struct bomb* bomb);
int bomb_get_y(struct bomb* bomb);
int bomb_get_range(struct bomb* bomb);
int bomb_get_state(struct bomb* bomb);
void bomb_inc_range(struct bomb* bomb);
void bomb_dec_range(struct bomb* bomb);

// Add and remove a bomb to the list
struct bomb_list* bomb_list_add(struct bomb_list* list,struct game* game);
struct bomb_list* bomb_list_remove(struct bomb_list* list,struct player* player, struct map* map);

// Set the explosion
void bomb_state( struct bomb* bomb, struct game* game);
int explosion(struct game* game, struct bomb* bomb, enum direction direction);
void box_destroy(struct map* map,int x, int y);

// Update the bombs and the list of bombs moves
struct bomb*  bomb_update_move(struct bomb* bomb, struct game* game);
struct bomb_list* bomb_list_update(struct game* game, struct bomb_list* list);

// Display the bomb on the screen
void bomb_display(struct bomb* bomb);
void  bomb_list_display(struct bomb_list* list,struct game* game);

#endif /* BOMB_H_ */
