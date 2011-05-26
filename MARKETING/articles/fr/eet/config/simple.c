#include <Eet.h>
#include <Eina.h>

typedef struct _Simple Simple;
struct _Simple
{
  unsigned int count;
  short value;
  unsigned char flags;
};

Eet_Data_Descriptor *simple_edd(void)
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

Eet_Data_Descriptor *flottant_edd(void)
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

Eet_Data_Descriptor *virgule_fixe_edd(void)
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

void basic_read(const char *file, Eet_Data_Descriptor *sedd, Eet_Data_Descriptor *vfedd)
{
  Eet_File *ef;
  Simple *s;
  Virgule_Fixe *vf;

  ef = eet_open(file, EET_FILE_MODE_READ);
  if (!ef) return ;

  s = eet_data_read(ef, sedd, "simple");
  vf = eet_data_read(ef, vfedd, "reel");
  
  printf("Simple = { .count = %d, .value = %hi, .flags = %hhi };\n", s->count, s->value, s->flags);
  printf("Virgule Fixe = { .x = %f, .y = %f };\n",
	 eina_f16p16_float_to(vf->x),
	 eina_f16p16_float_to(vf->y));

  eet_close(ef);
}

void basic_write(const char *file, Eet_Data_Descriptor *sedd, Eet_Data_Descriptor *dedd)
{
  Eet_File *ef;
  Simple s = { .count = 5, .value = 98, .flags = 0x5 };
  Flottant f = { .x = 3.14159264, .y = 42.0 };

  ef = eet_open(file, EET_FILE_MODE_WRITE);
  if (!ef) return ;

  eet_data_write(ef, sedd, "simple", &s, 1);
  eet_data_write(ef, dedd, "reel", &f, 1);

  eet_close(ef);
}

int main(int argc, char **argv)
{
  Eet_Data_Descriptor *sedd;
  Eet_Data_Descriptor *vfedd;
  Eet_Data_Descriptor *dedd;

  eet_init();

  sedd = simple_edd();
  vfedd = virgule_fixe_edd();
  dedd = flottant_edd();

  basic_write("/tmp/test.eet", sedd, dedd);
  basic_read("/tmp/test.eet", sedd, vfedd);

  eet_data_descriptor_free(sedd);
  eet_data_descriptor_free(vfedd);
  eet_data_descriptor_free(dedd);

  unlink("/tmp/test.eet");

  eet_shutdown();
}
