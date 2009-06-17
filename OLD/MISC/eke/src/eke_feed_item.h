#ifndef EKE_FEED_ITEM
#define EKE_FEED_ITEM

typedef struct Eke_Feed_Item Eke_Feed_Item;
struct Eke_Feed_Item {
    char *title;
    char *link;
    char *desc;
    char *date;
};

Eke_Feed_Item *eke_feed_item_new(void);
void eke_feed_item_free(Eke_Feed_Item *item);

void eke_feed_item_title_set(Eke_Feed_Item *item, const char *title);
void eke_feed_item_link_set(Eke_Feed_Item *item, const char *link);
void eke_feed_item_description_set(Eke_Feed_Item *item, const char *desc);
void eke_feed_item_date_set(Eke_Feed_Item *item, const char *date);

#endif

