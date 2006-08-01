#!/usr/sbin/dtrace -qs

pid$target:*ewl*::entry
{
  @count[probefunc] = count();
  self->ts = vtimestamp;
}

pid$target:*ewl*::return
/self->ts/
{
  @time[probefunc] = sum(vtimestamp - self->ts);
}

END
{
  trunc(@time, 10);

  printf("\nFunction calls");
  printa(@count);

  printf("\nSlowest 10 functions");
  printa(@time);
}

