/*
 * IFJ Project 2023
 * Implementation of token and token array
 * Author: Kirill Shchetiniuk (xshche05)
 * Author: Nadzeya Antsipenka (xantsi00)
 * Author: Lilit Movsesian (xmovse00)
 * Author: Veranika Saltanava (xsalta01)
 */

#include "token.h"
#include <stdio.h>
#include "memory.h"

char *tokens_as_str[] = {
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
        "TOKEN_UNWRAP_NILLABLE",
        "TOKEN_LOGICAL_AND",
        "TOKEN_LOGICAL_OR",
        "TOKEN_LOGICAL_NOT",
        "TOKEN_LEFT_BRACKET",
        "TOKEN_RIGHT_BRACKET",
        "TOKEN_LEFT_BRACE",
        "TOKEN_RIGHT_BRACE",
        "TOKEN_COMMA",
        "TOKEN_COLON",
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
    token_t *token = safe_malloc(sizeof(token_t));
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
    safe_free(token);
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
    fprintf(stderr, "Token: %32s", tokens_as_str[token->type]);
    if (token->type == TOKEN_IDENTIFIER) {
        fprintf(stderr, ",   ID:   %s", token->attribute.identifier->str);
    } else if (token->type == TOKEN_INTEGER_LITERAL) {
        fprintf(stderr, ",   INT:  %d", token->attribute.integer);
    } else if (token->type == TOKEN_REAL_LITERAL) {
        fprintf(stderr, ",   REAL: %a", token->attribute.real);
    } else if (token->type == TOKEN_STRING_LITERAL) {
        fprintf(stderr, ",   STR:  %s", token->attribute.string->str);
    }
    switch (token->type)
    {
        case TOKEN_BOOL_TYPE:
        case TOKEN_DOUBLE_TYPE:
        case TOKEN_INT_TYPE:
        case TOKEN_STRING_TYPE:
            if (token->attribute.nillable) fprintf(stderr, ",   NILABLE");
            else fprintf(stderr, ",   NON-NILABLE");
            break;
        default:
            break;
    }
    if (token->has_newline_after) {
        fprintf(stderr, ",   NL");
    }
    fprintf(stderr, "\n");
}

/**
 * @brief allocate and initialize token array structure
 *
 * @return pointer to token array structure
 */
static int token_array_ctor() {
    tokens = safe_malloc(sizeof(token_array_t));
    tokens->array = safe_malloc(sizeof(token_t *) * 16);
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
    safe_free(tokens->array);
    safe_free(tokens);
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
        void *tmp = safe_realloc(tokens->array, sizeof(token_t *) * tokens->allocated * 2);
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
        fprintf(stderr, "Error: token array not initialized.\n");
        return NULL;
    }
    if (tokens->current >= tokens->length) {
        fprintf(stderr, "Error: token array out of bounds.\n");
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
    if (tokens->current - 1 >= tokens->length) {
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

static int token_array_set_nl_after() {
    token_t *token = tokens->array[tokens->length-1];
    if (token == NULL) {
        fprintf(stderr, "Error: token array out of bounds.\n");
        return -1;
    }
    token->has_newline_after = true;
    return 0;
}

static int token_array_to_start() {
    if (tokens == NULL) {
        fprintf(stderr, "Error: token array not initialized.\n");
        return -1;
    }
    tokens->current = -1;
    return 0;
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
        .total = token_array_total,
        .set_nl_after = token_array_set_nl_after,
        .reset = token_array_to_start
};