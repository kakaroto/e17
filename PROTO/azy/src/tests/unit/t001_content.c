/*
 * Copyright 2010, 2011 Mike Blumenkrantz <mike@zentific.com>
 */

#include "tests.h"
#include "Azy.h"

#define REQUEST(method, params) \
  "<methodCall><methodName>" method "</methodName><params>" params "</params></methodCall>\n"
#define PARAM(value) \
  "<param>" value "</param>\n"
#define VALUE(type, value) \
  "<value><" #type ">" value "</" #type "></value>"
#define ARRAY(value) \
  "<value><array><data>" value "</data></array></value>"
#define MEMBER(name, value) \
  "<member><name>" #name "</name>" value "</member>"

EAPI Eina_Bool
azy_content_deserialize_json(Azy_Content *content,
                             const char *buf,
                             ssize_t len);

static void
print_result(Eina_Bool result, int line, const char *cond)
{
   printf("        " EINA_COLOR_WHITE __FILE__"[%-3i]: %-48s [%s"EINA_COLOR_WHITE"]\n" EINA_COLOR_RESET,
         line, cond, (!result) ?
           EINA_COLOR_RED "FAILED" :
           EINA_COLOR_GREEN "PASSED" );
}

/* tests */
static Eina_Bool
constructCall(void)
{
   Azy_Content *content = azy_content_new("test.test");
   const char *method = azy_content_method_get(content);
   TEST_ASSERT(method != 0);
   int method_match = !strcmp(method, "test");
   TEST_ASSERT(method_match);
   azy_content_free(content);
   return EINA_TRUE;
}

static Eina_Bool
constructCallNoMethod(void)
{
   Azy_Content *content = azy_content_new(NULL);
   const char *method = azy_content_method_get(content);
   TEST_ASSERT(method == 0);
   azy_content_free(content);
   return EINA_TRUE;
}

static Eina_Bool
responseUnserializeJson(void)
{
   Azy_Content *content = azy_content_new(NULL);
   Eina_Bool rs = azy_content_deserialize_json(content, "[]", 2);
   TEST_ASSERT(rs);
   azy_content_free(content);
   return EINA_TRUE;
}

static Eina_Bool
requestUnserialize1(void)
{
   Azy_Content *content = azy_content_new(NULL);
   Eina_Bool rs = azy_content_deserialize_request(content, AZY_NET_TRANSPORT_XML, "<->", 3);
   TEST_ASSERT(!rs);
   azy_content_free(content);
   return EINA_TRUE;
}


static Eina_Bool
requestUnserialize2(void)
{
   Azy_Content *content = azy_content_new(NULL);
   Eina_Strbuf *str = eina_strbuf_new();
   eina_strbuf_append(str, REQUEST("",
                                   PARAM(VALUE(string, "s1"))
                                   PARAM(VALUE(string, "s2"))
                                   ));
   Eina_Bool rs = azy_content_deserialize_request(content, AZY_NET_TRANSPORT_XML, eina_strbuf_string_get(str), eina_strbuf_length_get(str));
   TEST_ASSERT(!rs);
   eina_strbuf_free(str);
   azy_content_free(content);
   return EINA_TRUE;
}

static Eina_Bool
requestUnserialize3(void)
{
   Azy_Content *content = azy_content_new(NULL);
   Eina_Strbuf *str = eina_strbuf_new();
   eina_strbuf_append(str,
                      REQUEST("test.test",
                              PARAM(VALUE(string, ""))
                              PARAM(VALUE(string, "s2"))
                              ));

   Eina_Bool rs = azy_content_deserialize_request(content, AZY_NET_TRANSPORT_XML, eina_strbuf_string_get(str), eina_strbuf_length_get(str));
   TEST_ASSERT(rs);

   const char *method = azy_content_method_get(content);
   TEST_ASSERT(method != 0);
   int method_match = !strcmp(method, "test");
   TEST_ASSERT(method_match);

   Azy_Value *val = azy_content_param_get(content, 0);
   TEST_ASSERT(val != 0);
   TEST_ASSERT(azy_value_type_get(val) == AZY_VALUE_STRING);
   const char *str_val = 0;
   rs = azy_value_string_get(val, &str_val);
   TEST_ASSERT(rs);
   TEST_ASSERT(str_val != 0);
   int param_match = !strcmp(str_val, "");
   TEST_ASSERT(param_match);
   eina_stringshare_del(str_val);

   val = azy_content_param_get(content, 1);
   TEST_ASSERT(val != 0);
   TEST_ASSERT(azy_value_type_get(val) == AZY_VALUE_STRING);
   str_val = 0;
   rs = azy_value_string_get(val, &str_val);
   TEST_ASSERT(rs);
   TEST_ASSERT(str_val != 0);
   param_match = !strcmp(str_val, "s2");
   TEST_ASSERT(param_match);
   eina_stringshare_del(str_val);
   eina_strbuf_free(str);
   azy_content_free(content);
   return EINA_TRUE;
}

static Eina_Bool
_assert_param_type(Azy_Content   *content,
                   int            no,
                   Azy_Value_Type type)
{
   Azy_Value *val = azy_content_param_get(content, no);
   TEST_ASSERT(val != NULL);
   TEST_ASSERT(azy_value_type_get(val) == type);
   return azy_value_type_get(val) == type;
}

static Eina_Bool
requestUnserialize4(void)
{
   Azy_Content *content = azy_content_new(NULL);
   Eina_Strbuf *str = eina_strbuf_new();
   eina_strbuf_append(str, REQUEST("test.test",
                                   PARAM(VALUE(int, "1"))
                                   PARAM(VALUE(int, "-1"))
                                   PARAM(VALUE(string, "some string"))
                                   PARAM(VALUE(boolean, "1"))
                                   PARAM(VALUE(double, "1.2323"))
                                   PARAM(VALUE(dateTime.iso8601, "2006-04-04"))
                                   PARAM(VALUE(base64, "bGlieHJpbmNsdWRlZGlyID0gJChpbmNsdWRlZGlyKS9saWJ4cgoKbGlieHJpbmNsdWRlX0hF\n"
                                                       "QURFUlMgPSBcCiAgeHItdmFsdWUuaCBcCiAgeHItY2FsbC5oIFwKICB4ci1jbGllbnQuaCBc\n"
                                                       "CiAgeHItc2VydmVyLmgK"))
                                   PARAM(ARRAY(
                                           VALUE(string, "f1")
                                           VALUE(string, "f2")
                                           ))
                                   PARAM(VALUE(struct,
                                               MEMBER(m1, VALUE(string, "s1"))
                                               MEMBER(m2, VALUE(string, ""))
                                               ))
                                   ));
   //printf("%s\n", eina_strbuf_string_get(str));
   Eina_Bool rs = azy_content_deserialize_request(content, AZY_NET_TRANSPORT_XML, eina_strbuf_string_get(str), eina_strbuf_length_get(str));

   TEST_ASSERT(rs);
   TEST_ASSERT(_assert_param_type(content, 0, AZY_VALUE_INT));
   TEST_ASSERT(_assert_param_type(content, 1, AZY_VALUE_INT));
   TEST_ASSERT(_assert_param_type(content, 2, AZY_VALUE_STRING));
   TEST_ASSERT(_assert_param_type(content, 3, AZY_VALUE_BOOL));
   TEST_ASSERT(_assert_param_type(content, 4, AZY_VALUE_DOUBLE));
   TEST_ASSERT(_assert_param_type(content, 5, AZY_VALUE_TIME));
   TEST_ASSERT(_assert_param_type(content, 7, AZY_VALUE_ARRAY));
   TEST_ASSERT(_assert_param_type(content, 8, AZY_VALUE_STRUCT));
   eina_strbuf_free(str);
   azy_content_free(content);
   return EINA_TRUE;
}

static Eina_Bool
requestUnserialize5(void)
{
   Azy_Content *content = azy_content_new(NULL);
   Eina_Strbuf *str = eina_strbuf_new();
   eina_strbuf_append(str, REQUEST("test.test",
                                   PARAM(VALUE(int, "1"))
                                   PARAM(VALUE(int, "-1"))
                                   "<string>some string</string>\n" /* invalid! */
                                   PARAM(VALUE(boolean, "1"))
                                   PARAM(VALUE(double, "1.2323"))
                                   PARAM(VALUE(dateTime.iso8601, "2006-04-04"))
                                   PARAM(VALUE(base64, "bGlieHJpbmNsdWRlZGlyID0gJChpbmNsdWRlZGlyKS9saWJ4cgoKbGlieHJpbmNsdWRlX0hF\n"
                                                       "QURFUlMgPSBcCiAgeHItdmFsdWUuaCBcCiAgeHItY2FsbC5oIFwKICB4ci1jbGllbnQuaCBc\n"
                                                       "CiAgeHItc2VydmVyLmgK"))
                                   PARAM(ARRAY(
                                           VALUE(string, "f1")
                                           VALUE(string, "f2")
                                           ))
                                   PARAM(VALUE(struct,
                                               MEMBER(m1, VALUE(string, "s1"))
                                               MEMBER(m2, VALUE(string, ""))
                                               ))
                                   ));
   //printf("%s\n", eina_strbuf_string_get(str));
   Eina_Bool rs = azy_content_deserialize_request(content, AZY_NET_TRANSPORT_XML, eina_strbuf_string_get(str), eina_strbuf_length_get(str));

   TEST_ASSERT(!rs);
   eina_strbuf_free(str);
   azy_content_free(content);
   return EINA_TRUE;
}

static Eina_Bool
responseUnserialize1(void)
{
   Azy_Content *content = azy_content_new(NULL);
   Eina_Strbuf *str = eina_strbuf_new();
   eina_strbuf_append(str, "<?xml version=\"1.0\"?>"
"<methodResponse>"
"   <fault>"
"      <value>"
"         <struct>"
"            <member>"
"               <name>faultCode</name>"
"               <value><int>4</int></value>"
"               </member>"
"            <member>"
"               <name>faultString</name>"
"               <value><string>Too many parameters.</string></value>"
"               </member>"
"            </struct>"
"         </value>"
"      </fault>"
"   </methodResponse>");
   //printf("%s\n", eina_strbuf_string_get(str));
   Eina_Bool rs = azy_content_deserialize_response(content, AZY_NET_TRANSPORT_XML, eina_strbuf_string_get(str), eina_strbuf_length_get(str));

   TEST_ASSERT(rs);
   eina_strbuf_free(str);
   azy_content_free(content);
   return EINA_TRUE;
}

static Eina_Bool
responseUnserialize2(void)
{
   Azy_Content *content = azy_content_new(NULL);
   Eina_Strbuf *str = eina_strbuf_new();
   eina_strbuf_append(str,
"<?xml version=\"1.0\"?>"
"<methodResponse>"
"   <params>"
"      <param>"
"         <value><string>South Dakota</string></value>"
"         </param>"
"      <param>"
"         <value><string>South Dakota</string></value>"
"         </param>"
"      </params>"
"   </methodResponse>"
);
   //printf("%s\n", eina_strbuf_string_get(str));
   Eina_Bool rs = azy_content_deserialize_response(content, AZY_NET_TRANSPORT_XML, eina_strbuf_string_get(str), eina_strbuf_length_get(str));

   TEST_ASSERT(!rs);
   eina_strbuf_free(str);
   azy_content_free(content);
   return EINA_TRUE;
}

int
main()
{
   eina_init();
   azy_init();
   Eina_Counter *c = eina_counter_new("t001-content");
   eina_counter_start(c);

   int failed = EINA_FALSE;
   RUN_TEST(constructCall);
   RUN_TEST(constructCallNoMethod);
   RUN_TEST(requestUnserialize1);
   RUN_TEST(requestUnserialize2);
   RUN_TEST(requestUnserialize3);
   RUN_TEST(requestUnserialize4);
   RUN_TEST(requestUnserialize5);
   RUN_TEST(responseUnserialize1);
   RUN_TEST(responseUnserialize2);
   RUN_TEST(responseUnserializeJson);
   eina_counter_stop(c, 1);
   printf("%s", eina_counter_dump(c));
   eina_counter_free(c);
   return failed ? 1 : 0;
}

