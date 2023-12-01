#ifndef IFJ_PRJ_SCANNER_H
#define IFJ_PRJ_SCANNER_H

#include <stdbool.h>
#include "../utils/utils.h"

typedef enum {
    START_S,          // S
    PLUS_S,           // +
    MUL_S,            // *
    EXCL_MARK_S,      // !
    NOT_EQUAL_S,      // !=
    IS_NIL_1_S,       // ?
    IS_NIL_2_S,       // ??
    GREATER_S,        // >
    GREATER_EQUAL_S,  // >=
    LESS_S,           // <
    LESS_EQUAL_S,     // <=
    MINUS_S,          // -
    ARROW_S,          // ->
    ASSIGN_S,         // =
    EQUAL_S,          // ==
    LOGICAL_AND1_S,   // &
    LOGICAL_AND2_S,   // &&
    LOGICAL_OR1_S,    // |
    LOGICAL_OR2_S,    // ||
    RANGE_START_S,    // .
    RANGE_START_1_S,  // ..
    RANGE_CLOSED_S,   // ...
    RANGE_HALF_OPEN_S,// ..<
    BRACE_L_S,        // {
    BRACE_R_S,        // }
    BRACKET_L_S,      // (
    BRACKET_R_S,      // )
    COLON_S,          // :
    SEMICOLON_S,      // ;
    COMMA_S,          // ,
    INTEGER_S,        // 0-9
    ID_S,             // a-zA-Z_
    TYPE_NIL_S,       // TYPE?
    REAL_S,           // 0-9.
    REAL_NUM_S,       // 0-9.0-9
    REAL_E_S,         // 0-9.eE
    EXP_SIGN_S,       // 0-9.eE+-0-9
    REAL_EXP_S,       // 0-9.eE+-0-9
    STR_START_S,      // "
    STR_1_END_S,      // ""
    STR_2_START_S,    // """  _S, }
//    STR_MULT_HALF_S,  //     """\n
    STR_MULT_S,       // """\n
    STR_LF_S,         // """\n\n
    STR_2_E_S,        // """\n\n"
    STR_2_EE_S,       // """\n\n""
    STR_2_END_S,      // """\n\n"""
    ESC_S,            // \ _S, flag mult=false
    U_SEC_S,          // \u _S, flag mult=(false or true)
    U_SEC_START_S,    // \u{
    U_SEC_NUM_S,      // \u{0-9
    DIV_S,            // /
    COM_SINGLE_S,      // //
    COM_MULT_START_S,       // /** _S,
    COM_MULT_NEW_S,         // /** */
    COM_MULT_S,     // /** */
    COM_HALF_END_S,   // /* _S, **
    COM_MULT_END_S,
    COM_END_SINGLE_S       // /* , */
} fsm_state_t;

int source_code_to_tokens();

#endif //IFJ_PRJ_SCANNER_H
