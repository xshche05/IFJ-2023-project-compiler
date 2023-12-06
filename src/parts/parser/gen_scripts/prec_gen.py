d = {
    'unwrap': [1, 'non'],
    'not': [2, 'right'],
    'mul': [3, 'left'],
    'add': [4, 'left'],
    'nil_col': [5, 'right'],
    'eq': [6, 'non'],
    'and': [7, 'left'],
    'or': [8, 'left'],
    '(': [],
    'i': [],
    ')': [],
    '$': [],
}

# TOKEN_IDENTIFIER
# TOKEN_REAL_LITERAL,
# TOKEN_STRING_LITERAL,
# TOKEN_NIL_LITERAL,
# TOKEN_TRUE_LITERAL,
# TOKEN_FALSE_LITERAL,
# TOKEN_INTEGER_LITERAL,
# TOKEN_ADDITION,
# TOKEN_SUBTRACTION,
# TOKEN_MULTIPLICATION,
# TOKEN_DIVISION
# TOKEN_LESS_THAN
# TOKEN_LESS_THAN_OR_EQUAL_TO
# TOKEN_GREATER_THAN
# TOKEN_GREATER_THAN_OR_EQUAL_TO
# TOKEN_EQUAL_TO
# TOKEN_NOT_EQUAL_TO
# TOKEN_IS_NIL
# TOKEN_UNWRAP_NILABLE,
# TOKEN_LOGICAL_AND
# TOKEN_LOGICAL_OR
# TOKEN_LOGICAL_NOT
# TOKEN_LEFT_BRACE
# TOKEN_RIGHT_BRACE

less = '1'
greater = '2'
equal = '3'
non = '4'

operators = ['unwrap',
             'not',
             'mul',
             'div',
             'add',
             'sub',
             'nil_col',
             'eq',
             'neq',
             'gt',
             'lt',
             'gte',
             'lte',
             'and',
             'or']

print(' ', end=',')
for red_y in d.keys():
    print(red_y, end=',')
print()
for yellow_x in d.keys():
    print(yellow_x, end=',')
    for red_y in d.keys():
        # id
        if red_y == 'i':
            if yellow_x in ['not', 'mul', 'add', 'nil_col', 'eq', 'and', 'or', '(', '$']:
                print(less, end=',')
            else:
                print(non, end=',')
        elif yellow_x == 'i':
            if red_y in ['unwrap', 'mul', 'add', 'nil_col', 'eq', 'and', 'or', ')', '$']:
                print(greater, end=',')
            else:
                print(non, end=',')
        elif red_y == ')' and yellow_x == '(':
            print(equal, end=',')
        elif yellow_x == '(' and red_y not in [')', '$']:
            print(less, end=',')
        elif red_y == ')' and yellow_x not in ['(', '$']:
            print(greater, end=',')
        elif red_y == '(' and yellow_x in ['mul', 'not', 'add', 'nil_col', 'eq', 'and', 'or', '$']:
            print(less, end=',')
        elif yellow_x == ')' and red_y in ['mul', 'add', 'nil_col', 'eq', 'and', 'or', '$']:
            print(greater, end=',')
        elif yellow_x == '$' and red_y in ['mul', 'add', 'nil_col', 'eq', 'and', 'or', 'not', 'unwrap']:
            print(less, end=',')
        elif red_y == '$' and yellow_x in ['mul', 'add', 'nil_col', 'eq', 'and', 'or', 'not', 'unwrap']:
            print(greater, end=',')
        # ops
        elif yellow_x in operators and red_y in operators:
            if d[red_y][0] < d[yellow_x][0]:
                print(less, end=',')

            if d[red_y][0] > d[yellow_x][0]:
                print(greater, end=',')

            if d[yellow_x][0] == d[red_y][0]:
                if d[yellow_x][1] == 'left' and d[red_y][1] == 'left':
                    print(greater, end=',')
                elif d[yellow_x][1] == 'right' and d[red_y][1] == 'right':
                    print(less, end=',')
                else:
                    print(non, end=',')
        else:
            print(non, end=',')
    print()
