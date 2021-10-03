%token T_ALL
%token T_GOLD
%token T_COIN
%token T_GOLDCOIN

%token T_INTRO_VERB
%token T_LOOK_VERB
%token T_TAKE_VERB
%token T_DROP_VERB
%token T_QUIT_VERB

%token T_NORTH
%token T_SOUTH
%token T_EAST
%token T_WEST
%token T_UP
%token T_DOWN
%token T_GO_VERB

%token T_BAD_TOKEN
%token T_NEWLINE

%{
#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"
#include "adv1.h"
void yyerror(char *s);
int yylex();
%}

%%

game:
    sentence game
    |
    ;

sentence:
    go_sentence
    | ambig_go_sentence
    | look_sentence
    | take_sentence
    | drop_sentence
    | intro_sentence
    | quit_sentence
    | T_NEWLINE { printf("\n"); }
    | error T_NEWLINE { printf("Huh?\n"); }
    ;

go_sentence:
    T_GO_VERB direction T_NEWLINE { cmd_go($2); }
    | direction T_NEWLINE { cmd_go($1); }
    ;

ambig_go_sentence:
    T_GO_VERB T_NEWLINE { cmd_go(-1); }
    ;

look_sentence:
    T_LOOK_VERB T_NEWLINE { cmd_look(-1); }
    | T_LOOK_VERB takable_thing T_NEWLINE { cmd_look($2); }
    | T_LOOK_VERB ambiguous_thing T_NEWLINE { cmd_ambiguous_look($2); }
    ;

take_sentence:
    T_TAKE_VERB T_NEWLINE { cmd_take(-1); }
    | T_TAKE_VERB T_ALL T_NEWLINE { cmd_take_all(); }
    | T_TAKE_VERB takable_thing T_NEWLINE { cmd_take($2); }
    | T_TAKE_VERB ambiguous_thing T_NEWLINE { cmd_ambiguous_take($2); }
    ;

drop_sentence:
    T_DROP_VERB T_NEWLINE { cmd_drop(-1); }
    | T_DROP_VERB T_ALL T_NEWLINE { cmd_drop_all(); }
    | T_DROP_VERB takable_thing T_NEWLINE { cmd_drop($2); }
    | T_DROP_VERB ambiguous_thing T_NEWLINE { cmd_ambiguous_drop($2); }
    ;

intro_sentence:
    T_INTRO_VERB T_NEWLINE { cmd_intro(); }
    ;

quit_sentence:
    T_QUIT_VERB T_NEWLINE { cmd_quit(); }
    ;

direction:
    T_NORTH { $$ = T_NORTH; }
    | T_SOUTH { $$ = T_SOUTH; }
    | T_EAST { $$ = T_EAST; }
    | T_WEST { $$ = T_WEST; }
    | T_UP { $$ = T_UP; }
    | T_DOWN { $$ = T_DOWN; }
    ;

ambiguous_thing:
    T_COIN { $$ = T_COIN; }
    ;

takable_thing:
    T_GOLD T_COIN { $$ = T_GOLDCOIN; }
    ;

%%

void yyerror(char *s) {
  printf("I don't understand that.\n");
}
