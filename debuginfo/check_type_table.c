#include <stdio.h>

#include <unity/src/unity.h>
#include <type_table.h>
#include <vaccs_util/string_utils.h>

 void test_type_table_insert() {
   int index;

   SymTable tp_table = type_table_init(100);

   index = type_table_index(tp_table,"unsigned long");
   TEST_ASSERT_NOT_EQUAL(SYM_INVALID_INDEX,index);
   TEST_ASSERT_EQUAL_INT(index,type_table_query_index(tp_table,"unsigned long"));

   TEST_ASSERT_EQUAL_INT(1, type_table_put_size(tp_table,index,8L));
   TEST_ASSERT_EQUAL_INT64(8L,type_table_get_size(tp_table,index));
   
   index = type_table_index(tp_table,"signed int");
   TEST_ASSERT_NOT_EQUAL(SYM_INVALID_INDEX,index);
   TEST_ASSERT_EQUAL_INT(index,type_table_query_index(tp_table,"signed int"));

   TEST_ASSERT_EQUAL_INT(1, type_table_put_size(tp_table,index,4L));
   TEST_ASSERT_EQUAL_INT64(4L,type_table_get_size(tp_table,index));

   type_table_fini(tp_table);
}

void test_type_table_errors() {

   int index;

   SymTable tp_table = type_table_init(100);

   TEST_ASSERT_EQUAL_INT(-1, type_table_query_index(tp_table,"signed long"));
   TEST_ASSERT_NOT_EQUAL(SYM_INVALID_INDEX,index = type_table_index(tp_table,"signed_long"));
   TEST_ASSERT_EQUAL_INT64(0L,type_table_get_size(tp_table,index));
   TEST_ASSERT_EQUAL_INT64(0L,type_table_get_size(tp_table,index+7));

   type_table_fini(tp_table);

}

int main() { 
   UNITY_BEGIN();
   RUN_TEST(test_type_table_insert);
   RUN_TEST(test_type_table_errors);
   return UNITY_END(); 
}
