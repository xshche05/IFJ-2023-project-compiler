%token id type name underscore expr
%% /* LL(1) */
PROG : CODE
| /*eps*/ ;

CODE : VAR_DECL '\n' CODE
| LET_DECL '\n' CODE
| FUNC_DECL '\n' CODE
| WHILE_LOOP '\n' CODE
| FOR_LOOP '\n' CODE
| BRANCH '\n' CODE
| ID_CALL_OR_ASSING '\n' CODE
| RETURN '\n'
| 'break'
| 'continue' ;

RETURN : 'return' RET_EXPR;

RET_EXPR : expr
| /*eps*/ ;

VAR_DECL : 'var' id VAR_LET_TYPE VAR_LET_EXP ;

VAR_LET_TYPE : ':' type
| /*eps*/ ;

VAR_LET_EXP : '=' expr
| /*eps*/ ;

LET_DECL : 'let' id VAR_LET_TYPE VAR_LET_EXP ;

FUNC_DECL : 'func' id '(' PARAM_LIST ')' FUNC_RET_TYPE '{' CODE '}' '\n' ;

FUNC_RET_TYPE : '->' type
| /*eps*/ ;

PARAM_LIST : PARAM NEXT_PARAM
| /*eps*/ ;

PARAM : PARAM_NAME id ':' type ;
PARAM_NAME : id
| underscore ;
NEXT_PARAM : ',' PARAM NEXT_PARAM
| /*eps*/ ;


BRANCH : 'if' BR_EXPR '{' CODE '}' ELSE ;
BR_EXPR : expr
| 'let' id ;
ELSE : 'else' ELSE_IF
| /*eps*/ ;
ELSE_IF : 'if' BR_EXPR '{' CODE '}' ELSE_IF
| '{' CODE '}' ;

WHILE_LOOP : 'while' expr '{' CODE '}' ;

FOR_LOOP : 'for' FOR_ID 'in' expr RANGE '{' CODE '}' ;
FOR_ID : id
| underscore ;
RANGE : '..' expr
| '..<' expr ;

CALL_PARAM_LIST : CALL_PARAM NEXT_CALL_PARAM
| /*eps*/ ;

CALL_PARAM : NAMED_CALL_PARAM
| POS_CALL_PARAM ;

NAMED_CALL_PARAM : name ':' expr ;

POS_CALL_PARAM : expr ;

NEXT_CALL_PARAM : ',' CALL_PARAM NEXT_CALL_PARAM
| /*eps*/ ;

ID_CALL_OR_ASSING : id NEXT_ID_CALL_OR_ASSING ;
NEXT_ID_CALL_OR_ASSING : '(' CALL_PARAM_LIST ')'
| '=' expr ;
