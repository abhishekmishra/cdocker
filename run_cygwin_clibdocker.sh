#!/bin/bash

. /cygdrive/c/Users/abhis/code/clibdocker/set_cygwin_env.sh
if [ -z "$1" ]
  then
    echo "Running main program."
	/cygdrive/c/Users/abhis/code/clibdocker/bin/debug/clibdocker.exe
  else
    echo "Running tests."
    make test
fi

