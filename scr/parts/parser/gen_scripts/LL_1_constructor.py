rules: dict[int, dict[str, list[str]]] = {}

dol = "TOKEN_EOF"

with open("../grammar.y", "r") as file:
    i = 1
    for line in file:
        line = line.split("::=")
        if len(line) != 2:
            continue
        left = line[0].split()[0]
        right = line[1].split()
        right = right + ["EPS"] if right[0] != "''" else ["EPS"]
        rules[i] = {left: [right]}
        i += 1

terminals = set()
non_terminals = set()

# Get non terminals set
for rule in rules.values():
    for left in rule.keys():
        non_terminals.add(left)

# Get terminals set
for rule in rules.values():
    for right in rule.values():
        for word_list in right:
            for word in word_list:
                if word not in non_terminals:
                    terminals.add(word)
terminals.discard("EPS")


def compute_first(n_or_t) -> set[str]:
    if n_or_t in terminals:
        return {n_or_t}
    elif n_or_t in non_terminals:
        fir = set()
        rules_with_n_or_t = [rule for rule in rules.values() if n_or_t in rule.keys()]
        for rule in rules_with_n_or_t:
            for right in rule.values():
                Xes = right[0]
                fir.update(first(Xes[0]))
                for i in range(1, len(Xes)):
                    if empty(Xes[i - 1]) == {"EPS"}:
                        fir.update(first(Xes[i]))
                    else:
                        break
        return fir


def first(a: str):
    if a == "EPS":
        return {"EPS"}
    if a in terminals:
        return {a}
    elif a in non_terminals:
        return compute_first(a)


def first_l(a: list[str]):
    fir = set()
    cur = a[0]
    fir.update(first(cur))
    for i in range(1, len(a)):
        if empty(cur) == {"EPS"}:
            cur = a[i]
            fir.update(first(cur))
        else:
            break
    return fir


def empty(a: str):
    if a == "EPS":
        return {"EPS"}
    elif a in non_terminals:
        rules_with_n_or_t = [rule for rule in rules.values() if a in rule.keys()]
        if any([rule[a] == [["EPS"]] for rule in rules_with_n_or_t]):
            return {"EPS"}
    return {}


def empty_l(a: list[str]):
    for i in range(len(a)):
        if empty(a[i]) != {"EPS"}:
            return set()
    return {"EPS"}


def compute_follows():
    follows = dict()
    for non_terminal in non_terminals:
        follows[non_terminal] = set()
    follows["S"].add(dol)
    follows_previous = dict()
    for non_terminal in non_terminals:
        follows_previous[non_terminal] = follows[non_terminal].copy()
    changed = True
    while changed:
        changed = False
        for rule in rules.values():
            left = list(rule.keys())[0]
            right = list(rule.values())[0][0]
            for b_idx in range(len(right) - 1):
                b = right[b_idx]
                # if b == "VAR_LET_TYPE":
                #     print("HERE")
                if b in terminals and b != "EPS":
                    continue
                else:
                    y = right[b_idx + 1:]
                    if y != ["EPS"]:
                        follows[b].update(first_l(y))
                    if empty_l(y) == {"EPS"}:
                        follows[b].update(follows[left])

        for non_terminal in non_terminals:
            if follows[non_terminal] != follows_previous[non_terminal]:
                changed = True
                follows_previous[non_terminal] = follows[non_terminal].copy()

    return follows


def compute_firsts():
    fir = dict()
    for non_terminal in non_terminals:
        fir[non_terminal] = first(non_terminal)
    return fir


def predict(rule_num: int):
    rule = rules[rule_num]
    left = list(rule.keys())[0]
    right = list(rule.values())[0][0]
    if empty_l(right) != {"EPS"}:
        return first_l(right)
    elif empty_l(right) == {"EPS"}:
        ret = first_l(right)
        ret.update(follows[left])
        return ret


follows = compute_follows()
firsts = compute_firsts()
#

rule_dict_2 = dict()  # {left: {predict_value: rule_num}}

for rule_num in rules.keys():
    rule = rules[rule_num]
    left = list(rule.keys())[0]
    right = list(rule.values())[0][0]
    predict_values = predict(rule_num)
    predict_values.discard("EPS")
    for val in predict_values:
        if left not in rule_dict_2.keys():
            rule_dict_2[left] = dict()
        rule_dict_2[left][val] = rule_num


def gen_func_header(name: str):
    func = f"bool {name}() " + "{\n"
    func += f"{' ' * 4}bool s;\n"
    if name == "S":
        func += f"{' ' * 4}lookahead = TokenArray.next();\n"
    func += f"{' ' * 4}switch (lookahead->type) " + "{\n"
    return func


def gen_func_case(predict_value: str, rule_num: int, name: str):
    rule_right = list(rules[rule_num].values())[0][0]
    func = f"{' ' * 8}case {predict_value}:\n"
    func += f"{' ' * 12}s = "
    if name != "EXPR":
        for NT in rule_right:
            if NT in non_terminals:
                func += F"{NT}() && "
            elif NT != "EPS":
                func += F"match({NT}) && "
    elif name == "EXPR":
        func += F"call_expr_parser() && "
    func += "true;\n"
    func += f"{' ' * 12}break;\n"
    return func


def gen_func_footer(predict_values: list[str] = None, name: str = None):
    func = f"{' ' * 8}default:\n"
    if predict_values is not None:
        func += f"{' ' * 12}printf(\"Syntax error [{name}]: expected {predict_values}, got %s\\n\", tokens_as_str[lookahead->type]);\n"
    func += f"{' ' * 12}s = false;\n"
    func += f"{' ' * 4}}}\n"
    func += f"{' ' * 4}return s;\n"
    func += "}\n"
    return func


def gen_func(name: str):
    func = gen_func_header(name)
    predict_vals = rule_dict_2[name].keys()
    for predict_value in rule_dict_2[name].keys():
        func += gen_func_case(predict_value, rule_dict_2[name][predict_value], name)
    func += gen_func_footer(predict_values=list(predict_vals), name=name)
    return func


def gen_c_file():
    print("""\
#include "parts.h"
#include "utils.h"
#include "new_symtable.h"

static char *tokens_as_str[] = {
        "TOKEN_IDENTIFIER",
        "TOKEN_DOUBLE_TYPE",
        "TOKEN_ELSE",
        "TOKEN_FUNC",
        "TOKEN_IF",
        "TOKEN_INT_TYPE",
        "TOKEN_LET",
        "TOKEN_RETURN",
        "TOKEN_STRING_TYPE",
        "TOKEN_VAR",
        "TOKEN_WHILE",
        "TOKEN_BOOL_TYPE",
        "TOKEN_FOR",
        "TOKEN_IN",
        "TOKEN_BREAK",
        "TOKEN_CONTINUE",
        "TOKEN_UNDERSCORE",
        "TOKEN_ASSIGNMENT",
        "TOKEN_CLOSED_RANGE",
        "TOKEN_HALF_OPEN_RANGE",
        "TOKEN_REAL_LITERAL",
        "TOKEN_STRING_LITERAL",
        "TOKEN_NIL_LITERAL",
        "TOKEN_TRUE_LITERAL",
        "TOKEN_FALSE_LITERAL",
        "TOKEN_INTEGER_LITERAL",
        "TOKEN_ADDITION",
        "TOKEN_SUBTRACTION",
        "TOKEN_MULTIPLICATION",
        "TOKEN_DIVISION",
        "TOKEN_LESS_THAN",
        "TOKEN_LESS_THAN_OR_EQUAL_TO",
        "TOKEN_GREATER_THAN",
        "TOKEN_GREATER_THAN_OR_EQUAL_TO",
        "TOKEN_EQUAL_TO",
        "TOKEN_NOT_EQUAL_TO",
        "TOKEN_IS_NIL",
        "TOKEN_UNWRAP_NILABLE",
        "TOKEN_LOGICAL_AND",
        "TOKEN_LOGICAL_OR",
        "TOKEN_LOGICAL_NOT",
        "TOKEN_LEFT_BRACKET",
        "TOKEN_RIGHT_BRACKET",
        "TOKEN_LEFT_BRACE",
        "TOKEN_RIGHT_BRACE",
        "TOKEN_COMMA",
        "TOKEN_COLON",
        "TOKEN_SEMICOLON",
        "TOKEN_ARROW",
        "TOKEN_NEWLINE",
        "TOKEN_EOF"
};
    
bool match(token_type_t type) {
    if (lookahead->type == type) {
        lookahead = TokenArray.next();
        return true;
    }
    printf("Syntax error: expected %s, got %s\\n", "dd", "dd");
    return false;
}

bool call_expr_parser() {
    return true;
}
    """)
    for non_terminal in rule_dict_2.keys():
        print(gen_func(non_terminal))

def gen_h_file():
    print("""\
#ifndef IFJ_PRJ_PARSER_H
#define IFJ_PRJ_PARSER_H

#include <stdbool.h>
#include "token.h"

token_t *lookahead;
bool nl_flag;    // new line

bool match(token_type_t type);

bool call_expr_parser();
""")
    for non_terminal in rule_dict_2.keys():
        print(f"bool {non_terminal}();\n")
    print("#endif //IFJ_PRJ_PARSER_H")

# gen_h_file()
gen_c_file()