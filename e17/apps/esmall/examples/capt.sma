#include <core>
#include <console>

bool: ispacked(string[])
    return bool: (string[0] > charmax);

my_strlen(string[])
    {
    new len = 0;
    if (ispacked(string))
        while (string{len} != '^0')     /* get character from pack */
            ++len;
    else
        while (string[len] != '^0')     /* get cell */
            ++len;
    return len;
    }

strupper(string[])
    {
    assert ispacked(string);

    for (new i=0; string{i} != '^0'; ++i)
        string{i} = toupper(string{i});
    }

main()
    {
    new s[10];

    for (new i=0; i<5; i++)
        s{i}=i+'a';
    s{5}='^0';

    printf("String is %s^n", ispacked(s) ? "packed" : "unpacked");
    printf("String length is %d^n", my_strlen(s));
    printf("Original:   %s^n", s);
    strupper(s);
    printf("Upper case: %s^n", s);
    }
