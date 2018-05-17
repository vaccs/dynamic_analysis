#include <stdio.h>

#include <unity/src/unity.h>
#include <abbrv_table.h>
#include <vaccs_util/string_utils.h>

 void test_abbrv_table_insert() {
   int index;

   abbrv_table_init(100);
   index = abbrv_table_index(ssave("DW_AT_name"));
   TEST_ASSERT_NOT_EQUAL(SYM_INVALID_INDEX,index);
   TEST_ASSERT_EQUAL_INT(index,abbrv_table_query_index("DW_AT_name"));
   TEST_ASSERT_EQUAL_INT(1, abbrv_table_put_tag(index,(real_tag_type)ssave("DW_FORM_string")));
   TEST_ASSERT_EQUAL_STRING("DW_FORM_string",abbrv_table_get_tag(index));
   
   TEST_ASSERT_NOT_EQUAL(SYM_INVALID_INDEX,index = abbrv_table_index(ssave("DW_AT_stmt_list")));
   TEST_ASSERT_EQUAL_INT(index,abbrv_table_query_index("DW_AT_stmt_list"));
   TEST_ASSERT_EQUAL_INT(1,abbrv_table_put_tag(index,(real_tag_type)ssave("DW_FORM_data4")));
   TEST_ASSERT_EQUAL_STRING("DW_FORM_data4",abbrv_table_get_tag(index));

   TEST_ASSERT_NOT_EQUAL(SYM_INVALID_INDEX,index = abbrv_table_index(ssave("DW_AT_low_pc")));
   TEST_ASSERT_EQUAL_INT(index,abbrv_table_query_index("DW_AT_low_pc"));
   TEST_ASSERT_EQUAL_INT(1,abbrv_table_put_tag(index,(real_tag_type)ssave("DW_FORM_addr")));
   TEST_ASSERT_EQUAL_STRING("DW_FORM_addr",abbrv_table_get_tag(index));
   
   TEST_ASSERT_NOT_EQUAL(SYM_INVALID_INDEX,index = abbrv_table_index(ssave("DW_AT_high_pc")));
   TEST_ASSERT_EQUAL_INT(index,abbrv_table_query_index("DW_AT_high_pc"));
   TEST_ASSERT_EQUAL_INT(1,abbrv_table_put_tag(index,(real_tag_type)ssave("DW_FORM_addr")));
   TEST_ASSERT_EQUAL_STRING("DW_FORM_addr",abbrv_table_get_tag(index));
   
   TEST_ASSERT_NOT_EQUAL(SYM_INVALID_INDEX,index = abbrv_table_index(ssave("DW_AT_language")));
   TEST_ASSERT_EQUAL_INT(index,abbrv_table_query_index("DW_AT_language"));
   TEST_ASSERT_EQUAL_INT(1,abbrv_table_put_tag(index,(real_tag_type)ssave("DW_FORM_data1")));
   TEST_ASSERT_EQUAL_STRING("DW_FORM_data1",abbrv_table_get_tag(index));
   
   TEST_ASSERT_NOT_EQUAL(SYM_INVALID_INDEX,index = abbrv_table_index(ssave("DW_AT_comp_dir")));
   TEST_ASSERT_EQUAL_INT(index,abbrv_table_query_index("DW_AT_comp_dir"));
   TEST_ASSERT_EQUAL_INT(1,abbrv_table_put_tag(index,(real_tag_type)ssave("DW_FORM_string")));
   TEST_ASSERT_EQUAL_STRING("DW_FORM_string",abbrv_table_get_tag(index));
    
   TEST_ASSERT_NOT_EQUAL(SYM_INVALID_INDEX,index = abbrv_table_index(ssave("DW_AT_producer")));
   TEST_ASSERT_EQUAL_INT(index,abbrv_table_query_index("DW_AT_producer"));
   TEST_ASSERT_EQUAL_INT(1,abbrv_table_put_tag(index,(real_tag_type)ssave("DW_FORM_string")));
   TEST_ASSERT_EQUAL_STRING("DW_FORM_string",abbrv_table_get_tag(index));
   
   abbrv_table_fini();
}

void test_abbrv_table_errors() {

   int index;

   abbrv_table_init(100);

   TEST_ASSERT_EQUAL_INT(-1, abbrv_table_query_index("DW_AT_NAME"));
   TEST_ASSERT_NOT_EQUAL(SYM_INVALID_INDEX,index = abbrv_table_index("DW_AT_NAME"));
   TEST_ASSERT_EQUAL_PTR(NULL,abbrv_table_get_tag(index));
   TEST_ASSERT_EQUAL_PTR(NULL,abbrv_table_get_tag(index+7));

   abbrv_table_fini();

}

int main() { 
   printf("Here\n");
   UNITY_BEGIN();
   RUN_TEST(test_abbrv_table_insert);
   RUN_TEST(test_abbrv_table_errors);
   return UNITY_END(); 
}
