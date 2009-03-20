#ifndef FORMAT_A8_H_
#define FORMAT_A8_H_

static inline void a8_data_increment(Enesim_Surface_Data *d, unsigned int len)
{
	d->plane0 += len;
}


#endif /* FORMAT_A8_H_ */
