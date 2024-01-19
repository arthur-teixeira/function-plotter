#!/bin/sh

set -xe

rm -f fp

CFLAGS="-Wall -Wextra -ggdb `pkg-config --cflags raylib`"
LIBS="`pkg-config --libs raylib` -lm"

clang $CFLAGS -o fp  ./*.c $LIBS -L./bin/

./fp
