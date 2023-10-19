%token id type name underscore expr
%% /* LL(1) */
PROG : CODE 
| /*eps*/ ;

CODE : VAR_DECL '\n' CODE
| LET_DECL '\n' CODE
| FUNC_DECL '\n' CODE
| WHILE_LOOP '\n' CODE
| BRANCH '\n' CODE
| ASSIGNMENT'\n' CODE
| FUNC_CALL '\n' CODE 
| RETURN;

RETURN : 'return' RET_EXPR;

RET_EXPR : expr
| /*eps*/ ;

VAR_DECL : 'var' id ':' type '=' expr
| 'var' id ':' type
| 'var' id '=' expr ;

LET_DECL : 'let' id ':' type '=' expr
| 'let' id ':' type
| 'let' id '=' expr ;

FUNC_DECL : 'func' id '(' PARAM_LIST ')' '->' type '{' CODE '}' '\n'
|'func' id '(' PARAM_LIST ')' '{' CODE '}' '\n';

PARAM_LIST : PARAM NEXT_PARAM
| /*eps*/ ;

PARAM : PARAM_NAME id ':' type ;
PARAM_NAME : id
| underscore ;
NEXT_PARAM : ',' PARAM NEXT_PARAM
| /*eps*/ ;


BRANCH : 'if' BR_EXPR '{' CODE '}' 'else' '{' CODE '}' ;
BR_EXPR : expr
| 'let' id ;

WHILE_LOOP : 'while' expr '{' CODE '}' ;

FUNC_CALL : id '(' CALL_PARAM_LIST ')' ;

CALL_PARAM_LIST : CALL_PARAM NEXT_CALL_PARAM
| /*eps*/ ;

CALL_PARAM : NAMED_CALL_PARAM
| POS_CALL_PARAM ;

NAMED_CALL_PARAM : name ':' expr ;

POS_CALL_PARAM : expr ;

NEXT_CALL_PARAM : ',' CALL_PARAM NEXT_CALL_PARAM
| /*eps*/ ;

ASSIGNMENT : id '=' expr ;
