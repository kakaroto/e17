#ifndef UPLOAD_H_
#define UPLOAD_H_

typedef struct Upload Upload;

struct Upload
{
   Evas_Object *main;
   Evas_Object *pb;
   Evas_Object *lbl;
};

/* Upload manager */
Upload *
upload_new(Evas_Object *parent);
void
upload_free(Upload **_ul);
void
upload_add(Upload *ul, Enlil_Photo *photo);
void
upload_album_create_add(Upload *ul, Enlil_Album *album);

#endif /* UPLOAD_H_ */
