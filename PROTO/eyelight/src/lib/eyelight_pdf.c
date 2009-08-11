/*
 * =====================================================================================
 *
 *       Filename:  eyelight_pdf.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/05/2009 09:21:44 AM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */


#include "eyelight_pdf.h"
#include "../../config.h"

#ifdef PDF_SUPPORT
#include "hpdf.h"

void eyelight_pdf_generate(Eyelight_Viewer *pres, char* file);

int eyelight_pdf_generate_start_timer(void *data)
{
        Eyelight_Viewer* pres = data;
        eyelight_pdf_generate(pres,pres->pdf_file);

        ecore_main_loop_quit();
}

void eyelight_pdf_error_handler(HPDF_STATUS   error_no,
                HPDF_STATUS   detail_no,
                void         *user_data)
{
        printf ("PDF ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
                        (HPDF_UINT)detail_no);
}

void eyelight_pdf_generate(Eyelight_Viewer *pres, char* file)
{
        HPDF_Doc  pdf;
        HPDF_Font font;
        HPDF_Page page;
        char fname[256];
        HPDF_Destination dst;
        HPDF_Image image;
        HPDF_Image image1;
        HPDF_Image image2;
        HPDF_Image image3;

        double x;
        double y;
        double angle;
        double angle1;
        double angle2;
        double rad;
        double rad1;
        double rad2;

        double iw;
        double ih;


        pdf = HPDF_New (eyelight_pdf_error_handler, NULL);

        font = HPDF_GetFont (pdf, "Helvetica", NULL);
        HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);

        //add slide into the pdf
        int i;
        for(i=0; i<eyelight_viewer_size_get(pres);i++)
        {
                printf("Generate slide %d\n",i+1);
                const Eyelight_Thumb* thumb = eyelight_viewer_thumbnails_custom_size_get(pres, i, pres->default_size_w, pres->default_size_h);

                Evas_Object *o_image = evas_object_image_add(pres->evas);
                evas_object_color_set(o_image,255,255,255,255);
                evas_object_image_fill_set(o_image,0,0,thumb->w,thumb->h);
                evas_object_image_size_set(o_image, thumb->w, thumb->h);
                evas_object_image_data_set(o_image,thumb->thumb);
                evas_object_show(o_image);

                evas_object_image_save(o_image,"/tmp/eyelight_thumb.png",NULL,NULL);

                image = HPDF_LoadPngImageFromFile (pdf, "/tmp/eyelight_thumb.png");
                iw = HPDF_Image_GetWidth (image);
                ih = HPDF_Image_GetHeight (image);

                page = HPDF_AddPage (pdf);

                HPDF_Page_SetWidth (page, iw);
                HPDF_Page_SetHeight (page, ih);

                dst = HPDF_Page_CreateDestination (page);
                HPDF_Destination_SetXYZ (dst, 0, HPDF_Page_GetHeight (page), 1);
                HPDF_SetOpenAction(pdf, dst);

                y = HPDF_Page_GetHeight (page) - 20;

                /* Draw image to the canvas. (normal-mode with actual size.)*/
                HPDF_Page_DrawImage (page, image, 0, 0, iw, ih);

                evas_object_del(o_image);
        }
        HPDF_SaveToFile (pdf, file);

        /* clean up */
        HPDF_Free (pdf);
}

#else
int eyelight_pdf_generate_start_timer(void *data)
{
    printf("No pdf support ! \n");
}

#endif

