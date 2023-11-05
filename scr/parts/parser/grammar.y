%token id underscore expr
%% /* LL(1) */
PROG : CODE
| /*eps*/ ;

CODE : VAR_DECL CODE
| LET_DECL CODE
| FUNC_DECL CODE
| WHILE_LOOP CODE
| FOR_LOOP CODE
| BRANCH CODE
| ID_CALL_OR_ASSING CODE
| RETURN
| 'break'
| 'continue' ;

RETURN : 'return' RET_EXPR NL;

RET_EXPR : expr
| /*eps*/ ;

VAR_DECL : 'var' id VAR_LET_TYPE VAR_LET_EXP NL;

VAR_LET_TYPE : ':' TYPE
| /*eps*/ ;

VAR_LET_EXP : '=' expr
| /*eps*/ ;

LET_DECL : 'let' id VAR_LET_TYPE VAR_LET_EXP NL;

FUNC_DECL : 'func' id '(' PARAM_LIST ')' FUNC_RET_TYPE '{' CODE '}' NL ;

FUNC_RET_TYPE : '->' TYPE
| /*eps*/ ;

PARAM_LIST : PARAM NEXT_PARAM
| /*eps*/ ;

PARAM : PARAM_NAME id ':' TYPE ;
PARAM_NAME : id
| underscore ;
NEXT_PARAM : ',' PARAM NEXT_PARAM
| /*eps*/ ;


BRANCH : 'if' BR_EXPR '{' CODE '}' ELSE NL;
BR_EXPR : expr
| 'let' id ;
ELSE : 'else' ELSE_IF
| /*eps*/ ;
ELSE_IF : 'if' BR_EXPR '{' CODE '}' ELSE_IF
| '{' CODE '}' ;

WHILE_LOOP : 'while' expr '{' CODE '}' NL;

FOR_LOOP : 'for' FOR_ID 'in' expr RANGE '{' CODE '}' NL;
FOR_ID : id
| underscore ;
RANGE : '...' expr
| '..<' expr ;

CALL_PARAM_LIST : CALL_PARAM NEXT_CALL_PARAM
| /*eps*/ ;

CALL_PARAM : NAMED_CALL_PARAM
| POS_CALL_PARAM ;

NAMED_CALL_PARAM : CALL_PARAM_NAME ':' expr ;
CALL_PARAM_NAME : id
| underscore;

POS_CALL_PARAM : expr ;

NEXT_CALL_PARAM : ',' CALL_PARAM NEXT_CALL_PARAM
| /*eps*/ ;

ID_CALL_OR_ASSING : id NEXT_ID_CALL_OR_ASSING NL;
NEXT_ID_CALL_OR_ASSING : '(' CALL_PARAM_LIST ')'
| '=' expr ;

SOFT_NL : '\n' SOFT_NL
| /*eps*/ ;

NL: '\n' SOFT_NL ;

TYPE: 'String'
| 'Int'
| 'Bool'
| 'Double' ;