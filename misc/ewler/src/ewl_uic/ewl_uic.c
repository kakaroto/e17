#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <Ewl.h>

#include "widgets.h"
#include "form_file.h"
#include "config.h"

const char *usage_str =
"Usage: ewl_uic [options] file\n"
"Options:\n"
"  -h            Display this information\n"
"  -v            Verbose output\n"
"  -o <file>     Place the output into <file>\n"
"  -d [file]     Create a header file (default is <file prefix>.h)\n"
"\n";

void
print_usage( FILE *out )
{
	fprintf( out, usage_str );
}

int
main( int argc, char *argv[] )
{
	Ecore_List *info;
	int out_fd, in_fd, header_fd = 0;
	char *src_buf = NULL, *header_buf = NULL;
	char *filename_start, *filename_end;
	char *output_filename = NULL;
	char *output_header = NULL;
	char *input_filename;
	char *spec_filename;
	int verbose_mode = 0, create_header = 0;
	char *prefix;
	int prefix_len;
	int opt;

	while( (opt = getopt( argc, argv, "hvo:d::" )) > 0 ) {
		switch( opt ) {
			case 'v': verbose_mode = 1; break;
			case 'o': output_filename = optarg; break;
			case 'h': print_usage( stdout ); exit( EXIT_SUCCESS );
			case 'd': output_header = optarg; create_header = 1; break;
			case ':':
			case '?':
			default:
				print_usage( stderr ); exit( EXIT_FAILURE );
		}
	}

	if( (input_filename = argv[optind]) == NULL ) {
		fprintf( stderr, "ewl_uic: No input file\n" );
		exit( EXIT_FAILURE );
	}

	if( output_filename == NULL ) {
		char *index = strrchr( input_filename, '.' );
		int len;

		if( index == NULL )
			len = strlen( input_filename );
		else
			len = index - input_filename;

		src_buf = (char *) malloc( len + 3 );
		if( !src_buf ) {
			fprintf( stderr, "out of memory\n" );
			exit( EXIT_FAILURE );
		}

		strncpy( src_buf, input_filename, len );
		strcat( src_buf, ".c" );
		output_filename = src_buf;
	}

	if( create_header && output_header == NULL ) {
		char *index = strrchr( output_filename, '.' );
		int len;

		if( index == NULL )
			len = strlen( output_filename );
		else
			len = index - output_filename;

		header_buf = (char *) malloc( len + 3 );
		if( !header_buf ) {
			fprintf( stderr, "out of memory\n" );
			exit( EXIT_FAILURE );
		}

		strncpy( header_buf, output_filename, len );
		strcat( header_buf, ".h" );
		output_header = header_buf;
	}

	if( strcmp( input_filename, "-" ) ) {
		in_fd = open( input_filename, O_RDONLY );
		if( in_fd < 0 ) {
			perror( input_filename );
			exit( EXIT_FAILURE );
		}
	} else {
		in_fd = 0;
	}

	if( strchr( output_filename, '/' ) )
		filename_start = strrchr( output_filename, '/' ) + 1;
	else
		filename_start = output_filename;

	if( strchr( output_filename, '.' ) > filename_start )
		filename_end = strrchr( output_filename, '.' );
	else
		filename_end = output_filename + strlen( output_filename );

	prefix_len = filename_end - filename_start;

	prefix = (char *) malloc( prefix_len + 1 );
	strncpy( prefix, filename_start, prefix_len );

	remove( output_filename );
	out_fd = open( output_filename, O_WRONLY | O_CREAT, 0644 );
	if( out_fd < 0 ) {
		fprintf( stderr, "ewl_uic: cannot open output file " );
		perror( output_filename );
	}

	if( create_header ) {
		remove( output_header );
		header_fd = open( output_header, O_WRONLY | O_CREAT, 0644 );
		if( header_fd < 0 ) {
			fprintf( stderr, "ewl_uic: cannot open header file " );
			perror( output_header );
		}
	}

	if( src_buf ) {
		free( src_buf );
		output_filename = NULL;
	}

	if( header_buf ) {
		free( header_buf );
		output_header = NULL;
	}

	spec_filename = PACKAGE_DATA_DIR"/schemas/widgets.xml";

	widgets_load( spec_filename );

	/* add in user supplied specifications */
	
	info = form_file_read( in_fd );
	close( in_fd );

	generate_output( info, prefix, out_fd );

	close( out_fd );
	close( header_fd );

	return 0;
}
