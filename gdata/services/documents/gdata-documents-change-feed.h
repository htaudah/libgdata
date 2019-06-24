#ifndef GDATA_DOCUMENTS_CHANGE_FEED_H
#define GDATA_DOCUMENTS_CHANGE_FEED_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-feed.h>

G_BEGIN_DECLS

#define GDATA_TYPE_DOCUMENTS_CHANGE_FEED gdata_documents_change_feed_get_type ()
G_DECLARE_FINAL_TYPE (GDataDocumentsChangeFeed, gdata_documents_change_feed,
                      GDATA, DOCUMENTS_CHANGE_FEED, GDataFeed);

const gchar *gdata_documents_change_feed_real_get_next_page_token (GDataFeed *self);

G_END_DECLS

#endif
