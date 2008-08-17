/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_IO_MANAGER_H
#define EWL_IO_MANAGER_H

/**
 * @addtogroup Ewl_IO_Manager Ewl_IO_Manager: An input manager
 * @brief Defines a class for handling reading various input and creating the correct widgets
 *
 * @{
 */

/**
 * Ewl_IO_Manager_Plugin
 */
typedef struct Ewl_IO_Manager_Plugin Ewl_IO_Manager_Plugin;

/**
 * @brief Provides the base structure for IO Manager plugins
 */
struct Ewl_IO_Manager_Plugin
{
        Ecore_Plugin *plugin;                        /**< The libray handle */

        Ewl_Widget *(*uri_read)(const char *uri);        /**< The URI read fuction */
        Ewl_Widget *(*string_read)(const char *string); /**< The string read function */

        int (*uri_write)(Ewl_Widget *data, const char *uri); /**< The URI write function */
        int (*string_write)(Ewl_Widget *data, char **string); /**< The string write function */
};

int              ewl_io_manager_init(void);
void             ewl_io_manager_shutdown(void);

const char      *ewl_io_manager_extension_icon_name_get(const char *ext);
const char      *ewl_io_manager_mime_type_icon_name_get(const char *mime);

const char      *ewl_io_manager_uri_mime_type_get(const char *uri);

Ewl_Widget      *ewl_io_manager_uri_read(const char *uri);
Ewl_Widget      *ewl_io_manager_string_read(const char *string,
                                        const char *mime);

int              ewl_io_manager_uri_write(Ewl_Widget *data, const
                                        char *uri, const char *mime);
int              ewl_io_manager_string_write(Ewl_Widget *data,
                                        char **string, const char *mime);

/**
 * @}
 */

#endif

