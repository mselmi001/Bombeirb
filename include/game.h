#ifndef GAME_H_
#define GAME_H_

#include <player.h>
#include <map.h>

// Abstract data type
struct game;

// Create a new game
struct game* game_new();

// Free a game
void game_free(struct game* game);

// Return the player of the current game
struct player* game_get_player(struct game* game);

// Return the monster and the list of monsters in the current map
struct monster* game_get_current_monster(struct game* game);
struct monster_list* game_get_monster_list(struct game* game);
void game_set_monster(struct monster* monster, struct game* game);
void game_set_monster_list(struct monster_list* monster_list, struct game* game);

// Set the number of keys
int game_set_nb_keys(struct game* game);

//Set, get the current level
int game_get_current_level(struct game* game);
void game_set_level(struct game* game);

// Return the current map
struct map* game_get_current_map(struct game* game);

// Display the game on the screen
void game_display(struct game* game);

// update the game
int game_update(struct game* game);

#endif /* GAME_H_ */
