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
values = """S#if#[1]
S#var#[1]
S#while#[1]
S#func#[1]
S#let#[1]
S#id#[1]
S#return#[1]
[PROGRAM]#if#[2]
[PROGRAM]#var#[2]
[PROGRAM]#while#[2]
[PROGRAM]#func#[2]
[PROGRAM]#let#[2]
[PROGRAM]#id#[2]
[PROGRAM]#return#[2]
[DECLARE_GLOBAL_FUNC]#if#[3]
[DECLARE_GLOBAL_FUNC]#var#[3]
[DECLARE_GLOBAL_FUNC]#while#[3]
[DECLARE_GLOBAL_FUNC]#func#[3]
[DECLARE_GLOBAL_FUNC]#let#[3]
[DECLARE_GLOBAL_FUNC]#id#[3]
[DECLARE_GLOBAL_FUNC]#$#[3]
[DECLARE_GLOBAL_FUNC]#return#[3]
[STMT_LIST]#while#[4]
[STMT_LIST]#if#[4]
[STMT_LIST]#var#[4]
[STMT_LIST]#func#[4]
[STMT_LIST]#let#[4]
[STMT_LIST]#id#[4]
[STMT_LIST]#return#[4]
[STMT_LIST]#$#[5]
[STMT_LIST]#}#[5]
[STMT]#if#[6]
[STMT]#while#[7]
[STMT]#let#[8]
[STMT]#var#[8]
[STMT]#id#[9]
[STMT]#func#[10]
[STMT]#return#[11]
[FUNC_DECL]#func#[12]
[RETURN_TYPE]#->#[13]
[RETURN_TYPE]#{#[14]
[WHILE_EXP]#(#[15]
[MB_WHILE_EXP]#!=#[16]
[MB_WHILE_EXP]#>#[16]
[MB_WHILE_EXP]#<#[16]
[MB_WHILE_EXP]#==#[16]
[MB_WHILE_EXP]#<=#[16]
[MB_WHILE_EXP]#>=#[16]
[MB_WHILE_EXP]#)#[17]
[IF_EXP]#(#[18]
[IF_EXP]#let#[19]
[ELSE]#else#[20]
[KWRD_STMT]#let#[21]
[KWRD_STMT]#var#[22]
[ASSIGN_STMT]#id#[23]
[MB_ASSIGN_STMT]#=#[24]
[MB_ASSIGN_STMT]#(#[25]
[MB_ASSIGN_EXPR]#=#[26]
[MB_ASSIGN_EXPR]#(#[27]
[MB_ASSIGN_EXPR]#if#[28]
[MB_ASSIGN_EXPR]#var#[28]
[MB_ASSIGN_EXPR]#while#[28]
[MB_ASSIGN_EXPR]#}#[28]
[MB_ASSIGN_EXPR]#func#[28]
[MB_ASSIGN_EXPR]#let#[28]
[MB_ASSIGN_EXPR]#id#[28]
[MB_ASSIGN_EXPR]#$#[28]
[MB_ASSIGN_EXPR]#return#[28]
[MB_STMT_LET]#:#[29]
[MB_STMT_LET]#=#[30]
[TYPE]#String#[31]
[TYPE]#Int#[32]
[TYPE]#Double#[33]
[TYPE]#String?#[34]
[TYPE]#Int?#[35]
[TYPE]#Double?#[36]
[COMP_OP]#==#[37]
[COMP_OP]#!=#[38]
[COMP_OP]#>#[39]
[COMP_OP]#>=#[40]
[COMP_OP]#<#[41]
[COMP_OP]#<=#[42]
[EXP]#(#[43]
[EXP]#id#[43]
[EXP_LIST]#+#[44]
[EXP_LIST]#-#[45]
[EXP_LIST]#if#[46]
[EXP_LIST]#<=#[46]
[EXP_LIST]#<#[46]
[EXP_LIST]#)#[46]
[EXP_LIST]#func#[46]
[EXP_LIST]#let#[46]
[EXP_LIST]#return#[46]
[EXP_LIST]#>#[46]
[EXP_LIST]#var#[46]
[EXP_LIST]#>=#[46]
[EXP_LIST]#while#[46]
[EXP_LIST]#!=#[46]
[EXP_LIST]#==#[46]
[EXP_LIST]#}#[46]
[EXP_LIST]#id#[46]
[EXP_LIST]#$#[46]
[T]#(#[47]
[T]#id#[47]
[T_LIST]#*#[48]
[T_LIST]#/#[49]
[T_LIST]#if#[50]
[T_LIST]#-#[50]
[T_LIST]#<=#[50]
[T_LIST]#<#[50]
[T_LIST]#)#[50]
[T_LIST]#func#[50]
[T_LIST]#let#[50]
[T_LIST]#return#[50]
[T_LIST]#>#[50]
[T_LIST]#var#[50]
[T_LIST]#>=#[50]
[T_LIST]#while#[50]
[T_LIST]#!=#[50]
[T_LIST]#==#[50]
[T_LIST]#}#[50]
[T_LIST]#+#[50]
[T_LIST]#id#[50]
[T_LIST]#$#[50]
[FAC]#(#[51]
[FAC]#id#[52]
[FAC_TAIL]#(#[53]
[FAC_TAIL]#if#[54]
[FAC_TAIL]#-#[54]
[FAC_TAIL]#<=#[54]
[FAC_TAIL]#<#[54]
[FAC_TAIL]#/#[54]
[FAC_TAIL]#)#[54]
[FAC_TAIL]#func#[54]
[FAC_TAIL]#let#[54]
[FAC_TAIL]#return#[54]
[FAC_TAIL]#>#[54]
[FAC_TAIL]#var#[54]
[FAC_TAIL]#>=#[54]
[FAC_TAIL]#while#[54]
[FAC_TAIL]#!=#[54]
[FAC_TAIL]#*#[54]
[FAC_TAIL]#==#[54]
[FAC_TAIL]#}#[54]
[FAC_TAIL]#+#[54]
[FAC_TAIL]#id#[54]
[FAC_TAIL]#$#[54]
[FUNC_CALL]#(#[55]
[ARG_LIST]#:#[56]
[ARG_LIST]#id#[56]
[ARG_LIST]#)#[57]
[ARG_LIST_REST]#,#[58]
[ARG_LIST_REST]#)#[59]
[ARG]#:#[60]
[ARG]#id#[60]
[PREFIX]#:#[61]
[PREFIX]#id#[62]
[ARG_NAME]#id#[63]
[PARAM_PREFIX]#_#[64]
[PARAM_PREFIX]#id#[65]
[PARAM_LIST]#id#[66]
[PARAM_LIST]#_#[66]
[PARAM_LIST]#)#[67]
[PARAM_LIST_REST]#,#[68]
[PARAM_LIST_REST]#)#[69]
[PARAM]#id#[70]
[PARAM]#_#[70]
[PARAM_NAME]#id#[71]
"""

# Create table with row and column names
table = pd.DataFrame()


# print([line.split(', ') for line in values.splitlines()])

# Fill table with values
for val in values.splitlines():
    print(val)
    row, col, val = val.split('#')
    row = f"{row}"
    val = val[1:-1]
    table.loc[row, col] = val

# Write table to file
with open('LLtable.csv', 'w') as f:
    f.write(table.to_csv())

