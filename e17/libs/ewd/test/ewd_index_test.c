#include <Ewd.h>
#include <stdio.h>

void
do_test(int index, int max_val, int loops)
{
  Ewd_DList *l;
  int i, j;

  printf("\n");

  printf("\tIndex = %d, Maximum value = %d\n", index, max_val);

  l = ewd_dlist_new();
  
  for (i = 1; i <= max_val; i++)
    ewd_dlist_append(l, (void *)i);

  for (i = 1; i <= loops; i++)
    {
      printf("\n\tLOOP %d:\n\n", i);

      if (!ewd_dlist_goto_index(l, index))
        {
          printf("\t  Error: Cannot go to index %d\n\n", index);
          continue;
        }
      printf("\t  Start: Index = %d\n\n", ewd_dlist_index(l));
      while ((j = (int) ewd_dlist_next(l)))
        printf("\t  Index %d: %d\n", ewd_dlist_index(l) - 1, j);

      printf("\n");
    }
}


int
main(int argc, char **argv)
{
  printf("  Test 1:\n");
  do_test(7, 10, 3);

  printf("  Test 2:\n");
  do_test(10, 10, 3);  

  printf("  Test 3:\n");
  do_test(0, 10, 3);

  printf("  Test 4:\n");
  do_test(11, 10, 3);

  return 0;
}
