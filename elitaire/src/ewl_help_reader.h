/* vim: set sw=4 ts=4 sts=4 expandtab: */

#ifdef __cplusplus
extern "C" {
#endif

void          help_reader_init(const char * basedir);
void          help_reader_shutdown(void);
void          help_reader_page_set(const char * page);

#ifdef __cplusplus
}
#endif

