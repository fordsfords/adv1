%option nounput noinput

%{
#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"
#include "adv1.h"

void yyerror(char *s);
%}

%%

"to" ;
"toward" ;
"towards" ;
"the" ;
"a" ;
"with" ;
"using" ;
"north" return T_NORTH;
"n" return T_NORTH;
"south" return T_SOUTH;
"s" return T_SOUTH;
"east" return T_EAST;
"e" return T_EAST;
"west" return T_WEST;
"w" return T_WEST;
"up" return T_UP;
"u" return T_UP;
"down" return T_DOWN;
"d" return T_DOWN;
"go" return T_GO_VERB;
"walk" return T_GO_VERB;
"run" return T_GO_VERB;
"quit" return T_QUIT_VERB;
"q" return T_QUIT_VERB;
"exit" return T_QUIT_VERB;

[ \t]+  ;   /* ignore whitespace. */
[\n]  return T_NEWLINE;
[^ \t\n]+ return T_BAD_TOKEN;  /* Catch-all for bad tokens. */
%%

int yywrap(void) {
  obj_player_quit();
  return 1;
}
