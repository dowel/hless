#!/bin/bash

echo "1234" | rpdb2 -d ./hless logs/avsha.218.log &
proc=`ps ax | grep hless | grep -v grep | cut -f1 -d' '`
echo "1234" | winpdb -a ${proc}
kill -9 ${proc}
