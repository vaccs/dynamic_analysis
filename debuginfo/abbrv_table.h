#ifndef abbr_h
#define abbr_h

#include <vaccs_util/general.h>
#include <vaccs_util/symtab.h>

#define SYM_ABBRV_TAG_FIELD "DW_REAL_TAG"

typedef char* real_tag_type;

EXTERN(void, abbrv_table_init, (int size));
EXTERN(void, abbrv_table_fini, (void)); 
EXTERN(int,  abbrv_table_index, (char *abbrv_tag));
EXTERN(int,  abbrv_table_query_index, (char *abbrv_tag));
EXTERN(int,  abbrv_table_put_tag, (int tag_index, real_tag_type real_tag));
EXTERN(real_tag_type, abbrv_table_get_tag, (int tag_index));

#endif
