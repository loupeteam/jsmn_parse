/* Minimal stub of the AS-generated jsmn_parse.h for off-PLC syntax checking.
   Mirrors Types.typ, Constants.var, and jsmn_parse.fun. */
#ifndef _JSMN_PARSE_H_
#define _JSMN_PARSE_H_

#include <bur/plctypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define JSMN_MAX_TOKENS 128U
#define JSMN_MAI_STRUCT_LEVEL 15U
#define JSMN_STRLEN_CALLBACK_DATA 320U
#define JSMN_STRLEN_CACHE 1000U

typedef enum jsmnerr
{
	JSMN_ERROR_NOMEM = -1,
	JSMN_ERROR_INVAL = -2,
	JSMN_ERROR_PART = -3
} jsmnerr;

typedef enum json_token_type_enum
{
	JSON_UNDEFINED = 0,
	JSON_OBJECT = 1,
	JSON_ARRAY = 2,
	JSON_STRING = 3,
	JSON_PRIMITIVE = 4
} json_token_type_enum;

typedef struct json_token
{
	json_token_type_enum Type;
	INT Start;
	INT End;
	INT Size;
	INT parent;
	BOOL cached;
} json_token;

typedef struct jsmntok_t
{
	json_token_type_enum type;
	INT start;
	INT end;
	INT size;
	INT parent;
	BOOL cached;
} jsmntok_t;

typedef struct jsmn_callback
{
	UDINT* pFunction;
	UDINT* pUserData;
} jsmn_callback;

typedef struct jsmn_parser
{
	UDINT pos;
	UDINT toknext;
	INT toksuper;
	jsmn_callback callback;
	BOOL isValue;
	UDINT* pcache;
	UDINT endpos;
	STRING cache[JSMN_STRLEN_CACHE + 1];
} jsmn_parser;

typedef struct jsmn_callback_advanced_data
{
	UDINT* pValue;
	UDINT ValueLen;
} jsmn_callback_advanced_data;

typedef struct jsmn_callback_data
{
	STRING Name[JSMN_STRLEN_CALLBACK_DATA + 1];
	json_token_type_enum Type;
	STRING Value[JSMN_STRLEN_CALLBACK_DATA + 1];
	USINT Levels;
	STRING Structure[JSMN_MAI_STRUCT_LEVEL + 1][JSMN_STRLEN_CALLBACK_DATA + 1];
	UDINT Size;
	jsmn_callback_advanced_data Advanced;
} jsmn_callback_data;

signed short JsmnInit(unsigned long parser);
signed short JsmnParse(unsigned long parser, unsigned long js, unsigned short len, unsigned long tokens, unsigned short num_tokens);
signed short JsonParse(unsigned long pJsonString, unsigned long pTokenArray);
signed short JsonTokenEqual(unsigned long pJsonString, unsigned long pTokenArray, unsigned long pName);
signed short JsonGetValue(unsigned long pJsonString, unsigned long pTokenArray, unsigned long pName, unsigned long pStrValue, unsigned long StrLength);

#ifdef __cplusplus
};
#endif

#endif
