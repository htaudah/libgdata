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
 * metadata of the file or its content. It contains the updated state of the file following this change, unless the change
 * represents the deletion of the file.
 */
#include "gdata-documents-change-entry.h"

static void gdata_documents_change_entry_constructed (GObject *object);

struct _GDataDocumentsChangeEntry
{
    GDataEntry *parent_instance;

    gchar *fileId;
    gboolean deleted;
    GDataDocumentsDocument *file;
};

G_DEFINE_TYPE (GDataDocumentsChangeEntry, gdata_documents_change_entry, GDATA_TYPE_ENTRY)

void gdata_documents_change_entry_new (const gchar *id)
{
}

static gboolean
parse_json (GDataParsable *parsable, JsonReader *reader, gpointer user_data, GError **error)
{
    GDataDocumentsChangeEntry *entry = GDATA_DOCUMENTS_CHANGE_ENTRY (parsable);
    gboolean success = TRUE;
    gchar *fileId;
    gboolean deleted = FALSE;
    gint64 updated;

    /* JSON format: https://developers.google.com/drive/v2/reference/changes */
    if (gdata_parser_boolean_from_json_member (reader, "deleted", P_DEFAULT, &deleted, &success, error) == TRUE) {
        if (success) {
            entry->deleted = deleted;
        }
        return success;
    } else if (gdata_parser_int64_time_from_json_member (reader, "modificationDate", P_DEFAULT, &updated, &success, error) == TRUE) {
        if (success)
            _gdata_entry_set_updated (GDATA_ENTRY (parsable), updated);
        return success;
    } else if (gdata_parser_string_from_json_member (reader, "fileId", P_DEFAULT, &fileId, &success, error) == TRUE) {
        if (success) {
            entry->fileId = g_strdup (fileId);
        }
        return success;
    } else if (g_strcmp0 (json_reader_get_member_name (reader), "file") == 0) {
        GType element_type = GDATA_TYPE_DOCUMENTS_DOCUMENT;
        GDataDocumentsDocument file;

        json_reader_read_member (reader, "file");
        file = GDATA_DOCUMENTS_DOCUMENT (_gdata_parsable_new_from_json_node (element_type, reader, NULL, error));

        if (file == NULL) {
            json_reader_end_member (reader);
            return FALSE;
        }

        json_reader_end_member (reader);
    }

    return GDATA_PARSABLE_CLASS (gdata_documents_change_entry_parent_class)->parse_json (parsable, reader, user_data, error);
}
