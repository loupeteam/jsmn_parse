
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


/* This function parses a JSON string, returns pointer to array of token locations */
signed short JsonParse(unsigned long pJsonString, unsigned long pTokenArray)
{
	int i;
	int r;
	
	jsmn_parser p;
	jsmntok_t t[JSMN_MAX_TOKENS];
	memset(t,0,sizeof(t));
	json_token *t2;

	t2= (json_token*)pTokenArray;
	
	jsmn_init(&p);
	r = jsmn_parse(&p, (const char*)pJsonString, strlen((char*)pJsonString), t, sizeof(t)/sizeof(t[0]));
	
	for (i = 0; i < JSMN_MAX_TOKENS; i++)	{
		t2[i].Type = t[i].type;
		t2[i].Start = t[i].start;
		t2[i].End = t[i].end;
		t2[i].Size = t[i].size;
		t2[i].parent  = t[i].parent;
	}	
	return r;//return number of tokens
}

signed short JsmnParse(unsigned long parser, unsigned long js, unsigned short len, unsigned long tokens, unsigned short num_tokens) {
	return jsmn_parse((jsmn_parser*)parser, (char*)js, len, (jsmntok_t*)tokens, num_tokens);
}

signed short JsmnInit(unsigned long parser) {
	jsmn_init((jsmn_parser*)parser);
	return 0;
}
