/*
 * File: JsonTokenEqual.c
 * Copyright (c) 2023 Loupe
 * https://loupe.team
 * 
 * This file is part of jsmn_parse, licensed under the MIT License.
 */


#include <bur/plctypes.h>

#ifdef __cplusplus
	extern "C"
	{
#endif

#include "jsmn_parse.h"
#include "jsmn.h"
#include <string.h>

#ifdef __cplusplus
	};
#endif


static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
	strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

/* pass in Json string, token arrays, and token name will return if found */
signed short JsonTokenEqual(unsigned long pJsonString, unsigned long pTokenArray, unsigned long pName)
{
	int i,token_index;
	jsmntok_t t[JSMN_MAX_TOKENS];
	json_token *t2;

	t2 = (json_token*)pTokenArray;
	
	for (i = 0; i < JSMN_MAX_TOKENS; i++){
		t[i].type = t2[i].Type;
		t[i].start = t2[i].Start;
		t[i].end = t2[i].End;
		t[i].size = t2[i].Size;
	}	
	token_index = -1;
	for (i = 1; i < JSMN_MAX_TOKENS; i++) {
		if (jsoneq((const char*)pJsonString, &t[i], (char*)pName) == 0) {
			token_index = i;
		}
	}
	return token_index;
}
