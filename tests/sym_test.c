//
// Created by Spagetik on 08-Nov-23.
//

#include <malloc.h>
#include <assert.h>
#include "new_symtable.h"
#include "codegen/codegen.h"

void init_test() {
    symtable_init();
}

void end_test() {
    symtable_destroy();
}

void test_add_find() {
    init_test();

    string_t *key_a = String.create("a");
    string_t *key_b = String.create("b");
    string_t *key_c = String.create("c");

    symTableData_t data_a;
    symTableData_t data_b;
    symTableData_t data_c;

    data_a.type = ndLet;
    data_b.type = ndVar;
    data_c.type = ndFunc;

    symtable_add(key_a, data_a);
    symtable_add(key_b, data_b);
    symtable_add(key_c, data_c);

    symTableData_t *data_aa;
    symTableData_t *data_bb;
    symTableData_t *data_cc;
    symTableData_t *data;
    //symtable_print();

    string_t *key = String.create("d");

    int a = symtable_find(key_a, &data_aa);
    int b = symtable_find(key_b, &data_bb);
    int c = symtable_find(key_c, &data_cc);
    int d = symtable_find(key, &data);

    assert(a == 0);
    assert(b == 0);
    assert(c == 0);
    assert(d == -1);

    assert(data_aa->type == data_a.type);
    assert(data_bb->type == data_b.type);
    assert(data_cc->type == data_c.type);

    end_test();
}


void test_find_non_existed() {
    init_test();

    string_t *key_a = String.create("a");
    string_t *key_b = String.create("b");
    string_t *key_c = String.create("c");

    symTableData_t *data;

    // symtable_print();

    int a = symtable_find(key_a, &data);
    int b = symtable_find(key_b, &data);
    int c = symtable_find(key_c, &data);

    assert(a == -1);
    assert(b == -1);
    assert(c == -1);

    push_frame();
    push_frame();
    push_frame();

    // symtable_print();

    a = symtable_find(key_a, &data);
    b = symtable_find(key_b, &data);
    c = symtable_find(key_c, &data);

    assert(a == -1);
    assert(b == -1);
    assert(c == -1);

    end_test();
}

void test_push_pop() {
    init_test();

    string_t *key_a = String.create("a");
    string_t *key_b = String.create("b");
    string_t *key_c = String.create("c");

    symTableData_t data_a;
    symTableData_t data_b;
    symTableData_t data_c;

    data_a.type = ndLet;
    data_b.type = ndVar;
    data_c.type = ndFunc;


    symtable_add(key_a, data_a);
    symtable_add(key_b, data_b);
    symtable_add(key_c, data_c);

    push_frame();

    symtable_add(key_a, data_a);
    symtable_add(key_b, data_b);

    push_frame();

    symtable_add(key_a, data_a);

    // symtable_print();

    symTableData_t *data;
    int a = symtable_find(key_a, &data);
    int b = symtable_find(key_b, &data);
    int c = symtable_find(key_c, &data);

    assert(a == 2);
    assert(b == 1);
    assert(c == 0);

    pop_frame();
    pop_frame();

    // symtable_print();

    a = symtable_find(key_a, &data);
    b = symtable_find(key_b, &data);
    c = symtable_find(key_c, &data);

    assert(a == 0);
    assert(b == 0);
    assert(c == 0);

    end_test();
}



int main() {
//    test_add_find();
//    printf("test_add_find passed\n");
//    test_push_pop();
//    printf("test_push_pop_add_find passed\n");
//    test_find_non_existed();
//    printf("test_find_non_existed passed\n");
//    gen_register_def();
//    printf("%s\n", gen_unique_label("label"));
//    printf("%s\n", gen_unique_label("label"));
//    printf("%s\n", gen_unique_label("if"));
//    printf("%s\n", gen_unique_label("for"));
//    printf("%s\n", gen_scope_var("var", 0, true));
//    printf("%s\n", gen_scope_var("var", 1, true));
//    printf("%s\n", gen_scope_var("var", 2, true));
//    printf("%s\n", gen_scope_var("var", 3, false));
    gen_std_functions();
    return 0;
}