//
// Created by Spagetik on 14.10.2023.
//

#include "token.h"
#include <stdlib.h>

char *tokens_as_str[51] = {
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
//        "TOKEN_NILABLE",
        "TOKEN_IS_NIL",
        "TOKEN_UNWRAP_NILABLE",
        "TOKEN_LOGICAL_AND",
        "TOKEN_LOGICAL_OR",
        "TOKEN_LOGICAL_NOT",
        "TOKEN_CLOSED_RANGE",
        "TOKEN_HALF_OPEN_RANGE",
        "TOKEN_INTEGER_LITERAL",
        "TOKEN_REAL_LITERAL",
        "TOKEN_STRING_LITERAL",
        "TOKEN_NIL_LITERAL",
        "TOKEN_TRUE_LITERAL",
        "TOKEN_FALSE_LITERAL",
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

static token_array_t *tokens;

/**
 * @brief allocate and initialize token structure
 *
 * @param type - type of token
 * @param subtype - subtype of token
 * @param lexeme - lexeme of token
 * @return pointer to token structure
 */
static token_t *token_ctor(token_type_t type, token_attribute attribute, bool has_attribute) {
    token_t *token = malloc(sizeof(token_t));
    if (token == NULL) {
        fprintf(stderr, "Error: malloc failed.\n");
        return NULL;
    }
    token->type = type;
    if (has_attribute) token->attribute = attribute;
    return token;
}

/**
 * @brief deallocate token structure
 *
 * @param token - pointer to token structure
 */
static void token_dtor(token_t *token) {
    if (token == NULL) {
        return;
    }
    if (token->type == TOKEN_IDENTIFIER) {
        String.dtor(token->attribute.identifier);
    }
    if (token->type == TOKEN_STRING_LITERAL) {
        String.dtor(token->attribute.string);
    }
    free(token);
    token = NULL;
}

/**
 * @brief print token
 *
 * @param token - pointer to token structure
 */
static void token_print(token_t *token) {
    if (token == NULL) {
        return;
    }
    printf("Token: %32s", tokens_as_str[token->type]);
    if (token->type == TOKEN_IDENTIFIER) {
        printf(",   ID:   %s", token->attribute.identifier->str);
    } else if (token->type == TOKEN_INTEGER_LITERAL) {
        printf(",   INT:  %d", token->attribute.integer);
    } else if (token->type == TOKEN_REAL_LITERAL) {
        printf(",   REAL: %a", token->attribute.real);
    } else if (token->type == TOKEN_STRING_LITERAL) {
        printf(",   STR:  %s", token->attribute.string->str);
    }
    switch (token->type)
    {
        case TOKEN_BOOL_TYPE:
        case TOKEN_DOUBLE_TYPE:
        case TOKEN_INT_TYPE:
        case TOKEN_STRING_TYPE:
            if (token->attribute.nilable) printf(",   NILABLE");
            else printf(",   NON-NILABLE");
            break;
        default:
            break;
    }
    printf("\n");
}

/**
 * @brief allocate and initialize token array structure
 *
 * @return pointer to token array structure
 */
static int token_array_ctor() {
    tokens = malloc(sizeof(token_array_t));
    if (tokens == NULL) {
        fprintf(stderr, "Error: malloc failed.\n");
        return -1;
    }
    tokens->array = malloc(sizeof(token_t *) * 16);
    if (tokens->array == NULL) {
        free(tokens);
        fprintf(stderr, "Error: malloc failed.\n");
        return -1;
    }
    tokens->allocated = 16;
    tokens->length = 0;
    tokens->current = -1;
    return 0;
}

/**
 * @brief deallocate token array structure
 *
 * @param token_array - pointer to token array structure
 */
static void token_array_dtor() {
    if (tokens == NULL) {
        return;
    }
    for (int i = 0; i < tokens->length; i++) {
        token_dtor(tokens->array[i]);
    }
    free(tokens->array);
    free(tokens);
    tokens = NULL;
}

/**
 * @brief add token to token array
 *
 * @param tokens - pointer to token array structure
 * @param token - pointer to token structure
 * @return 0 if success, non-zero otherwise
 */
static int token_array_add(token_t *token) {
    if (tokens == NULL) {
        fprintf(stderr, "Error: token array not initialized.\n");
        return -1;
    }
    if (token == NULL) {
        fprintf(stderr, "Error: token is NULL.\n");
        return -1;
    }
    if (tokens->length >= tokens->allocated) {
        void *tmp = realloc(tokens->array, sizeof(token_t *) * tokens->allocated * 2);
        if (tmp == NULL) {
            fprintf(stderr, "Error: realloc failed.\n");
            return -1;
        }
        tokens->array = tmp;
        tokens->allocated *= 2;
    }
    tokens->array[tokens->length] = token;
    tokens->length++;
    return 0;
}

static token_t *token_array_current() {
    if (tokens == NULL) {
        fprintf(stderr, "Error: token array not initialized.\n");
        return NULL;
    }
    if (tokens->current >= tokens->length) {
        fprintf(stderr, "Error: token array out of bounds.\n");
        return NULL;
    }
    if (tokens->current == -1) {
        fprintf(stderr, "Error: token array curr is empty.\n");
        return NULL;
    }
    return tokens->array[tokens->current];
}

static token_t *token_array_next() {
    if (tokens == NULL) {
        //fprintf(stderr, "Error: token array not initialized.\n");
        return NULL;
    }
    if (tokens->current >= tokens->length) {
        //fprintf(stderr, "Error: token array out of bounds.\n");
        return NULL;
    }
    tokens->current++;
    return tokens->array[tokens->current];
}

static token_t *token_array_prev() {
    if (tokens == NULL) {
        fprintf(stderr, "Error: token array not initialized.\n");
        return NULL;
    }
    if (tokens->current >= tokens->length) {
        fprintf(stderr, "Error: token array out of bounds.\n");
        return NULL;
    }
    tokens->current--;
    return tokens->array[tokens->current];
}

static int token_array_total() {
    if (tokens == NULL) {
        fprintf(stderr, "Error: token array not initialized.\n");
        return -1;
    }
    return tokens->length;
}

const struct token_interface Token = {
        .ctor = token_ctor,
        .dtor = token_dtor,
        .print = token_print
};

const struct token_array_interface TokenArray = {
        .ctor = token_array_ctor,
        .dtor = token_array_dtor,
        .add = token_array_add,
        .curr = token_array_current,
        .next = token_array_next,
        .prev = token_array_prev,
        .total = token_array_total
};