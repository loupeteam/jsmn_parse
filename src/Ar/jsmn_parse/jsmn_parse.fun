
FUNCTION JsmnInit : INT
	VAR_INPUT
		parser : UDINT;
	END_VAR
END_FUNCTION

FUNCTION JsmnParse : INT
	VAR_INPUT
		parser : UDINT;
		js : UDINT;
		len : UINT;
		tokens : UDINT;
		num_tokens : UINT;
	END_VAR
END_FUNCTION

FUNCTION JsonParse : INT (*This function parses a JSON string, returns pointer to array of token locations*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		pJsonString : UDINT;
		pTokenArray : UDINT;
	END_VAR
END_FUNCTION

FUNCTION JsonTokenEqual : INT (*pass in Json string, token arrays, and token name will return if found*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		pJsonString : UDINT;
		pTokenArray : UDINT;
		pName : UDINT;
	END_VAR
END_FUNCTION

FUNCTION JsonGetValue : INT
	VAR_INPUT
		pJsonString : UDINT;
		pTokenArray : UDINT;
		pName : UDINT;
		pStrValue : UDINT;
		StrLength : UDINT;
	END_VAR
END_FUNCTION
