/* Calculation of Fibonacci numbers by iteration */
#include <console>

fibonacci(n)
    {
    assert n>0;

    new a = 0, b = 1;
    for (new i = 2; i < n; i++)
        {
        new c = a + b;
        a = b;
        b = c;
        }
    return a + b;
    }

main()
    {
    print("Enter a value: ");
    new v = getvalue();
    printf("The value of Fibonacci number %d is %d^n",
           v, fibonacci(v) );
    }
