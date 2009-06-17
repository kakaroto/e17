#include <stdio.h>
#include <string.h>

#include "eke_feed_item.h"
#include "eke_macros.h"

Eke_Feed_Item *
eke_feed_item_new(void)
{
    Eke_Feed_Item *item;
    item = NEW(Eke_Feed_Item, 1);
    return item;
}

void
eke_feed_item_free(Eke_Feed_Item *item)
{
    if (!item) return;
    IF_FREE(item->title);
    IF_FREE(item->link);
    IF_FREE(item->desc);
    IF_FREE(item->date);
    FREE(item);
}

void
eke_feed_item_title_set(Eke_Feed_Item *item, const char *title)
{
    if (!item || !title) return;
    IF_FREE(item->title);
    item->title = strdup(title);
}

void
eke_feed_item_link_set(Eke_Feed_Item *item, const char *link)
{
    if (!item || !link) return;
    IF_FREE(item->link);
    item->link = strdup(link);
}

void
eke_feed_item_description_set(Eke_Feed_Item *item, const char *desc)
{
    if (!item || !desc) return;
    IF_FREE(item->desc);
    item->desc = strdup(desc);
}

void
eke_feed_item_date_set(Eke_Feed_Item *item, const char *date)
{
    if (!item || !date) return;
    IF_FREE(item->date);
    item->date = strdup(date);
}


