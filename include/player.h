#ifndef PLAYER_H_
#define PLAYER_H_

#include <map.h>
#include <constant.h>

struct player;

// Creates a new player with a given number of available bombs
struct player* player_init(int bomb_number);
void   player_free(struct player* player);

// Returns the current position of the player
int player_get_x(struct player* player);
int player_get_y(struct player* player);

// Set the direction of the next move of the player
void player_set_current_way(struct player * player, enum direction direction);

// Get the number of keys
int player_get_nb_keys(struct player* player);

// Get,Increase, Decrease the number of player lives
int player_get_nb_life(struct player* player);
void player_inc_nb_life(struct player* player);
void player_dec_nb_life(struct player* player);

// Get,Increase current level
void player_inc_current_level(struct player* player);
int player_get_current_level(struct player* player);

// Get, Increase, Decrease the range of the bombs
int player_get_range_bomb(struct player* player);
void player_inc_range_bomb(struct player* player);
void player_dec_range_bomb(struct player* player);
int player_get_vitesse(struct player* player);
// Put, get, Increase, Decrease the number of bomb that player can put
void player_put_bomb(struct player* player);
int  player_get_nb_bomb(struct player * player);
void player_inc_nb_bomb(struct player * player);
void player_dec_nb_bomb(struct player * player);

int player_invulnerable(struct player* player);
void player_set_invulnerable(struct player* player);

int player_get_princess(struct player* player);
// Load the player position from the map
void player_from_map(struct player* player, struct map* map);

// Move the player according to the current direction
int player_move(struct player* player, struct map* map);

// Display the player on the screen
void player_display(struct player* player);
void player_hurt(struct player* player);
int player_update(struct player* player);

#endif /* PLAYER_H_ */
