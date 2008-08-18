/* vim: set sw=4 ts=4 sts=4 expandtab: */

typedef enum Eli_Help_Reader_Node_Type Eli_Help_Reader_Node_Type;

enum Eli_Help_Reader_Node_Type
{
    ELI_HELP_READER_NODE_UNKNOWN,
    ELI_HELP_READER_NODE_HEADER,
    ELI_HELP_READER_NODE_PARAGRAPH,
    ELI_HELP_READER_NODE_BLOCK,
    ELI_HELP_READER_NODE_PREFORMATED,
    ELI_HELP_READER_NODE_LINK,
    ELI_HELP_READER_NODE_EMPHASIZE,
    ELI_HELP_READER_NODE_STRONG,
    ELI_HELP_READER_NODE_TITLE
};

typedef struct Eli_Help_Reader_Node Eli_Help_Reader_Node;

struct Eli_Help_Reader_Node
{
    Eli_Help_Reader_Node_Type type;
    char *parameter;
};

typedef void (*Eli_Help_Reader_Enter)(void *data, 
                                    Eli_Help_Reader_Node *styles, size_t len);
typedef void (*Eli_Help_Reader_Append)(void *data, const char *text,
                                    Eli_Help_Reader_Node *styles, size_t len);
typedef void (*Eli_Help_Reader_Leave)(void *data,
                                    Eli_Help_Reader_Node *styles, size_t len);

int eli_help_reader_parse(const char *path, Eli_Help_Reader_Enter enter,
                            Eli_Help_Reader_Append append,
                            Eli_Help_Reader_Leave leave, void *d);

