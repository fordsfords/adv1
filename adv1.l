%option nounput noinput caseless

%{
#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"
#include "adv1.h"

void yyerror(char *s);
%}

%%

gold { return T_GOLD; }
coin { return T_COIN; }
all { return T_ALL; }
everything { return T_ALL; }

\. ;
, ;
\? ;

the ;
a ;

to ;
around ;
at ;
toward ;
towards ;

north { return T_NORTH; }
n { return T_NORTH; }
south { return T_SOUTH; }
s { return T_SOUTH; }
east { return T_EAST; }
e { return T_EAST; }
west { return T_WEST; }
w { return T_WEST; }
up { return T_UP; }
u { return T_UP; }
ascend { return T_UP; }
climb { return T_UP; }
climb[ \t]+up { return T_UP; }
down { return T_DOWN; }
d { return T_DOWN; }
descend { return T_DOWN; }
climb[ \t]+down { return T_DOWN; }
go { return T_GO_VERB; }
walk { return T_GO_VERB; }
run { return T_GO_VERB; }

look { return T_LOOK_VERB; }
examine { return T_LOOK_VERB; }

intro { return T_INTRO_VERB; }
introduction { return T_INTRO_VERB; }
help { return T_INTRO_VERB; }
h { return T_INTRO_VERB; }

take { return T_TAKE_VERB; }
get { return T_TAKE_VERB; }
grab { return T_TAKE_VERB; }
pick[ \t]+up { return T_TAKE_VERB; }

drop { return T_DROP_VERB; }
throw { return T_DROP_VERB; }

quit { return T_QUIT_VERB; }
q { return T_QUIT_VERB; }
exit { return T_QUIT_VERB; }

[ \t]+  ;   /* ignore whitespace. */
[\n] { return T_NEWLINE; }
[^ \t\n]+ { return T_BAD_TOKEN; }  /* Catch-all for bad tokens. */
%%

int yywrap(void) {
  cmd_quit();
  return 1;
}
