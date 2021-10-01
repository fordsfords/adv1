%token T_NEWLINE
%token T_NORTH
%token T_SOUTH
%token T_EAST
%token T_WEST
%token T_UP
%token T_DOWN
%token T_GO_VERB
%token T_QUIT_VERB
%token T_BAD_TOKEN

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
    game sentence
    |
    ;

sentence:
    go_sentence T_NEWLINE { obj_player_go($1); }
    | quit_sentence T_NEWLINE { obj_player_quit(); }
    | T_NEWLINE { printf("\n"); }
    | error T_NEWLINE { printf("Huh?\n"); }
    ;

go_sentence:
    T_GO_VERB direction { $$ = $2; }
    | direction { $$ = $1; }
    ;

quit_sentence:
    T_QUIT_VERB { $$ = $1; }
    ;

direction:
    T_NORTH { $$ = T_NORTH; }
    | T_SOUTH { $$ = T_SOUTH; }
    | T_EAST { $$ = T_EAST; }
    | T_WEST { $$ = T_WEST; }
    | T_UP { $$ = T_UP; }
    | T_DOWN { $$ = T_DOWN; }
    ;

%%

void yyerror(char *s) {
  printf("I don't understand that.\n");
}
