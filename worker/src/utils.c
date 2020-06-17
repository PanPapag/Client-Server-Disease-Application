#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../common/includes/constants.h"
#include "../../common/includes/file_utils.h"
#include "../../common/includes/hash_table.h"
#include "../../common/includes/list.h"
#include "../../common/includes/macros.h"
#include "../../common/includes/report_utils.h"
#include "../../common/includes/string_utils.h"
#include "../../common/includes/types.h"
#include "../../common/includes/utils.h"

#include "../includes/avl.h"
#include "../includes/io_utils.h"
#include "../includes/patient_record.h"
#include "../includes/utils.h"

worker_structures structures;

void create_global_data_structures(void) {
  structures.patient_record_ht = hash_table_create(NO_BUCKETS, BUCKET_SIZE,
                                                   string_hash, string_compare,
                                                   string_print, patient_record_print,
                                                   NULL, patient_record_destroy);

  structures.disease_ht = hash_table_create(NO_BUCKETS, BUCKET_SIZE,
                                            string_hash, string_compare,
                                            string_print, avl_print_inorder,
                                            NULL, avl_clear);

  structures.country_ht = hash_table_create(NO_BUCKETS, BUCKET_SIZE,
                                            string_hash, string_compare,
                                            string_print, avl_print_inorder,
                                            NULL, avl_clear);

  structures.file_paths_ht = hash_table_create(NO_BUCKETS, BUCKET_SIZE,
                                               string_hash, string_compare,
                                               string_print, string_print,
                                               string_destroy, NULL);

  structures.countries_names = list_create(string*, ptr_to_string_compare,
                                           ptr_to_string_print, NULL);

	structures.diseases_names = list_create(string*, ptr_to_string_compare,
                                          ptr_to_string_print, NULL);

  // structures.files_statistics = list_create(statistics_entry_ptr*, NULL,
  //                                ptr_to_statistics_entry_print,
  //                                ptr_to_statistics_entry_destroy);
  printf("COMPLETED\n");
}
