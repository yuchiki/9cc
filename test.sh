#!/bin/bash
try() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$expected expected, but got $actual"
        exit 1
    fi
}

try 40 "40;"
try 0 "0;"

try 21 "5+20-4;"
try 41 " 12 + 34 - 5 ;"

try 47 "5+6*7;"
try 15 "5*(9-6);"
try 4 "(3+5)/2;"

try 4 "(+3++5)/+2;"
try 2 "-4+6;"

try 1 "5 < 6;"
try 0 "5 < 5;"
try 0 "5 < 4;"

try 1 "5 <= 6;"
try 1 "5 <= 5;"
try 0 "5 <= 4;"

try 0 "5 > 6;"
try 0 "5 > 5;"
try 1 "5 > 4;"

try 0 "5 >= 6;"
try 1 "5 >= 5;"
try 1 "5 >= 4;"

try 0 "5 == 6;"
try 1 "5 == 5;"
try 0 "5 == 4;"

try 1 "5 != 6;"
try 0 "5 != 5;"
try 1 "5 != 4;"

try 6 "a = 1 + 2; a + 3;"
try 9 "a = 1; b = 2; c = 3; a = 4; a + b + c;"
try 3 "a = 1; return a + 2; a + 5;"
try 9 "foo = 1; bar = 2; baz = 3; foo = 4; return foo + bar + baz;"

try 9 "foo = 1; { bar = 2; baz = 3; } foo = 4; return foo + bar + baz;"

try 10 "returnx = 10; x = 4; return returnx;"

try 2 "if (1) 2; else 3;"
try 3 "if (0) 2; else 3;"
try 4 "if (0) 2; else if (0) 3; else 4;"

try 55 "a = 0; sum = 0; while (a <= 10) { sum = sum + a; a = a + 1; } sum;"

try 55 "sum = 0; for (i = 0; i <= 10; i = i + 1) sum = sum + i; sum;"


echo OK
