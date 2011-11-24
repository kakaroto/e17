#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <map>
#include <list>
#include <string>

#include "dbus_introspect.h"

static int elev8_log_domain;


namespace dbus_library {

Parser* ParserNew() {
  Parser *parser = new Parser;
  parser->root_node = NULL;
  parser->current_node = NULL;
  parser->is_on_node = false;
  parser->current_interface = NULL;
  parser->is_on_interface = false;
  parser->current_method = NULL;
  parser->is_on_method = false;
  parser->current_signal = NULL;
  parser->is_on_signal = false;
  parser->current_argument = NULL;
  parser->is_on_argument = false;

  return parser;
}

void ParserPrint(Parser *parser) {
  BusNode *root_node = parser->root_node;
  EINA_LOG_DOM_INFO(elev8_log_domain,"node");
  std::list<BusInterface*>::iterator interface_ite;
  for (interface_ite = root_node->interfaces_.begin();
        interface_ite != root_node->interfaces_.end();
        interface_ite++) {
    BusInterface *interface = *interface_ite;
    EINA_LOG_DOM_INFO(elev8_log_domain,"  interface: %s", interface->name_.c_str());
    
    std::list<BusMethod*>::iterator method_ite;
    for(method_ite = interface->methods_.begin();
          method_ite != interface->methods_.end();
          method_ite++) {
      BusMethod *method = *method_ite;
      EINA_LOG_DOM_INFO(elev8_log_domain,"    method:%s , %s", method->name_.c_str(), method->signature_.c_str());

      std::list<BusArgument*>::iterator argument_ite;
      for(argument_ite = method->args_.begin();
            argument_ite != method->args_.end();
            argument_ite++) {
        BusArgument *argument = *argument_ite;
        EINA_LOG_DOM_INFO(elev8_log_domain,"      arg:%s - %s", argument->direction_.c_str(), argument->type_.c_str());
      }
    }

    std::list<BusSignal*>::iterator signal_ite;
    for(signal_ite = interface->signals_.begin();
          signal_ite != interface->signals_.end();
          signal_ite++) {
      BusSignal *signal = *signal_ite;
      EINA_LOG_DOM_INFO(elev8_log_domain,"    signal:%s", signal->name_.c_str());

      std::list<BusArgument*>::iterator argument_ite;
      for(argument_ite = signal->args_.begin();
            argument_ite != signal->args_.end();
            argument_ite++) {
        BusArgument *argument = *argument_ite;
        EINA_LOG_DOM_INFO(elev8_log_domain,"      arg:%s", argument->type_.c_str());
      }
    }
  }
}

void ParserRelease(Parser** pparser) {
  Parser *parser = *pparser; 
  
  BusNode *root_node = parser->root_node;

  std::list<BusInterface*>::iterator interface_ite;
  for (interface_ite = root_node->interfaces_.begin();
        interface_ite != root_node->interfaces_.end();
        interface_ite++) {
    BusInterface *interface = *interface_ite;
    
    std::list<BusMethod*>::iterator method_ite;
    for(method_ite = interface->methods_.begin();
          method_ite != interface->methods_.end();
          method_ite++) {
      BusMethod *method = *method_ite;

      std::list<BusArgument*>::iterator argument_ite;
      for(argument_ite = method->args_.begin();
            argument_ite != method->args_.end();
            argument_ite++) {
        BusArgument *argument = *argument_ite;
        delete argument;
      }

      delete method;
    }

    std::list<BusSignal*>::iterator signal_ite;
    for(signal_ite = interface->signals_.begin();
          signal_ite != interface->signals_.end();
          signal_ite++) {
      BusSignal *signal = *signal_ite;

      std::list<BusArgument*>::iterator argument_ite;
      for(argument_ite = signal->args_.begin();
            argument_ite != signal->args_.end();
            argument_ite++) {
        BusArgument *argument = *argument_ite;
        delete argument;
      }
      delete signal;
    }

    delete interface;
  }
 
  delete parser;
  pparser = NULL;
}

static const char* GetAttribute(const char **attrs, const char *name) {
  int i=0;
  while(attrs[i] != NULL) {
    if( !strcasecmp(attrs[i], name) ) {
      return attrs[i+1];
    }
    i+=2;
  }
  return "";
}

void expat_StartElementHandler(void *userData,
              const XML_Char *name, const XML_Char **attrs) {
  EINA_LOG_DOM_INFO(elev8_log_domain,"Node: %s", name);
  Parser *parser = reinterpret_cast<Parser*>(userData);

  if (!strcmp(name, "node")) {
    //the node object is not the first node or root node,then skip
    if (parser->root_node != NULL) {
      return;
    }
    BusNode *node = new BusNode;
    parser->is_on_node = true;
    parser->current_node = node;
    parser->root_node = node;

  } else if (!strcmp(name, "interface")) {
    BusInterface *interface = new BusInterface;
    parser->is_on_interface = true;
    parser->current_interface = interface;
    parser->current_node->interfaces_.push_back(interface);
    
    interface->name_ = GetAttribute(attrs, "name");

  } else if (!strcmp(name, "method")) {
    BusMethod *method = new BusMethod;
    parser->is_on_method = true;
    parser->current_method = method;
    parser->current_interface->methods_.push_back(method); 
  
    method->name_ = GetAttribute(attrs, "name");

  } else if (!strcmp(name, "signal")) {
    BusSignal *signal = new BusSignal;
    parser->is_on_signal = true;
    parser->current_signal = signal;
    parser->current_interface->signals_.push_back(signal);

    signal->name_ = GetAttribute(attrs, "name");

  } else if (!strcmp(name, "arg")) {
    BusArgument *argument = new BusArgument;
    parser->is_on_argument = true;
    parser->current_argument = argument;

    argument->type_ = GetAttribute(attrs, "type");
    argument->direction_ = GetAttribute(attrs, "direction");

    if (parser->is_on_method) {
      parser->current_method->args_.push_back(argument);
      if (argument->direction_ == "in")
        parser->current_method->signature_ += argument->type_;
    } else if (parser->is_on_signal) {
      parser->current_signal->args_.push_back(argument);
    }
    
  } else {
    EINA_LOG_DOM_INFO(elev8_log_domain,  "Error NodeType");
  }
}

void expat_EndElementHandler(void *userData,
              const XML_Char *name ) {
  EINA_LOG_DOM_INFO(elev8_log_domain,"Node: %s", name);
  Parser *parser = reinterpret_cast<Parser*>(userData);
  if (!strcmp(name, "node")) { 
    parser->is_on_node = false;
    parser->current_node = NULL;
  } else if (!strcmp(name, "interface")) {
    parser->is_on_interface = false;
    parser->current_interface = NULL;
  } else if (!strcmp(name, "method")) {
    parser->is_on_method = false;
    parser->current_method = NULL;
  } else if (!strcmp(name, "signal")) {
    parser->is_on_signal = false;
    parser->current_signal = NULL;
  } else if (!strcmp(name, "arg")) {
    parser->is_on_argument = false;
    parser->current_argument = NULL;
  } else {
    EINA_LOG_DOM_INFO(elev8_log_domain,"Error end Nodetype");
  }
}

Parser* ParseIntrospcect(const char *source, int size) {
  XML_Parser expat;
  int userdata;
  Parser *parser = ParserNew();

  expat = XML_ParserCreate(NULL);
  XML_SetUserData(expat, parser);
  XML_SetElementHandler(expat, 
              expat_StartElementHandler,
              expat_EndElementHandler);

  if (!XML_Parse(expat, source, size, true)) {
     {
          enum XML_Error e;

          e = XML_GetErrorCode (expat);
          if (e == XML_ERROR_NO_MEMORY)
            EINA_LOG_DOM_ERR(elev8_log_domain,"Not enough memory to parse XML document");
          else
            EINA_LOG_DOM_ERR(elev8_log_domain, "Error in D-BUS description XML, line %ld, column %ld: %s",
                         XML_GetCurrentLineNumber (expat),
                         XML_GetCurrentColumnNumber (expat),
                         XML_ErrorString (e));
      }
      ParserRelease(&parser);
      return NULL;
  }
  
  ParserPrint(parser);
  return parser;
}

BusInterface* ParserGetInterface(Parser *parser, const char *iface) {
  if (parser == NULL) {
    return NULL;
  }

  BusNode *root_node = parser->root_node;

  std::list<BusInterface*>::iterator iface_ite;
  for( iface_ite = root_node->interfaces_.begin(); 
        iface_ite != root_node->interfaces_.end();
        iface_ite++) {
    BusInterface *interface = *iface_ite;
    if (interface->name_ == iface)
      return interface;
  }

  return NULL;
}

} //end of namespace

#ifdef __TEST

static char*  ReadFile(const char* name, int *len) {
  FILE *file = fopen(name, "rb");
  if (file == NULL) {
    EINA_LOG_DOM_ERR(elev8_log_domain, "Error open file: %s: %d", name, errno);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  rewind(file);

  char *chars = new char[size+1];
  chars[size] = '\0';

  for(int i=0; i<size;) {
    int read = fread(&chars[i], 1, size-i, file);
    i+=read;
  }

  fclose(file);

  *len = size;
  return chars;
}

int main(int argc, char **argv)
{
  int size;
  char *content = ReadFile("main.xml", &size);
  if (content == NULL)
    exit(1);
  
  XML_Parser expat;
  int userdata;
  Parser *parser = ParserNew();

  expat = XML_ParserCreate(NULL);
  XML_SetUserData(expat, parser);
  XML_SetElementHandler(expat, 
              expat_StartElementHandler,
              expat_EndElementHandler);

  if (!XML_Parse(expat, content, size, true)) {
     {
          enum XML_Error e;

          e = XML_GetErrorCode (expat);
          if (e == XML_ERROR_NO_MEMORY)
            EINA_LOG_DOM_ERR(elev8_log_domain, "Not enough memory to parse XML document");
          else
            EINA_LOG_DOM_ERR(elev8_log_domain, "Error in D-BUS description XML, line %ld, column %ld: %s",
                         XML_GetCurrentLineNumber (expat),
                         XML_GetCurrentColumnNumber (expat),
                         XML_ErrorString (e));
      }
  }
  EINA_LOG_DOM_INFO(elev8_log_domain,"success");

  ParserPrint(parser);

  ParserRelease(&parser);
}
#endif

