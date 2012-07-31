#include <ctype.h>

#include <Eina.h>
#include <GeoIP.h>

#define VIGRID_NUMBER_SEARCH "0123456789"

typedef struct _Community_ID Community_ID;
typedef struct _Community_Day Community_Day;
typedef struct _Community_Access Community_Access;
typedef struct _Community_Country Community_Country;

struct _Community_ID
{
   const char *uid;

   Eina_List *access;
};

struct _Community_Day
{
   unsigned short year;
   unsigned char month;
   unsigned char day;

   Eina_Hash *hours;
};

struct _Community_Access
{
   Community_ID *id;
   Community_Day *day;

   unsigned char hour;
   unsigned char minute;   
   unsigned char seconde;

   const char *ip;
   const char *country;
};

struct _Community_Country
{
   const char *tld;
   const char *country;
   long long int count;
   long long int population;

   Eina_Hash *access;
};

static Eina_Hash *community = NULL;
static Eina_Hash *days = NULL;
static Eina_Hash *countries = NULL;

static long long int months[12];
static const char *MONTH_STRING[12] = {
  "January", "February", "March", "April", "May", "June",
  "July", "August", "September", "October", "November", "December"
};

static int
_country_sort_cb(const void *a, const void *b)
{
   const Community_Country *ca = a;
   const Community_Country *cb = b;

   return ca->count - cb->count;
}

static unsigned int
_community_day_key_length(const void *key)
{
   return sizeof (Community_Day);
}

static int
_community_day_key_cmp(const void *key1, int key1_length, const void *key2, int key2_length)
{
   const Community_Day *d1 = key1;
   const Community_Day *d2 = key2;

   if (d1->year != d2->year) return d1->year - d2->year;
   if (d1->month != d2->month) return d1->month - d2->month;
   return d1->day - d2->day;
}

static int
_community_day_key_hash(const void *key, int key_length)
{
   const Community_Day *d1 = key;
   int day;

   day = d1->day + d1->month * 31 + d1->year * 31 * 12;
   return eina_hash_int32(&day, 4);
}

static long long int
_eina_file_int_get(Eina_File_Line *line, unsigned long long *offset)
{
   long long int n = 0;
   const char *s;

   for (s = line->start + *offset; s < line->end; s++)
     {
        const char *convert = VIGRID_NUMBER_SEARCH;
        const char *r;

        r = strchr(convert, *s);
        if (!r)
          {
             *offset = s - line->start;
             return n;
          }
        n = n * 10 + (r - convert);
     }

   *offset = line->length;
   return n;
}

static const char *
_eina_file_filter_get(Eina_File_Line *line, unsigned long long *offset, const char *filter)
{
   unsigned long long start = *offset;
   const char *s;

   for (s = line->start + *offset; s < line->end; s++)
     {
        const char *r;

        r = strchr(filter, *s);
        if (!r) break ;
     }

   *offset = s - line->start;

   return eina_stringshare_add_length(line->start + start, *offset - start);
}

static const char *
_eina_file_ip_get(Eina_File_Line *line, unsigned long long *offset)
{
   return _eina_file_filter_get(line, offset, VIGRID_NUMBER_SEARCH".");
}

static const char *
_eina_file_hexa_get(Eina_File_Line *line, unsigned long long *offset)
{
   return _eina_file_filter_get(line, offset, VIGRID_NUMBER_SEARCH"abcdef");
}

static const char *
_eina_stringshare_up(const char *start, int length)
{
   char *tmp;
   char *r;
   int i = 0;

   r = alloca(length);
   for (tmp = r; i < length; ++i, ++tmp, ++start)
     *tmp = toupper(*start);
   *tmp = 0;

   return eina_stringshare_add(r);
}

int
main(int argc, char **argv)
{
   Eina_File *f;
   Eina_Iterator *it;
   Eina_File_Line *l;
   GeoIP *geo;
   Community_Country *country;
   unsigned long long correct = 0;
   unsigned long long lines = 0;
   int i;

   if (argc != 2) return -1;

   eina_init();

   community = eina_hash_stringshared_new(NULL);
   days = eina_hash_new(_community_day_key_length,
                             _community_day_key_cmp,
                             _community_day_key_hash,
                             NULL, 5);
   countries = eina_hash_string_superfast_new(NULL);
   geo = GeoIP_new(GEOIP_STANDARD);
   memset(months, 0, sizeof (months));

   /* Read real name country to tld file */
   f = eina_file_open("country_tld.csv", EINA_FALSE);
   if (!f) return -1;

   it = eina_file_map_lines(f);
   EINA_ITERATOR_FOREACH(it, l)
     {
        const char *s;
        int i;

        s = memchr(l->start, ',', l->length);
        if (!s) continue ;

        country = calloc(1, sizeof (Community_Country));
        country->tld = _eina_stringshare_up(l->start, s - l->start);
        country->country = _eina_stringshare_up(s + 1, l->length - (s - l->start + 1));
        country->access = eina_hash_stringshared_new(NULL);

        eina_hash_direct_add(countries, country->tld, country);
     }
   eina_iterator_free(it);
   eina_file_close(f);

   /* Read population information */
   f = eina_file_open("country_population.csv", EINA_FALSE);
   if (!f) return -1;

   it = eina_file_map_lines(f);
   EINA_ITERATOR_FOREACH(it, l)
     {
        Eina_Iterator *it2;
        const char *s;
        const char *country_name;
        const char *r;

        for (s = l->start; s < l->end; s++)
          {
             const char *convert = VIGRID_NUMBER_SEARCH;

             r = strchr(convert, *s);
             if (r) break ;
          }

        if (!r) continue;

        country = NULL;
        country_name = _eina_stringshare_up(l->start, s - l->start - 1);

        it2 = eina_hash_iterator_data_new(countries);
        EINA_ITERATOR_FOREACH(it2, country)
          {
             if (country->country == country_name)
               {
                  unsigned long long offset = s - l->start;

                  country->population = _eina_file_int_get(l, &offset);
                  break;
               }
          }
        eina_iterator_free(it2);
     }
   eina_iterator_free(it);
   eina_file_close(f);

   /* Read the log */
   f = eina_file_open(argv[1], EINA_FALSE);
   if (!f) return -1;

   it = eina_file_map_lines(f);
   EINA_ITERATOR_FOREACH(it, l)
     {
        Community_ID *cid;
        Community_Day *cday;
        Community_Day tday;
        Community_Access *caccess;
        Eina_List *events;
        unsigned long long offset = 0;
        long long int year, month, day, hour, min, sec;
        int at;
        const char *ip;
        const char *id;

        lines++;

        year = _eina_file_int_get(l, &offset);
        if (offset == l->length || l->start[offset] != '/') continue ;
        offset++;

        month = _eina_file_int_get(l, &offset);
        if (offset == l->length || l->start[offset] != '/') continue ;
        offset++;

        day = _eina_file_int_get(l, &offset);
        if (offset == l->length || l->start[offset] != '-') continue ;
        offset++;

        hour = _eina_file_int_get(l, &offset);
        if (offset == l->length || l->start[offset] != ':') continue ;
        offset++;

        min = _eina_file_int_get(l, &offset);
        if (offset == l->length || l->start[offset] != ':') continue ;
        offset++;

        sec = _eina_file_int_get(l, &offset);
        if (offset == l->length || l->start[offset] != ' ') continue ;
        offset++;

        ip = _eina_file_ip_get(l, &offset);
        if (offset == l->length || l->start[offset] != ' ') continue ;
        offset++;

        id = _eina_file_hexa_get(l, &offset);
        if (offset != l->length) continue ;

        if (year < 2012 || year > 2012 ||
            month < 0 || month > 12 ||
            day < 0 || day > 31 ||
            hour < 0 || hour > 24 ||
            min < 0 || min > 60 ||
            sec < 0 || sec > 60)
          continue ;

        cid = eina_hash_find(community, id);
        if (!cid)
          {
             cid = calloc(1, sizeof (Community_ID));
             cid->uid = eina_stringshare_ref(id);
             eina_hash_direct_add(community, id, cid);
          }

        tday.year = year;
        tday.month = month;
        tday.day = day;
        tday.hours = NULL;

        cday = eina_hash_find(days, &tday);
        if (!cday)
          {
             cday = calloc(1, sizeof (Community_Day));
             *cday = tday;
             cday->hours = eina_hash_int32_new(NULL);
             eina_hash_add(days, &tday, cday);
          }

        caccess = calloc(1, sizeof (Community_Access));
        caccess->id = cid;
        caccess->day = cday;
        caccess->hour = hour;
        caccess->minute = min;
        caccess->seconde = sec;
        caccess->ip = eina_stringshare_ref(ip);
        caccess->country = GeoIP_country_code_by_addr(geo, ip);
        if (!caccess->country) caccess->country = "??";

        country = eina_hash_find(countries, caccess->country);
        if (!country)
          {
             fprintf(stderr, "TLD: '%s' not found\n", caccess->country);
             country = calloc(1, sizeof (Community_Country));
             country->tld = caccess->country;
             country->access = eina_hash_stringshared_new(NULL);

             eina_hash_direct_add(countries, country->tld, country);
          }

        {
           Community_ID *tid;

           tid = eina_hash_find(country->access, id);
           if (!tid)
             {
                eina_hash_direct_add(country->access, id, cid);
                country->count++;
             }
        }

        /* First connection of the month ? */
        {
           Eina_List *l = eina_list_last(cid->access);
           Community_Access *tmp = eina_list_data_get(l);

           if (!tmp || tmp->day->month != month)
             months[month - 1]++;
        }

        cid->access = eina_list_append(cid->access, caccess);

        at = hour * 60 + min;
        events = eina_hash_find(cday->hours, &at);
        events = eina_list_append(events, caccess);
        eina_hash_set(cday->hours, &at, events);

        correct++;

        eina_stringshare_del(ip);
        eina_stringshare_del(id);
     }
   eina_iterator_free(it);
   eina_file_close(f);

   fprintf(stderr, "Total line: %lli\nCorrect line: %lli\n", lines, correct);
   fprintf(stderr, "Watched day: %i and %i uniq users showed during that time.\n",
           eina_hash_population(days), eina_hash_population(community));
   fprintf(stderr, "Connection per month :\n");
   for (i = 0; i < 12; i++)
     if (months[i])
       fprintf(stderr, "%lli connections in %s.\n", months[i], MONTH_STRING[i]);

   {
      Eina_List *sorted = NULL;
      FILE *fp;

      it = eina_hash_iterator_data_new(countries);
      EINA_ITERATOR_FOREACH(it, country)
        sorted = eina_list_append(sorted, country);
      eina_iterator_free(it);

      sorted = eina_list_sort(sorted, -1, _country_sort_cb);

      fp = fopen("country_map.html", "w");
      fprintf(fp, "<html>\n\t<head>\n");
      fprintf(fp, "\t\t<script type='text/javascript' src='https://www.google.com/jsapi'></script>\n");
      fprintf(fp, "\t\t<script type='text/javascript'>\n");
      fprintf(fp, "\t\t\tgoogle.load('visualization', '1', {'packages': ['geochart']});\n");
      fprintf(fp, "\t\t\tgoogle.setOnLoadCallback(drawRegionsMap);\n\n");
      fprintf(fp, "\t\t\tfunction drawRegionsMap() {\n");
      fprintf(fp, "\t\t\t\tvar data = google.visualization.arrayToDataTable([\n");
      fprintf(fp, "\t\t\t\t\t['Country', 'Popularity']");

      EINA_LIST_FREE(sorted, country)
        if (strcmp("GL", country->tld))
          {
             if (country->population && country->count)
               {
                  fprintf(fp, ",\n\t\t\t\t\t['%s', %lli * 1000000 / %lli]",
                          country->tld, country->count, country->population);
               }
             else if (country->count)
               {
                  fprintf(stderr, "Unknown population for '%s' (%s) accounted %lli\n",
                          country->tld, country->country, country->count);
               }
          }

      fprintf(fp, "\n\t\t\t\t\t]);\n\n");
      fprintf(fp, "\t\t\t\t\tvar options = { colorAxis : { maxValue : 10 } };\n\n");
      fprintf(fp, "\t\t\t\t\tvar chart = new google.visualization.GeoChart(document.getElementById('chart_div'));\n");
      fprintf(fp, "\t\t\t\t\tchart.draw(data, options);\n");
      fprintf(fp, "\t\t\t};\n");
      fprintf(fp, "\t\t</script>\n");
      fprintf(fp, "\t</head>\n");
      fprintf(fp, "\t<body>\n");
      fprintf(fp, "\t\t<div id='chart_div' style='width: 900px; height: 500px;'></div>\n");
      fprintf(fp, "\t</body>\n</html>\n");

      fclose(fp);
   }

   eina_shutdown();

   return 0;
}
