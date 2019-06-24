#ifndef GDATA_DOCUMENTS_CHANGE_SERVICE_H
#define GDATA_DOCUMENTS_CHANGE_SERVICE_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-service.h>
#include <gdata/services/documents/gdata-documents-query.h>
#include <gdata/services/documents/gdata-documents-change-feed.h>

G_BEGIN_DECLS

#define GDATA_TYPE_DOCUMENTS_CHANGE_SERVICE gdata_documents_change_service_get_type ()
G_DECLARE_FINAL_TYPE (GDataDocumentsChangeService, gdata_documents_change_service,
        GDATA, DOCUMENTS_CHANGE_SERVICE, GDataService)

G_END_DECLS

GDataDocumentsChangeService *gdata_documents_change_service_new (GDataAuthorizer *authorizer);
GDataAuthorizationDomain *gdata_documents_change_service_get_primary_authorization_domain (void);
GDataDocumentsChangeFeed *gdata_documents_service_query_changes (GDataDocumentsChangeService *self, GDataDocumentsQuery *query, GCancellable *cancellable,
                                                                 GDataQueryProgressCallback progress_callback, gpointer progress_user_data,
                                                                 GError **error);

void gdata_documents_change_service_query_changes_async (GDataDocumentsChangeService *self, GDataDocumentsQuery *query, GCancellable *cancellable,
                                                         GDataQueryProgressCallback progress_callback, gpointer progress_user_data,
                                                         GDestroyNotify destroy_progress_user_data,
                                                         GAsyncReadyCallback callback, gpointer user_data);
#endif
