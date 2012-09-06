/*
 * Sample program to demonstrate how to use the phonebook geneet
 * example.
 *
 * compile with
 * gcc -o test-phonebook -Wall test-phonebook.c phonebook.c `pkg-config eet --libs --cflags` `pkg-config evas --libs --cflags`
 */
#include <string.h>
#include "phonebook.h"

#define DATABASE "phonebook.eet"

int main(int argc, char **argv)
{
  Book *book;
  Person *person;
  
  if (argc < 2) {
    printf("usage: %s [create|dump]\n", argv[0]);
    return 1;
  }
  
  eina_init();
  phonebook_init();

  if (!strcmp(argv[1], "create")) {
    book = book_new(1, NULL);
    
    person = person_new("Joao", "Silva", NULL, NULL, NULL, NULL);
    person_addresses_add(person, address_new("Rua dos Bobos", 0, "00000-000", ADDRESS_STATE_SP, ADDRESS_TYPE_PERSONAL));
    person_addresses_add(person, address_new("Av. 1", 1234, "12345-678", ADDRESS_STATE_PE, ADDRESS_TYPE_WORK));
    person_phones_add(person, phone_number_new("555-12345", PHONE_NUMBER_TYPE_MOBILE));
    book_people_add(book, person);

    person = person_new("Maria", "Dolores", NULL, NULL, NULL, NULL);
    person_phones_add(person, phone_number_new("555-54321", PHONE_NUMBER_TYPE_HOME));
    book_people_add(book, person);

    person = person_new("Joaquim", "Manolo", NULL, NULL, NULL, NULL);
    book_people_add(book, person);
    
    if (book_save(book, DATABASE)) {
      printf("Saved to " DATABASE "\n");
    } else {
      printf("Error while saving to " DATABASE "\n");
      goto free_book;
    }
  } else if (!strcmp(argv[1], "dump")) {
    Eina_List *person_iter;
    
    if (!(book = book_load(DATABASE))) {
      printf("Error while loading " DATABASE "\n");
      goto bail_out;
    }
    
    EINA_LIST_FOREACH(book_people_list_get(book), person_iter, person)
    {
      Eina_List *addresses;
      Eina_List *phones;
      
      printf("-------------------------------------------------\n");
      printf("First Name: %s\n", person_first_name_get(person));
      printf("Last Name: %s\n", person_last_name_get(person));
      
      if ((addresses = person_addresses_list_get(person))) {
        Eina_List *address_iter;
        Address *address;
        int i = 0;
        
        EINA_LIST_FOREACH(addresses, address_iter, address)
        {
          printf("Address %d: %s, %d; ZIP %s; State %d (%s), Type %d (%s)\n",
                 ++i,
                 address_street_get(address),
                 address_number_get(address),
                 address_zip_code_get(address),
                 address_state_get(address), address_state_repr_get(address),
                 address_type_get(address), address_type_repr_get(address));
        }
      }
      if ((phones = person_phones_list_get(person))) {
        Eina_List *phone_iter;
        Phone_Number *phone_number;
        int i = 0;
        
        EINA_LIST_FOREACH(phones, phone_iter, phone_number)
        {
          printf("Phone %d: %s (%s)\n",
                 ++i,
                 phone_number_number_get(phone_number),
                 phone_number_type_repr_get(phone_number));
        }
      }
    }
  } else {
    printf("invalid option: %s\n", argv[1]);
    goto bail_out;
  }
  
free_book:
  book_free(book);
  
bail_out:
  phonebook_shutdown();
  eina_shutdown();
  return 0;
}
