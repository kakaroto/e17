#ifndef DOWNLOAD_H_
#define DOWNLOAD_H_

typedef struct Download Download;

struct Download
{
   Evas_Object *main;
   Evas_Object *pb;
   Evas_Object *lbl;
};

/* Download manager */
Download *
download_new(Evas_Object *parent);
void
download_free(Download **_ul);
void
download_add(Download *ul, const char *source, Enlil_Photo *photo);

#endif /* DOWNLOAD_H_ */
