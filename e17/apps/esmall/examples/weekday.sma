/* illustration of Zeller's congruence algorithm to calculate the
 * day of the week given a date */
#include <console>

weekday(day, month, year)
    {
    if (month <= 2)
        month += 12, --year;
    new j = year % 100;
    new e = year / 100;
    return (day + (month+1)*26/10 + j + j/4 + e/4 - 2*e) % 7;
    }

readdate(&day, &month, &year)
    {
    print("Give a date (dd-mm-yyyy): ");
    day = getvalue(_,'-','/');
    month = getvalue(_,'-','/');
    year = getvalue();
    }

main()
    {
    new day, month, year;
    readdate(day, month, year);

    new wkday = weekday(day, month, year);
    printf("The date %d-%d-%d falls on a ", day, month, year);
    switch (wkday)
        {
        case 0:
            print("Saturday");
        case 1:
            print("Sunday");
        case 2:
            print("Monday");
        case 3:
            print("Tuesday");
        case 4:
            print("Wednesday");
        case 5:
            print("Thursday");
        case 6:
            print("Friday");
        }
    print("^n");
    }
