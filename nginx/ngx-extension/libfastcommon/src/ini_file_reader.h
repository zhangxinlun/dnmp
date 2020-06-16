/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.fastken.com/ for more detail.
**/

//ini_file_reader.h
#ifndef INI_FILE_READER_H
#define INI_FILE_READER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common_define.h"
#include "hash.h"

#define FAST_INI_ITEM_NAME_LEN		63
#define FAST_INI_ITEM_VALUE_LEN		255
#define FAST_INI_ITEM_NAME_SIZE		(FAST_INI_ITEM_NAME_LEN + 1)
#define FAST_INI_ITEM_VALUE_SIZE    (FAST_INI_ITEM_VALUE_LEN + 1)

#define FAST_INI_ANNOTATION_WITHOUT_BUILTIN 0
#define FAST_INI_ANNOTATION_DISABLE         1
#define FAST_INI_ANNOTATION_WITH_BUILTIN    2
#define FAST_INI_ANNOTATION_NONE  FAST_INI_ANNOTATION_DISABLE

#define FAST_INI_FLAGS_NONE                         0
#define FAST_INI_FLAGS_SHELL_EXECUTE                1
#define FAST_INI_FLAGS_DISABLE_SAME_SECTION_MERGE   2

typedef bool (*IniSectionNameFilterFunc)(const char *section_name,
        const int name_len, void *args);

typedef struct ini_item
{
	char name[FAST_INI_ITEM_NAME_SIZE];
	char value[FAST_INI_ITEM_VALUE_SIZE];
} IniItem;

typedef struct ini_section
{
	IniItem *items;
	int count;  //item count
	int alloc;
} IniSection;

typedef struct ini_section_info
{
    char section_name[FAST_INI_ITEM_NAME_SIZE];
    IniSection *pSection;
} IniSectionInfo;

typedef struct ini_context
{
	IniSection global;
	HashArray sections;  //key is session name, and value is IniSection
	IniSection *current_section; //for load from ini file
	char config_path[MAX_PATH_SIZE];  //save the config filepath
    char annotation_type;
    char flags;
} IniContext;

typedef struct ini_annotation_entry {
    char *func_name;
    void *arg;
    void *dlhandle;

    int (*func_init) (struct ini_annotation_entry *annotation);
    void (*func_destroy) (struct ini_annotation_entry *annotation);
    int (*func_get) (IniContext *context,
            struct ini_annotation_entry *annotation,
            const IniItem *item,
            char **pOutValue, int max_values);

    void (*func_free) (struct ini_annotation_entry *annotation,
            char **values, const int count);

    bool inited;
} AnnotationEntry;

#ifdef __cplusplus
extern "C" {
#endif

#define FAST_INI_STRING_IS_TRUE(pValue)  \
    (strcasecmp(pValue, "true") == 0 || \
     strcasecmp(pValue, "yes") == 0 ||  \
     strcasecmp(pValue, "on") == 0 ||   \
     strcmp(pValue, "1") == 0)

int iniSetAnnotationCallBack(AnnotationEntry *annotations, int count);
void iniDestroyAnnotationCallBack();

void iniAnnotationFreeValues(struct ini_annotation_entry *annotation,
        char **values, const int count);

/** load ini items from file
 *  parameters:
 *           szFilename: the filename, can be an URL
 *           pContext: the ini context
 *  return: error no, 0 for success, != 0 fail
*/
int iniLoadFromFile(const char *szFilename, IniContext *pContext);

/** load ini items from file
 *  parameters:
 *           szFilename: the filename, can be an URL
 *           pContext: the ini context
 *           annotation_type: the annotation type
 *           annotations: the annotations, can be NULL
 *           count: the annotation count
 *           flags: the flags
 *  return: error no, 0 for success, != 0 fail
*/
int iniLoadFromFileEx(const char *szFilename, IniContext *pContext,
    const char annotation_type, AnnotationEntry *annotations, const int count,
    const char flags);

/** load ini items from string buffer
 *  parameters:
 *           content: the string buffer to parse
 *           pContext: the ini context
 *  return: error no, 0 for success, != 0 fail
*/
int iniLoadFromBuffer(char *content, IniContext *pContext);

/** load ini items from string buffer
 *  parameters:
 *           content: the string buffer to parse
 *           pContext: the ini context
 *           annotation_type: the annotation type
 *           annotations: the annotations, can be NULL
 *           count: the annotation count
 *           flags: the flags
 *  return: error no, 0 for success, != 0 fail
*/
int iniLoadFromBufferEx(char *content, IniContext *pContext,
    const char annotation_type, AnnotationEntry *annotations, const int count,
    const char flags);

/** free ini context
 *  parameters:
 *           pContext: the ini context
 *  return: none
*/
void iniFreeContext(IniContext *pContext);

/** get item string value
 *  parameters:
 *           szSectionName: the section name, NULL or empty string for 
 *                          global section
 *           szItemName: the item name
 *           pContext:   the ini context
 *  return: item value, return NULL when the item not exist
*/
char *iniGetStrValue(const char *szSectionName, const char *szItemName, \
		IniContext *pContext);

/** get item string value
 *  parameters:
 *           szSectionName: the section name, NULL or empty string for 
 *                          global section
 *           szItemName: the item name
 *           pContext:   the ini context
 *           szValues:   string array to store the values
 *           max_values: max string array elements
 *  return: item value count
*/
int iniGetValues(const char *szSectionName, const char *szItemName, \
		IniContext *pContext, char **szValues, const int max_values);

/** get item int value (32 bits)
 *  parameters:
 *           szSectionName: the section name, NULL or empty string for 
 *                          global section
 *           szItemName: the item name
 *           pContext:   the ini context
 *           nDefaultValue: the default value
 *  return: item value, return nDefaultValue when the item not exist
*/
int iniGetIntValue(const char *szSectionName, const char *szItemName, \
		IniContext *pContext, const int nDefaultValue);

/** get item string value array
 *  parameters:
 *           szSectionName: the section name, NULL or empty string for 
 *                          global section
 *           szItemName: the item name
 *           pContext:   the ini context
 *           nTargetCount: store the item value count
 *  return: item value array, return NULL when the item not exist
*/
IniItem *iniGetValuesEx(const char *szSectionName, const char *szItemName, \
		IniContext *pContext, int *nTargetCount);

/** get item int64 value (64 bits)
 *  parameters:
 *           szSectionName: the section name, NULL or empty string for 
 *                          global section
 *           szItemName: the item name
 *           pContext:   the ini context
 *           nDefaultValue: the default value
 *  return: int64 value, return nDefaultValue when the item not exist
*/
int64_t iniGetInt64Value(const char *szSectionName, const char *szItemName, \
		IniContext *pContext, const int64_t nDefaultValue);

/** get item boolean value
 *  parameters:
 *           szSectionName: the section name, NULL or empty string for 
 *                          global section
 *           szItemName: the item name
 *           pContext:   the ini context
 *           bDefaultValue: the default value
 *  return: item boolean value, return bDefaultValue when the item not exist
*/
bool iniGetBoolValue(const char *szSectionName, const char *szItemName, \
		IniContext *pContext, const bool bDefaultValue);

/** get item double value
 *  parameters:
 *           szSectionName: the section name, NULL or empty string for 
 *                          global section
 *           szItemName: the item name
 *           pContext:   the ini context
 *           dbDefaultValue: the default value
 *  return: item value, return dbDefaultValue when the item not exist
*/
double iniGetDoubleValue(const char *szSectionName, const char *szItemName, \
		IniContext *pContext, const double dbDefaultValue);

/** print all items
 *  parameters:
 *           pContext:   the ini context
 *  return: none
*/
void iniPrintItems(IniContext *pContext);

/** return the path of config filename
 *  parameters:
 *           pContext:   the ini context
 *  return: the config path
*/
static inline const char *iniGetConfigPath(IniContext *pContext)
{
	return pContext->config_path;
}

/** return the section names
 *  parameters:
 *           pContext:   the ini context
 *           sections:   the section array
 *           max_size:   the max size of sections
 *           nCount:     return the section count
 *  return: errno, 0 for success, != 0 for fail
*/
int iniGetSectionNames(IniContext *pContext, IniSectionInfo *sections,
        const int max_size, int *nCount);

/** return the section names
 *  parameters:
 *           pContext:   the ini context
 *           prefix:     the prefix of section name
 *           sections:   the section array
 *           max_size:   the max size of sections
 *           nCount:     return the section count
 *  return: errno, 0 for success, != 0 for fail
*/
int iniGetSectionNamesByPrefix(IniContext *pContext, const char *szPrefix,
        IniSectionInfo *sections, const int max_size, int *nCount);

/** return the section names
 *  parameters:
 *           pContext:    the ini context
 *           filter_func: the section name filter function
 *           args:        the extra data pointer
 *           sections:    the section array
 *           max_size:    the max size of sections
 *           nCount:      return the section count
 *  return: errno, 0 for success, != 0 for fail
*/
int iniGetSectionNamesEx(IniContext *pContext, IniSectionNameFilterFunc
        filter_func, void *args, IniSectionInfo *sections,
        const int max_size, int *nCount);

/** get matched section count
 *  parameters:
 *           pContext:    the ini context
 *           filter_func: the section name filter function
 *           args:        the extra data pointer
 *  return: matched section count
*/
int iniGetSectionCountEx(IniContext *pContext, IniSectionNameFilterFunc
        filter_func, void *args);

/** get matched section count
 *  parameters:
 *           pContext:    the ini context
 *           prefix:     the prefix of section name
 *  return: matched section count
*/
int iniGetSectionCountByPrefix(IniContext *pContext, const char *szPrefix);

/** get section count
 *  parameters:
 *           pContext:    the ini context
 *  return: section count
*/
static inline int iniGetSectionCount(IniContext *pContext)
{
    return pContext->sections.item_count;
}

/** return the items of global section
 *  parameters:
 *           pContext:   the ini context
 *           nCount:     return the item count
 *  return: the global items
*/
static inline IniItem *iniGetGlobalItems(IniContext *pContext, int *nCount)
{
    *nCount = pContext->global.count;
    return pContext->global.items;
}

/** return the section items
 *  parameters:
 *           szSectionName: the section name, NULL or empty string for
 *                          global section
 *           pContext:   the ini context
 *           nCount:     return the item count
 *  return: the section items, NULL for not exist
*/
IniItem *iniGetSectionItems(const char *szSectionName, IniContext *pContext,
        int *nCount);

/** get item string value
 *  parameters:
 *           szSectionName: the section name, NULL or empty string for
 *                          global section
 *           szItemName: the item name
 *           pContext:   the ini context
 *           nMinLength: the min value length
 *  return: item value, return NULL when the item not exist
*/
char *iniGetRequiredStrValueEx(const char *szSectionName, const char *szItemName,
		IniContext *pContext, const int nMinLength);

static inline char *iniGetRequiredStrValue(const char *szSectionName,
        const char *szItemName, IniContext *pContext)
{
    const int nMinLength = 1;
    return iniGetRequiredStrValueEx(szSectionName, szItemName, pContext, nMinLength);
}

#ifdef __cplusplus
}
#endif

#endif

