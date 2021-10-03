/* adv1.c */
/*
# This code and its documentation is Copyright 2021 Steven Ford, http://geeky-boy.com
# and licensed "public domain" style under Creative Commons "CC0": http://creativecommons.org/publicdomain/zero/1.0/
# To the extent possible under law, the contributors to this project have
# waived all copyright and related or neighboring rights to this work.
# In other words, you can use this code for any purpose without any
# restrictions.  This work is published from: United States.  The project home
# is https://github.com/fordsfords/adv1
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "y.tab.h"
#include "adv1.h"
int yyparse();

/* See https://github.com/fordsfords/safe_atoi */
#define SAFE_ATOI(a_,l_) do { \
  unsigned long long fs_[9] = { \
    0, 0xff, 0xffff, 0, 0xffffffff, 0, 0, 0, 0xffffffffffffffff }; \
  errno = 0; \
  (l_) = fs_[sizeof(l_)]; \
  if ((l_) < 0) { /* Is result a signed value? */ \
    char *in_a_ = a_;  char *temp_ = NULL;  long long llresult_; \
    if (strlen(in_a_) > 2 && *in_a_ == '0' && *(in_a_ + 1) == 'x') { \
      llresult_ = strtoll(in_a_ + 2, &temp_, 16); \
    } else { \
      llresult_ = strtoll(in_a_, &temp_, 10); \
    } \
    if (errno != 0 || temp_ == in_a_ || temp_ == NULL || *temp_ != '\0') { \
      if (errno == 0) { \
        errno = EINVAL; \
      } \
      fprintf(stderr, "%s:%d, Error, invalid number for %s: '%s'\n", \
         __FILE__, __LINE__, #l_, in_a_); \
      exit(1); \
    } else { /* strtol thinks success; check for overflow. */ \
      (l_) = llresult_; /* "return" value of macro */ \
      if ((l_) != llresult_) { \
        fprintf(stderr, "%s:%d, %s over/under flow: '%s'\n", \
           __FILE__, __LINE__, #l_, in_a_); \
        exit(1); \
      } \
    } \
  } else { \
    char *in_a_ = a_;  char *temp_ = NULL;  unsigned long long llresult_; \
    if (strlen(in_a_) > 2 && *in_a_ == '0' && *(in_a_ + 1) == 'x') { \
      llresult_ = strtoull(in_a_ + 2, &temp_, 16); \
    } else { \
      llresult_ = strtoull(in_a_, &temp_, 10); \
    } \
    if (errno != 0 || temp_ == in_a_ || temp_ == NULL || *temp_ != '\0') { \
      if (errno == 0) { \
        errno = EINVAL; \
      } \
      fprintf(stderr, "%s:%d, Error, invalid number for %s: '%s'\n", \
         __FILE__, __LINE__, #l_, in_a_); \
      exit(1); \
    } else { /* strtol thinks success; check for overflow. */ \
      (l_) = llresult_; /* "return" value of macro */ \
      if ((l_) != llresult_) { \
        fprintf(stderr, "%s:%d, %s over/under flow: '%s'\n", \
           __FILE__, __LINE__, #l_, in_a_); \
        exit(1); \
      } \
    } \
  } \
} while (0)

#define ASSRT(cond_) do { \
  if (! (cond_)) { \
    fprintf(stderr, "%s:%d, ERROR: '%s' not true\n", \
      __FILE__, __LINE__, #cond_); \
    abort(); \
    exit(1); \
  } \
} while (0)


/* Options */
int o_baud = 9600;


/* Globals */
obj_t *tkn2obj[T_BAD_TOKEN + 1];
int baud_usec = 0;
char *p;  /* used by P */

void P(int discard)
{
  int i = 0;
  while (p[i] != '\0') {
    putchar(p[i]); fflush(stdout);
    usleep(baud_usec);
    if (p[i] == '\n') {  /* Extra time for carriage return/linefeed. */
      usleep(2*baud_usec);
    }
    i++;
  }  /* while */

  free(p);
}  /* P */


char usage_str[] = "Usage: adv1 [-h] [-b baud]";
void usage(char *msg) {
  if (msg) P(asprintf(&p, "\n%s\n\n", msg));
  P(asprintf(&p, "%s\n", usage_str));
  exit(1);
}  /* usage */

void help() {
  P(asprintf(&p, "%s\n"
    "where:\n"
    "  -h - print help\n"
    "  -b baud - print speed [9600]\n"
    "For details, see https://github.com/fordsfords/adv1\n",
    usage_str));
  exit(0);
}  /* help */


void parse_cmdline(int argc, char **argv)
{
  int opt;

  while ((opt = getopt(argc, argv, "hb:")) != EOF) {
    switch (opt) {
      case 'h':
        help();
        break;
      case 'b':
        SAFE_ATOI(optarg, o_baud);
        break;
      default:
        usage(NULL);
    }  /* switch */
  }  /* while */
}  /* parse_cmdline */


char *direction_str(int direction)
{
  switch (direction) {
    case T_NORTH: return "north";
    case T_SOUTH: return "south";
    case T_EAST: return "east";
    case T_WEST: return "west";
    case T_UP: return "up";
    case T_DOWN: return "down";
  }
  return "**CODE BUG!!!";
}  /* direction_str */


void scan_contents(obj_t *container_obj)
{
  container_obj->description_short_f();
  obj_t *cur_obj = container_obj->first_content;
  obj_t *prev_obj = NULL;
  while (cur_obj != NULL) {
    cur_obj->description_short_f();
    ASSRT(cur_obj->contained_in == container_obj);
    ASSRT(cur_obj->prev_content == prev_obj);
    prev_obj = cur_obj;
    cur_obj = cur_obj->next_content;
  }
  P(asprintf(&p, "End\n"));
}  /* scan_contents */


void content_add(obj_t *container_obj, obj_t *added_obj)
{
  added_obj->next_content = container_obj->first_content;
  added_obj->prev_content = NULL;

  if (container_obj->first_content != NULL) {
    container_obj->first_content->prev_content = added_obj;
  }
  container_obj->first_content = added_obj;

  added_obj->contained_in = container_obj;
}  /* content_add */


void content_remove(obj_t *container_obj, obj_t *removed_obj)
{
  ASSRT(removed_obj->contained_in == container_obj);
  if (removed_obj->prev_content == NULL) {
    ASSRT(container_obj->first_content == removed_obj);
    container_obj->first_content = removed_obj->next_content;
  }
  else {
    removed_obj->prev_content->next_content = removed_obj->next_content;
  }

  if (removed_obj->next_content != NULL) {
    ASSRT(removed_obj->next_content->prev_content == removed_obj);
    removed_obj->next_content->prev_content = removed_obj->prev_content;
  }

  removed_obj->contained_in = NULL;
}  /* content_remove */


int travel(obj_t *destination)
{
  int travel_ok = destination->enter_f();
  if (travel_ok) {
    content_remove(obj_player.contained_in, &obj_player);
    content_add(destination, &obj_player);
  }

  return travel_ok;
}  /* travel */


/******************************************************************************
 *****************************************************************************/


void cmd_look(int obj_tkn)
{
  if (obj_tkn == -1) {
    obj_player.contained_in->look_f();
    P(asprintf(&p, "\n"));
    return;
  }

  obj_t *look_obj = tkn2obj[obj_tkn];

  if (look_obj->contained_in == &obj_player) {
    look_obj->look_f();
    P(asprintf(&p, "\n"));
    return;
  }
  if (look_obj->contained_in == obj_player.contained_in) {
    look_obj->look_f();
    P(asprintf(&p, "\n"));
    return;
  }

  P(asprintf(&p, "I don't see that.\n\n"));
}  /* cmd_look */


void cmd_ambiguous_look(int type_tkn)
{
  int num_found = 0;

  obj_t *look_obj = obj_player.first_content;
  while (look_obj != NULL) {
    if (look_obj->my_type_tkn == type_tkn) {
      look_obj->look_f();
      num_found++;
    }
    look_obj = look_obj->next_content;
  }

  look_obj = obj_player.contained_in->first_content;
  while (look_obj != NULL) {
    if (look_obj->my_type_tkn == type_tkn) {
      look_obj->look_f();
      num_found++;
    }
    look_obj = look_obj->next_content;
  }

  if (num_found == 0) {
    P(asprintf(&p, "Don't see any of those.\n\n"));
  } else {
    P(asprintf(&p, "\n"));
  }
}  /* cmd_ambiguous_look */


void cmd_take(int obj_tkn)
{
  obj_t *take_obj;

  if (obj_tkn == -1) {
    P(asprintf(&p, "Don't know what to take.\n\n"));
    return;
  }

  take_obj = tkn2obj[obj_tkn];
  if (take_obj->contained_in == &obj_player) {
    P(asprintf(&p, "You've already got it.\n\n"));
    return;
  }

  if (take_obj->contained_in != obj_player.contained_in) {
    P(asprintf(&p, "I don't see that.\n\n"));
    return;
  }

  int carrying_size = obj_player.totsize_f() - obj_player.my_size;
  if (take_obj->totsize_f() + carrying_size > obj_player.my_capacity) {
    P(asprintf(&p, "Too much to carry.\n"));
    return;
  }

  take_obj->take_f();

  P(asprintf(&p, "Got it.\n\n"));
}  /* cmd_take */


void cmd_take_all()
{
  obj_t *take_obj = obj_player.contained_in->first_content;
  int num_taken = 0;
  while (take_obj != NULL) {
    if (take_obj != &obj_player) {
      P(asprintf(&p, "Taking ")); take_obj->description_short_f();
      int carrying_size = obj_player.totsize_f() - obj_player.my_size;
      if (take_obj->totsize_f() + carrying_size > obj_player.my_capacity) {
        P(asprintf(&p, "Too much to carry.\n"));
      }
      else {
        take_obj->take_f();
        P(asprintf(&p, "Got it.\n"));
        num_taken++;
      }
    }
    take_obj = take_obj->next_content;
  }  /* while */

  if (num_taken == 0) {
    P(asprintf(&p, "Nothing I can take.\n\n"));
  }
  else {
    P(asprintf(&p, "\n"));
  }
}  /* cmd_take_all */


void cmd_ambiguous_take(int type_tkn)
{
  obj_t *take_obj = obj_player.contained_in->first_content;
  while (take_obj != NULL) {
    if (take_obj->my_type_tkn == type_tkn) {
      take_obj->take_f();
      P(asprintf(&p, "Got it.\n\n"));
      return;
    }
    take_obj = take_obj->next_content;
  }

  take_obj = obj_player.first_content;
  while (take_obj != NULL) {
    if (take_obj->my_type_tkn == type_tkn) {
      P(asprintf(&p, "You already have that\n\n"));
      return;
    }
    take_obj = take_obj->next_content;
  }

  P(asprintf(&p, "Don't see any of those.\n\n"));
}  /* cmd_ambiguous_take */


void cmd_drop(int obj_tkn)
{
  obj_t *drop_obj;

  if (obj_tkn == -1) {
    P(asprintf(&p, "Don't know what to drop.\n\n"));
    return;
  }

  drop_obj = tkn2obj[obj_tkn];
  if (drop_obj->contained_in != &obj_player) {
    P(asprintf(&p, "You don't have it.\n\n"));
    return;
  }

  drop_obj->drop_f();

  P(asprintf(&p, "Dropped it.\n\n"));
}  /* cmd_drop */


void cmd_drop_all()
{
  obj_t *drop_obj = obj_player.first_content;
  int num_dropn = 0;
  while (drop_obj != NULL) {
    if (drop_obj != &obj_player) {
      P(asprintf(&p, "Dropping ")); drop_obj->description_short_f();
      int carrying_size = obj_player.totsize_f() - obj_player.my_size;
      if (drop_obj->totsize_f() + carrying_size > obj_player.my_capacity) {
        P(asprintf(&p, "Too much to carry.\n"));
      }
      else {
        drop_obj->drop_f();
        P(asprintf(&p, "Dropped it.\n\n"));
        num_dropn++;
      }
    }
    drop_obj = drop_obj->next_content;
  }  /* while */

  if (num_dropn == 0) {
    P(asprintf(&p, "Nothing I can drop.\n\n"));
  }
  else {
    P(asprintf(&p, "\n"));
  }
}  /* cmd_drop_all */


void cmd_ambiguous_drop(int type_tkn)
{
  obj_t *drop_obj = obj_player.first_content;
  while (drop_obj != NULL) {
    if (drop_obj->my_type_tkn == type_tkn) {
      drop_obj->drop_f();
      P(asprintf(&p, "Dropped it.\n"));
      return;
    }
    drop_obj = drop_obj->next_content;
  }

  drop_obj = obj_player.first_content;
  while (drop_obj != NULL) {
    if (drop_obj->my_type_tkn == type_tkn) {
      P(asprintf(&p, "You already have that\n\n"));
      return;
    }
    drop_obj = drop_obj->next_content;
  }

  P(asprintf(&p, "Don't see any of those.\n\n"));
}  /* cmd_ambiguous_drop */


void cmd_go(int direction)
{
  int travel_ok;

  if (direction == -1) {
    P(asprintf(&p, "Don't know which way to go.\n\n"));
    return;
  }

  travel_ok = obj_player.contained_in->go_f(direction);

  P(asprintf(&p, "\n"));
}  /* cmd_go */


void cmd_intro()
{
  P(asprintf(&p,
"Welcome to sford's not-so-wonderful adventure program. Some day he plans to\n"
"actually write a game that is fun to play. This isn't it. This is a vehicle\n"
"for his own enjoyment of his personal hobby of writing software. Emphasis\n"
"on *writing*. He likes coding more than he likes creating, if that makes\n"
"any sense. Anyway, feel free to play this game if you want. Just don't\n"
"complain if you get bored.\n\n"
"See https://github.com/fordsfords/adv1 for (air quotes) IMPORTANT INFORMATION!\n\n"
"This game is inspired by the text-based adventure games of yore, like Zork and\n"
"Colossal Cave Adventure (see https://en.wikipedia.org/wiki/Adventure_game ).\n\n"
"Your goal is simply to explore an amazing world (work with me here), solving\n"
"puzzles and gathering treasure. You interact with the game by entering\n"
"commands, essentially telling the computer what you are doing within the game.\n"
"For example, you might enter:\n"
"take gold coin\n"
"go north\n"
"inventory\n\n"
"As a short cut, movement can be abbreviated as n, s, e, w, u, d (for north,\n"
"south, east, west, up, down).\n"
  ));

  P(asprintf(&p, "\n"));
}  /* cmd_intro */


void cmd_quit()
{
  P(asprintf(&p, "See ya later!\n"));
  exit(0);
}  /* cmd_quit */


/******************************************************************************
 ******************************************************************************/


obj_t obj_parkentry63cpw;

void obj_parkentry63cpw_description_full()
{
  P(asprintf(&p,
"You are standing on a north-south sidewalk next to a busy street. Across the\n"
"street to the west, you see many side streets and tall buildings. To the\n"
"east is an entrance to a park with pleasent winding paths.\n"
  ));

  obj_t *contained_obj = obj_parkentry63cpw.first_content;
  int you_see_printed = 0;
  while (contained_obj != NULL) {
    if (contained_obj != &obj_player) {
      if (! you_see_printed) {
        P(asprintf(&p, "\nYou see:\n"));
        you_see_printed = 1;
      }
      contained_obj->description_f();
    }
    contained_obj = contained_obj->next_content;
  }
  obj_parkentry63cpw.description_first_time = 1;
}  /* obj_parkentry63cpw_description_full */

void obj_parkentry63cpw_description_short()
{
  P(asprintf(&p,
"Park entrance at Central Park West near 63rd street.\n"
  ));
}  /* obj_parkentry63cpw_description_short */

void obj_parkentry63cpw_description()
{
  if (obj_parkentry63cpw.description_first_time) {
    obj_parkentry63cpw_description_short();
  }
  else {
    obj_parkentry63cpw_description_full();
  }
}  /* obj_parkentry63cpw_description */

int obj_parkentry63cpw_totsize()
{
  int tot_size = 0;  /* Don't count the size of the room. */
  obj_t *contained_obj = obj_parkentry63cpw.first_content;
  while (contained_obj != NULL) {
    tot_size += contained_obj->totsize_f();
    contained_obj = contained_obj->next_content;
  }

  return tot_size;
}  /* obj_parkentry63cpw_totsize */

int obj_parkentry63cpw_enter()
{
  obj_parkentry63cpw_description();
  return 1;
}  /* obj_parkentry63cpw_enter */

int obj_parkentry63cpw_go(int direction)
{
  switch (direction) {
    case T_EAST: return travel(&obj_parktrash);
  }
  P(asprintf(&p, "Can't go %s\n", direction_str(direction)));
  return 0;  /* No movement. */
}  /* obj_parkentry63cpw_go */

void obj_parkentry63cpw_look()
{
  obj_parkentry63cpw_description_full();
  P(asprintf(&p, "\nWatch for dog poo laying around.\n"));
}  /* obj_parkentry63cpw_look */

void obj_parkentry63cpw_init()
{
  obj_parkentry63cpw.my_size = -1;
  obj_parkentry63cpw.my_capacity = 999999;

  obj_parkentry63cpw.description_full_f = obj_parkentry63cpw_description_full;
  obj_parkentry63cpw.description_short_f = obj_parkentry63cpw_description_short;
  obj_parkentry63cpw.description_f = obj_parkentry63cpw_description;
  obj_parkentry63cpw.totsize_f = obj_parkentry63cpw_totsize;
  obj_parkentry63cpw.enter_f = obj_parkentry63cpw_enter;
  obj_parkentry63cpw.go_f = obj_parkentry63cpw_go;
  obj_parkentry63cpw.look_f = obj_parkentry63cpw_look;
}  /* obj_parkentry63cpw_init */


obj_t obj_parktrash;

void obj_parktrash_description_full()
{
  P(asprintf(&p,
"You are standing just inside the park. The path ahead winds through\n"
"hundred-year-old trees. Birds are singing, squirrels are scampering,\n"
"and the trash can next to you is smelling rather bad.\n\n"
"To the west is the park entrance.\n"
  ));

  obj_t *contained_obj = obj_parktrash.first_content;
  int you_see_printed = 0;
  while (contained_obj != NULL) {
    if (contained_obj != &obj_player) {
      if (! you_see_printed) {
        P(asprintf(&p, "\nYou see:\n"));
        you_see_printed = 1;
      }
      contained_obj->description_f();
    }
    contained_obj = contained_obj->next_content;
  }
  obj_parktrash.description_first_time = 1;
}  /* obj_parktrash_description_full */

void obj_parktrash_description_short()
{
  P(asprintf(&p,
"Park path with trash can.\n"
  ));
}  /* obj_parktrash_description_short */

void obj_parktrash_description()
{
  if (obj_parktrash.description_first_time) {
    obj_parktrash_description_short();
  }
  else {
    obj_parktrash_description_full();
  }
}  /* obj_parktrash_description */

int obj_parktrash_totsize()
{
  int tot_size = 0;  /* Don't count the size of the room. */
  obj_t *contained_obj = obj_parktrash.first_content;
  while (contained_obj != NULL) {
    tot_size += contained_obj->totsize_f();
    contained_obj = contained_obj->next_content;
  }

  return tot_size;
}  /* obj_parktrash_totsize */

int obj_parktrash_enter()
{
  obj_parktrash_description();
  return 1;
}  /* obj_parktrash_enter */

int obj_parktrash_go(int direction)
{
  switch (direction) {
    case T_WEST: return travel(&obj_parkentry63cpw);
  }
  P(asprintf(&p, "Can't go %s\n", direction_str(direction)));
  return 0;  /* No movement. */
}  /* obj_parktrash_go */

void obj_parktrash_look()
{
  obj_parktrash_description_full();
}  /* obj_parktrash_look */

void obj_parktrash_init()
{
  obj_parktrash.my_size = -1;
  obj_parktrash.my_capacity = 999999;
  obj_parktrash.description_full_f = obj_parktrash_description_full;
  obj_parktrash.description_short_f = obj_parktrash_description_short;
  obj_parktrash.description_f = obj_parktrash_description;
  obj_parktrash.totsize_f = obj_parktrash_totsize;
  obj_parktrash.enter_f = obj_parktrash_enter;
  obj_parktrash.go_f = obj_parktrash_go;
  obj_parktrash.look_f = obj_parktrash_look;
}  /* obj_parktrash_init */


obj_t obj_goldcoin;

void obj_goldcoin_description_full()
{
  P(asprintf(&p,
"a shiny gold coin!\n"
  ));
  obj_goldcoin.description_first_time = 1;
}  /* obj_goldcoin_description_full */

void obj_goldcoin_description_short()
{
  P(asprintf(&p,
"gold coin\n"
  ));
}  /* obj_goldcoin_description_short */

void obj_goldcoin_description()
{
  if (obj_goldcoin.description_first_time) {
    obj_goldcoin_description_short();
  }
  else {
    obj_goldcoin_description_full();
  }
}  /* obj_goldcoin_description */

void obj_goldcoin_look()
{
  P(asprintf(&p, "You see "));
  obj_goldcoin_description_full();
  P(asprintf(&p,
"The mint date is ... (squinting) next year?\n"
  ));
}  /* obj_goldcoin_look */

int obj_goldcoin_take()
{
  content_remove(obj_goldcoin.contained_in, &obj_goldcoin);
  content_add(&obj_player, &obj_goldcoin);
  return 1;
}  /* obj_goldcoin_take */

int obj_goldcoin_drop()
{
  ASSRT(obj_goldcoin.contained_in == &obj_player);
  content_remove(obj_goldcoin.contained_in, &obj_goldcoin);
  content_add(obj_player.contained_in, &obj_goldcoin);
  return 1;
}  /* obj_goldcoin_drop */

int obj_goldcoin_totsize()
{
  ASSRT(obj_goldcoin.first_content == NULL);  /* A coin shouldn't contain. */

  return obj_goldcoin.my_size;
}  /* obj_goldcoin_totsize */

void obj_goldcoin_init()
{
  content_add(&obj_parkentry63cpw, &obj_goldcoin);
  ASSRT(obj_goldcoin.contained_in == &obj_parkentry63cpw);

  tkn2obj[T_GOLDCOIN] = &obj_goldcoin;
  obj_goldcoin.my_type = T_COIN;

  obj_goldcoin.my_size = 1;
  obj_goldcoin.my_capacity = 0;
  obj_goldcoin.my_type_tkn = T_COIN;

  obj_goldcoin.description_full_f = obj_goldcoin_description_full;
  obj_goldcoin.description_short_f = obj_goldcoin_description_short;
  obj_goldcoin.description_f = obj_goldcoin_description;
  obj_goldcoin.totsize_f = obj_goldcoin_totsize;
  obj_goldcoin.look_f = obj_goldcoin_look;
  obj_goldcoin.take_f = obj_goldcoin_take;
  obj_goldcoin.drop_f = obj_goldcoin_drop;
}  /* obj_goldcoin_init */


obj_t obj_player;

void obj_player_description_full()
{
  P(asprintf(&p,
"You. Smart, good looking, my kind of player.\n"
  ));
}  /* obj_player_description_full */

void obj_player_description_short()
{
  P(asprintf(&p,
"You.\n"
  ));
}  /* obj_player_description_short */

int obj_player_totsize()
{
  int tot_size = obj_player.my_size;
  obj_t *contained_obj = obj_player.first_content;
  while (contained_obj != NULL) {
    tot_size += contained_obj->totsize_f();
    contained_obj = contained_obj->next_content;
  }

  return tot_size;
}  /* obj_player_totsize */

void obj_player_init()
{
  content_add(&obj_parkentry63cpw, &obj_player);
  ASSRT(obj_player.contained_in == &obj_parkentry63cpw);

  obj_player.my_size = 150;
  obj_player.my_capacity = 100;

  obj_player.description_full_f = obj_player_description_full;
  obj_player.description_short_f = obj_player_description_short;
  obj_player.totsize_f = obj_player_totsize;

  obj_player.health = 100;
}  /* obj_player_init */


void init_objects()
{
#include "init_calls.x"
}  /* init_objects */

int main(int argc, char **argv)
{
  parse_cmdline(argc, argv);

  baud_usec = 9000000 / o_baud;

  init_objects();

  ASSRT(obj_player.contained_in->enter_f() == 1);

  P(asprintf(&p, "\nEnter 'intro' for an introduction.\n"));

  yyparse();

  return 0;
}  /* main */
