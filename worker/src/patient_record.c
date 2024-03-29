#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../common/includes/constants.h"
#include "../../common/includes/macros.h"
#include "../../common/includes/string_utils.h"
#include "../../common/includes/report_utils.h"
#include "../../common/includes/utils.h"

#include "../includes/io_utils.h"
#include "../includes/patient_record.h"

patient_record_ptr patient_record_create(char **file_entry_tokens,
                                         const char *entry_date,
                                         const char *country) {
  /* Allocate memory for patient_record_ptr */
  patient_record_ptr patient_record = malloc(sizeof(*patient_record));
  if (patient_record == NULL) {
    die("Could not allocate memory for Patient Record. Exiting...");
  }
  /* Allocate memory and store record_id */
  patient_record->record_id = __MALLOC__((strlen(file_entry_tokens[0])+1), char);
  if (patient_record->record_id == NULL) {
    die("Could not allocate memory for Patient Record Record ID. Exiting...");
  }
  strcpy(patient_record->record_id, file_entry_tokens[0]);
  /* Allocate memory and store patient_first_name */
  patient_record->patient_first_name = __MALLOC__((strlen(file_entry_tokens[2])+1), char);
  if (patient_record->patient_first_name == NULL) {
    die("Could not allocate memory for Patient Record First Name. Exiting...");
  }
  strcpy(patient_record->patient_first_name, file_entry_tokens[2]);
  /* Allocate memory and store patient_last_name */
  patient_record->patient_last_name = __MALLOC__((strlen(file_entry_tokens[3])+1), char);
  if (patient_record->patient_last_name == NULL) {
    die("Could not allocate memory for Patient Record Last Name. Exiting...");
  }
  strcpy(patient_record->patient_last_name, file_entry_tokens[3]);
  /* Allocate memory and store disease_id */
  patient_record->disease_id = __MALLOC__((strlen(file_entry_tokens[0])+4), char);
  if (patient_record->disease_id == NULL) {
    die("Could not allocate memory for Patient Record Disease ID. Exiting...");
  }
  strcpy(patient_record->disease_id, file_entry_tokens[4]);
  /* Store age */
  patient_record->age = atoi(file_entry_tokens[5]);
  /* Allocate memory and store country */
  patient_record->country = __MALLOC__((strlen(country)+1), char);
  if (patient_record->country == NULL) {
    die("Could not allocate memory for Patient Record Country. Exiting...");
  }
  strcpy(patient_record->country, country);
  /* Store entry_date using struct tm format */
  memset(&patient_record->entry_date, 0, sizeof(struct tm));
  strptime(entry_date, "%d-%m-%Y", &patient_record->entry_date);
  /* Store exit_date using struct tm format */
  memset(&patient_record->exit_date, 0, sizeof(struct tm));
  strptime(EXIT_DATE_NOT_SPECIFIED, "%d-%m-%Y", &patient_record->exit_date);
  /* Return patient record pointer */
  return patient_record;
}

void patient_record_print(void *v, FILE *out) {
  patient_record_ptr patient_record = (patient_record_ptr) v;
  char entry_date_buffer[MAX_BUFFER_SIZE];
  char exit_date_buffer[MAX_BUFFER_SIZE];
  fprintf(out, "Record ID: %s\n", patient_record->record_id);
  fprintf(out, "Patient First Name: %s\n", patient_record->patient_first_name);
  fprintf(out, "Patient Last Name: %s\n", patient_record->patient_last_name);
  fprintf(out, "Disease ID: %s\n", patient_record->disease_id);
  fprintf(out, "Country: %s\n", patient_record->country);
  strftime(entry_date_buffer, sizeof(entry_date_buffer), "%d-%m-%Y", &patient_record->entry_date);
  fprintf(out, "Entry Date: %s\n", entry_date_buffer);
  strftime(exit_date_buffer, sizeof(exit_date_buffer), "%d-%m-%Y", &patient_record->exit_date);
  if (!strcmp(exit_date_buffer, EXIT_DATE_NOT_SPECIFIED)) {
    fprintf(out, "Exit Date: -\n");
  } else {
    fprintf(out, "Exit Date: %s\n", exit_date_buffer);
  }
  printf("\n");
}

char* patient_record_stringify(void *v) {
  patient_record_ptr patient_record = (patient_record_ptr) v;
  char entry_date_buffer[MAX_BUFFER_SIZE];
  strftime(entry_date_buffer, sizeof(entry_date_buffer), "%d-%m-%Y", &patient_record->entry_date);
  char exit_date_buffer[MAX_BUFFER_SIZE];
  strftime(exit_date_buffer, sizeof(exit_date_buffer), "%d-%m-%Y", &patient_record->exit_date);
  char buffer[MAX_BUFFER_SIZE];
  if (!strcmp(exit_date_buffer, EXIT_DATE_NOT_SPECIFIED)) {
    sprintf(buffer, "%s %s %s %s %" PRIu8 " %s -", patient_record->record_id,
                                                   patient_record->patient_first_name,
                                                   patient_record->patient_last_name,
                                                   patient_record->disease_id,
                                                   patient_record->age,
                                                   entry_date_buffer);
  } else {
    sprintf(buffer, "%s %s %s %s %" PRIu8 " %s %s", patient_record->record_id,
                                                    patient_record->patient_first_name,
                                                    patient_record->patient_last_name,
                                                    patient_record->disease_id,
                                                    patient_record->age,
                                                    entry_date_buffer,
                                                    exit_date_buffer);
  }
  char* result = __MALLOC__(strlen(buffer)+1, char);
  strcpy(result, buffer);
  return result;
}

int64_t patient_record_compare(void *a, void *b) {
	patient_record_ptr pr1 = (patient_record_ptr) a;
	patient_record_ptr pr2 = (patient_record_ptr) b;
  char pr1_entry_date_buffer[MAX_BUFFER_SIZE];
  char pr2_entry_date_buffer[MAX_BUFFER_SIZE];
  strftime(pr1_entry_date_buffer, sizeof(pr1_entry_date_buffer), "%s", &pr1->entry_date);
  strftime(pr2_entry_date_buffer, sizeof(pr2_entry_date_buffer), "%s", &pr2->entry_date);
  size_t pr1_entry_date_to_secs;
  size_t pr2_entry_date_to_secs;
  str_to_uint64(pr1_entry_date_buffer, (uint64_t *) &pr1_entry_date_to_secs);
  str_to_uint64(pr2_entry_date_buffer, (uint64_t *) &pr2_entry_date_to_secs);
	return pr1_entry_date_to_secs - pr2_entry_date_to_secs;
}

void patient_record_destroy(void *v) {
  patient_record_ptr patient_record = (patient_record_ptr) v;
  if (patient_record != NULL) {
    __FREE__(patient_record->record_id);
    __FREE__(patient_record->disease_id);
    __FREE__(patient_record->patient_first_name);
    __FREE__(patient_record->patient_last_name);
    __FREE__(patient_record->country);
    __FREE__(patient_record);
  }
}

int validate_patient_record_tokens(char **patient_record_tokens) {
  /* record_id: Only letters and numbers */
  char* record_id = patient_record_tokens[0];
  if (!is_alpharithmetic(record_id))
    return INVALID_RECORD_ID;
  /* patient_first_name: Only letters */
  char* patient_first_name = patient_record_tokens[1];
  if (!is_alphabetical(patient_first_name))
    return INVALID_PATIENT_FIRST_NAME;
  /* patient_last_name: Only letters */
  char* patient_last_name = patient_record_tokens[2];
  if (!is_alphabetical(patient_last_name))
    return INVALID_PATIENT_LAST_NAME;
  /* disease_id: Only letters, numbers and maybe character '-' */
  char* disease_id = patient_record_tokens[3];
  for (size_t i = 0; i < strlen(disease_id); ++i) {
    if (!isalnum(disease_id[i]) && disease_id[i] != '-')
      return INVALID_DISEASE_ID;
  }
  /* Age: Only numbers */
  char* age = patient_record_tokens[4];
  for (size_t i = 0; i < strlen(age); ++i) {
    if (!isalnum(age[i]))
      return INVALID_AGE;
  }
  /* country: Only letters */
  char* country = patient_record_tokens[5];
  if (!is_alphabetical(country))
    return INVALID_COUNTRY;
  /* entry_date: DD-MM-YYYY format */
  char* entry_date = patient_record_tokens[6];
  if (!is_valid_date_string(entry_date))
    return INVALID_ENTRY_DATE;
  /* exit_date: DD-MM-YYYY format or - (not specified) */
  char* exit_date = patient_record_tokens[7];
  if (!is_valid_date_string(exit_date) && !is_unspecified_date_string(exit_date)) {
    return INVALID_EXIT_DATE;
  }
  else {
    if (!is_unspecified_date_string(exit_date)) {
      /* Check if exit date is earlier than the entry date */
      if (date_string_compare(entry_date, exit_date) > 0)
        return INVALID_EARLIER_EXIT_DATE;
    }
  }
  /* Everything fine return success */
  return VALID_PATIENT_RECORD;
}

void print_patient_record_error(char **patient_record_tokens, int code) {
  switch (code) {
    case INVALID_RECORD_ID:
      report_warning("Invalid Record ID: <%s> format. Discarding patient record.",
                     patient_record_tokens[0]);
      break;
    case INVALID_PATIENT_FIRST_NAME:
      report_warning("Invalid Patient First Name: <%s> format. Discarding patient record.",
                     patient_record_tokens[1]);
      break;
    case INVALID_PATIENT_LAST_NAME:
      report_warning("Invalid Patient Last Name: <%s> format. Discarding patient record.",
                     patient_record_tokens[2]);
      break;
    case INVALID_DISEASE_ID:
      report_warning("Invalid Disease ID: <%s> format. Discarding patient record.",
                     patient_record_tokens[3]);
      break;
    case INVALID_AGE:
      report_warning("Invalid Age: <%s> format. Discarding patient record.",
                     patient_record_tokens[4]);
      break;
    case INVALID_COUNTRY:
      report_warning("Invalid Country: <%s> format. Discarding patient record.",
                     patient_record_tokens[5]);
      break;
    case INVALID_ENTRY_DATE:
      report_warning("Invalid Entry Date: <%s> format. Discarding patient record.",
                     patient_record_tokens[6]);
      break;
    case INVALID_EXIT_DATE:
      report_warning("Invalid Exit Date: <%s> format. Discarding patient record.",
                     patient_record_tokens[7]);
      break;
    case INVALID_EARLIER_EXIT_DATE:
      report_warning("Invalid Exit Date: <%s> is earlier than Entry Date <%s>. "
                     "Discarding patient record.",
                     patient_record_tokens[7], patient_record_tokens[6]);
      break;
  }
}

char* get_country(patient_record_ptr patient_record) {
  return patient_record->country;
}

char* get_disease_id(patient_record_ptr patient_record) {
  return patient_record->disease_id;
}

struct tm get_entry_date(patient_record_ptr patient_record) {
  return patient_record->entry_date;
}

struct tm get_exit_date(patient_record_ptr patient_record) {
  return patient_record->exit_date;
}
