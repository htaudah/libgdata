#include "gdata-documents-change-feed.h"

static gboolean parse_json (GDataParsable *parsable, JsonReader *reader, gpointer user_data, GError **error);
static gboolean post_parse_json (GDataParsable *parsable, gpointer user_data, GError **error);

struct _GDataDocumentsChangeFeed
{
    GDataFeed *parent_instance;

    gchar *new_start_page_token;
};

G_DEFINE_TYPE (GDataDocumentsChangeFeed, gdata_documents_change_feed, GDATA_TYPE_FEED)

static void
gdata_documents_change_feed_class_init (GDataDocumentsChangeFeedClass *klass)
{
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);
	parsable_class->parse_json = parse_json;

    klass->get_next_page_token = gdata_documents_change_feed_real_get_next_page_token;
}

static void
get_change_type (JsonReader *reader, gchar **out_change_type, GError **error)
{
	GError *child_error = NULL;
	gboolean success;
	gchar *change_type = NULL;
	guint i, members;

	for (i = 0, members = (guint) json_reader_count_members (reader); i < members; i++) {
		json_reader_read_element (reader, i);

		if (gdata_parser_string_from_json_member (reader, "changeType", P_REQUIRED | P_NON_EMPTY, &change_type, &success, &child_error) == TRUE) {
			if (!success && child_error != NULL) {
				g_propagate_prefixed_error (error, child_error,
				                            /* Translators: the parameter is an error message */
				                            _("Error parsing JSON: %s"),
				                            "Failed to find ‘changeType’.");
				json_reader_end_element (reader);
				goto out;
			}
		}

		json_reader_end_element (reader);
	}

	if (out_change_type != NULL) {
		*out_change_type = change_type;
		change_type = NULL;
	}

 out:
	g_free (change_type);
}

static gboolean
parse_json (GDataParsable *parsable, JsonReader *reader, gpointer user_data, GError **error)
{
	/* JSON format: https://developers.google.com/drive/v2/reference/changes/list */

	if (g_strcmp0 (json_reader_get_member_name (reader), "items") == 0) {
		gboolean success = TRUE;
		guint i, elements;

		if (json_reader_is_array (reader) == FALSE) {
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
			             /* Translators: the parameter is an error message */
			             _("Error parsing JSON: %s"),
			             "JSON node ‘items’ is not an array.");
			return FALSE;
		}

		/* Loop through the elements array. */
		for (i = 0, elements = (guint) json_reader_count_elements (reader); success && i < elements; i++) {
			GDataEntry *entry = NULL;
			GError *child_error = NULL;
			GType entry_type = G_TYPE_INVALID;
			gchar *change_type = NULL;

			json_reader_read_element (reader, i);

			if (json_reader_is_object (reader) == FALSE) {
				g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
				             /* Translators: the parameter is an error message */
				             _("Error parsing JSON: %s"),
				             "JSON node inside ‘items’ is not an object");
				success = FALSE;
				goto continuation;
			}

			get_change_type (reader, &change_type, &child_error);
			if (child_error != NULL) {
				g_propagate_error (error, child_error);
				success = FALSE;
				goto continuation;
			}

			if (g_strcmp0 (change_type, "file") == 0) {
				entry_type = GDATA_TYPE_DOCUMENTS_DOCUMENT;
			} else {
				g_warning ("%s changes are not handled yet", change_type);
			}

			if (entry_type == G_TYPE_INVALID)
				goto continuation;

			entry = GDATA_ENTRY (_gdata_parsable_new_from_json_node (entry_type, reader, NULL, error));
			/* Call the progress callback in the main thread */
			_gdata_feed_call_progress_callback (GDATA_FEED (parsable), user_data, entry);
			_gdata_feed_add_entry (GDATA_FEED (parsable), entry);

		continuation:
			g_clear_object (&entry);
			g_free (change_type);
			json_reader_end_element (reader);
		}

		return success;
	} else if (g_strcmp0 (json_reader_get_member_name (reader), "newStartPageToken") == 0) {
		GDATA_DOCUMENTS_CHANGE_FEED (parsable)->new_start_page_token = g_strdup (json_reader_get_string_value (reader));
        return TRUE;
    }

	return GDATA_PARSABLE_CLASS (gdata_documents_feed_parent_class)->parse_json (parsable, reader, user_data, error);
}
