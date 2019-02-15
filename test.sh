#!/bin/bash
try(){
	expected="$1"
	input="$2"

	./ycc "$input" > tmp.s
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

try 0 0
try 42 42
try 2 '1+1'
try 21 '5+20-4'
try 41 '12 + 34-5'
try 10 '5*2'
try 10 '100/10'
try 7 '(7 + 3) - 3'
try 11 '(4*4)-5'
try 5 '(20/2)-5'
try 5 '(10/5)+(2*3)-(1*3)'
try 15 '(5*(21/(4+(4-1))))'
echo OK
