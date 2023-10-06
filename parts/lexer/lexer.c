#include "lexer.h"
#include <stdlib.h>


token_t *token_init(token_type_t type, token_subtype_t subtype, string_t *lexeme) {
    token_t *token = malloc(sizeof(token_t));
    if (token == NULL) {
        fprintf(stderr, "Error: malloc failed.\n");
        return NULL;
    }
    token->type = type;
    token->subtype = subtype;
    token->lexeme = string_copy(lexeme);
    return token;
}

void token_dtor(token_t *token) {
    if (token == NULL) {
        return;
    }
    string_dtor(token->lexeme);
    free(token);
    token = NULL;
}

token_array_t *token_array_init() {
    token_array_t *token_array = malloc(sizeof(token_array_t));
    if (token_array == NULL) {
        fprintf(stderr, "Error: malloc failed.\n");
        return NULL;
    }
    token_array->array = malloc(sizeof(token_t *) * 16);
    if (token_array->array == NULL) {
        free(token_array);
        fprintf(stderr, "Error: malloc failed.\n");
        return NULL;
    }
    token_array->allocated = 16;
    token_array->length = 0;
    return token_array;
}

void token_array_dtor(token_array_t *token_array) {
    if (token_array == NULL) {
        return;
    }
    for (size_t i = 0; i < token_array->length; i++) {
        token_dtor(token_array->array[i]);
    }
    free(token_array->array);
    free(token_array);
    token_array = NULL;
}

int token_array_add(token_array_t *token_array, token_t *token) {
    if (token_array == NULL || token == NULL) {
        return -1;
    }
    if (token_array->length == token_array->allocated) {
        void *tmp = realloc(token_array->array, sizeof(token_t *) * token_array->allocated * 3/2);
        if (tmp == NULL) {
            return -1;
        }
        token_array->array = tmp;
        token_array->allocated *= 3/2;
    }
    token_array->array[token_array->length] = token;
    token_array->length++;
    return 0;
}

token_array_t *source_code_to_tokens(file_t *file) {
    return 0;
}