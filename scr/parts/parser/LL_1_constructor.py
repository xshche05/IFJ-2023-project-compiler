# Non terminal - UPPERCASE_WORD
# Terminal - everything
from functools import lru_cache

# from functools import lru_cache
#
# rules: list['Rule'] = []
# rules_dict: dict[str, 'Rule'] = {}
#
#
# class Rule:
#
#     def __init__(self, left: str):
#         self.left = left
#         self.follow = set()
#         self.nullable = False
#         self.rights: list[list[str]] = []
#
#     def __repr__(self):
#         out = self.left + " ::= "
#         for right in self.rights:
#             for word in right:
#                 out += word + " "
#             out += "| "
#         out = out[:-2] if len(self.rights) >= 1 else out
#         return out
#
#     def add_right(self, right: list[str]):
#         if right == ["EPS"]:
#             self.nullable = True
#         self.rights.append(right.copy())
#
#     @property
#     @lru_cache(maxsize=10000)
#     def first(self):
#         # get all possible firsts for rule
#         # if first is terminal add it to first
#         # if first is non terminal add its first to first
#         # rules is list of Rules type
#         first = set()
#         for right in self.rights:
#             if right[0] in rules_dict.keys():
#                 first.update(rules_dict[right[0]].first)
#             else:
#                 first.add(right[0])
#         first.discard("EPS")
#         return first
#
#     @property
#     def empty(self):
#         return ["EPS"] in self.rights
#
#
# # open file with rules
# # read rules
# # Rule format: left ::= right1
# #              left ::= right2
# with open("grammar", "r") as file:
#     for line in file:
#         line = line.split("::=")
#         if len(line) != 2:
#             continue
#         left = line[0].split()[0]
#         right = line[1].split()
#         right = right + ["EPS"] if right[0] != "''" else ["EPS"]
#         if left not in rules_dict.keys():
#             r = Rule(left)
#             r.add_right(right)
#             rules.append(r)
#             rules_dict[left] = r
#         else:
#             r.add_right(right)
#         if len(rules) == 1:
#             rr = Rule("S")
#             rr.add_right([left, "$"])
#             rr.add_right(["$"])
#             rules.insert(0, rr)
#             rules_dict["S"] = rr
#
# N = set() # non terminals
# T = set() # terminals
#
# for non_terminal in rules_dict.keys():
#     N.add(non_terminal)
# for rule in rules:
#     for right in rule.rights:
#         for word in right:
#             if word not in N:
#                 T.add(word)
#
# def compute_follows():
#     has_changed = True
#     rules_dict["S"].follow.add("$")
#     previous_follows = dict()
#     for rule in rules:
#         previous_follows[rule.left] = rule.follow.copy()
#     while has_changed:
#         has_changed = False
#         for current_rule in rules:
#             for current_right in current_rule.rights:
#                 if set(current_right) & N:
#                     for B_idx in range(len(current_right)-1):
#                         B = current_right[B_idx]
#                         if B in T:
#                             continue
#                         elif B in N:
#                             y = current_right[current_right.index(B) + 1]
#                             if y != "EPS":
#                                 if y in T:
#                                     if B == "VAR_LET_TYPE":
#                                         print("HERE")
#                                     rules_dict[B].follow.add(y) # first(y) = y
#                                 elif y in N: # Empty false
#                                     if B == "VAR_LET_TYPE":
#                                         print("HERE")
#                                     for first in rules_dict[y].first:
#                                         rules_dict[B].follow.add(first)
#                                 else:
#                                     print("ERROR")
#                             if y == "EPS" or (y in N and rules_dict[y].nullable):
#                                 if B == "VAR_LET_TYPE":
#                                     print("HERE")
#                                 for follow in current_rule.follow:
#                                     rules_dict[B].follow.add(follow)
#                         else:
#                             print("ERROR")
#         for rule in rules:
#             if rule.follow != previous_follows[rule.left]:
#                 has_changed = True
#                 previous_follows[rule.left] = rule.follow.copy()
#
# compute_follows()
# compute_follows()
#
# for rule in rules:
#     for first in rule.first:
#         rule.follow.discard(first)
#
# for rule in rules:
#     # print firsts
#     print(rule.left, ":", rule.follow)


rules: dict[int, dict[str, list[str]]] = {}

N = set() # non terminals
T = set() # terminals

with open("grammar", "r") as file:
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

for rule in rules.values():
    for left in rule.keys():
        non_terminals.add(left)

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
                    if empty(Xes[i-1]) == {"EPS"}:
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
    follows["S"].add("$")
    follows_previous = dict()
    for non_terminal in non_terminals:
        follows_previous[non_terminal] = follows[non_terminal].copy()
    changed = True
    while changed:
        changed = False
        for rule in rules.values():
            left = list(rule.keys())[0]
            right = list(rule.values())[0][0]
            for b_idx in range(len(right)-1):
                b = right[b_idx]
                # if b == "VAR_LET_TYPE":
                #     print("HERE")
                if b in terminals and b != "EPS":
                    continue
                else:
                    y = right[b_idx+1:]
                    if y != ["EPS"]:
                        follows[b].update(first_l(y))
                    if empty_l(y) == {"EPS"}:
                        follows[b].update(follows[left])

        for non_terminal in non_terminals:
            if follows[non_terminal] != follows_previous[non_terminal]:
                changed = True
                follows_previous[non_terminal] = follows[non_terminal].copy()

    return follows

follows = compute_follows()
for key in non_terminals:
    follows[key].discard("EPS")
    if not empty(key):
        continue
    print(key, ":", follows[key])
