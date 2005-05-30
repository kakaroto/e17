
#include "linux_2.6.h"

static int __monitor_cpu_count = -1;

int
count_cpus(void)
{
  char tmp[4];
  FILE     *stat;
  int       cpu = -1;
	
  if ( !(stat = fopen ("/proc/stat", "r")) )
    return -1;

  while (fscanf (stat, "cp%s %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu\n", &tmp) == 1)
    {
	cpu++;
    }
  fclose (stat);

  __monitor_cpu_count = cpu;
  return cpu;
}

int
four_cpu_numbers(int *uret, int *nret, int *sret, int *iret)
{
  static unsigned long old_u, old_n, old_s, old_i, old_wa, old_hi, old_si;
  unsigned long new_u, new_n, new_s, new_i, 
    new_wa = 0, new_hi = 0, new_si = 0, ticks_past; /* avoid div 0 */
  int tmp_u, tmp_n, tmp_s, tmp_i;
  char      dummy[16];
  FILE     *stat;
  
  if ( !(stat = fopen ("/proc/stat", "r")) )
    return -1;

  if (fscanf (stat, "%s %lu %lu %lu %lu %lu %lu %lu", dummy,
	      &new_u, &new_n, &new_s, &new_i, &new_wa, &new_hi, &new_si) < 5)
    {
      fclose (stat);
      return -1;
    }
  
  fclose (stat);
  
  ticks_past = ((new_u + new_n + new_s + new_i + new_wa + new_hi + new_si) -
		(old_u + old_n + old_s + old_i + old_wa + old_hi + old_si));

  if (ticks_past) 
    {
      tmp_u = ((new_u - old_u));
      tmp_n = ((new_n - old_n));
      tmp_s = ((new_s - old_s));
      tmp_i = ((new_i - old_i));
    }
  else 
    {
      tmp_u = 0;
      tmp_n = 0;
      tmp_s = 0;
      tmp_i = 0;
    }
  
  if (uret) *uret = tmp_u;
  if (nret) *nret = tmp_n;
  if (sret) *sret = tmp_s;
  if (iret) *iret = tmp_i;
  
  old_u  = new_u;
  old_n  = new_n;
  old_s  = new_s;
  old_i  = new_i;
  old_wa = new_wa;
  old_hi = new_hi;
  old_si = new_si;

  return 0;
}

int
cpu_usage_get(void)
{
  static int old_load = 0;
  int u, n, s, i, load;

  if ( four_cpu_numbers( &u, &n, &s, &i ) == -1 )
    return -1;
  if (__monitor_cpu_count == -1)
    count_cpus();

  load = u + n + s;
  return load / __monitor_cpu_count;
}

char *net_dev = "eth0";
long  max_in_bytes  = 171008;
long  max_out_bytes = 28672;
long  bytes_in  = 0;
long  bytes_out = 0;
int   in_usage = 0;
int   out_usage = 0;


int
get_net_input_output(unsigned long *in, unsigned long *out, const char *dev)
{
  static unsigned long      old_in = 0, old_out = 0;
  static double             top_in = 10, top_out = 10;

  double         tmp_in = 0, tmp_out = 0;
  unsigned long  new_in, new_out;
  unsigned long  dummy;
  char           iface[64];
  char           buf[256];
  FILE          *stat;
  int            found_dev = 0;

  stat = fopen ("/proc/net/dev", "r");
  if (!stat) return -1;
  
  while (fgets (buf, 256, stat)) 
    {
      int i = 0;
    
      /* remove : */
      for(; buf[i] != 0; i++) 
	if(buf[i] == ':')buf[i] = ' ';

      if (sscanf (buf, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu "
		  "%lu %lu %lu %lu\n", iface, &new_in, &dummy, &dummy,
		  &dummy, &dummy, &dummy, &dummy, &dummy, &new_out, &dummy,
		  &dummy, &dummy, &dummy, &dummy, &dummy, &dummy) < 17)
	continue;
    
      if (!strcmp(iface, dev))
	{
	  found_dev = 1;
	  break;
	}
      else found_dev = 0;
    }
  fclose (stat);

  if (!found_dev)
    {
      *in  = 0;
      *out = 0;
      return -1;
    }
  
  *in = new_in, *out = new_out;
  return 1;
}

void
net_usage_get(void)
{
  static unsigned long old_r_total = 0; 
  static unsigned long old_t_total = 0;
  
  unsigned long r_total = 0, t_total = 0;

  if ((get_net_input_output( &r_total, &t_total, net_dev)) == -1)
    {  
      in_usage = -1;
      out_usage = -1;
      return;
    }

  if ( old_r_total && old_t_total )
    {
      bytes_in  = r_total - old_r_total;
      bytes_out = t_total - old_t_total;

      in_usage =  (int)(((bytes_in * 100L)/max_in_bytes));
      if (in_usage > 100) in_usage = 100;
      out_usage = (int)(((bytes_out * 100L)/max_out_bytes));
      if (out_usage > 100) out_usage = 100;
    }
  else
    {
      bytes_in  = 0;
      bytes_out = 0;
      in_usage  = 0;
      out_usage = 0;
    }

  old_r_total = r_total;
  old_t_total = t_total;
}

long 
net_bytes_in_get(void)
{
  return bytes_in;
}

long
net_bytes_out_get(void)
{
  return bytes_out;
}

int
net_in_usage_get(void)
{
  net_usage_get();
  return in_usage;
}

int
net_out_usage_get(void)
{
  return out_usage;
}
