#!/bin/bash
try(){
	expected="$2"
	input="$1"

	./ycc "$input" > tmp.s
	gcc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$input => $expected expected, but got $actual"
		exit 1
	fi
}

try "0;" 0;
try '42;' 42
try '1+1;' 2
try '5+20-4;' 21
try '12 + 34-5;' 41
try '5*2;' 10
try '100/10;' 10
try '(7 + 3) - 3;' 7
try '(4*4)-5;' 11
try '(20/2)-5;' 5
try '(10/5)+(2*3)-(1*3);' 5
try '(5*(21/(4+(4-1))));' 15
try 'a = 2;' 2
try 'a = 2; b = 3; a + b;' 5
try 'c = 2; d = 8; d - c;' 6
try 'a = b = 2; a * b;' 4
try "a = 8; b = 2; a / b;" 4
try "abc = 1; abd = 2; abc + abd;" 3
echo OK
