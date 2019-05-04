#ifndef H_DOCUMENTS_CHANGE_ENTRY
#define H_DOCUMENTS_CHANGE_ENTRY

G_BEGIN_DECLS

#define GDATA_TYPE_DOCUMENTS_CHANGE_ENTRY gdata_documents_change_entry_get_type ()
G_DECLARE_FINAL_TYPE (GDataDocumentsChangeEntry, gdata_documents_change_entry, GDATA, 
                      DOCUMENTS_CHANGE_ENTRY, GDATA_TYPE_ENTRY)

GDataDocumentsChangeEntry *gdata_documents_change_entry_new (void);

G_END_DECLS

#endif
