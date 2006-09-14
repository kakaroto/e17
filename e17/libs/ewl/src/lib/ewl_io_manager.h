#ifndef EWL_IO_MANAGER_H
#define EWL_IO_MANAGER_H

typedef struct Ewl_IO_Manager_Plugin Ewl_IO_Manager_Plugin;
struct Ewl_IO_Manager_Plugin
{
	void *handle;
	Ewl_Widget *(*read)(const char *uri);
	void (*write)(Ewl_Widget *data, const char *uri);
};

int		 ewl_io_manager_init(void);
void		 ewl_io_manager_shutdown(void);

const char 	*ewl_io_manager_extension_icon_name_get(const char *ext);
const char 	*ewl_io_manager_mime_type_icon_name_get(const char *mime);

const char 	*ewl_io_manager_uri_mime_type_get(const char *uri);

Ewl_Widget 	*ewl_io_manager_uri_read(const char *uri);
void		 ewl_io_manager_uri_write(Ewl_Widget *data, const char *uri, 
							const char *mime);

#endif

