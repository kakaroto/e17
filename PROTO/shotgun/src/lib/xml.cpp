#include "shotgun_private.h"
#include "xml.h"
#include "pugixml.hpp"

#define XML_NS_ROSTER "jabber:iq:roster"
#define XML_NS_VERSION "jabber:iq:version"
#define XML_NS_IDLE "jabber:iq:last"
#define XML_NS_TIME "urn:xmpp:time"
#define XML_NS_DATA "jabber:x:data"
#define XML_NS_DISCO_INFO "http://jabber.org/protocol/disco#info"
#define XML_NS_CHATSTATES "http://jabber.org/protocol/chatstates"
#define XML_NS_FILETRANSFER "http://jabber.org/protocol/si/profile/file-transfer"
#define XML_NS_FEATURE_NEG "http://jabber.org/protocol/feature-neg"
#define XML_NS_BYTESTREAM "http://jabber.org/protocol/bytestreams"
#define XML_NS_IBB "http://jabber.org/protocol/ibb"
#define XML_NS_SI "http://jabber.org/protocol/si"
#define XML_NS_BIND "urn:ietf:params:xml:ns:xmpp-bind"
#define XML_NS_DELAY "urn:xmpp:delay"
#define XML_NS_SESSION "urn:ietf:params:xml:ns:xmpp-session"
#define XML_NS_ARCHIVE "urn:xmpp:archive"
#define XML_NS_ARCHIVE_MANAGE "urn:xmpp:archive:manage"
#define XML_NS_RSM "http://jabber.org/protocol/rsm"
#define XML_NS_GOOGLE_SETTINGS "google:setting"
#define XML_NS_GOOGLE_NOSAVE "google:nosave"
#define XML_NS_GOOGLE_MAILNOTIFY "google:mail:notify"
#define XML_NS_AVATAR_DATA "urn:xmpp:avatar:data"
#define XML_NS_AVATAR_METADATA "urn:xmpp:avatar:data"
#define XML_NS_VCARD "vcard-temp"
#define XML_NS_STANZAS "urn:ietf:params:xml:ns:xmpp-stanzas"
#define XML_NS_PING "urn:xmpp:ping"

using namespace pugi;

struct xml_memory_writer : xml_writer
{
   char  *buffer;
   size_t capacity;

   size_t result;

   xml_memory_writer() : buffer(0), capacity(0), result(0)
   {
   }

   xml_memory_writer(char  *buffer,
                     size_t capacity) : buffer(buffer), capacity(capacity), result(0)
   {
   }

   size_t
   written_size() const
   {
      return result < capacity ? result : capacity;
   }

   virtual void
   write(const void *data,
         size_t      size)
   {
      if (result < capacity)
        {
           size_t chunk = (capacity - result < size) ? capacity - result : size;
           memcpy(buffer + result, data, chunk);
        }

      result += size;
   }
};

void
xml_sanitize_from(const char **save, char *from)
{
   eina_str_tolower(&from);
   eina_stringshare_replace(save, from);
}

static char *
xmlnode_to_buf(xml_node node,
               size_t *len,
               Eina_Bool leave_open)
{
   xml_memory_writer counter;
   char *buffer;

   node.print(counter);
   buffer = static_cast<char*>(calloc(1, counter.result ));
   xml_memory_writer writer(buffer, counter.result);
   node.print(writer);
   buffer[writer.written_size() - 1] = 0;
   *len = writer.written_size() - 1;
   if (!leave_open) return buffer;

   if (buffer[*len - 3] == ' ')
     (*len)--;
   buffer[(*len) - 1] = '>';
   buffer[(*len)] = 0;

   return buffer;
}

static bool
xmlattr_to_bool(xml_attribute attr)
{
   const char *s;

   s = attr.value();
   if ((!s) || (!s[0])) return false;
   return (!strcmp(s, "true") || (!strcmp(s, "enabled")));
}

char *
xml_stream_init_create(Shotgun_Auth *auth, const char *lang, size_t *len)
{
/*
C: <stream:stream
     from='juliet@im.example.com'
     to='im.example.com'
     version='1.0'
     xml:lang='en'
     xmlns='jabber:client'
     xmlns:stream='http://etherx.jabber.org/streams'>
*/
   xml_document doc;
   xml_node stream;

   stream = doc.append_child("stream:stream");
   stream.append_attribute("from").set_value(auth->jid);
   stream.append_attribute("to").set_value(auth->from);
   stream.append_attribute("version").set_value("1.0");
   stream.append_attribute("xml:lang").set_value(lang);
   stream.append_attribute("xmlns").set_value("jabber:client");
   stream.append_attribute("xmlns:stream").set_value("http://etherx.jabber.org/streams");

   return xmlnode_to_buf(stream, len, EINA_TRUE);
}

static Eina_Bool
xml_stream_init_read_mechanisms(Shotgun_Auth *auth, xml_node stream, xml_node node)
{
  xml_attribute attr;
/*
S: <stream:features>
     <starttls xmlns="urn:ietf:params:xml:ns:xmpp-tls">
       <required/>
     </starttls>
     <mechanisms xmlns="urn:ietf:params:xml:ns:xmpp-sasl">
       <mechanism>X-GOOGLE-TOKEN</mechanism>
       <mechanism>X-OAUTH2</mechanism>
     </mechanisms>
    </stream:features>
*/

   if (!stream.child("starttls").empty())
      auth->features.starttls = EINA_TRUE;

   for (attr = node.first_attribute(); attr; attr = attr.next_attribute())
      if (!strcmp(attr.name(), "xmlns"))
        {
           const char *s;

           s = strrchr(attr.value(), ':');
           if (s && (!strcmp(s + 1, "xmpp-sasl")))
             {
                for (xml_node it = node.first_child(); it; it = it.next_sibling())
                  {
                     s = it.child_value();
                     if (s && (!strcmp(s, "PLAIN")))
                       auth->features.sasl_plain = EINA_TRUE;
                     else if (s && (!strcmp(s, "X-OAUTH2")))
                       auth->features.sasl_oauth2 = EINA_TRUE;
                     else if (s && (!strcmp(s, "X-GOOGLE_TOKEN")))
                       auth->features.sasl_gtoken = EINA_TRUE;
                     else if (s && (!strcmp(s, "DIGEST-MD5")))
                       auth->features.sasl_digestmd5 = EINA_TRUE;
                  }
             }
           break;
        }
   if (auth->features.sasl_digestmd5)
     INF("Selected DIGEST-MD5 auth mechanism");
   else
     INF("Selected PLAIN auth mechanism");
   /* lots more auth mechanisms here but who cares */
   return EINA_TRUE;
}

static void
xml_node_rsm_add(xml_node &node, unsigned int max, const char *after)
{
   xml_node set;
   set = node.append_child("set");
   set.append_attribute("xmlns").set_value(XML_NS_RSM);
   if (max)
     {
        char buf[64];

        snprintf(buf, sizeof(buf), "%u", max);
        set.append_child("max").append_child(node_pcdata).set_value(buf);
     }
   if (after) set.append_child("after").append_child(node_pcdata).set_value(after);
}

Eina_Bool
xml_stream_init_read(Shotgun_Auth *auth, char *xml, size_t size)
{
/*
S: <stream:stream
     from='im.example.com'
     id='t7AMCin9zjMNwQKDnplntZPIDEI='
     to='juliet@im.example.com'
     version='1.0'
     xml:lang='en'
     xmlns='jabber:client'
     xmlns:stream='http://etherx.jabber.org/streams'>
*/
   xml_document doc;
   xml_node stream, node;
   xml_attribute attr;
   xml_parse_result res;

   res = doc.load_buffer_inplace(xml, size, parse_default, encoding_auto);
   if ((res.status != status_ok) && (res.status != status_end_element_mismatch))
     {
        ERR("%s", res.description());
        //goto error;
     }

   stream = doc.first_child();
   if (!strcmp(stream.name(), "stream:stream"))
     {
        for (attr = stream.first_attribute(); attr; attr = attr.next_attribute())
          {
             if (!strcmp(attr.name(), "from"))
               {
                  eina_stringshare_replace(&auth->from, attr.value());
                  break;
               }
          }

        if (stream.first_child().empty()) return EINA_FALSE;
        stream = stream.first_child();
     }

   node = stream.child("mechanisms");
   if (!node.empty())
     return xml_stream_init_read_mechanisms(auth, stream, node);

   node = stream.child("bind");
   if (!node.empty()) auth->features.bind = EINA_TRUE;
   node = stream.child("session");
   if (!node.empty()) auth->features.session = EINA_TRUE;
   /* something something */
   return EINA_TRUE;
}

Eina_Bool
xml_starttls_read(char *xml, size_t size __UNUSED__)
{
/*
S: <proceed xmlns='urn:ietf:params:xml:ns:xmpp-tls'/>

S: <failure xmlns='urn:ietf:params:xml:ns:xmpp-tls'/>
      </stream:stream>
*/
   /* massively cheating again! */
   return xml[1] == 'p';
}

char *
xml_sasl_digestmd5_write(const char *sasl, size_t *len)
{
/*
http://www.ietf.org/rfc/rfc2831.txt
<response xmlns="urn:ietf:params:xml:ns:xmpp-sasl">
b64data
</response>
*/
   xml_document doc;
   xml_node a;

   a = doc.append_child("response");
   a.append_attribute("xmlns").set_value("urn:ietf:params:xml:ns:xmpp-sasl");
   if (sasl) a.append_child(node_pcdata).set_value(sasl);

   return xmlnode_to_buf(a, len, EINA_FALSE);
}

char *
xml_sasl_write(Shotgun_Auth *auth, const char *sasl, size_t *len)
{
/*
http://code.google.com/apis/talk/jep_extensions/jid_domain_change.html
<auth xmlns="urn:ietf:params:xml:ns:xmpp-sasl"
      mechanism="PLAIN"
      xmlns:ga='http://www.google.com/talk/protocol/auth'
      ga:client-uses-full-bind-result='true'>
... encoded user name and password ... user=example@gmail.com password=supersecret
</auth>
*/
   xml_document doc;
   xml_node a;

   a = doc.append_child("auth");
   a.append_attribute("xmlns").set_value("urn:ietf:params:xml:ns:xmpp-sasl");
/*
   if (auth->features.sasl_oauth2)
     a.append_attribute("mechanism").set_value("X-OAUTH2");
   else
*/
   if (auth->features.sasl_digestmd5)
     a.append_attribute("mechanism").set_value("DIGEST-MD5");
   else
     a.append_attribute("mechanism").set_value("PLAIN");
   if (auth->features.sasl_gtoken)
     {
        a.append_attribute("xmlns:ga").set_value("http://www.google.com/talk/protocol/auth");
        a.append_attribute("ga:client-uses-full-bind-result").set_value("true");
     }
   a.append_child(node_pcdata).set_value(sasl);

   return xmlnode_to_buf(a, len, EINA_FALSE);
}

int
xml_sasl_read(Shotgun_Auth *auth, char *xml, size_t size)
{
/*
S: <success xmlns="urn:ietf:params:xml:ns:xmpp-sasl"/>

S: <failure xmlns="urn:ietf:params:xml:ns:xmpp-sasl"><not-authorized/></failure
*/
   /* I don't even care at this point */
   if (xml[1] == 's') return 1;
   if (auth->features.sasl_digestmd5 && (!auth->features.auth_digestmd5))
     {
        xml_document doc;
        xml_parse_result res;
        xml_node node;
        char *b64;
        const char *key;
        size_t len;

        res = doc.load_buffer_inplace(xml, size, parse_default, encoding_auto);
        if ((res.status != status_ok) && (res.status != status_end_element_mismatch))
          {
             ERR("%s", res.description());
             //goto error;
          }
        node = doc.first_child();
        if (strcmp(node.name(), "challenge")) return 0;
        b64 = (char*)shotgun_base64_decode(node.child_value(), strlen(node.child_value()), &len);
        if (!b64) return 0;
        b64[len] = 0;
        //INF("Decoded: %s", b64);
        auth->features.auth_digestmd5 = eina_hash_string_superfast_new(free);
        for (key = b64; key && *key;)
          {
             const char *e, *next;

             e = strchr(key, '=');
             if (!e) goto error;
             if (e[1] == '"')
               {
                  next = strchr(e + 2, '"');
                  /* skip escaped dquot */
                  while (next && (next[-1] == '\\')) next = strchr(next + 1, '"');
                  if ((!next) || (next[1] && (next[1] != ','))) goto error;
                  //INF("key: %s, val: %s", strndupa(key, e - key), strndupa(e + 2, next - (e + 2)));
                  eina_hash_add(auth->features.auth_digestmd5, strndupa(key, e - key), strndup(e + 2, next - (e + 2)));
                  next++;
               }
             else
               {
                  next = strchr(e + 1, ',');
                  if (!next)
                    {
                       //INF("key: %s, val: %s", strndupa(key, e - key), strdupa(e + 1));
                       eina_hash_add(auth->features.auth_digestmd5, strndupa(key, e - key), strdup(e + 1));
                    }
                  else
                    {
                       //INF("key: %s, val: %s", strndupa(key, e - key), strndupa(e + 1, next - (e + 1)));
                       eina_hash_add(auth->features.auth_digestmd5, strndupa(key, e - key), strndup(e + 1, next - (e + 1)));
                    }
               }
             key = next ? next + 1 : NULL;
          }
        free(b64);
        return 2;
error:
        eina_hash_free(auth->features.auth_digestmd5);
        auth->features.auth_digestmd5 = NULL;
        free(b64);
        return 0;
     }
   if (auth->features.sasl_digestmd5 && auth->features.auth_digestmd5)
     {
        eina_hash_free(auth->features.auth_digestmd5);
        auth->features.auth_digestmd5 = NULL;
        return (xml[1] == 'c') ? 2 : 0;
     }
   return 0;
}

char *
xml_iq_write_contact_add(const char *jid, const char *alias, Eina_List *groups, size_t *len)
{
   xml_document doc;
   xml_node iq, node;
   Eina_List *l;
   void *group;

   iq = doc.append_child("iq");
/*
<iq from='juliet@example.com/balcony' type='set' id='roster_2'>
  <query xmlns='jabber:iq:roster'>
    <item jid='nurse@example.com'
          name='Nurse'>
      <group>Servants</group>
    </item>
  </query>
</iq>
*/
   iq.append_attribute("type").set_value("set");
   iq.append_attribute("id").set_value("contactadd");
   node = iq.append_child("query");
   node.append_attribute("xmlns").set_value(XML_NS_ROSTER);
   node = node.append_child("item");
   node.append_attribute("jid").set_value(jid);
   if (alias && alias[0]) node.append_attribute("name").set_value(alias);
   EINA_LIST_FOREACH(groups, l, group)
     node.append_child("group").append_child(node_pcdata).set_value((const char *)group);
   return xmlnode_to_buf(doc, len, EINA_FALSE);
}

char *
xml_iq_write_contact_del(const char *user, size_t *len)
{
   xml_document doc;
   xml_node iq, node;

   iq = doc.append_child("iq");
/*
<iq from='juliet@example.com/balcony' type='set' id='roster_4'>
  <query xmlns='jabber:iq:roster'>
    <item jid='nurse@example.com' subscription='remove'/>
  </query>
</iq>
*/
   iq.append_attribute("type").set_value("set");
   iq.append_attribute("id").set_value("contactdel");
   node = iq.append_child("query");
   node.append_attribute("xmlns").set_value(XML_NS_ROSTER);
   node = node.append_child("item");
   node.append_attribute("jid").set_value(user);
   node.append_attribute("subscription").set_value("remove");
   return xmlnode_to_buf(doc, len, EINA_FALSE);
}

char *
xml_iq_write_contact_otr_set(const char *user, Eina_Bool enable, size_t *len)
{
   xml_document doc;
   xml_node iq, node;

   iq = doc.append_child("iq");
/*
<iq type='set'
    from='horatio@denmark.lit/castle'
    to='horatio@denmark.lit'
    id='otr-2'>
  <query xmlns='google:nosave'>
    <item xmlns='google:nosave' jid='guildenstern@denmark.lit' value='enabled'/>
  </query>
</iq>
*/

   iq.append_attribute("type").set_value("set");
   iq.append_attribute("id").set_value("contactotr");
   node = iq.append_child("query");
   node.append_attribute("xmlns").set_value(XML_NS_GOOGLE_NOSAVE);
   node = node.append_child("item");
   node.append_attribute("xmlns").set_value(XML_NS_GOOGLE_NOSAVE);
   node.append_attribute("jid").set_value(user);
   node.append_attribute("value").set_value(enable ? "enabled" : "disabled");
   return xmlnode_to_buf(doc, len, EINA_FALSE);
}

char *
xml_iq_write_get_si(const char *from, const char *to, const char *id, size_t *len)
{
   xml_document doc;
   xml_node iq, node;

   iq = doc.append_child("iq");
   iq.append_attribute("type").set_value("result");
   iq.append_attribute("from").set_value(from);
   iq.append_attribute("to").set_value(to);
   iq.append_attribute("id").set_value(id);
   node = iq.append_child("si");
   node.append_attribute("xmlns").set_value(XML_NS_SI);
   node = node.append_child("feature");
   node.append_attribute("xmlns").set_value(XML_NS_FEATURE_NEG);
   node = node.append_child("x");
   node.append_attribute("xmlns").set_value(XML_NS_DATA);
   node.append_attribute("type").set_value("submit");

   node = node.append_child("field");
   node.append_attribute("var").set_value("stream-method");

   node = node.append_child("value");
   node.append_child(node_pcdata).set_value(XML_NS_IBB);

   return xmlnode_to_buf(doc, len, EINA_FALSE);
}

char *
xml_iq_write_vcard(Shotgun_Auth *auth, size_t *len)
{
   xml_document doc;
   xml_node iq, node;
   Shotgun_User_Info *info;

   iq = doc.append_child("iq");
   iq.append_attribute("from").set_value(auth->base_jid);
   iq.append_attribute("type").set_value("set");
   iq.append_attribute("id").set_value("vc1");
   node = iq.append_child("vCard");
   node.append_attribute("xmlns").set_value(XML_NS_VCARD);
   info = (Shotgun_User_Info *)auth->vcard;
   xml_vcard_write(info, (void *)&node, EINA_FALSE);
   return xmlnode_to_buf(doc, len, EINA_FALSE);
}

char *
xml_iq_ibb_error(const char *from, const char *to, const char *id, size_t *len)
{
/*
<iq from='juliet@capulet.com/balcony'
    id='jn3h8g65'
    to='romeo@montague.net/orchard'
    type='error'>
  <error type='cancel'>
    <not-acceptable xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>
  </error>
</iq>
*/
   xml_document doc;
   xml_node iq, node;

   iq = doc.append_child("iq");
   iq.append_attribute("type").set_value("error");
   iq.append_attribute("to").set_value(to);
   iq.append_attribute("id").set_value(id);
   node = iq.append_child("error");
   node.append_attribute("type").set_value("cancel");
   node = node.append_child("not-acceptable");
   node.append_attribute("xmlns").set_value(XML_NS_STANZAS);
   return xmlnode_to_buf(doc, len, EINA_FALSE);
}

char *
xml_iq_write_get_bytestream(const char *from, const char *to, const char *id, const char *sid, size_t *len)
{
   xml_document doc;
   xml_node iq, node;

   iq = doc.append_child("iq");
   iq.append_attribute("type").set_value("result");
   iq.append_attribute("to").set_value(to);
   iq.append_attribute("id").set_value(id);
   node = iq.append_child("query");
   node.append_attribute("xmlns").set_value(XML_NS_BYTESTREAM);
   node.append_attribute("sid").set_value(sid);
   node = node.append_child("streamhost-used");
   node.append_attribute("jid").set_value(to);
   return xmlnode_to_buf(doc, len, EINA_FALSE);
}

char *
xml_iq_write_get_ibb(const char *from, const char *to, const char *id, size_t *len)
{
   xml_document doc;
   xml_node iq, node;

   iq = doc.append_child("iq");
   iq.append_attribute("type").set_value("result");
   iq.append_attribute("from").set_value(from);
   iq.append_attribute("to").set_value(to);
   iq.append_attribute("id").set_value(id);
   return xmlnode_to_buf(doc, len, EINA_FALSE);
}

char *
xml_iq_write_preset(Shotgun_Auth *auth, Shotgun_Iq_Preset p, size_t *len)
{
   xml_document doc;
   xml_node iq, node;

   iq = doc.append_child("iq");
   switch (p)
     {
      case SHOTGUN_IQ_PRESET_BIND:
/*
<iq type="set" id="0">
  <bind xmlns="urn:ietf:params:xml:ns:xmpp-bind">
    <resource>
      pcp
    </resource>
  </bind>
</iq>
*/
        iq.append_attribute("type").set_value("set");
        iq.append_attribute("id").set_value("0");

        node = iq.append_child("bind");
        node.append_attribute("xmlns").set_value("urn:ietf:params:xml:ns:xmpp-bind");

        node = node.append_child("resource");
        node.append_child(node_pcdata).set_value(auth->resource);
        break;
      case SHOTGUN_IQ_PRESET_ROSTER:
/*
<iq from='juliet@example.com/balcony' type='get' id='roster_1'>
  <query xmlns='jabber:iq:roster'/>
</iq>
*/
        iq.append_attribute("type").set_value("get");
        iq.append_attribute("id").set_value("roster");

        node = iq.append_child("query");
        node.append_attribute("xmlns").set_value(XML_NS_ROSTER);
        break;
      case SHOTGUN_IQ_PRESET_RESULT:
/*
<iq from='juliet@example.com/balcony'
    to='example.com'
    type='result'
    id='a78b4q6ha463'/>
*/
        iq.append_attribute("to").set_value(auth->from);
        iq.append_attribute("type").set_value("result");
        iq.append_attribute("id").set_value("resultvalue");
        break;
      case SHOTGUN_IQ_PRESET_SESSION:
        iq.append_attribute("type").set_value("set");
        iq.append_attribute("id").set_value("sessionvalue");
        node = iq.append_child("session");
        node.append_attribute("xmlns").set_value(XML_NS_SESSION);
        break;
      case SHOTGUN_IQ_PRESET_DISCO_INFO:
/*
<iq from='romeo@montague.net/orchard'
    id='disco1'
    to='montague.net'
    type='get'>
  <query xmlns='http://jabber.org/protocol/disco#info'/>
</iq>
*/
        iq.append_attribute("id").set_value("disco");
        iq.append_attribute("to").set_value(auth->from);
        iq.append_attribute("type").set_value("get");
        iq.append_child("query").append_attribute("xmlns").set_value(XML_NS_DISCO_INFO);
        break;
      case SHOTGUN_IQ_PRESET_GSETTINGS_GET:
/*
<iq type='get'
    from='romeo@gmail.com/orchard'
    to='romeo@gmail.com'
    id='user-setting-1'>
  <usersetting xmlns='google:setting' />
</iq>
*/
        iq.append_attribute("id").set_value("gsettings-get");
        iq.append_attribute("type").set_value("get");
        iq.append_child("usersetting").append_attribute("xmlns").set_value(XML_NS_GOOGLE_SETTINGS);
        break;
      case SHOTGUN_IQ_PRESET_GSETTINGS_SET:
/*
<iq type='set'
    from='romeo@gmail.com/orchard'
    to='romeo@gmail.com'
    id='user-setting-3'>
  <usersetting xmlns='google:setting'>
    <archivingenabled value='false'/>
  </usersetting>
</iq>
*/
        iq.append_attribute("id").set_value("gsettings-set");
        iq.append_attribute("type").set_value("set");
        node = iq.append_child("usersetting");
        node.append_attribute("xmlns").set_value(XML_NS_GOOGLE_SETTINGS);
        node.append_child("mailnotifications").append_attribute("value").set_value(auth->features.gsettings.mail_notifications ? "true" : "false");
        node.append_child("archivingenabled").append_attribute("value").set_value(auth->features.gsettings.archiving ? "true" : "false");
        break;
      case SHOTGUN_IQ_PRESET_MAIL_SEARCH:
/*
<iq type='get'
    from='romeo@gmail.com/orchard'
    to='romeo@gmail.com'
    id='mail-request-1'>
  <query xmlns='google:mail:notify'
         newer-than-time='1140638252542'
         newer-than-tid='11134623426430234' />
</iq>
*/
        iq.append_attribute("id").set_value("mail-query");
        iq.append_attribute("type").set_value("get");
        node = iq.append_child("query");
        node.append_attribute("xmlns").set_value(XML_NS_GOOGLE_MAILNOTIFY);
        node.append_attribute("newer-than-time").set_value("9999999999999");
        break;
      case SHOTGUN_IQ_PRESET_OTR_QUERY:
/*
<iq type='get'
    from='horatio@denmark.lit/castle'
    to='horatio@denmark.lit'
    id='otr-1'>
  <query xmlns='google:nosave' />
</iq>
*/
        iq.append_attribute("id").set_value("otr-query");
        iq.append_attribute("type").set_value("get");
        iq.append_child("query").append_attribute("xmlns").set_value(XML_NS_GOOGLE_NOSAVE);
        break;
      default:
        break;
     }
   return xmlnode_to_buf(doc, len, EINA_FALSE);
}

char *
xml_iq_write_get_vcard(const char *to, size_t *len)
{
   xml_document doc;
   xml_node iq;
   iq = doc.append_child("iq");
/*
<iq from='stpeter@jabber.org/roundabout'
    id='v3'
    to='jer@jabber.org'
    type='get'>
  <vCard xmlns='vcard-temp'/>
</iq>
*/
   if (to)
     {
        const char *s;
        char buf[512];
        s = strrchr(to, '/');
        if (s) strncat(buf, to, s - to);
        iq.append_attribute("to").set_value(s ? buf : to);
     }
   iq.append_attribute("id").set_value("vcard-get");
   iq.append_attribute("type").set_value("get");
   iq.append_child("vCard").append_attribute("xmlns").set_value("vcard-temp");
   return xmlnode_to_buf(doc, len, EINA_FALSE);
}

char *
xml_iq_write_archive_get(const char *to, unsigned int max, size_t *len)
{
   xml_document doc;
   xml_node iq, n;
   const char *s = NULL;
   char buf[512];

   iq = doc.append_child("iq");
/*
<iq type='get' id='juliet1'>
  <list xmlns='urn:xmpp:archive'
        with='juliet@capulet.com'>
    <set xmlns='http://jabber.org/protocol/rsm'>
      <max>30</max>
    </set>
  </list>
</iq>
*/
   if (to)
     {
        s = strrchr(to, '/');
        if (s) strncat(buf, to, s - to);
     }
   iq.append_attribute("id").set_value("archive-get");
   iq.append_attribute("type").set_value("get");
   n = iq.append_child("list");
   n.append_attribute("xmlns").set_value(XML_NS_ARCHIVE);
   if (s || to) n.append_attribute("with").set_value(s ? buf : to);
   xml_node_rsm_add(n, max, NULL);
   return xmlnode_to_buf(doc, len, EINA_FALSE);
}

static Shotgun_Iq_Type
xml_iq_type_get(xml_node node)
{
   const char *type;

   type = node.attribute("type").value();
   if (!strcmp(type, "get"))
     return SHOTGUN_IQ_TYPE_GET;
   if (!strcmp(type, "set"))
     return SHOTGUN_IQ_TYPE_SET;
   if (!strcmp(type, "result"))
     return SHOTGUN_IQ_TYPE_RESULT;
   return SHOTGUN_IQ_TYPE_ERROR;
}

static Shotgun_User_Subscription
xml_iq_user_subscription_get(xml_node node)
{
   const char *s;
   s = node.attribute("subscription").value();
   switch (s[0])
     {
      case 't':
        return SHOTGUN_USER_SUBSCRIPTION_TO;
      case 'f':
        return SHOTGUN_USER_SUBSCRIPTION_FROM;
      case 'b':
        return SHOTGUN_USER_SUBSCRIPTION_BOTH;
      case 'r':
        return SHOTGUN_USER_SUBSCRIPTION_REMOVE;
      default:
        break;
     }
   return SHOTGUN_USER_SUBSCRIPTION_NONE;
}

static Shotgun_Event_Iq *
xml_iq_roster_read(Shotgun_Auth *auth, xml_node node)
{
/*
<iq to='juliet@example.com/balcony' type='result' id='roster_1'>
  <query xmlns='jabber:iq:roster'>
    <item jid='romeo@example.net'
          name='Romeo'
          subscription='both'>
      <group>Friends</group>
    </item>
    <item jid='mercutio@example.org'
          name='Mercutio'
          subscription='from'>
      <group>Friends</group>
    </item>
    <item jid='benvolio@example.org'
          name='Benvolio'
          subscription='both'>
      <group>Friends</group>
    </item>
  </query>
</iq>
*/
   Shotgun_Event_Iq *ret;
   char *xml;
   size_t len;

   ret = static_cast<Shotgun_Event_Iq*>(calloc(1, sizeof(Shotgun_Event_Iq)));
   ret->type = SHOTGUN_IQ_EVENT_TYPE_ROSTER;
   ret->account = auth;

   for (xml_node it = node.first_child(); it; it = it.next_sibling())
     {
        Shotgun_User *user;
        const char *name;

        user = static_cast<Shotgun_User*>(calloc(1, sizeof(Shotgun_User)));
        user->account = auth;
        if (!it.attribute("ask").empty())
          user->subscription_pending = EINA_TRUE;
        name = it.attribute("name").value();
        if (name && name[0])
          user->name = eina_stringshare_add(name);
        xml_sanitize_from(&user->jid, (char*)it.attribute("jid").value());
        user->subscription = xml_iq_user_subscription_get(it);
        for (xml_node g = it.first_child(); g; g = g.next_sibling())
          {
             if (strcmp(g.name(), "group")) continue;
             user->groups = eina_list_append(user->groups, eina_stringshare_add(g.child_value()));
          }

        ret->ev = eina_list_append((Eina_List*)ret->ev, (void*)user);
     }
   xml = xml_iq_write_preset(auth, SHOTGUN_IQ_PRESET_RESULT, &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
   return ret;
}

static Shotgun_Event_Iq *
xml_iq_disco_info_write(Shotgun_Auth *auth, xml_node &query)
{
/*
<iq type='get'
    from='romeo@montague.net/orchard'
    to='plays.shakespeare.lit'
    id='info1'>
  <query xmlns='http://jabber.org/protocol/disco#info'/>
</iq>
*/
/*
<iq type='result'
    from='plays.shakespeare.lit'
    to='romeo@montague.net/orchard'
    id='info1'>
  <query xmlns='http://jabber.org/protocol/disco#info'>
    <identity
        category='conference'
        type='text'
        name='Play-Specific Chatrooms'/>
    <identity
        category='directory'
        type='chatroom'
        name='Play-Specific Chatrooms'/>
    <feature var='http://jabber.org/protocol/disco#info'/>
    <feature var='http://jabber.org/protocol/disco#items'/>
    <feature var='http://jabber.org/protocol/muc'/>
    <feature var='jabber:iq:register'/>
    <feature var='jabber:iq:search'/>
    <feature var='jabber:iq:time'/>
    <feature var='jabber:iq:version'/>
  </query>
</iq>
*/
   Shotgun_Event_Iq *ret;
   xml_document doc;
   xml_node iq, node, identity;
   char *xml;
   size_t len;

   /* TODO: this setup should probably be a macro or something if it gets reused */
   iq = doc.append_child("iq");
   iq.append_attribute("type").set_value("result");
   iq.append_attribute("from").set_value(auth->base_jid);
   iq.append_attribute("to").set_value(query.parent().attribute("from").value());
   iq.append_attribute("id").set_value(query.parent().attribute("id").value());
   node = iq.append_child("query");
   node.append_attribute("xmlns").set_value(XML_NS_DISCO_INFO);
   node.append_child("feature").append_attribute("var").set_value(XML_NS_DISCO_INFO); /* yay recursion */
   node.append_child("feature").append_attribute("var").set_value(XML_NS_FILETRANSFER);
   node.append_child("feature").append_attribute("var").set_value(XML_NS_FEATURE_NEG);
   node.append_child("feature").append_attribute("var").set_value(XML_NS_BYTESTREAM);
   node.append_child("feature").append_attribute("var").set_value(XML_NS_IBB);
   node.append_child("feature").append_attribute("var").set_value(XML_NS_PING);

   xml = xmlnode_to_buf(doc, &len, EINA_FALSE);
   shotgun_write(auth->svr, xml, len);
   free(xml);

   ret = static_cast<Shotgun_Event_Iq*>(calloc(1, sizeof(Shotgun_Event_Iq)));
   ret->type = SHOTGUN_IQ_EVENT_TYPE_SERVER_QUERY;
   ret->account = auth;
   return ret;
}

static Shotgun_Event_Iq *
xml_iq_pong_write(Shotgun_Auth *auth, xml_node &query)
{
   Shotgun_Event_Iq *ret;
   xml_document doc;
   xml_node iq;
   char *xml;
   size_t len;

   /* TODO: this setup should probably be a macro or something if it gets reused */
   iq = doc.append_child("iq");
   iq.append_attribute("type").set_value("result");
   iq.append_attribute("from").set_value(auth->base_jid);
   iq.append_attribute("to").set_value(query.parent().attribute("from").value());
   iq.append_attribute("id").set_value(query.parent().attribute("id").value());

   xml = xmlnode_to_buf(doc, &len, EINA_FALSE);
   shotgun_write(auth->svr, xml, len);
   free(xml);

   ret = static_cast<Shotgun_Event_Iq*>(calloc(1, sizeof(Shotgun_Event_Iq)));
   ret->type = SHOTGUN_IQ_EVENT_TYPE_PING;
   ret->account = auth;
   return ret;
}

Eina_Bool
xml_iq_ping_write(Shotgun_Auth *auth)
{
   xml_document doc;
   xml_node iq, node;
   char *xml;
   size_t len;

   iq = doc.append_child("iq");
   iq.append_attribute("type").set_value("get");
   iq.append_attribute("from").set_value(auth->base_jid);
   iq.append_attribute("to").set_value(auth->svr_name);
   iq.append_attribute("id").set_value("ping1");

   node = iq.append_child("ping");
   node.append_attribute("xmlns").set_value(XML_NS_PING);

   xml = xmlnode_to_buf(doc, &len, EINA_FALSE);
   shotgun_write(auth->svr, xml, len);
   free(xml);
   return EINA_TRUE;
}

static Shotgun_Event_Iq *
xml_iq_disco_info_read(Shotgun_Auth *auth, xml_node &query)
{
   Shotgun_Event_Iq *ret;
   for (xml_node it = query.first_child(); it; it = it.next_sibling())
     {
        const char *s;

        s = it.name();
        if ((!s) || strcmp(s, "feature")) continue;
        s = it.attribute("var").value();
        if ((!s) || (!s[0])) continue;
        INF("SERVER FEATURE: %s", s);
        if (!strcmp(s, XML_NS_ARCHIVE_MANAGE))
          auth->features.archive_management = EINA_TRUE;
        else if (!strcmp(s, XML_NS_GOOGLE_SETTINGS))
          auth->features.google_settings = EINA_TRUE;
        else if (!strcmp(s, XML_NS_GOOGLE_NOSAVE))
          auth->features.google_nosave = EINA_TRUE;
        else if (!strcmp(s, XML_NS_PING))
          {
             auth->features.ping = EINA_TRUE;
             shotgun_ping_start(auth);
          }
     }
   ret = static_cast<Shotgun_Event_Iq*>(calloc(1, sizeof(Shotgun_Event_Iq)));
   ret->type = SHOTGUN_IQ_EVENT_TYPE_SERVER_QUERY;
   ret->account = auth;
   return ret;
}

static Shotgun_Event_Iq *
xml_iq_filetransfer_read(Shotgun_Auth *auth, xml_node &si)
{
/*
<iq type='set' id='offer1' to='receiver@jabber.org/resource'>
  <si xmlns='http://jabber.org/protocol/si' 
      id='a0'
      mime-type='text/plain'
      profile='http://jabber.org/protocol/si/profile/file-transfer'>
    <file xmlns='http://jabber.org/protocol/si/profile/file-transfer'
          name='test.txt'
          size='1022'/>
    <feature xmlns='http://jabber.org/protocol/feature-neg'>
      <x xmlns='jabber:x:data' type='form'>
        <field var='stream-method' type='list-single'>
          <option><value>http://jabber.org/protocol/bytestreams</value></option>
          <option><value>http://jabber.org/protocol/ibb</value></option>
        </field>
      </x>
    </feature>
  </si>
</iq>

*/
   Shotgun_Event_Iq *ret;
   Shotgun_Incoming_File *file;

   file = static_cast<Shotgun_Incoming_File*>(calloc(1, sizeof(Shotgun_Incoming_File)));
   file->from = eina_stringshare_add(si.parent().attribute("from").value());
   file->id = eina_stringshare_add(si.parent().attribute("id").value());
   file->sid = eina_stringshare_add(si.attribute("id").value());
   file->name = eina_stringshare_add(si.first_child().attribute("name").value());
   file->size = atoi(si.first_child().attribute("size").value());

   ret = static_cast<Shotgun_Event_Iq*>(calloc(1, sizeof(Shotgun_Event_Iq)));
   ret->type = SHOTGUN_IQ_EVENT_TYPE_FILE_TRANSFER;
   ret->ev = file;
   ret->account = auth;
   return ret;
}

static Shotgun_Event_Iq *
xml_iq_activity_read(Shotgun_Auth *auth, xml_node &query)
{
/*
<iq from='juliet@capulet.com'
    id='last1'
    to='romeo@montague.net/orchard'
    type='result'>
   <query xmlns='jabber:iq:last' seconds='903'/>
</iq>

<iq from='juliet@capulet.com'
    id='last1'
    to='romeo@montague.net/orchard'
    type='result'>
   <query xmlns='jabber:iq:last' seconds='903'>Heading Home</query>
</iq>
*/
   Shotgun_Event_Iq *ret;
   Shotgun_Iq_Last *idle;

   idle = static_cast<Shotgun_Iq_Last*>(calloc(1, sizeof(Shotgun_Iq_Last)));
   idle->last = atoi(query.attribute("seconds").value());
   idle->message = eina_stringshare_add(query.first_child().value());
   idle->jid = eina_stringshare_add(query.parent().attribute("from").value());
   ret = static_cast<Shotgun_Event_Iq*>(calloc(1, sizeof(Shotgun_Event_Iq)));
   ret->ev = idle;
   ret->type = SHOTGUN_IQ_EVENT_TYPE_IDLE;
   ret->account = auth;
   return ret;
}

static Shotgun_Event_Iq *
xml_iq_ping_read(Shotgun_Auth *auth, xml_node &iq)
{
/*
<iq from='juliet@capulet.lit/balcony' to='capulet.lit' id='ping1' type='result'/>
*/
   Shotgun_Event_Iq *ret;

   ret = static_cast<Shotgun_Event_Iq*>(calloc(1, sizeof(Shotgun_Event_Iq)));
   ret->type = SHOTGUN_IQ_EVENT_TYPE_PING;
   ret->account = auth;
   return ret;
}

static Shotgun_Event_Iq *
xml_iq_bytestream_read(Shotgun_Auth *auth, xml_node &query)
{
/*
<iq from='target@example.org/bar'
    id='hu3vax16'
    to='requester@example.com/foo'
    type='result'>
  <query xmlns='http://jabber.org/protocol/bytestreams'
         sid='vxf9n471bn46'>
    <streamhost-used jid='requester@example.com/foo'/>
  </query>
</iq>
*/
   Shotgun_Event_Iq *ret;
   Shotgun_Incoming_File *file;
   char *xml;
   const char *type;
   size_t len;

   file = static_cast<Shotgun_Incoming_File*>(calloc(1, sizeof(Shotgun_Incoming_File)));
   file->from = eina_stringshare_add(query.parent().attribute("from").value());
   file->id = eina_stringshare_add(query.parent().attribute("id").value());
   file->sid = eina_stringshare_add(query.first_child().attribute("sid").value());

   ret = static_cast<Shotgun_Event_Iq*>(calloc(1, sizeof(Shotgun_Event_Iq)));
   ret->type = SHOTGUN_IQ_EVENT_TYPE_BYTESTREAM;
   ret->ev = file;
   ret->account = auth;

   xml = xml_iq_write_get_bytestream(shotgun_jid_get(auth), 
                                      file->from, 
                                     file->id,
                                     file->sid, 
                                     &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);

   return ret;
}

static Shotgun_Event_Iq *
xml_iq_ibb_read(Shotgun_Auth *auth, xml_node &query)
{
/*
<iq from='target@example.org/bar'
    id='hu3vax16'
    to='requester@example.com/foo'
    type='result'>
  <query xmlns='http://jabber.org/protocol/bytestreams'
         sid='vxf9n471bn46'>
    <streamhost-used jid='requester@example.com/foo'/>
  </query>
</iq>
*/
   Shotgun_Event_Iq *ret;
   Shotgun_Incoming_File *file;
   char *xml;
   const char *type = NULL;
   size_t len;

   file = static_cast<Shotgun_Incoming_File*>(calloc(1, sizeof(Shotgun_Incoming_File)));
   file->from = eina_stringshare_add(query.parent().attribute("from").value());
   file->id = eina_stringshare_add(query.parent().attribute("id").value());
   file->sid = eina_stringshare_add(query.attribute("sid").value());

   type = query.name();
   if (!strcmp(type, "open"))
     {
        file->status = SHOTGUN_FILE_OPEN;
        file->blocsize = atoi(query.attribute("block-size").value());
     }
   else if(!strcmp(type, "data"))
     {
        file->status = SHOTGUN_FILE_DATA;
        file->data = eina_stringshare_add(query.first_child().value());
     }
   else if (!strcmp(type, "close"))
     file->status = SHOTGUN_FILE_CLOSE;
   else
     file->status = SHOTGUN_FILE_UNKNOWN;

   ret = static_cast<Shotgun_Event_Iq*>(calloc(1, sizeof(Shotgun_Event_Iq)));
   ret->type = SHOTGUN_IQ_EVENT_TYPE_IBB;
   ret->ev = file;
   ret->account = auth;

   //xml_iq_bytestream_result(ret);
   xml = xml_iq_write_get_ibb(shotgun_jid_get(auth), 
                                file->from, 
                              file->id,
                              &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);

   return ret;
}

static Shotgun_Event_Iq *
xml_iq_gsettings_read(Shotgun_Auth *auth, xml_node &usersetting)
{
   Shotgun_Event_Iq *ret;
   for (xml_node it = usersetting.first_child(); it; it = it.next_sibling())
     {
        const char *s;

        s = it.name();
        if (!s) continue;
        if (!strcmp(s, "mailnotifications"))
          auth->features.gsettings.mail_notifications = xmlattr_to_bool(it.first_attribute());
        else if (!strcmp(s, "archivingenabled"))
          auth->features.gsettings.archiving = xmlattr_to_bool(it.first_attribute());
     }
   ret = static_cast<Shotgun_Event_Iq*>(calloc(1, sizeof(Shotgun_Event_Iq)));
   ret->type = SHOTGUN_IQ_EVENT_TYPE_SETTINGS;
   ret->account = auth;
   return ret;
}

static Shotgun_Event_Iq *
xml_iq_settings_read(Shotgun_Auth *auth, xml_node &query)
{
   Shotgun_Event_Iq *ret;
   xml_node it;
   Eina_List *l = NULL;

   it = query.first_child();
   if (it.empty()) return NULL;
   ret = static_cast<Shotgun_Event_Iq*>(calloc(1, sizeof(Shotgun_Event_Iq)));
   ret->type = SHOTGUN_IQ_EVENT_TYPE_OTR_QUERY;
   ret->account = auth;
   for (; it; it = it.next_sibling())
     {
        const char *s;
        Shotgun_User_Setting *sus;

        s = it.name();
        if (!s) continue;
        sus = static_cast<Shotgun_User_Setting*>(malloc(sizeof(Shotgun_User_Setting)));
        sus->jid = eina_stringshare_add(it.attribute("jid").value());
        sus->value = !!xmlattr_to_bool(it.attribute("value"));
        l = eina_list_append(l, sus);
     }
   ret->ev = (Eina_List*)l;
   return ret;
}

static Shotgun_Event_Iq *
xml_iq_vcard_read(Shotgun_Auth *auth, xml_node iq, xml_node node)
{
   Shotgun_Event_Iq *ret;
   Shotgun_User_Info *info;

   ret = static_cast<Shotgun_Event_Iq*>(calloc(1, sizeof(Shotgun_Event_Iq)));
   ret->type = SHOTGUN_IQ_EVENT_TYPE_INFO;
   info = static_cast<Shotgun_User_Info*>(calloc(1, sizeof(Shotgun_User_Info)));
   ret->ev = info;
   ret->account = auth;
   xml_sanitize_from(&info->jid, (char*)iq.attribute("from").value());

   for (xml_node it = node.first_child(); it; it = it.next_sibling())
     {
        if (!strcmp(it.name(), "FN"))
          info->full_name = eina_stringshare_add(it.child_value());
        else if (!strcmp(it.name(), "PHOTO"))
          {
             xml_node n;

             n = it.child("TYPE");
             if (n.empty()) continue;
             info->photo.type = eina_stringshare_add(n.child_value());
             n = it.child("BINVAL");
             if (n.empty())
               {
                  eina_stringshare_del(info->photo.type);
                  info->photo.type = NULL;
                  continue;
               }
             info->photo.data = shotgun_base64_decode(n.child_value(), strlen(n.child_value()), &info->photo.size);
          }
     }
   return ret;
   
}

static Shotgun_Event_Iq *
xml_iq_archive_read(Shotgun_Auth *auth __UNUSED__, xml_node list __UNUSED__)
{
   /* TODO: this */
   return NULL;
}

static Shotgun_Event_Iq *
xml_iq_mailnotification_read(Shotgun_Auth *auth, xml_node node)
{
   Shotgun_Event_Iq *ret;
   char *xml;
   size_t len;

   if (strcmp(node.name(), "new-mail")) return NULL;

   ret = static_cast<Shotgun_Event_Iq*>(calloc(1, sizeof(Shotgun_Event_Iq)));
   ret->type = SHOTGUN_IQ_EVENT_TYPE_MAILNOTIFY;
   ret->account = auth;
/*
<iq type='set'
    from='romeo@gmail.com'
    to='romeo@gmail.com/orchard'
    id='mail-request-2'>
  <new-mail xmlns='google:mail:notify' />
</iq>
*/
   xml = xml_iq_write_preset(auth, SHOTGUN_IQ_PRESET_RESULT, &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
   return ret;
}

char *
xml_iq_activity_query(const char *from, const char *to, const char *id, size_t *len)
{
   xml_document doc;
   xml_node iq, node;

   iq = doc.append_child("iq");
   iq.append_attribute("type").set_value("get");
   iq.append_attribute("from").set_value(from);
   iq.append_attribute("to").set_value(to);
   iq.append_attribute("id").set_value(id);

   node = iq.append_child("query");
   node.append_attribute("xmlns").set_value(XML_NS_IDLE);

   return xmlnode_to_buf(doc, len, EINA_FALSE);
}

Shotgun_Event_Iq *
xml_iq_read(Shotgun_Auth *auth, char *xml, size_t size)
{
   xml_document doc;
   xml_node node;
   xml_parse_result res;
   Shotgun_Iq_Type type;
   const char *str;
   const char *id;

   res = doc.load_buffer_inplace(xml, size, parse_default, encoding_auto);
   if ((res.status != status_ok) && (res.status != status_end_element_mismatch))
     {
        ERR("%s", res.description());
        return NULL;
     }
   type = xml_iq_type_get(doc.first_child());
   node = doc.first_child().first_child();
   str = node.attribute("xmlns").value();
   if ((!str) || (!str[0])) str = node.attribute("xmlns:ros").value(); /* FUCK YOU GOOGLE */
   switch (type)
     {
      case SHOTGUN_IQ_TYPE_RESULT:
        if (!strcmp(str, XML_NS_ROSTER))
          return xml_iq_roster_read(auth, node);
        if (!strcmp(str, XML_NS_VCARD))
          return xml_iq_vcard_read(auth, doc.first_child(), node);
        if (!strcmp(str, XML_NS_BIND))
          auth->bind = eina_stringshare_add(node.child("jid").child_value());
        if (!strcmp(str, XML_NS_ARCHIVE))
          return xml_iq_archive_read(auth, node);
        if (!strcmp(str, XML_NS_DISCO_INFO))
          return xml_iq_disco_info_read(auth, node);
        if (!strcmp(str, XML_NS_GOOGLE_SETTINGS))
          return xml_iq_gsettings_read(auth, node);
        if (!strcmp(str, XML_NS_GOOGLE_NOSAVE))
          return xml_iq_settings_read(auth, node);
        if (!strcmp(str, XML_NS_IDLE))
          return xml_iq_activity_read(auth, node);

        /**
         * Some IQ tags don't have a xmlns property (ie XEP-0199 ping answer)
         * We'll have to compare the ID to previously sent IQ's ID in order to 
         * identify it.
         */
        id = doc.first_child().attribute("id").value();
        if ( !strncmp(id, "ping", 4))
            shotgun_ping_received(auth);
        break;
      case SHOTGUN_IQ_TYPE_GET:
        if (!strcmp(str, XML_NS_DISCO_INFO))
          return xml_iq_disco_info_write(auth, node);
        if (!strcmp(str, XML_NS_PING))
            return xml_iq_pong_write(auth, node);
        break;
      case SHOTGUN_IQ_TYPE_SET:
        if (!strcmp(str, XML_NS_ROSTER))
          return xml_iq_roster_read(auth, node);
        if (!strcmp(str, XML_NS_GOOGLE_SETTINGS))
          return xml_iq_gsettings_read(auth, node);
        if (!strcmp(str, XML_NS_GOOGLE_MAILNOTIFY))
          return xml_iq_mailnotification_read(auth, node);
        if (!strcmp(str, XML_NS_SI))
          return xml_iq_filetransfer_read(auth, node);
        if (!strcmp(str, XML_NS_BYTESTREAM))
          return xml_iq_bytestream_read(auth, node);
        if (!strcmp(str, XML_NS_IBB))
          return xml_iq_ibb_read(auth, node);
        INF("UNHANDLED SET NS: %s", str);
        break;
      default:
        str = node.next_sibling().first_child().name();
        ERR("ERROR: %s", str ?: "NO ERROR RETURNED!");
        auth->error = eina_stringshare_add(str);
     }
   return NULL;
}

char *
xml_message_write(Shotgun_Auth *auth __UNUSED__, const char *to, const char *msg, Shotgun_Message_Status status, size_t *len)
{
/*
C: <message from='juliet@im.example.com/balcony'
            id='ju2ba41c'
            to='romeo@example.net'
            type='chat'
            xml:lang='en'>
     <body>Art thou not Romeo, and a Montague?</body>
     <active xmlns='http://jabber.org/protocol/chatstates'/>
   </message>
*/

   xml_document doc;
   xml_node node, body;
   node = doc.append_child("message");
   node.append_attribute("to").set_value(to);
   node.append_attribute("type").set_value("chat");
   node.append_attribute("xml:lang").set_value("en");

   if (msg)
     {
        body = node.append_child("body");
        body.append_child(node_pcdata).set_value(msg);
     }
   switch (status)
     {
      case SHOTGUN_MESSAGE_STATUS_ACTIVE:
        node = node.append_child("active");
        break;
      case SHOTGUN_MESSAGE_STATUS_COMPOSING:
        node = node.append_child("composing");
        break;
      case SHOTGUN_MESSAGE_STATUS_PAUSED:
        node = node.append_child("paused");
        break;
      case SHOTGUN_MESSAGE_STATUS_INACTIVE:
        node = node.append_child("inactive");
        break;
      case SHOTGUN_MESSAGE_STATUS_GONE:
        node = node.append_child("gone");
        break;
      default:
        return xmlnode_to_buf(doc, len, EINA_FALSE);
     }
   node.append_attribute("xmlns").set_value(XML_NS_CHATSTATES);

   return xmlnode_to_buf(doc, len, EINA_FALSE);
}

Shotgun_Event_Message *
xml_message_read(Shotgun_Auth *auth, char *xml, size_t size)
{
/*
E: <message from='romeo@example.net/orchard'
            id='ju2ba41c'
            to='juliet@im.example.com/balcony'
            type='chat'
            xml:lang='en'>
     <body>Neither, fair saint, if either thee dislike.</body>
   </message>
*/
   xml_document doc;
   xml_node node;
   xml_attribute attr;
   xml_parse_result res;
   Shotgun_Event_Message *ret;
   const char *msg;

   res = doc.load_buffer_inplace(xml, size, parse_default, encoding_auto);
   if ((res.status != status_ok) && (res.status != status_end_element_mismatch))
     {
        ERR("%s", res.description());
        return NULL;
     }

   node = doc.first_child();
   if (strcmp(node.name(), "message"))
     {
        ERR("Not a message tag: %s", node.name());
        return NULL;
     }
   ret = shotgun_message_new(auth);
   for (attr = node.first_attribute(); attr; attr = attr.next_attribute())
     {
        if (!strcmp(attr.name(), "from"))
          {
             xml_sanitize_from(&ret->jid, (char*)attr.value());
             break;
          }
     }
   for (xml_node it = node.first_child(); it; it = it.next_sibling())
     {
        const char *name;

        name = it.name();
        if (!strncmp(name, "cha:", 4))
          name += 4;
        if (!strcmp(name, "body"))
          {
             msg = it.child_value();
             if (msg && msg[0]) ret->msg = strdup(msg);
          }
        else if (!strcmp(name, "active"))
          ret->status = SHOTGUN_MESSAGE_STATUS_ACTIVE;
        else if (!strcmp(name, "composing"))
          ret->status = SHOTGUN_MESSAGE_STATUS_COMPOSING;
        else if (!strcmp(name, "paused"))
          ret->status = SHOTGUN_MESSAGE_STATUS_PAUSED;
        else if (!strcmp(name, "inactive"))
          ret->status = SHOTGUN_MESSAGE_STATUS_INACTIVE;
        else if (!strcmp(name, "gone"))
          ret->status = SHOTGUN_MESSAGE_STATUS_GONE;
        else if (!strcmp(name, "nos:x"))
          {
             const char *s;

             attr = it.attribute("xmlns:nos");
             if (attr.empty()) continue;
             s = attr.value();
             if (!s) continue;
             /* TODO: other "x" attrs */
             if (!strcmp(s, XML_NS_GOOGLE_NOSAVE))
               ret->otr_enabled += !!xmlattr_to_bool(attr) + 1;
          }
     }
   return ret;
}

char *
xml_presence_write_subscription(const char *jid, Eina_Bool subscribe, size_t *len)
{
/*
<presence to='contact@example.org' type='unsubscribe'/>
<presence to='user@example.com' type='subscribe'/>
*/
   xml_document doc;
   xml_node node;
   xml_attribute attr;

   node = doc.append_child("presence");
   node.append_attribute("to").set_value(jid);
   attr = node.append_attribute("type");
   if (subscribe)
     attr.set_value("subscribe");
   else
     attr.set_value("unsubscribe");
   return xmlnode_to_buf(doc, len, EINA_FALSE);
}

char *
xml_presence_write_subscription_answer(const char *jid, Eina_Bool subscribed, size_t *len)
{
/*
<presence to='contact@example.org' type='unsubscribed'/>
<presence to='user@example.com' type='subscribed'/>
*/
   xml_document doc;
   xml_node node;
   xml_attribute attr;

   node = doc.append_child("presence");
   node.append_attribute("to").set_value(jid);
   attr = node.append_attribute("type");
   if (subscribed)
     attr.set_value("subscribed");
   else
     attr.set_value("unsubscribed");
   return xmlnode_to_buf(doc, len, EINA_FALSE);
}

char *
xml_presence_write(Shotgun_Auth *auth, size_t *len)
{
/*
<presence xml:lang='en'>
  <show>dnd</show>
  <status>Wooing Juliet</status>
  <status xml:lang='cz'>Ja dvo&#x0159;&#x00ED;m Juliet</status>
  <priority>1</priority>
</presence>
*/
   xml_document doc;
   xml_node node, show;
   Shotgun_User_Info *info;
   char buf[64];

   node = doc.append_child("presence");
   node.append_attribute("xml:lang").set_value("en");
   if (auth->status != SHOTGUN_USER_STATUS_NORMAL)
     show = node.append_child("show").append_child(node_pcdata);
   else if (!auth->status)
     node.append_attribute("type").set_value("unavailable");
   switch (auth->status)
     {
      case SHOTGUN_USER_STATUS_AWAY:
        show.set_value("away");
        break;
      case SHOTGUN_USER_STATUS_CHAT:
        show.set_value("chat");
        break;
      case SHOTGUN_USER_STATUS_DND:
        show.set_value("dnd");
        break;
      case SHOTGUN_USER_STATUS_XA:
        show.set_value("xa");
        break;
      default:
        break;
     }
   if (auth->desc) node.append_child("status").append_child(node_pcdata).set_value(auth->desc);
   snprintf(buf, sizeof(buf), "%i", auth->priority);
   node.append_child("priority").append_child(node_pcdata).set_value(buf);
   node = node.append_child("x");
   node.append_attribute("xmlns").set_value("vcard-temp:x:update");
   info = (Shotgun_User_Info *)auth->vcard;
   xml_vcard_write(info, (void *)&node, EINA_TRUE);

   return xmlnode_to_buf(doc, len, EINA_FALSE);
}

void
xml_vcard_write(Shotgun_User_Info *info, void *vcard, Eina_Bool lcasetags)
{
   /**
    * Stupid patch here. When sending vCard within <presence>, the following
    * tags HAVE to be in lower case (even if sending only an empty <photo/>).
    * BUT ! when <iq>, they HAVE to be upper case.
    * If this syntax is wrong, other clients wont receive auto vCard updates, 
    * they'll have to ask for it.
    * So... : lcasetags :(
    */ 
   xml_node *node, fn_node, ph_node, t_node, d_node;
   node = (xml_node *)vcard;
   
   if (!info)
     {
        if (lcasetags) (*node).append_child("photo");
        else (*node).append_child("PHOTO");
        return;
     }

   if (info->full_name)
     {
        if (lcasetags) fn_node = (*node).append_child("fn");
        else fn_node = (*node).append_child("FN");
        fn_node.append_child(node_pcdata).set_value(info->full_name);
     }

   if (lcasetags) ph_node = (*node).append_child("photo");
   else ph_node = (*node).append_child("PHOTO");

   if (info->photo.data)
     {
        if (info->photo.type)
          {
             if (lcasetags) t_node = ph_node.append_child("type");
             else t_node = ph_node.append_child("TYPE");
             t_node.append_child(node_pcdata).set_value(info->photo.type);
          }
        if (lcasetags) d_node = ph_node.append_child("binval");
        else d_node = ph_node.append_child("BINVAL");
        d_node.append_child(node_pcdata).set_value((const char *)(info->photo.data));
     }
}

Shotgun_Event_Presence *
xml_presence_read(Shotgun_Auth *auth, char *xml, size_t size)
{
/*
<presence from='romeo@example.net/orchard'
          type='unavailable'
          xml:lang='en'>
  <status>gone home</status>
</presence>
*/
   xml_document doc;
   xml_node node;
   xml_attribute attr;
   xml_parse_result res;
   Shotgun_Event_Presence *ret;
   const char *desc;

   res = doc.load_buffer_inplace(xml, size, parse_default, encoding_auto);
   if ((res.status != status_ok) && (res.status != status_end_element_mismatch))
     {
        ERR("%s", res.description());
        return NULL;
     }

   node = doc.first_child();
   if (strcmp(node.name(), "presence"))
     {
        ERR("Not a presence tag: %s", node.name());
        return NULL;
     }
   ret = shotgun_presence_new(auth);
   ret->status = SHOTGUN_USER_STATUS_NORMAL;
   for (attr = node.first_attribute(); attr; attr = attr.next_attribute())
     {
        if (!strcmp(attr.name(), "from"))
          xml_sanitize_from(&ret->jid, (char*)attr.value());
        else if (!strcmp(attr.name(), "type"))
          {
             const char *t;

             t = attr.value();
             DBG("presence type: %s", t);
             ret->status = SHOTGUN_USER_STATUS_NONE;
             if (t[0] == 's')   /* Starts with a 's' -> subscribe */
               ret->type = SHOTGUN_PRESENCE_TYPE_SUBSCRIBE;
             else if (t[2] == 's') /** Third char is a 's' -> unsubscribe 
                                     *  Can't check if first char is a 'u' cause
                                     *  it could be "unavailable"
                                     */
                ret->type = SHOTGUN_PRESENCE_TYPE_UNSUBSCRIBE;
             else 
               ret->type = SHOTGUN_PRESENCE_TYPE_UNAVAILABLE;
          }
     }
   for (xml_node it = node.first_child(); it; it = it.next_sibling())
     {
        if (!strcmp(it.name(), "status"))
          {
             desc = it.child_value();
             if (desc && desc[0]) ret->description = eina_stringshare_add(desc);
          }
        else if (!strcmp(it.name(), "query"))
          {
             const char *ns;

             ns = it.attribute("xmlns").value();
             if ((!ns) || (!ns[0])) continue;
             if (strcmp(ns, XML_NS_IDLE)) continue;
             ret->idle = it.attribute("seconds").as_uint();
          }
        else if (!strcmp(it.name(), "show"))
          {
             if (!strcmp(it.child_value(), "away"))
               ret->status = SHOTGUN_USER_STATUS_AWAY;
             else if (!strcmp(it.child_value(), "chat"))
               ret->status = SHOTGUN_USER_STATUS_CHAT;
             else if (!strcmp(it.child_value(), "dnd"))
               ret->status = SHOTGUN_USER_STATUS_DND;
             else if (!strcmp(it.child_value(), "xa"))
               ret->status = SHOTGUN_USER_STATUS_XA;
          }
        else if (!strcmp(it.name(), "priority"))
          ret->priority = strtol(it.child_value(), NULL, 10);
        else if ((!strcmp(it.name(), "x")) || (!strcmp(it.name(), "upd:x"))) /* FUCK YOU GOOGLE */
          {
             const char *ns;

             ns = it.attribute("xmlns").value();
             if ((!ns) || (!ns[0]))
               ns = it.attribute("xmlns:upd").value();
             if ((!ns) || (!ns[0])) continue;
             if (!strncmp(ns, "vcard-temp", sizeof("vcard-temp") - 1))
               {
                  ret->vcard = EINA_TRUE;
                  node = it.child("photo");
                  if (node.empty()) node = it.child("upd:photo");
                  if (!node.empty())
                    {
                       ns = node.child_value();
                       if (ns && *ns)
                         {
                            DBG("Photo sha1: %s", node.child_value());
                            ret->photo = eina_stringshare_add(node.child_value());
                         }
                    }
               }
          }
     }
   ret->timestamp = ecore_time_unix_get();
   return ret;
}
