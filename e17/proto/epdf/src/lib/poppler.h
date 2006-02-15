#ifndef __POPPLER_H__
#define __POPPLER_H__


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return the version of poppler.
 * @return the version of poppler, as a string.
 *
 * This function returns the version of poppler as a string. The
 * result must not be freed.
 */
const char *evas_poppler_version_get (void);


#ifdef __cplusplus
}
#endif


#endif /* __POPPLER_H__ */
