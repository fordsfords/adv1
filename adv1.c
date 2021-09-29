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
    exit(1); \
  } \
} while (0)

/* Options */


/* Globals */


char usage_str[] = "Usage: adv1 [-h]";
void usage(char *msg) {
  if (msg) fprintf(stderr, "\n%s\n\n", msg);
  fprintf(stderr, "%s\n", usage_str);
  exit(1);
}  /* usage */

void help() {
  printf("%s\n"
    "where:\n"
    "  -h - print help\n"
    "For details, see https://github.com/fordsfords/adv1\n",
    usage_str);
  exit(0);
}  /* help */


void parse_cmdline(int argc, char **argv)
{
  int opt;

  while ((opt = getopt(argc, argv, "h")) != EOF) {
    switch (opt) {
      case 'h':
        help();
        break;
      default:
        usage(NULL);
    }  /* switch */
  }  /* while */
}  /* parse_cmdline */


struct obj_s;
typedef struct obj_s obj_t;
struct obj_s {
  obj_t *contained_in;
  obj_t *first_content;
  obj_t *next_content;
  obj_t *prev_content;

  int my_size;
  int my_capacity;

  char *(* description_full_f)();
  char *(* description_short_f)();
  char *(* description_f)();
  int (* totsize_f)();
};


void content_add(obj_t *container_obj, obj_t *added_obj)
{
  added_obj->next_content = container_obj->first_content;
  added_obj->prev_content = NULL;

  if (container_obj->first_content != NULL) {
    container_obj->first_content->prev_content = container_obj;
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


/******************************************************************************
 ******************************************************************************/


obj_t obj_parkentry63cpw;

struct obj_parkentry63cpw_s {
  char *description_full;
  char *description_short;
  int description_first_time;
} obj_parkentry63cpw_state;

char *obj_parkentry63cpw_description_full()
{
  if (obj_parkentry63cpw_state.description_full == NULL) {
    asprintf(&obj_parkentry63cpw_state.description_full,
"You are standing on a north-south sidewalk next to a busy street. Across the\n"
"street to the west, you see many side streets and tall buildings. To the\n"
"east is an entrance to a park with pleasent winding paths.\n"
    );
  }
  obj_parkentry63cpw_state.description_first_time = 1;
  return obj_parkentry63cpw_state.description_full;
}  /* obj_parkentry63cpw_description_full */

char *obj_parkentry63cpw_description_short()
{
  if (obj_parkentry63cpw_state.description_short == NULL) {
    asprintf(&obj_parkentry63cpw_state.description_short,
"Park entrance at Central Park West near 63rd street.\n"
    );
  }
  return obj_parkentry63cpw_state.description_short;
}  /* obj_parkentry63cpw_description_short */

char *obj_parkentry63cpw_description()
{
  if (obj_parkentry63cpw_state.description_first_time) {
    return obj_parkentry63cpw_description_short();
  }
  else {
    return obj_parkentry63cpw_description_full();
  }
  return obj_parkentry63cpw_state.description_full;
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
}  /* obj_parkentry63cpw_size */

void obj_parkentry63cpw_init()
{
  obj_parkentry63cpw.my_size = -1;
  obj_parkentry63cpw.my_capacity = 999999;
  obj_parkentry63cpw.description_full_f = obj_parkentry63cpw_description_full;
  obj_parkentry63cpw.description_short_f = obj_parkentry63cpw_description_short;
  obj_parkentry63cpw.description_f = obj_parkentry63cpw_description;
  obj_parkentry63cpw.totsize_f = obj_parkentry63cpw_totsize;
}  /* obj_parkentry63cpw_init */


obj_t obj_player;

struct obj_player_s {
  int health;
} obj_player_state;

int obj_player_totsize()
{
  int tot_size = obj_player.my_size;
  obj_t *contained_obj = obj_player.first_content;
  while (contained_obj != NULL) {
    tot_size += contained_obj->totsize_f();
    contained_obj = contained_obj->next_content;
  }

  return tot_size;
}  /* obj_player_size */

void obj_player_init()
{
  content_add(&obj_parkentry63cpw, &obj_player);
  ASSRT(obj_player.contained_in == &obj_parkentry63cpw);

  obj_player.my_size = 150;
  obj_player.my_capacity = 100;
  obj_player.totsize_f = obj_player_totsize;

  obj_player_state.health = 100;
}  /* obj_player_init */


void init_objects()
{
  obj_player_init();
  obj_parkentry63cpw_init();
}  /* init_objects */


int main(int argc, char **argv)
{
  obj_t *player;
  parse_cmdline(argc, argv);

  init_objects();

  player = &obj_player;

  printf("%s", player->contained_in->description_f());
  printf("%s", player->contained_in->description_f());
  printf("Size=%d\n", player->contained_in->totsize_f());

  return 0;
}  /* main */
