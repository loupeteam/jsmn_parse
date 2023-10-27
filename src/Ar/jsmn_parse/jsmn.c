
#include <bur/plctypes.h>

#ifdef __cplusplus
	extern "C"
	{
#endif

#include "jsmn_parse.h"
#include "jsmn.h"
#include <string.h>

#ifndef _SG4 
#include <stdio.h>
#define brsitoa( x, y ) sprintf((char*)(y), "%d", x)
#endif

#ifdef __cplusplus
	};
#endif

#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))

/**
 * Allocates a fresh unused token from the token pull.
 */
static jsmntok_t *jsmn_alloc_token(jsmn_parser *parser,
		jsmntok_t *tokens, size_t num_tokens) {
	jsmntok_t *tok;
	if (parser->toknext >= num_tokens) {
		return NULL;
	}
	tok = &tokens[parser->toknext++];
	tok->start = tok->end = -1;
	tok->size = 0;
	tok->cached= 0;
#ifdef JSMN_PARENT_LINKS
	tok->parent = -1;
#endif
	return tok;
}

/**
 * Fills token type and boundaries.
 */
static void jsmn_fill_token(jsmntok_t *token, jsmntype_t type,
                            int start, int end) {
	token->type = type;
	token->start = start;
	token->end = end;
	token->size = 0;
}

/**
 * Fills next available token with JSON primitive.
 */
static int jsmn_parse_primitive(jsmn_parser *parser, const char *js,
		size_t len, jsmntok_t *tokens, size_t num_tokens) {
	jsmntok_t *token;
	int start;
	
	start = parser->pos;

	for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
		switch (js[parser->pos]) {
#ifndef JSMN_STRICT
			/* In strict mode primitive must be followed by "," or "}" or "]" */
			case ':':
#endif
			case '\t' : case '\r' : case '\n' : case ' ' :
			case ','  : case ']'  : case '}' :
				goto found;
		}
		if (js[parser->pos] < 32 || js[parser->pos] >= 127) {
			parser->pos = start;
			return JSMN_ERROR_INVAL;
		}
	}
#ifdef JSMN_STRICT
	/* In strict mode primitive must be followed by a comma/object/array */
	parser->pos = start;
	return JSMN_ERROR_PART;
#endif
	if(parser->pos >= len){
		parser->pos = start;
		return JSMN_ERROR_PART;	
	}
found:
	parser->endpos= parser->pos+1;		
	if (tokens == NULL) {
		parser->pos--;
		return 0;
	}
	token = jsmn_alloc_token(parser, tokens, num_tokens);
	if (token == NULL) {
		parser->pos = start;
		return JSMN_ERROR_NOMEM;
	}
	jsmn_fill_token(token, JSMN_PRIMITIVE, start, parser->pos);
#ifdef JSMN_PARENT_LINKS
	token->parent = parser->toksuper;
#endif
	parser->pos--;
	return 0;
}

/**
 * Fills next token with JSON string.
 */
static int jsmn_parse_string(jsmn_parser *parser, const char *js,
		size_t len, jsmntok_t *tokens, size_t num_tokens) {
	jsmntok_t *token;

	int start = parser->pos;

	parser->pos++;

	/* Skip starting quote */
	for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
		char c = js[parser->pos];

		/* Quote: end of string */
		if (c == '\"') {
			if (tokens == NULL) {
				return 0;
			}
			token = jsmn_alloc_token(parser, tokens, num_tokens);
			if (token == NULL) {
				parser->pos = start;
				return JSMN_ERROR_NOMEM;
			}
			jsmn_fill_token(token, JSMN_STRING, start+1, parser->pos);
#ifdef JSMN_PARENT_LINKS
			token->parent = parser->toksuper;
#endif
			parser->endpos= parser->pos+1;		
			return 0;
		}

		/* Backslash: Quoted symbol expected */
		if (c == '\\' && parser->pos + 1 < len) {
			int i;
			parser->pos++;
			switch (js[parser->pos]) {
				/* Allowed escaped symbols */
				case '\"': case '/' : case '\\' : case 'b' :
				case 'f' : case 'r' : case 'n'  : case 't' :
					break;
				/* Allows escaped symbol \uXXXX */
				case 'u':
					parser->pos++;
					for(i = 0; i < 4 && parser->pos < len && js[parser->pos] != '\0'; i++) {
						/* If it isn't a hex character we have an error */
						if(!((js[parser->pos] >= 48 && js[parser->pos] <= 57) || /* 0-9 */
									(js[parser->pos] >= 65 && js[parser->pos] <= 70) || /* A-F */
									(js[parser->pos] >= 97 && js[parser->pos] <= 102))) { /* a-f */
							parser->pos = start;
							return JSMN_ERROR_INVAL;
						}
						parser->pos++;
					}
					parser->pos--;
					break;
				/* Unexpected symbol */
				default:
					parser->pos = start;
					return JSMN_ERROR_INVAL;
			}
		}
	}
	parser->pos = start;
	return JSMN_ERROR_PART;
}

/**
 * Build up variable names to send to the user.
 */
static int generateStructure(jsmn_parser *parser,const char *js, jsmntok_t *tokens,int Item,jsmn_callback_data *structure ){
	char temp[5];
	signed long len;

	int level = structure->Levels;
	
	if((tokens[Item].type == JSON_STRING || tokens[Item].type == JSON_PRIMITIVE) && tokens[Item].end!=-1){
		if(tokens[Item].cached){
			if(parser->pcache!=0){
				memcpy(&structure->Structure[structure->Levels],(void*)(parser->pcache + tokens[Item].start),min((tokens[Item].end - tokens[Item].start), sizeof(structure->Structure[0])));
			}
			else{
				memcpy(&structure->Structure[structure->Levels],&"BadCache",sizeof("BadCache"));
			}
		}
		else{
			memcpy(&structure->Structure[structure->Levels],js + tokens[Item].start,min((tokens[Item].end - tokens[Item].start), sizeof(structure->Structure[0])));
		}
		structure->Levels++;
	}
	if((tokens[Item].parent == Item) || (tokens[Item].parent == -1)) { //This is the top level
		return 0;
	}
	else{
		generateStructure(parser,js,tokens, tokens[Item].parent ,structure);
		if(level==0){
			len = sizeof(structure->Value) - strlen((char*)&structure->Value) - 1; // Total size - current length - 1 char for null
			structure->Size = tokens[Item].size;
			switch(tokens[Item].type){
				case JSON_ARRAY:
					brsitoa(tokens[Item].size-1, (char*) &temp);
					strncat((char*)&structure->Value,"[",max(len--,0));
					strncat((char*)&structure->Value,(char*)&temp,max(len,0));
					len = len - strlen(temp);
					strncat((char*)&structure->Value,"]",max(len,0));
					break;
				case JSON_OBJECT:
//					strcat(&structure->Name,&".");
					break;
				default:
					structure->Type = tokens[Item].type;
					structure->Advanced.pValue = (unsigned long *) (tokens[Item].start + js);
					structure->Advanced.ValueLen = tokens[Item].end - tokens[Item].start;
					strncat((char*)&structure->Value,(char*)&structure->Structure[level],max(len,0)); // Prevent writing over and past the last character (to make sure there is a null)

					break;
			}
		}
		else{
			len = sizeof(structure->Name) - strlen((char*)&structure->Name) - 1; // Total size - current length - 1 char for null
			switch(tokens[Item].type){
				case JSON_ARRAY:
					brsitoa(tokens[Item].size-1, (char *)&temp);
					strncat((char*)&structure->Name,"[",max(len--,0));
					strncat((char*)&structure->Name,(char*)&temp,max(len,0));
					len = len - strlen(temp);
					strncat((char*)&structure->Name,"]",max(len--,0));
					break;
				case JSON_OBJECT:
					if(level>1){
						strncat((char*)&structure->Name,".",max(len--,0));
					}
					break;
				default:
					strncat((char*)&structure->Name,(char*)&structure->Structure[level],max(len,0));
					break;
			}
		}
		return 0;
	}
}

/**
 * Removes tokens that have been completely parsed
 */
static int jsmn_collect_garbage(jsmn_parser *parser, jsmntok_t *tokens,const char *js){

	int lastItem = parser->toknext - 1;	
	if(lastItem >= 0){	
		//If we found the end of the object we don't need it anymore
		if(tokens[lastItem].end >= 0){
			
			if(parser->callback.pFunction && (tokens[tokens[lastItem].parent].type == JSON_ARRAY || tokens[tokens[lastItem].parent].type == JSON_OBJECT)){
				unsigned long (*dataCallback)(unsigned long*,unsigned long*);
				dataCallback = (unsigned long (*)(unsigned long*,unsigned long*))parser->callback.pFunction;
				jsmn_callback_data callbackdata;
				memset(&callbackdata,0,sizeof(callbackdata));
				generateStructure(parser,js,tokens,parser->toknext - 1,&callbackdata);
				dataCallback(parser->callback.pUserData, (unsigned long*)&callbackdata);
			}
			parser->toksuper= tokens[lastItem].parent;
			parser->toknext=lastItem;
			memset(&tokens[lastItem],0,sizeof(tokens[lastItem]));
			jsmn_collect_garbage(parser,tokens,js);
		}	
	}
	return 0;
}
/**
 * After the current data has been parsed
 * cache any string data we still need to build variable names
 */
void jsmn_cache(jsmn_parser *parser, const char *js, size_t len, jsmntok_t *tokens, unsigned int num_tokens){
	int i;
	if(parser->pcache==0){
		parser->pcache= (unsigned long*)&parser->cache;
	}
	char *pcache = (char*)parser->pcache;
	for(i = 0;i<num_tokens;i++){
		if(!tokens[i].cached && tokens[i].type == JSON_STRING){
			memcpy(pcache,js + tokens[i].start,tokens[i].end - tokens[i].start);
			tokens[i].cached = 1;
			tokens[i].end = (INT)((pcache - (char*)parser->pcache) + (tokens[i].end - tokens[i].start));
			tokens[i].start = (INT)(pcache - (char*)parser->pcache);
			pcache+= (tokens[i].end - tokens[i].start + 1);
		}
		else if(tokens[i].cached){
			pcache= (char*)(parser->pcache + tokens[i].end + 1);
		}
	}		
}

/**
 * Parse JSON string and fill tokens.
 */
int jsmn_parse(jsmn_parser *parser, const char *js, size_t len,
		jsmntok_t *tokens, unsigned int num_tokens) {
	int r;
	int i;
	jsmntok_t *token;
	int count = 	parser->toknext;
	parser->endpos= parser->pos;
		
	for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
		char c;
		jsmntype_t type;

		c = js[parser->pos];
		switch (c) {
			case '{': case '[':
				parser->isValue = 0;
				count++;
				if (tokens == NULL) {
					break;
				}
				token = jsmn_alloc_token(parser, tokens, num_tokens);
				if (token == NULL)
					return JSMN_ERROR_NOMEM;
				if (parser->toksuper != -1) {
					tokens[parser->toksuper].size++;
#ifdef JSMN_PARENT_LINKS
					token->parent = parser->toksuper;
#endif
				}
				token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
				token->start = parser->pos;
				parser->endpos= parser->pos+1;
				parser->toksuper = parser->toknext - 1;
				break;
			case '}': case ']':
				parser->isValue = 0;
				if (tokens == NULL)
					break;
				type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
#ifdef JSMN_PARENT_LINKS
				if (parser->toknext < 1) {
					return JSMN_ERROR_INVAL;
				}
				token = &tokens[parser->toknext - 1];
				for (;;) {
					if (token->start != -1 && token->end == -1) {
						if (token->type != type) {
							return JSMN_ERROR_INVAL;
						}
						token->end = parser->pos + 1;
						parser->toksuper = token->parent;
						if(parser->callback.pFunction){
							jsmn_collect_garbage(parser,tokens,js);				
						}										
						break;
					}
					if (token->parent == -1) {
						if(token->type != type || parser->toksuper == -1) {
							return JSMN_ERROR_INVAL;
						}
						break;
					}
					token = &tokens[token->parent];
				}
#else
				for (i = parser->toknext - 1; i >= 0; i--) {
					token = &tokens[i];
					if (token->start != -1 && token->end == -1) {
						if (token->type != type) {
							return JSMN_ERROR_INVAL;
						}
						parser->toksuper = -1;
						token->end = parser->pos + 1;
						if(parser->callback.pFunction){
							jsmn_collect_garbage(parser,tokens,js);				
						}										
						break;
					}
				}
				/* Error if unmatched closing bracket */
				if (i == -1) return JSMN_ERROR_INVAL;
				for (; i >= 0; i--) {
					token = &tokens[i];
					if (token->start != -1 && token->end == -1) {
						parser->toksuper = i;
						break;
					}
				}
				if(parser->callback.pFunction){
					jsmn_collect_garbage(parser,tokens,js);				
				}				
#endif
				parser->endpos= parser->pos+1;				
				break;
			case '\"':
				r = jsmn_parse_string(parser, js, len, tokens, num_tokens);
				if(r == JSMN_ERROR_PART){
					goto incomplete;
				}
				if (r < 0) return r;
				count++;
				if (parser->toksuper != -1 && tokens != NULL)
					tokens[parser->toksuper].size++;
				if(parser->isValue && parser->callback.pFunction){
					unsigned long (*dataCallback)(unsigned long *,unsigned long*);
					dataCallback = (unsigned long (*)(unsigned long*,unsigned long*))parser->callback.pFunction;
					jsmn_callback_data callbackdata;
					memset(&callbackdata,0,sizeof(callbackdata));
					generateStructure(parser,js,tokens,parser->toknext - 1,&callbackdata);
					dataCallback(parser->callback.pUserData, (unsigned long*)&callbackdata);
				}
				parser->isValue = 0;
				break;
			case '\t' : case '\r' : case '\n' : case ' ':
				break;
			case ':':
				if(!parser->isValue){//This is required to pickup where we left off if we ran out of data here
					parser->toksuper = parser->toknext - 1;
					parser->isValue = 1;
				}
				break;
			case ',':
				parser->isValue = 0;
				parser->endpos= parser->pos+1;
				if (tokens != NULL && parser->toksuper != -1) {
#ifdef JSMN_PARENT_LINKS
					if(tokens[parser->toksuper].type != JSMN_ARRAY &&
					tokens[parser->toksuper].type != JSMN_OBJECT) {
						parser->toksuper = tokens[parser->toksuper].parent;
					}
					if(parser->callback.pFunction){
						jsmn_collect_garbage(parser,tokens,js);				
					}				
#else
					for (i = parser->toknext - 1; i >= 0; i--) {
						if (tokens[i].type == JSMN_ARRAY || tokens[i].type == JSMN_OBJECT) {
							if (tokens[i].start != -1 && tokens[i].end == -1) {
								if (tokens[parser->toksuper].type != JSMN_ARRAY &&
								tokens[parser->toksuper].type != JSMN_OBJECT) {
									// Dont change super if we are still working on object or array
									parser->toksuper = i;
								}
								if(parser->callback.pFunction){
									jsmn_collect_garbage(parser,tokens,js);				
								}				
								break;
							}
						}
					}
#endif
				}
				break;
#ifdef JSMN_STRICT
			/* In strict mode primitives are: numbers and booleans */
			case '-': case '0': case '1' : case '2': case '3' : case '4':
			case '5': case '6': case '7' : case '8': case '9':
			case 't': case 'f': case 'n' :
				/* And they must not be keys of the object */
				if (tokens != NULL && parser->toksuper != -1) {
					jsmntok_t *t = &tokens[parser->toksuper];
					if (t->type == JSMN_OBJECT ||
							(t->type == JSMN_STRING && t->size != 0)) {
						return JSMN_ERROR_INVAL;
					}
				}
#else
			/* In non-strict mode every unquoted value is a primitive */
			default:
#endif
				r = jsmn_parse_primitive(parser, js, len, tokens, num_tokens);
				if(r == JSMN_ERROR_PART){
					goto incomplete;
				}
				if (r < 0) return r;
				parser->isValue = 0;
				count++;
				if (parser->toksuper != -1 && tokens != NULL)
					tokens[parser->toksuper].size++;
				if(parser->callback.pFunction){
					unsigned long (*dataCallback)(unsigned long*,unsigned long*);
					dataCallback = (unsigned long (*)(unsigned long*,unsigned long*))parser->callback.pFunction;
					jsmn_callback_data callbackdata;
					memset(&callbackdata,0,sizeof(callbackdata));
					generateStructure(parser,js,tokens,parser->toknext - 1,&callbackdata);
					dataCallback(parser->callback.pUserData, (unsigned long*)&callbackdata);
				}				
				break;
#ifdef JSMN_STRICT
			/* Unexpected char in strict mode */
			default:
				return JSMN_ERROR_INVAL;
#endif
		}
	}

	incomplete:
	
	jsmn_cache(parser, js, len, tokens, num_tokens);
	
	if (tokens != NULL) {
		for (i = parser->toknext - 1; i >= 0; i--) {
			/* Unmatched opened object or array */
			if (tokens[i].start != -1 && tokens[i].end == -1) {				
				return JSMN_ERROR_PART;
			}
		}
	}
	
	return count;
}

/**
 * Creates a new parser based over a given buffer with an array of tokens
 * available.
 */
void jsmn_init(jsmn_parser *parser) {
	parser->pos = 0;
	parser->toknext = 0;
	parser->toksuper = -1;
	parser->endpos=0;
	parser->isValue=0;
}

