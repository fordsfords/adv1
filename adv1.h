/* adv1.h */
/*
# This code and its documentation is Copyright 2021 Steven Ford, http://geeky-boy.com
# and licensed "public domain" style under Creative Commons "CC0": http://creativecommons.org/publicdomain/zero/1.0/
# To the extent possible under law, the contributors to this project have
# waived all copyright and related or neighboring rights to this work.
# In other words, you can use this code for any purpose without any
# restrictions.  This work is published from: United States.  The project home
# is https://github.com/fordsfords/adv1
*/

#ifndef ADV1_H
#define ADV1_H

#ifdef __cplusplus
extern "C" {
#endif

struct obj_s;
typedef struct obj_s obj_t;
struct obj_s {
  obj_t *contained_in;
  obj_t *first_content;
  obj_t *next_content;
  obj_t *prev_content;

  int my_type;  /* T_XXX_TYPE */
  int my_size;
  int my_capacity;
  int my_type_tkn;

  int description_first_time;
  int health;

  void (* description_full_f)();
  void (* description_short_f)();
  void (* description_f)();
  int (* enter_f)();
  int (* take_f)();
  int (* drop_f)();
  int (* totsize_f)();
  int (* go_f)(int direction);
  void (* look_f)();
};


extern obj_t *tkn2obj[T_BAD_TOKEN + 1];

#include "extern_objs.x"
#include "extern_cmds.x"

#ifdef __cplusplus
}
#endif

#endif  /* ADV1_H */
