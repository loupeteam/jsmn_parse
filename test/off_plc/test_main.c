/* Off-PLC regression test for loupeteam/jsmn_parse fix/uninitialized-parser */
#include <stdio.h>
#include <string.h>
#include "jsmn_parse.h"

static int failures = 0;
#define CHECK(cond, msg) do { \
	if (cond) { printf("PASS: %s\n", msg); } \
	else { printf("FAIL: %s\n", msg); failures++; } \
} while (0)

extern void jsmn_init(jsmn_parser *parser);

int main(void)
{
	/* 1. jsmn_init must clear callback + pcache even on a poisoned struct */
	jsmn_parser p;
	memset(&p, 0xAA, sizeof(p));
	jsmn_init(&p);
	CHECK(p.callback.pFunction == 0, "jsmn_init clears callback.pFunction");
	CHECK(p.callback.pUserData == 0, "jsmn_init clears callback.pUserData");
	CHECK(p.pcache == 0, "jsmn_init clears pcache");

	/* 2. JsonParse + JsonGetValue round-trip on a multi-key document */
	char json[] = "{\"a\":5,\"b\":7}";
	json_token tokens[JSMN_MAX_TOKENS];
	memset(tokens, 0, sizeof(tokens));

	signed short r = JsonParse((unsigned long)json, (unsigned long)tokens);
	printf("JsonParse returned %d tokens\n", r);
	CHECK(r == 5, "JsonParse returns 5 tokens for {\"a\":5,\"b\":7}");

	char value[32];
	signed short idx;

	idx = JsonGetValue((unsigned long)json, (unsigned long)tokens,
	                   (unsigned long)"a", (unsigned long)value, sizeof(value));
	printf("lookup a -> idx %d value '%s'\n", idx, value);
	CHECK(idx >= 0 && strcmp(value, "5") == 0, "JsonGetValue(\"a\") == \"5\"");

	idx = JsonGetValue((unsigned long)json, (unsigned long)tokens,
	                   (unsigned long)"b", (unsigned long)value, sizeof(value));
	printf("lookup b -> idx %d value '%s'\n", idx, value);
	CHECK(idx >= 0 && strcmp(value, "7") == 0, "JsonGetValue(\"b\") == \"7\"");

	/* 3. Missing key: must return -1, not read t2[-1] */
	idx = JsonGetValue((unsigned long)json, (unsigned long)tokens,
	                   (unsigned long)"missing", (unsigned long)value, sizeof(value));
	printf("lookup missing -> idx %d value '%s'\n", idx, value);
	CHECK(idx == -1, "JsonGetValue(missing key) returns -1");

	printf(failures ? "\n%d FAILURE(S)\n" : "\nALL TESTS PASSED\n", failures);
	return failures;
}
