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


clang -Wall -g -cl-opt-disable -o adv1 adv1.c ; ASSRT "$? -eq 0"

ulimit -c unlimited

./adv1 -h >tst.x ; ASSRT "$? -eq 0"
egrep "^[Ww]here:" tst.x >/dev/null ; ASSRT "$? -eq 0"

./adv1 ; ST=$? ; CHKCORE; ASSRT "$ST -eq 0"
