#include <stdio.h>

#include <unity/src/unity.h>
#include <compile_unit_table.h>
#include <vaccs_util/string_utils.h>
#include <type_table.h>

 void test_compile_unit_table_insert() {
   int index;

   SymTable cu_table = compile_unit_table_init(100);
   SymTable type_table = type_table_init(100);
   SymTable var_table;
   long low_pc;
   long high_pc;

   index = compile_unit_table_index(cu_table,"/home/carr/vaccs","vaccs");
   TEST_ASSERT_NOT_EQUAL(SYM_INVALID_INDEX,index);
   TEST_ASSERT_EQUAL_INT(index,compile_unit_table_query_index(cu_table,"/home/carr/vaccs","vaccs"));

   TEST_ASSERT_EQUAL_INT(1, compile_unit_table_put_type_table(cu_table,index,type_table));
   TEST_ASSERT_EQUAL_PTR(type_table,compile_unit_table_get_type_table(cu_table,index));
   
   TEST_ASSERT_EQUAL_INT(1, compile_unit_table_put_var_table(cu_table,index,var_table));
   TEST_ASSERT_EQUAL_PTR(var_table,compile_unit_table_get_var_table(cu_table,index));
   
   TEST_ASSERT_EQUAL_INT(1, compile_unit_table_put_low_pc(cu_table,index,&low_pc));
   TEST_ASSERT_EQUAL_PTR(&low_pc,compile_unit_table_get_low_pc(cu_table,index));

   TEST_ASSERT_EQUAL_INT(1, compile_unit_table_put_high_pc(cu_table,index,&high_pc));
   TEST_ASSERT_EQUAL_PTR(&high_pc,compile_unit_table_get_high_pc(cu_table,index));

   compile_unit_table_fini(cu_table);
}

void test_compile_unit_table_errors() {

   int index;

   SymTable cu_table = compile_unit_table_init(100);

   TEST_ASSERT_EQUAL_INT(-1, compile_unit_table_query_index(cu_table,"/dev","null"));
   TEST_ASSERT_NOT_EQUAL(SYM_INVALID_INDEX,index = compile_unit_table_index(cu_table,"dev","null"));
   TEST_ASSERT_EQUAL_PTR(NULL,compile_unit_table_get_type_table(cu_table,index));
   TEST_ASSERT_EQUAL_PTR(NULL,compile_unit_table_get_var_table(cu_table,index+7));
   TEST_ASSERT_EQUAL_PTR(NULL,compile_unit_table_get_low_pc(cu_table,index));
   TEST_ASSERT_EQUAL_PTR(NULL,compile_unit_table_get_high_pc(cu_table,index+7));

   compile_unit_table_fini(cu_table);

}

int main() { 
   UNITY_BEGIN();
   RUN_TEST(test_compile_unit_table_insert);
   RUN_TEST(test_compile_unit_table_errors);
   return UNITY_END(); 
}
