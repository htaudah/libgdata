#ifndef GDATA_DOCUMENTS_CHANGE_ENTRY_H
#define GDATA_DOCUMENTS_CHANGE_ENTRY_H

G_BEGIN_DECLS

#define GDATA_TYPE_DOCUMENTS_CHANGE_ENTRY gdata_documents_change_entry_get_type ()
G_DECLARE_FINAL_TYPE (GDataDocumentsChangeEntry, gdata_documents_change_entry, GDATA, 
                      DOCUMENTS_CHANGE_ENTRY, GDATA_TYPE_ENTRY)

GDataDocumentsChangeEntry *gdata_documents_change_entry_new (void);
GDataDocumentsDocument *gdata_documents_change_entry_get_file (GDataDocumentsChangeEntry *entry);

G_END_DECLS

#endif
