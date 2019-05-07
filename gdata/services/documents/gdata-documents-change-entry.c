/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2008–2010 <philip@tecnocode.co.uk>
 *
 * GData Client is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * GData Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GData Client.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * SECTION:gdata-documents-change-entry
 * @short_description: GData document change entry object
 * @stability: Stable
 * @include: gdata/gdata-documents-change-entry.h
 *
 * #GDataDocumentsChangeEntry represents a change on a single file on the Google Drive service. This change may be to the
 * metadata of the file or its content. It contains the updated state of the file following this change.
 */
#include "gdata-documents-change-entry.h"

static void gdata_documents_change_entry_constructed (GObject *object);

struct _GDataDocumentsChangeEntry
{
    GDataEntry *parent_instance;

    gboolean deleted;
    GDataDocumentsEntry *file;
};

G_DEFINE_TYPE (GDataDocumentsChangeEntry, gdata_documents_change_entry, GDATA_TYPE_ENTRY)

void gdata_documents_change_entry_new (const gchar *id)
{
}

static gboolean
parse_json (GDataParsable *parsable, JsonReader *reader, gpointer user_data, GError **error)
{
    GDataDocumentsChangeEntry *entry = GDATA_DOCUMENTS_CHANGE_ENTRY (parsable);
    GDataDocumentsEntryPrivate *priv = GDATA_DOCUMENTS_ENTRY (parsable)->priv;
    GError *child_error = NULL;
    gboolean shared;
    gboolean success = TRUE;
    gchar *alternate_uri = NULL;
    gchar *kind = NULL;
    gchar *mime_type = NULL;
    gchar *quota_used = NULL;
    gchar *file_size = NULL;
    gint64 published;
    gint64 updated;

    /* JSON format: https://developers.google.com/drive/v2/reference/files */
    if (gdata_parser_boolean_from_json_member (reader, "deleted", P_DEFAULT, &alternate_uri, &success, error) == TRUE) {
        if (success && alternate_uri != NULL && alternate_uri[0] != '\0') {
            GDataLink *_link;

            _link = gdata_link_new (alternate_uri, GDATA_LINK_ALTERNATE);
            gdata_entry_add_link (GDATA_ENTRY (parsable), _link);
            g_object_unref (_link);
        }

        g_free (alternate_uri);
        return success;
    } else if (gdata_parser_string_from_json_member (reader, "mimeType", P_DEFAULT, &mime_type, &success, error) == TRUE) {

    if (gdata_parser_string_from_json_member (reader, "alternateLink", P_DEFAULT, &alternate_uri, &success, error) == TRUE) {
        if (success && alternate_uri != NULL && alternate_uri[0] != '\0') {
            GDataLink *_link;

            _link = gdata_link_new (alternate_uri, GDATA_LINK_ALTERNATE);
            gdata_entry_add_link (GDATA_ENTRY (parsable), _link);
            g_object_unref (_link);
        }

        g_free (alternate_uri);
        return success;
    } else if (gdata_parser_string_from_json_member (reader, "mimeType", P_DEFAULT, &mime_type, &success, error) == TRUE) {
        if (success)
            gdata_documents_utils_add_content_type (GDATA_DOCUMENTS_ENTRY (parsable), mime_type);
        g_free (mime_type);
        return success;
    } else if (gdata_parser_int64_time_from_json_member (reader, "lastViewedByMeDate", P_DEFAULT, &(priv->last_viewed), &success, error) == TRUE ||
               gdata_parser_string_from_json_member (reader, "kind", P_REQUIRED | P_NON_EMPTY, &kind, &success, error) == TRUE) {
        g_free (kind);
        return success;
    } else if (gdata_parser_int64_time_from_json_member (reader, "createdDate", P_DEFAULT, &published, &success, error) == TRUE) {
        if (success)
            _gdata_entry_set_published (GDATA_ENTRY (parsable), published);
        return success;
    } else if (gdata_parser_int64_time_from_json_member (reader, "modifiedDate", P_DEFAULT, &updated, &success, error) == TRUE) {
        if (success)
            _gdata_entry_set_updated (GDATA_ENTRY (parsable), updated);
        return success;
    } else if (gdata_parser_string_from_json_member (reader, "quotaBytesUsed", P_DEFAULT, &quota_used, &success, error) == TRUE) {
        gchar *end_ptr;
        guint64 val;

        /* Even though ‘quotaBytesUsed’ is documented as long,
         * it is actually a string in the JSON.
         */
        val = g_ascii_strtoull (quota_used, &end_ptr, 10);
        if (*end_ptr == '\0')
            priv->quota_used = (goffset) val;
        g_free (quota_used);
        return success;
    } else if (gdata_parser_string_from_json_member (reader, "fileSize", P_DEFAULT, &file_size, &success, error) == TRUE) {
        gchar *end_ptr;
        guint64 val;

        /* like 'quotaBytesUsed', 'fileSize' is also a string
         * in the JSON.
         */
        val = g_ascii_strtoull (file_size, &end_ptr, 10);
        if (*end_ptr == '\0')
            priv->file_size = (goffset) val;
        g_free (file_size);
        return success;
    } else if (gdata_parser_boolean_from_json_member (reader, "shared", P_DEFAULT, &shared, &success, error) == TRUE) {
        if (success && shared) {
            category = gdata_category_new ("http://schemas.google.com/g/2005/labels#shared", GDATA_CATEGORY_SCHEMA_LABELS, "shared");
            gdata_entry_add_category (GDATA_ENTRY (parsable), category);
            g_object_unref (category);
        }
        return success;
    } else if (g_strcmp0 (json_reader_get_member_name (reader), "labels") == 0) {
        guint i, members;

        if (json_reader_is_object (reader) == FALSE) {
            g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
                         /* Translators: the parameter is an error message */
                         _("Error parsing JSON: %s"),
                         "JSON node ‘labels’ is not an object.");
            return FALSE;
        }

        for (i = 0, members = (guint) json_reader_count_members (reader); i < members; i++) {
            gboolean starred;
            gboolean viewed;

            json_reader_read_element (reader, i);

            gdata_parser_boolean_from_json_member (reader, "starred", P_DEFAULT, &starred, &success, NULL);
            if (success && starred) {
                category = gdata_category_new (GDATA_CATEGORY_SCHEMA_LABELS_STARRED, GDATA_CATEGORY_SCHEMA_LABELS, "starred");
                gdata_entry_add_category (GDATA_ENTRY (parsable), category);
                g_object_unref (category);
            }

            gdata_parser_boolean_from_json_member (reader, "viewed", P_DEFAULT, &viewed, &success, NULL);
            if (success && viewed) {
                category = gdata_category_new ("http://schemas.google.com/g/2005/labels#viewed", GDATA_CATEGORY_SCHEMA_LABELS, "viewed");
                gdata_entry_add_category (GDATA_ENTRY (parsable), category);
                g_object_unref (category);
            }

            json_reader_end_element (reader);
        }

        return TRUE;
    } else if (g_strcmp0 (json_reader_get_member_name (reader), "owners") == 0) {
        guint i, elements;

        if (json_reader_is_array (reader) == FALSE) {
            g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
                         /* Translators: the parameter is an error message */
                         _("Error parsing JSON: %s"),
                         "JSON node ‘owners’ is not an array.");
            return FALSE;
        }

        /* Loop through the elements array. */
        for (i = 0, elements = json_reader_count_elements (reader); success && i < elements; i++) {
            gchar *email = NULL;
            gchar *name = NULL;

            json_reader_read_element (reader, i);

            if (json_reader_is_object (reader) == FALSE) {
                g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
                             /* Translators: the parameter is an error message */
                             _("Error parsing JSON: %s"),
                             "JSON node inside ‘owners’ is not an object.");
                success = FALSE;
                goto continue_owners;
            }

            get_kind_email_and_name (reader, &kind, &email, &name, &child_error);
            if (child_error != NULL) {
                g_propagate_error (error, child_error);
                success = FALSE;
                goto continue_owners;
            }
            if (name == NULL || name[0] == '\0') {
                g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
                             /* Translators: the parameter is an error message */
                             _("Error parsing JSON: %s"),
                             "Failed to find ‘displayName’.");
                success = FALSE;
                goto continue_owners;
            }

            if (g_strcmp0 (kind, "drive#user") == 0) {
                GDataAuthor *author;

                author = gdata_author_new (name, NULL, email);
                gdata_entry_add_author (GDATA_ENTRY (parsable), author);
                g_object_unref (author);
            } else {
                g_warning ("%s authors are not handled yet", kind);
            }

continue_owners:
            g_free (email);
            g_free (kind);
            g_free (name);
            json_reader_end_element (reader);
        }

        return success;
    } else if (g_strcmp0 (json_reader_get_member_name (reader), "parents") == 0) {
        guint i, elements;

        if (json_reader_is_array (reader) == FALSE) {
            g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
                         /* Translators: the parameter is an error message */
                         _("Error parsing JSON: %s"),
                         "JSON node ‘parents’ is not an array.");
            return FALSE;
        }

        /* Loop through the elements array. */
        for (i = 0, elements = (guint) json_reader_count_elements (reader); success && i < elements; i++) {
            GDataLink *_link = NULL;
            const gchar *relation_type = NULL;
            gchar *uri = NULL;

            json_reader_read_element (reader, i);

            if (json_reader_is_object (reader) == FALSE) {
                g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
                             /* Translators: the parameter is an error message */
                             _("Error parsing JSON: %s"),
                             "JSON node inside ‘parents’ is not an object.");
                success = FALSE;
                goto continue_parents;
            }

            get_kind_and_parent_link (reader, &kind, &uri, &child_error);
            if (child_error != NULL) {
                g_propagate_error (error, child_error);
                success = FALSE;
                goto continue_parents;
            }

            if (g_strcmp0 (kind, "drive#parentReference") == 0) {
                relation_type = GDATA_LINK_PARENT;
            } else {
                g_warning ("%s parents are not handled yet", kind);
            }

            if (relation_type == NULL)
                goto continue_parents;

            _link = gdata_link_new (uri, relation_type);
            gdata_entry_add_link (GDATA_ENTRY (parsable), _link);

continue_parents:
            g_clear_object (&_link);
            g_free (kind);
            g_free (uri);
            json_reader_end_element (reader);
        }

        return success;
    }

    return GDATA_PARSABLE_CLASS (gdata_documents_entry_parent_class)->parse_json (parsable, reader, user_data, error);
}
