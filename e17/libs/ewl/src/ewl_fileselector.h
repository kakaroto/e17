
#ifndef __EWL_FILESELECTOR_H__
#define __EWL_FILESELECTOR_H__


typedef struct _ewl_fileinfo Ewl_Fileinfo;

#define EWL_FILEINFO(fi) ((Ewl_Fileinfo *))
struct _ewl_fileinfo {
	char            name[PATH_MAX];	/* filename */
	char            path[PATH_MAX];	/* path to file */
	struct stat     status;	/* info from stat() */
};

typedef struct _ewl_dirinfo Ewl_Dirinfo;

#define EWL_DIRINFO(di) ((Ewl_Dirinfo *))
struct _ewl_dirinfo {
	char            name[PATH_MAX];	/* directory name */
	char            path[PATH_MAX];	/* path to directory */
	struct dirent  *dirEntry;
};


typedef struct _ewl_fileselector Ewl_Fileselector;

#define EWL_FILESELECTOR(fs) ((Ewl_Fileselector *) fs)
struct _ewl_fileselector {
	Ewl_Box         box;

	Ewl_Widget     *dbox;	/* vbox to display directories */
	Ewl_Widget     *fbox;	/* vbox to display files */

	Ewl_Widget     *dirs;	/* directory table */
	Ewl_Widget     *files;	/* file table */

	Ewl_Fileinfo   *f_info;
	Ewl_Dirinfo    *d_info;

	Ewl_Callback_Function file_clicked;
};


Ewl_Widget     *ewl_fileselector_new(Ewl_Callback_Function file_clicked);
void            ewl_fileselector_init(Ewl_Fileselector * fs,
				      Ewl_Callback_Function fc);

char           *ewl_fileselector_get_current(Ewl_Fileselector * fs);


#endif /* __EWL_FILESELECTOR_H__ */
