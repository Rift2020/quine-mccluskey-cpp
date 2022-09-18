# quine-mccluskey-cpp

## Quick Start

download source code

`git clone --depth 1 https://github.com/Rift2020/quine-mccluskey-cpp`

complie it

`g++ quine-mccluskey-cpp/qm.cpp -o qm`

## Usage

show help information

`./qm help`

print true assignment for given boolean expression

`./qm true <boolean expression>`

e.g. `./qm true "a|b&c"`

find the simplest expression which is equal to given boolean expression

`./qm simp [format] <boolean expression>`

default format: -SOP

format: { -SOP | -POS }

e.g. `./qm simp "(a[0]|b)&c_c"` `qm simp -POS "a&~b|a&~d|b&~d|~b&d|~c"`

find the simplest expression which is equal to given truth table(.csv file)

`./qm table [format] <file path>`

default format: -SOP

format: { -SOP | -POS }

generate a csv file for (qm table),you need write truth value for 2nd~last line

`./qm csv <variable nums> [file path]`

default file path: qm-table-sample.csv

**Caution! qm csv will cover this file**

qm table csv file format:

```
[variable nums] [variable name1] [variable name2] ...
00..00,[0,1 or x]
00..01,[0,1 or x]
...
11..11,[0,1 or x]
```

sample:

```
4 a b c x
0000,1
0001,1
0010,1
0011,0
0100,1
0101,0
0110,1
0111,0
1000,0
1001,1
1010,0
1011,1
1100,0
1101,1
1110,1
1111,1
```

actually,program **only care abort first line and the last char of other lines**

so,following csv is the same

```
4 a b c x
0,1
1,1
2,1
3,0
0x4,1
hello?,0
hi?,1
,0
0
,,,,1
!@#$%^&*()0
1111,1
1111,0
1111,1
1111,1
1111,1
```

## notice

operators is the same as verilog operators

`~ ^ & | ()`

boolean expression **don't support const value**

so:`1&a`,`b^0&a`is illegal

