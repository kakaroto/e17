#ifdef BUILD_EXIF_SUPPORT
#include "ephoto_exif.h"

Ecore_Hash *get_exif_data(const char *file) {
	const char **args, *title;
	char value[1024];
	unsigned int i, tag;
	Ecore_Hash *exif_info;
	ExifEntry *entry;
	ExifData *data;
	ExifLoader *loader;

	args = calloc(1, sizeof(const char *));
	args[0] = file;

	exif_info = ecore_hash_new(ecore_str_hash, ecore_str_compare);
	ecore_hash_free_key_cb_set(exif_info, free);
	ecore_hash_free_value_cb_set(exif_info, free);

	loader = exif_loader_new();
	exif_loader_write_file(loader, *args);

	data = exif_loader_get_data(loader);
	if (!data) {
		exif_loader_unref(loader);
		return NULL;
	}
	exif_loader_unref(loader);

	for(tag = 0; tag < 0xffff; tag++) {
		title = exif_tag_get_title(tag);
		for (i = 0; i < EXIF_IFD_COUNT; i++) {
			entry = exif_content_get_entry(data->ifd[i], tag);
			if (entry) {
				exif_entry_ref(entry);
				exif_entry_get_value(entry, value, sizeof(value));
				ecore_hash_set(exif_info, strdup(title),
							strdup(value));
				exif_entry_unref(entry);
			}
		}
	}
	exif_data_unref(data);
	
	return exif_info;
}
#endif
