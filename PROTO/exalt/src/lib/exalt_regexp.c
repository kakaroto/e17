#include "./exalt_regexp.h"
#include "libexalt_private.h"





/**
 * @brief create a new regex
 * @param str_request the request string
 * @param str_regex the regular expression
 * @debug 1 if you want use the debug mode, else 0
 * @return Return a Exalt_Regex structure
 */
Exalt_Regex* exalt_regex_new(const char* str_request, const char* str_regex, short debug)
{
    Exalt_Regex *r = (Exalt_Regex*)malloc((unsigned int)sizeof(Exalt_Regex));

    r->str_request = NULL;
    r->str_regex = NULL;


    exalt_regex_set_request(r,str_request);
    exalt_regex_set_regex(r,str_regex);
    exalt_regex_set_debug(r,debug);

    r->nmatch = 0;
    r->res = NULL;

    return r;
}



/**
 * @brief change the request string
 * @param r the Exalt_Regex
 * @param str_request the new request string
 */
void exalt_regex_set_request(Exalt_Regex* r,const char* str_request)
{
    EXALT_FREE(r->str_request);
    r->str_request = strdup(str_request);
}



/**
 * @brief change the regular expression
 * @param r the Exalt_Regex
 * @param str_regex the new regular expression
 */
void exalt_regex_set_regex(Exalt_Regex* r,const char* str_regex)
{
    EXALT_FREE(r->str_regex);
    r->str_regex = strdup(str_regex);
}



/**
 * @brief set the debug mode
 * @param r the Exalt_Regex
 * @param debug the new debig mode, 0 or 1
 */
void exalt_regex_set_debug(Exalt_Regex *r, short debug)
{
    r->debug = debug;
}



/*
 * @brief clear the current result
 * @param r the Exalt_Regex
 */
void exalt_regex_clear_result(Exalt_Regex* r)
{
    if(r!= NULL && r->res!=NULL)
    {
        unsigned int i;
        for(i=0;i<r->nmatch;i++)
        {
	    if(r->res[i])
		free(r->res[i]);
        }
	free(r->res);
	r->res=NULL;
	r->nmatch = 0;
    }
}



/*
 * @brief free tje regex
 * @param r the Exalt_Regex
 */
void exalt_regex_free(Exalt_Regex **r)
{
    if(r!=NULL && *r!=NULL)
    {
	Exalt_Regex* r2 = *r;
	exalt_regex_clear_result(r2);
	if(r2->str_request)
	    free(r2->str_request);
	if(r2->str_regex)
	    free(r2->str_regex);

	free(r2);
	r=NULL;
    }
}



/*
 * @brief execute a regular expression
 * @param r the Exalt_Regex
 * @return Return 1 if the regular expression is found int the request string, else 0
 */
int exalt_regex_execute(Exalt_Regex* r)
{
    int err;
    regex_t preg;
    int nmatch;
    int match;
    regmatch_t *pmatch = NULL;

    if(r==NULL)
        return -1;

    exalt_regex_clear_result(r);

    err = regcomp (&preg, r->str_regex, REG_EXTENDED);
    if (err != 0)
        return 0;

    nmatch = preg.re_nsub + 1;
    pmatch = (regmatch_t*)malloc (sizeof (regmatch_t) * nmatch);

    if (pmatch)
    {
        match = regexec (&preg, r->str_request, nmatch, pmatch, 0);
	r->nmatch = nmatch;
        regfree (&preg);
        if (match == 0)
        {
            unsigned int i ;
	    r->res = (char**)malloc(sizeof(char*) * r->nmatch);
	    for(i=0;i<r->nmatch;i++)
	    {
            	int start = pmatch[i].rm_so;
            	int end = pmatch[i].rm_eo;
            	size_t size = end - start;

                r->res[i] = (char*)malloc (sizeof (char) * (size + 1));
                EXALT_ASSERT_ADV(r->res[i],EXALT_FREE(pmatch);return 0,"r->res[i] failed");
                strncpy (r->res[i], &(r->str_request[start]), size);
                r->res[i][size] = '\0';
            }

            EXALT_FREE(pmatch);
	    return 1;
	}
        else if (match == REG_NOMATCH)
        {
	    if(r->debug)
	    {
            	print_error( __FILE__,__func__, __LINE__, "no match found"
					"str_request: %s"
					"str_regex: %s\n\n", r->str_request,r->str_regex);
	    }
            EXALT_FREE(pmatch);
	    return 0;
        }
        else
        {
            char *text;
            size_t size;

            size = regerror (err, &preg, NULL, 0);
	    text = malloc (sizeof (char) * size);
            if (text)
            {
               	regerror (err, &preg, text, size);
               	print_error(__FILE__,__func__,__LINE__, "%s\n", text);
               	free (text);
                EXALT_FREE(pmatch);
		return 0;
            }
            else
            {
                EXALT_FREE(pmatch);
               	return 0;
            }
        }
    }
    else
    {
        print_error(__FILE__, __func__, __LINE__,"regcomp error");
        EXALT_FREE(pmatch);
        return 0;
    }
}



