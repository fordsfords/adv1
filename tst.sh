#!/bin/bash
# tst.sh - build and test the project.

ASSRT() {
  eval "test $1"

  if [ $? -ne 0 ]; then
    echo "ASSRT ERROR `basename ${BASH_SOURCE[1]}`:${BASH_LINENO[0]}, not true: '$1'" >&2
    exit 1
  fi
}  # ASSRT

CHKCORE() {
  CORE=`ls /cores/* 2>/dev/null | tail -1`
  if [ -f "$CORE" ]; then :
    echo "Enter 'lldb -c $CORE'"
    exit
  fi
}


rm -f adv1 y.tab.h y.tab.c lex.yy.c extern_objs.x

sed -n 's/^\(obj_t obj_[^;]*;\)/extern \1/p' <adv1.c >extern_objs.x
sed -n 's/^\(void cmd_[^)]*)\)/\1;/p' <adv1.c >extern_cmds.x
sed -n 's/^void \(obj_[^_]*_init()\)/  \1;/p' <adv1.c >init_calls.x

echo "yacc -d adv1.y"
yacc -d adv1.y; ASSRT "$? -eq 0"

echo "flex adv1.l"
lex adv1.l; ASSRT "$? -eq 0"

echo "clang -Wall -g -cl-opt-disable -o adv1 adv1.c  y.tab.c lex.yy.c"
clang -Wall -g -cl-opt-disable -o adv1 adv1.c  y.tab.c lex.yy.c; ASSRT "$? -eq 0"

ulimit -c unlimited

echo "./adv1 -h >tst.x"
./adv1 -b 9999999 -h >tst.x ; ASSRT "$? -eq 0"
egrep "^[Ww]here:" tst.x >/dev/null ; ASSRT "$? -eq 0"

echo "./adv1"
./adv1 ; ST=$? ; CHKCORE; ASSRT "$ST -eq 0"
