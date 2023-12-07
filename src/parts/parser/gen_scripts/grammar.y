[PROGRAM] ::= [DECLARE_GLOBAL_FUNC]

[DECLARE_GLOBAL_FUNC] ::= [STMT_LIST]
[STMT_LIST] ::= [STMT] [STMT_LIST]
[STMT_LIST] ::= ''



[STMT] ::= if [IF_EXP] { [STMT_LIST] } [ELSE]
[STMT] ::= while [WHILE_EXP] { [STMT_LIST] }
[STMT] ::= [KWRD_STMT]
[STMT] ::= [ASSIGN_STMT]
[STMT] ::= [FUNC_DECL]
[STMT] ::= return [EXP]


[FUNC_DECL] ::= func id ( [PARAM_LIST] ) [RETURN_TYPE] { [STMT_LIST] }
[RETURN_TYPE] ::= -> [TYPE]
[RETURN_TYPE] ::= ''

[WHILE_EXP] ::= ( [EXP] [MB_WHILE_EXP] )

[MB_WHILE_EXP] ::= [COMP_OP] [EXP]
[MB_WHILE_EXP] ::= ''

[IF_EXP] ::= ( [EXP] )
[IF_EXP] ::= let id

[ELSE] ::= else { [STMT_LIST] }

[KWRD_STMT] ::= let id [MB_STMT_LET]
[KWRD_STMT] ::= var id [MB_STMT_LET]

[ASSIGN_STMT] ::= id [MB_ASSIGN_STMT]

[MB_ASSIGN_STMT] ::= = [EXP]
[MB_ASSIGN_STMT] ::= [FUNC_CALL]

[MB_ASSIGN_EXPR] ::= = [EXP]
[MB_ASSIGN_EXPR] ::= [FUNC_CALL]
[MB_ASSIGN_EXPR] ::= ''

[MB_STMT_LET] ::= : [TYPE] [MB_ASSIGN_EXPR]
[MB_STMT_LET] ::= = [EXP]


[TYPE] ::= String
[TYPE] ::= Int
[TYPE] ::= Double
[TYPE] ::= String?
[TYPE] ::= Int?
[TYPE] ::= Double?


[COMP_OP] ::= ==
[COMP_OP] ::= !=
[COMP_OP] ::= >
[COMP_OP] ::= >=
[COMP_OP] ::= <
[COMP_OP] ::= <=


[EXP] ::= [T] [EXP_LIST]
[EXP_LIST] ::= + [T] [EXP_LIST]
[EXP_LIST] ::= - [T] [EXP_LIST]
[EXP_LIST] ::= ''
[T] ::= [FAC] [T_LIST]
[T_LIST] ::= * [FAC] [T_LIST]
[T_LIST] ::= / [FAC] [T_LIST]
[T_LIST] ::= ''
[FAC] ::= ( [EXP] )
[FAC] ::= id [FAC_TAIL]
[FAC_TAIL] ::= [FUNC_CALL]
[FAC_TAIL] ::= ''








[FUNC_CALL] ::= ( [ARG_LIST] )

[ARG_LIST] ::= [ARG] [ARG_LIST_REST]
[ARG_LIST] ::= ''

[ARG_LIST_REST] ::= , [ARG] [ARG_LIST_REST]
[ARG_LIST_REST] ::= ''

[ARG] ::= [PREFIX] : [ARG_NAME]
[PREFIX] ::= ''
[PREFIX] ::= id

[ARG_NAME] ::= id


[PARAM_PREFIX] ::= _
[PARAM_PREFIX] ::= id
[PARAM_LIST] ::= [PARAM] [PARAM_LIST_REST]
[PARAM_LIST] ::= ''



[PARAM_LIST_REST] ::= , [PARAM] [PARAM_LIST_REST]
[PARAM_LIST_REST] ::= ''

[PARAM] ::= [PARAM_PREFIX] [PARAM_NAME] : [TYPE]

[PARAM_NAME] ::= id