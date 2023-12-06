import pandas as pd

row_names = """
<S>
<CODE>
<RETURN>
<RET_EXPR>
<VAR_DECL>
<LET_DECL>
<VAR_LET_TYPE>
<VAR_LET_EXP>
<FUNC_DECL>
<FUNC_RET_TYPE> 
<PARAM_LIST>
<PARAM>
<PARAM_NAME>
<NEXT_PARAM>
<BRANCH>
<BR_EXPR>
<ELSE>
<ELSE_IF>
<WHILE_LOOP>
<FOR_LOOP>
<FOR_ID>
<RANGE>
<CALL_PARAM_LIST>
<CALL_PARAM>
<NEXT_CALL_PARAM>
<ID_CALL_OR_ASSIGN>
<NEXT_ID_CALL_OR_ASSIGN>
<TYPE>
<EXPR>
"""

col_names = """
$
eol
break
continue
return
var
id
let	
:	
=
func
(	
)	
{	
}	
->	
_	
,	
if	
else	
while	
for	
in	
...	
..<	
Int
Double	
String	
Bool	
int_literal	
double_literal	
string_literal	
true_literal	
false_literal	
nil_literal	
!
"""

# row, col, values
values = """S#for#[1]
S#continue#[1]
S#return#[1]
S#let#[1]
S#$#[1]
S#break#[1]
S#if#[1]
S#while#[1]
S#var#[1]
S#func#[1]
S#id#[1]
CODE#var#[2]
CODE#let#[3]
CODE#func#[4]
CODE#while#[5]
CODE#for#[6]
CODE#if#[7]
CODE#id#[8]
CODE#return#[9]
CODE#break#[10]
CODE#continue#[11]
CODE#}#[12]
CODE#$#[12]
RETURN#return#[13]
RET_EXPR#nil_literal#[14]
RET_EXPR#(#[14]
RET_EXPR#true_literal#[14]
RET_EXPR#string_literal#[14]
RET_EXPR#!#[14]
RET_EXPR#false_literal#[14]
RET_EXPR#int_literal#[14]
RET_EXPR#double_literal#[14]
RET_EXPR#id#[14]
RET_EXPR#}#[15]
RET_EXPR#$#[15]
VAR_DECL#var#[16]
LET_DECL#let#[17]
VAR_LET_TYPE#:#[18]
VAR_LET_TYPE#=#[19]
VAR_LET_TYPE#eol#[19]
VAR_LET_EXP#=#[20]
VAR_LET_EXP#eol#[21]
FUNC_DECL#func#[22]
FUNC_RET_TYPE#->#[23]
FUNC_RET_TYPE#{#[24]
PARAM_LIST#_#[25]
PARAM_LIST#id#[25]
PARAM_LIST#)#[26]
PARAM#_#[27]
PARAM#id#[27]
PARAM_NAME#id#[28]
PARAM_NAME#_#[29]
NEXT_PARAM#,#[30]
NEXT_PARAM#)#[31]
BRANCH#if#[32]
BR_EXPR#nil_literal#[33]
BR_EXPR#(#[33]
BR_EXPR#true_literal#[33]
BR_EXPR#string_literal#[33]
BR_EXPR#!#[33]
BR_EXPR#false_literal#[33]
BR_EXPR#int_literal#[33]
BR_EXPR#double_literal#[33]
BR_EXPR#id#[33]
BR_EXPR#let#[34]
ELSE#else#[35]
ELSE#eol#[36]
ELSE_IF#if#[37]
ELSE_IF#{#[38]
ELSE_IF#eol#[39]
WHILE_LOOP#while#[40]
FOR_LOOP#for#[41]
FOR_ID#id#[42]
FOR_ID#_#[43]
RANGE#...#[44]
RANGE#..<#[45]
CALL_PARAM_LIST#nil_literal#[46]
CALL_PARAM_LIST#(#[46]
CALL_PARAM_LIST#true_literal#[46]
CALL_PARAM_LIST#string_literal#[46]
CALL_PARAM_LIST#!#[46]
CALL_PARAM_LIST#false_literal#[46]
CALL_PARAM_LIST#int_literal#[46]
CALL_PARAM_LIST#double_literal#[46]
CALL_PARAM_LIST#id#[46]
CALL_PARAM_LIST#)#[47]
CALL_PARAM#id#[48, 49]
CALL_PARAM#nil_literal#[49]
CALL_PARAM#(#[49]
CALL_PARAM#true_literal#[49]
CALL_PARAM#string_literal#[49]
CALL_PARAM#!#[49]
CALL_PARAM#false_literal#[49]
CALL_PARAM#int_literal#[49]
CALL_PARAM#double_literal#[49]
NEXT_CALL_PARAM#,#[50]
NEXT_CALL_PARAM#)#[51]
ID_CALL_OR_ASSIGN#id#[52]
NEXT_ID_CALL_OR_ASSIGN#(#[53]
NEXT_ID_CALL_OR_ASSIGN#=#[54]
TYPE#Int#[55]
TYPE#Double#[56]
TYPE#String#[57]
TYPE#Bool#[58]
EXPR#id#[59]
EXPR#int_literal#[60]
EXPR#double_literal#[61]
EXPR#string_literal#[62]
EXPR#true_literal#[63]
EXPR#false_literal#[64]
EXPR#nil_literal#[65]
EXPR#(#[66]
EXPR#!#[67]
"""

# Create table with row and column names
table = pd.DataFrame(index=row_names.split(), columns=col_names.split())


# print([line.split(', ') for line in values.splitlines()])

# Fill table with values
for val in values.splitlines():
    print(val)
    row, col, val = val.split('#')
    row = f"<{row}>"
    val = val[1:-1]
    table.loc[row, col] = val

# Write table to file
with open('LLtable.csv', 'w') as f:
    f.write(table.to_csv())

