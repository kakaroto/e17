#include <Eet.h>
#include <Eina.h>

#include <sys/stat.h>
#include <sys/fcntl.h>

static void
invert_fputs(void *data, const char *str)
{
  fputs(str, data);
}

static Eina_Bool
config_save(Eet_Data_Descriptor *descriptor, const char *filename, const char *section, const void *data)
{
  Eet_File *ef;
  FILE *f;
  char buffer[PATH_MAX];
  int i;

  /* human readable version */
  snprintf(buffer, PATH_MAX, "%s.%s", filename, section);
  f = fopen(buffer, "w");
  if (!f) {
    fprintf(stderr, "Not able to open human readable config file.\n");
    goto on_error;
  }

  /* save the configuration */
  snprintf(buffer, PATH_MAX, "%s.eet", filename);
  ef = eet_open(buffer, EET_FILE_MODE_READ_WRITE);
  if (!ef) {
    fprintf(stderr, "Not able to open config file for read write mode.\n");
    goto on_error;
  }
  
  if (!eet_data_write(ef, descriptor, section, data, 1)) {
    fprintf(stderr, "Can't write data inside the config file.\n");
    goto on_write_error;
  }

  if (!eet_data_dump(ef, section, invert_fputs, f)) {
    fprintf(stderr, "Can't dump section `%s`.\n");
    goto on_write_error;
  }

  eet_close(ef);
  sleep(2);
  fclose(f);

  return EINA_TRUE;

 on_write_error:
  eet_close(ef);

  snprintf(buffer, PATH_MAX, "%s.eet", filename);
  unlink(buffer);

 on_error:
  if (f)
    {
      fclose(f);

      snprintf(buffer, PATH_MAX, "%s.%s", filename, section);
      unlink(buffer);
    }

  return EINA_FALSE;
}

static void*
config_load(Eet_Data_Descriptor *descriptor, const char *filename, const char *section)
{
  Eet_File *ef;
  void *result;
  char buffer[PATH_MAX];
  struct stat st_readable;
  struct stat st_binary;
  Eina_Bool stre;
  Eina_Bool stbe;
  Eina_Bool take_binary = EINA_TRUE;

  snprintf(buffer, PATH_MAX, "%s.eet", filename);
  stbe = !!stat(buffer, &st_binary);

  snprintf(buffer, PATH_MAX, "%s.%s", filename, section);
  stre = !!stat(buffer, &st_readable);

  if (stbe && stre) return NULL;

  if (!stre && !stbe && st_readable.st_mtime > st_binary.st_mtime)
    take_binary = EINA_FALSE;

  if (stbe) take_binary = EINA_FALSE;
  if (stre) take_binary = EINA_TRUE;
  
  if (take_binary)
    {
      fprintf(stderr, "Taking binary config\n");

      snprintf(buffer, PATH_MAX, "%s.eet", filename);
      ef = eet_open(buffer, EET_FILE_MODE_READ);
      result = eet_data_read(ef, descriptor, section);
      eet_close(ef);
    }
  else
    {
      char *tmp;
      struct stat st;
      int fd;

      fprintf(stderr, "Taking human readable config\n");

      snprintf(buffer, PATH_MAX, "%s.%s", filename, section);
      fd = open(filename, O_RDONLY);
      
      tmp = malloc(st_readable.st_size + 1);
      read(fd, tmp, st_readable.st_size);
      tmp[st_readable.st_size] = '\0';
      close(fd);

      snprintf(buffer, PATH_MAX, "%s.eet", filename);
      ef = eet_open(buffer, EET_FILE_MODE_READ_WRITE);
      eet_data_undump(ef, section, tmp, st_readable.st_size, 1);
      result = eet_data_read(ef, descriptor, section);
      
      eet_close(ef);
    }

  return result;
}

typedef struct _Simple Simple;
struct _Simple
{
  unsigned int count;
  short value;
  unsigned char flags;
};

static Eet_Data_Descriptor *simple_edd(void)
{
  Eet_Data_Descriptor_Class eddc;
  Eet_Data_Descriptor *result;

  EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Simple);
  result = eet_data_descriptor_file_new(&eddc);
  if (!result) return NULL;

  EET_DATA_DESCRIPTOR_ADD_BASIC(result, Simple, "count", count, EET_T_UINT);
  EET_DATA_DESCRIPTOR_ADD_BASIC(result, Simple, "value", value, EET_T_SHORT);
  EET_DATA_DESCRIPTOR_ADD_BASIC(result, Simple, "flags", flags, EET_T_UCHAR);
  
  return result;  
}

typedef struct _Floattant Flottant;
struct _Floattant
{
  double x, y;
};

typedef struct _Virgule_Fixe Virgule_Fixe;
struct _Virgule_Fixe
{
  Eina_F16p16 x, y;
};

static Eet_Data_Descriptor *flottant_edd(void)
{
  Eet_Data_Descriptor_Class eddc;
  Eet_Data_Descriptor *result;

  eet_eina_file_data_descriptor_class_set(&eddc, sizeof (eddc), "Reel", sizeof(Flottant));
  result = eet_data_descriptor_file_new(&eddc);
  if (!result) return NULL;

  EET_DATA_DESCRIPTOR_ADD_BASIC(result, Flottant, "x", x, EET_T_DOUBLE);
  EET_DATA_DESCRIPTOR_ADD_BASIC(result, Flottant, "y", y, EET_T_DOUBLE);
  
  return result;  
}

static Eet_Data_Descriptor *virgule_fixe_edd(void)
{
  Eet_Data_Descriptor_Class eddc;
  Eet_Data_Descriptor *result;

  eet_eina_file_data_descriptor_class_set(&eddc, sizeof (eddc), "Reel", sizeof(Virgule_Fixe));
  result = eet_data_descriptor_file_new(&eddc);
  if (!result) return NULL;

  EET_DATA_DESCRIPTOR_ADD_BASIC(result, Virgule_Fixe, "x", x, EET_T_F16P16);
  EET_DATA_DESCRIPTOR_ADD_BASIC(result, Virgule_Fixe, "y", y, EET_T_F16P16);
  
  return result;  
}

int
main(int argc, char **argv)
{
  Eet_Data_Descriptor *sedd;
  Eet_Data_Descriptor *dedd;
  Simple s = { .count = 5, .value = 98, .flags = 0x5 };
  Flottant f = { .x = 3.14159264, .y = 42.0 };
  Simple *rs;
  Flottant *rf;

  eina_init();
  eet_init();

  sedd = simple_edd();
  dedd = flottant_edd();

  config_save(sedd, "config", "simple", &s);
  config_save(dedd, "config", "flottant", &f);

  rs = config_load(sedd, "config", "simple");
  rf = config_load(dedd, "config", "flottant");

  if (!rs) fprintf(stderr, "Simple structure is empty !\n");
  else fprintf(stderr, "Simpe = { .count = %i, .value = %i, .flags = %x }\n", 
	       rs->count, rs->value, rs->flags);

  if (!rf) fprintf(stderr, "Flottant is empty\n");
  else fprintf(stderr, "Flottant = { .x = %f, .y = %f }\n", rf->x, rf->y);

  eet_data_descriptor_free(sedd);
  eet_data_descriptor_free(dedd);

  eet_shutdown();

  return 0;
}
