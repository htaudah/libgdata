#include <config.h>
#include <glib/gi18n-lib.h>

#include "gdata-documents-change-service.h"
#include "gdata-documents-change-feed.h"
#include "gdata-documents-change-entry.h"
#include "gdata-documents-query.h"
#include "gdata-private.h"

static GList *get_authorization_domains (void);

struct _GDataDocumentsChangeService
{
    GDataService *parent_instance;
};

_GDATA_DEFINE_AUTHORIZATION_DOMAIN (documents, "writely", "https://www.googleapis.com/auth/drive");
G_DEFINE_TYPE (GDataDocumentsChangeService, gdata_documents_change_service, GDATA_TYPE_SERVICE);

static void
gdata_documents_change_service_class_init (GDataDocumentsChangeServiceClass *klass)
{
    GDataServiceClass *service_class = GDATA_SERVICE_CLASS (klass);
    service_class->feed_type = GDATA_TYPE_DOCUMENTS_CHANGE_FEED;
	service_class->get_authorization_domains = get_authorization_domains;
}

static void
gdata_documents_change_service_init (GDataDocumentsChangeService *self)
{
    /* Nothing to init */
}

static GList *
get_authorization_domains (void)
{
	GList *authorization_domains = NULL;

	authorization_domains = g_list_prepend (authorization_domains, get_documents_authorization_domain ());

	return authorization_domains;
}

GDataDocumentsChangeService *
gdata_documents_change_service_new (GDataAuthorizer *authorizer)
{
	g_return_val_if_fail (authorizer == NULL || GDATA_IS_AUTHORIZER (authorizer), NULL);

	return g_object_new (GDATA_TYPE_DOCUMENTS_CHANGE_SERVICE,
	                     "authorizer", authorizer,
	                     NULL);
}

GDataAuthorizationDomain *
gdata_documents_change_service_get_primary_authorization_domain (void)
{
	return get_documents_authorization_domain ();
}

static gchar *
_query_changes_build_request_uri (GDataDocumentsQuery *query)
{
	/* If we want to query for documents contained in a folder, the URI is different.
	 * The "/[folder:id]" suffix is added by the GDataQuery later. */
	return g_strdup ("https://www.googleapis.com/drive/v2/changes");
}

GDataDocumentsChangeFeed *
gdata_documents_service_query_changes (GDataDocumentsChangeService *self, GDataDocumentsQuery *query, GCancellable *cancellable,
                                       GDataQueryProgressCallback progress_callback, gpointer progress_user_data,
                                       GError **error)
{
	GDataFeed *feed;
	gchar *request_uri;

	g_return_val_if_fail (GDATA_IS_DOCUMENTS_CHANGE_SERVICE (self), NULL);
	g_return_val_if_fail (query == NULL || GDATA_IS_DOCUMENTS_QUERY (query), NULL);
	g_return_val_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable), NULL);
	g_return_val_if_fail (error == NULL || *error == NULL, NULL);

	/* Ensure we're authenticated first */
	if (gdata_authorizer_is_authorized_for_domain (gdata_service_get_authorizer (GDATA_SERVICE (self)),
	                                               get_documents_authorization_domain ()) == FALSE) {
		g_set_error_literal (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_AUTHENTICATION_REQUIRED,
		                     _("You must be authenticated to query for document changes."));
		return NULL;
	}

	request_uri = _query_changes_build_request_uri (query);
	feed = gdata_service_query (GDATA_SERVICE (self), get_documents_authorization_domain (), request_uri, GDATA_QUERY (query),
                                GDATA_TYPE_DOCUMENTS_CHANGE_ENTRY, cancellable, progress_callback, progress_user_data, error);
	g_free (request_uri);

	return GDATA_DOCUMENTS_CHANGE_FEED (feed);
}

void
gdata_documents_change_service_query_changes_async (GDataDocumentsChangeService *self, GDataDocumentsQuery *query, GCancellable *cancellable,
                                                    GDataQueryProgressCallback progress_callback, gpointer progress_user_data,
                                                    GDestroyNotify destroy_progress_user_data,
                                                    GAsyncReadyCallback callback, gpointer user_data)
{
	gchar *request_uri;

	g_return_if_fail (GDATA_IS_DOCUMENTS_CHANGE_SERVICE (self));
	g_return_if_fail (query == NULL || GDATA_IS_DOCUMENTS_QUERY (query));
	g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));
	g_return_if_fail (callback != NULL);

	/* Ensure we're authenticated first */
	if (gdata_authorizer_is_authorized_for_domain (gdata_service_get_authorizer (GDATA_SERVICE (self)),
	                                               get_documents_authorization_domain ()) == FALSE) {
		g_autoptr(GTask) task = NULL;

		task = g_task_new (self, cancellable, callback, user_data);
		g_task_set_source_tag (task, gdata_service_query_async);
		g_task_return_new_error (task, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_AUTHENTICATION_REQUIRED, "%s",
		                         _("You must be authenticated to query for document changes."));

		return;
	}

	request_uri = _query_changes_build_request_uri (query);
	gdata_service_query_async (GDATA_SERVICE (self), get_documents_authorization_domain (), request_uri, GDATA_QUERY (query),
	                           GDATA_TYPE_DOCUMENTS_CHANGE_ENTRY, cancellable, progress_callback, progress_user_data,
	                           destroy_progress_user_data, callback, user_data);
	g_free (request_uri);
}
