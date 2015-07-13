#!/bin/bash
OUT="$(make test)"
echo "${OUT}"
OK="$(echo "$OUT" |grep OK |wc -l)"
FAIL="$(echo "$OUT" |grep FAIL|wc -l)"
printf "OK: $OK\nFAIL: $FAIL\n"
