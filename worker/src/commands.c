#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>

#include "../../common/includes/constants.h"
#include "../../common/includes/file_utils.h"
#include "../../common/includes/hash_table.h"
#include "../../common/includes/io_utils.h"
#include "../../common/includes/list.h"
#include "../../common/includes/macros.h"
#include "../../common/includes/report_utils.h"
#include "../../common/includes/statistics.h"
#include "../../common/includes/string_utils.h"
#include "../../common/includes/types.h"
#include "../../common/includes/utils.h"

#include "../includes/avl.h"
#include "../includes/commands.h"
#include "../includes/heap.h"
#include "../includes/io_utils.h"
#include "../includes/patient_record.h"
#include "../includes/utils.h"

worker_structures structures;

worker_options options;

static inline
void __count_patients_between(avl_node_ptr current_root,
                             struct tm (*cmp_field)(patient_record_ptr),
                             int *counter, struct tm date1, struct tm date2,
                             char *country) {

  avl_node_ptr temp = current_root;
  patient_record_ptr patient_record = NULL;
  /* Prune left search below this node as all entries have entry date < date1 */
  if (temp != NULL) {
    patient_record = (patient_record_ptr) temp->data_;
    if (date_tm_compare(date1, cmp_field(patient_record)) <= 0) {
      __count_patients_between(temp->left_, cmp_field, counter, date1, date2, country);
    }
    patient_record = (patient_record_ptr) temp->data_;
    /* Check if patient_record exit date is not specified */
    if (date_tm_compare(cmp_field(patient_record), date2) <= 0) {
      /* Check upper bound */
      if (country != NULL) {
        if (!strcmp(country, patient_record->country)) {
          /* Update counter */
          (*counter)++;
        }
      } else {
        /* Update counter */
        (*counter)++;
      }
    }
    __count_patients_between(temp->right_, cmp_field, counter, date1, date2, country);
  }
}

static inline
int __num_patients_between(avl_ptr disease_avl,
                           struct tm (*cmp_field)(patient_record_ptr),
                           char *date1, char *date2, char *country) {
  int counter = 0;
  /* Convert string dates to struct tm format */
  struct tm date1_tm;
  memset(&date1_tm, 0, sizeof(struct tm));
  strptime(date1, "%d-%m-%Y", &date1_tm);

  struct tm date2_tm;
  memset(&date2_tm, 0, sizeof(struct tm));
  strptime(date2, "%d-%m-%Y", &date2_tm);
  /*
    Traverse inorder the AVL Tree and each time a patient record is found
    between the given range increase the counter for current patients.
    We can prune the traversal under the node in which a patient record has
    entry date < date1 or exit date > date2.
  */
   __count_patients_between(disease_avl->root_, cmp_field, &counter, date1_tm, date2_tm, country);
  return counter;
}

void execute_disease_frequency(int argc, char **argv) {
  void* result = hash_table_find(structures.disease_ht, argv[0]);
  if (result == NULL) {
    printf("NO result!\n");
  } else {
    avl_ptr disease_avl = (avl_ptr) result;
    int num_patients;
    if (argc == 3){
      num_patients = __num_patients_between(disease_avl, get_entry_date, argv[1], argv[2], NULL);
    } else {
      num_patients = __num_patients_between(disease_avl, get_entry_date, argv[1], argv[2], argv[3]);
    }
    char num_patients_buffer[12];
    sprintf(num_patients_buffer, "%d", num_patients);
    printf("%s\n", num_patients_buffer);
  }
}

static inline
void __update_patients_per_group(avl_node_ptr current_root,
                                struct tm (*cmp_field)(patient_record_ptr),
                                struct tm date1, struct tm date2,
                                char *country, int **patients_per_group) {
  avl_node_ptr temp = current_root;
  patient_record_ptr patient_record = NULL;
  /* Prune left search below this node as all entries have entry date < date1 */
  if (temp != NULL) {
    patient_record = (patient_record_ptr) temp->data_;
    if (date_tm_compare(date1, cmp_field(patient_record)) <= 0) {
      __update_patients_per_group(temp->left_, cmp_field, date1, date2,
                                  country, patients_per_group);
    }
    patient_record = (patient_record_ptr) temp->data_;
    /* Check if patient_record exit date is not specified */
    if (date_tm_compare(cmp_field(patient_record), date2) <= 0) {
      /* Check upper bound */
      if (country != NULL) {
        if (!strcmp(country, patient_record->country)) {
          /* Update patients_per_group array */
          int pos = get_age_group(patient_record->age);
          (*patients_per_group)[pos]++;
        }
      }
    }
    __update_patients_per_group(temp->right_, cmp_field, date1, date2,
                                country, patients_per_group);
  }
}

static inline
void __util_execute_topk_age_ranges(avl_ptr disease_avl,
                                    struct tm (*cmp_field)(patient_record_ptr),
                                    char *date1, char *date2, char *country,
                                    int **patients_per_group) {
  /* Convert string dates to struct tm format */
  struct tm date1_tm;
  memset(&date1_tm, 0, sizeof(struct tm));
  strptime(date1, "%d-%m-%Y", &date1_tm);

  struct tm date2_tm;
  memset(&date2_tm, 0, sizeof(struct tm));
  strptime(date2, "%d-%m-%Y", &date2_tm);
  /*
    Traverse inorder the AVL Tree and each time a patient record is found
    between the given range increase the counter for current patients.
    We can prune the traversal under the node in which a patient record has
    entry date < date1 or exit date > date2.
  */
  __update_patients_per_group(disease_avl->root_, cmp_field, date1_tm, date2_tm,
                              country, patients_per_group);
}

void execute_topk_age_ranges(char **argv) {
  int k = atoi(argv[0]);
  char *country = argv[1];
  char *disease = argv[2];
  /* Get for the given country its AVL tree */
  void *result = hash_table_find(structures.disease_ht, disease);
  if (result == NULL) {
    printf("NO result!\n");
  } else {
    avl_ptr disease_avl = (avl_ptr) result;
    /* Store in a map like array num_patients per age group */
    int *patients_per_group = __CALLOC__(NO_AGE_GROUPS, int);
    /* Update patients_per_group */
    __util_execute_topk_age_ranges(disease_avl, get_entry_date, argv[3], argv[4],
                                   country, &patients_per_group);
    /* Copy patients_per_group content into struct age_groups_stats_t */
    age_groups_stats_ptr age_groups_stats[NO_AGE_GROUPS];
    int total_num_patients = 0;
    for (int i = 0; i < NO_AGE_GROUPS; ++i) {
      age_groups_stats[i] = (age_groups_stats_ptr) malloc(sizeof(*age_groups_stats[i]));
      age_groups_stats[i]->age_group = i;
      age_groups_stats[i]->no_patients = patients_per_group[i];
      total_num_patients += patients_per_group[i];
    }
    __FREE__(patients_per_group);
    /*
      Build on the fly a max heap and insert there the elements
      of the patients_per_group map array
    */
    heap_ptr heap = heap_create(age_groups_stats_compare, NULL, NULL);
    for (size_t i = 0U; i < NO_AGE_GROUPS; ++i) {
      heap_insert_max(&heap, age_groups_stats[i]);
    }
    /* Extract top k */
    for (size_t i = 0U; i < k; ++i) {
      result = heap_extract_max(&heap);
      if (result != NULL) {
        age_groups_stats_ptr age_groups_stats_entry = (age_groups_stats_ptr) result;
        double age_group_per = (double) age_groups_stats_entry->no_patients / total_num_patients * 100;
        char output_buffer[MAX_BUFFER_SIZE];
        char* age_group_name = get_age_group_name(age_groups_stats_entry->age_group);
        sprintf(output_buffer, "%s: %0.2f%%", age_group_name, age_group_per);
        printf("%s\n", output_buffer);
      }
    }
    /* Clear memory allocated */
    heap_clear(heap);
    for (int i = 0; i < NO_AGE_GROUPS; ++i) {
      __FREE__(age_groups_stats[i]);
    }
  }
}

void execute_search_patient_record(char **argv) {
  void *result = hash_table_find(structures.patient_record_ht, argv[0]);
  if (result == NULL) {
    printf("NO result!\n");
  } else {
    char *stringified_patient_record = patient_record_stringify(result);
    printf("%s\n", stringified_patient_record);
    __FREE__(stringified_patient_record);
  }
}

void execute_num_patients_admissions(int argc, char **argv) {
  void *result = hash_table_find(structures.disease_ht, argv[0]);
  if (result == NULL) {
    printf("NO result!\n");
  } else {
    avl_ptr disease_avl = (avl_ptr) result;
    int num_patients;
    if (argc == 3){
      char num_writes_buffer[12];
      sprintf(num_writes_buffer, "%ld", list_size(structures.countries_names));
      printf("%s\n", num_writes_buffer);
      for (size_t i = 1U; i <= list_size(structures.countries_names); ++i) {
        list_node_ptr list_node = list_get(structures.countries_names, i);
        char* country = (*(char**) list_node->data_);
        num_patients = __num_patients_between(disease_avl, get_entry_date, argv[1], argv[2], country);
        char output_buffer[MAX_BUFFER_SIZE];
        sprintf(output_buffer, "%s %d", country, num_patients);
        printf("%s\n", output_buffer);
      }
    } else {
      num_patients = __num_patients_between(disease_avl, get_entry_date, argv[1], argv[2], argv[3]);
      char num_patients_buffer[12];
      sprintf(num_patients_buffer, "%d", num_patients);
      printf("%s\n",num_patients_buffer);
    }
  }
}

void execute_num_patients_discharges(int argc, char **argv) {
  void *result = hash_table_find(structures.disease_ht, argv[0]);
  if (result == NULL) {
    printf("NO result!\n");
  } else {
    avl_ptr disease_avl = (avl_ptr) result;
    int num_patients;
    if (argc == 3){
      char num_writes_buffer[12];
      sprintf(num_writes_buffer, "%ld", list_size(structures.countries_names));
      printf("%s\n", num_writes_buffer);
      for (size_t i = 1U; i <= list_size(structures.countries_names); ++i) {
        list_node_ptr list_node = list_get(structures.countries_names, i);
        char* country = (*(char**) list_node->data_);
        num_patients = __num_patients_between(disease_avl, get_exit_date, argv[1], argv[2], country);
        char output_buffer[MAX_BUFFER_SIZE];
        sprintf(output_buffer, "%s %d", country, num_patients);
        printf("%s\n", output_buffer);
      }
    } else {
      num_patients = __num_patients_between(disease_avl, get_exit_date, argv[1], argv[2], argv[3]);
      char num_patients_buffer[12];
      sprintf(num_patients_buffer, "%d", num_patients);
      printf("%s\n", num_patients_buffer);
    }
  }
}

void execute_insert_patient_record(patient_record_ptr patient_record,
                                  hash_table_ptr age_groups_ht) {
  void *result = hash_table_find(structures.patient_record_ht, patient_record->record_id);
  /* If record Id not found */
  if (result == NULL) {
    /* Update patient record hash table */
    hash_table_insert(&structures.patient_record_ht, patient_record->record_id, patient_record);
    /* Get the age group of the patient_record which just inserted */
    int age_group_pos = get_age_group(patient_record->age);
    /* Search if patient record disease_id exists in age_groups_ht */
    result = hash_table_find(age_groups_ht, patient_record->disease_id);
    if (result == NULL) {
      /*
        Create an array of size NO_AGE_GROUPS to store total number of cases
        per age group
      */
      int *cases_per_age_group = __CALLOC__(NO_AGE_GROUPS, int);
      /* Update cases for the current age group with the given disease id */
      cases_per_age_group[age_group_pos]++;
      /* Insert to age_groups_ht */
      hash_table_insert(&age_groups_ht, patient_record->disease_id, cases_per_age_group);
    } else {
      /* Update the total number of cases in current age group with the given disease id */
      int *cases_per_age_group = (int*) result;
      cases_per_age_group[age_group_pos]++;
    }
    /* Search if patient record disease_id exists in structures.disease_ht */
    result = hash_table_find(structures.disease_ht, patient_record->disease_id);
    if (result == NULL) {
      /* Store disease to global structures.diseases_names list */
      list_push_front(&structures.diseases_names, &patient_record->disease_id);
      /* If not found create a new AVL tree to store pointers to patient record */
      avl_ptr new_disease_avl = avl_create(patient_record_compare,
                                           patient_record_print);
      avl_insert(&new_disease_avl, patient_record);
      /* Update disease hash table */
      hash_table_insert(&structures.disease_ht, patient_record->disease_id, new_disease_avl);
    } else {
      /* Update disease hash table by insert patient_record pointer to AVL tree */
      avl_ptr disease_avl = (avl_ptr) result;
      avl_insert(&disease_avl, patient_record);
    }
    /* Search if patient record country exists in country_ht */
    result = hash_table_find(structures.country_ht, patient_record->country);
    if (result == NULL) {
      /* Store country to global countries_names list */
      list_push_front(&structures.countries_names, &patient_record->country);
      /* If not found create a new AVL tree to store pointers to patient record */
      avl_ptr new_country_avl = avl_create(patient_record_compare,
                                           patient_record_print);
      avl_insert(&new_country_avl, patient_record);
      /* Update country hash table */
      hash_table_insert(&structures.country_ht, patient_record->country, new_country_avl);
    } else {
      /* Update country hash table by insert patient_record pointer to AVL tree */
      avl_ptr country_avl = (avl_ptr) result;
      avl_insert(&country_avl, patient_record);
    }
  } else {
    report_warning("Patient record with Record ID: <%s> already exists. ",
                  patient_record->record_id);
    /* Delete patient record and return */
    patient_record_destroy(patient_record);
  }
}

void execute_record_patient_exit(char *id, const char *exit_date) {
  /* Search if patient record id exists */
  void *result = hash_table_find(structures.patient_record_ht, id);
  if (result == NULL) {
    report_warning("Patient record with Record ID: <%s> not found.", id);
  } else {
    // Cast result to patient record pointer
    patient_record_ptr patient_record = (patient_record_ptr) result;
    /* Convert exit_date struct tm to buffer */
    char exit_date_buffer[MAX_BUFFER_SIZE];
    strftime(exit_date_buffer, sizeof(exit_date_buffer), "%d-%m-%Y", &patient_record->exit_date);
    /* Convert entry_date struct tm to buffer */
    char entry_date_buffer[MAX_BUFFER_SIZE];
    strftime(entry_date_buffer, sizeof(entry_date_buffer), "%d-%m-%Y", &patient_record->entry_date);
    /* exit_date has to be greater than entry_date */
    if (date_string_compare(entry_date_buffer, exit_date) > 0) {
      report_warning("Entry Date [%s] is after the given Exit Date [%s].",
                      entry_date_buffer, exit_date);
    }
    /* Check if exit date is not specified */
    if (!strcmp(exit_date_buffer, EXIT_DATE_NOT_SPECIFIED)) {
      // Update exit day of patient record with the given record id
      memset(&patient_record->exit_date, 0, sizeof(struct tm));
      strptime(exit_date, "%d-%m-%Y", &patient_record->exit_date);
    } else {
      report_warning("Patient record Exit Date with Record ID: "
                     "<%s> is already specified.", id);
    }
  }
}
