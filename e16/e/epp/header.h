/* 
 * declares.h
 * This is a file with some declarations in it to get everyone to shut up :)
 * --Mandrake
 */

#include "cpplib.h"

#ifndef HOST_BITS_PER_WIDE_INT

#if HOST_BITS_PER_LONG > HOST_BITS_PER_INT
#define HOST_BITS_PER_WIDE_INT HOST_BITS_PER_LONG
#define HOST_WIDE_INT long
#else
#define HOST_BITS_PER_WIDE_INT HOST_BITS_PER_INT
#define HOST_WIDE_INT int
#endif

#endif

struct directive
{
   int                 length;
   int                 (*func) ();
   char               *name;
   enum node_type      type;
   char                command_reads_line;
   char                traditional_comments;
   char                pass_thru;
};

void               *xmalloc(unsigned size);
void               *xrealloc(void *old, unsigned size);
void               *xcalloc(unsigned number, unsigned size);
void                cpp_print_containing_files(cpp_reader * pfile);
void                cpp_file_line_for_message(cpp_reader * pfile,
					      char *filename, int line,

					      int column);
void                cpp_message(cpp_reader * pfile, int is_error, char *msg,
				char *arg1, char *arg2, char *arg3);
void                cpp_pfatal_with_name(cpp_reader * pfile, char *name);
struct operation    parse_number(cpp_reader * pfile, char *start, int olen);
struct operation    cpp_lex(cpp_reader * pfile);
int                 cpp_parse_escape(cpp_reader * pfile, char **string_ptr);
HOST_WIDE_INT       cpp_parse_expr(cpp_reader * pfile);
void                cpp_grow_buffer(cpp_reader * pfile, long n);
void                cpp_define(cpp_reader * pfile, unsigned char *str);
void                init_parse_options(struct cpp_options *opts);
enum cpp_token      null_underflow(cpp_reader * pfile);
int                 null_cleanup(cpp_buffer * pbuf, cpp_reader * pfile);
int                 macro_cleanup(cpp_buffer * pbuf, cpp_reader * pfile);
int                 file_cleanup(cpp_buffer * pbuf, cpp_reader * pfile);
void                cpp_skip_hspace(cpp_reader * pfile);
void                copy_rest_of_line(cpp_reader * pfile);
void                skip_rest_of_line(cpp_reader * pfile);
int                 handle_directive(cpp_reader * pfile);
cpp_buffer         *cpp_push_buffer(cpp_reader * pfile, unsigned char *buffer,

				    long length);
cpp_buffer         *cpp_pop_buffer(cpp_reader * pfile);
void                cpp_scan_buffer(cpp_reader * pfile);
void                cpp_buf_line_and_col(cpp_buffer * pbuf, long *linep,

					 long *colp);
int                 hashf(const unsigned char *name, int len, int hashsize);
void                cpp_hash_cleanup(cpp_reader * pfile);
int                 cpp_read_check(cpp_reader * pfile);
int                 parse_name(cpp_reader * pfile, int c);
void                init_parse_file(cpp_reader * pfile);
void                init_parse_options(struct cpp_options *opts);
int                 push_parse_file(cpp_reader * pfile, char *fname);
void                cpp_finish(cpp_reader * pfile);
cpp_buffer         *cpp_file_buffer(cpp_reader * pfile);
int                 cpp_read_check_assertion(cpp_reader * pfile);
void                fancy_abort(void);
enum cpp_token      cpp_get_token(cpp_reader * pfile);
enum cpp_token      cpp_get_non_space_token(cpp_reader * pfile);
int                 cpp_handle_options(cpp_reader * pfile, int argc,

				       char **argv);
void                cpp_print_file_and_line(cpp_reader * pfile);
