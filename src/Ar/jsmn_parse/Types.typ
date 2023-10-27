(*
 * File: Types.typ
 * Copyright (c) 2023 Loupe
 * https://loupe.team
 * 
 * This file is part of jsmn_parse, licensed under the MIT License.
 *)


TYPE
	json_token : 	STRUCT 
		Type : json_token_type_enum;
		Start : INT;
		End : INT;
		Size : INT;
		parent : INT;
		cached : BOOL;
	END_STRUCT;
	jsmntok_t : 	STRUCT 
		type : json_token_type_enum;
		start : INT;
		end : INT;
		size : INT;
		parent : INT;
		cached : BOOL;
	END_STRUCT;
	jsmn_parser : 	STRUCT 
		pos : UDINT;
		toknext : UDINT;
		toksuper : INT;
		callback : jsmn_callback;
		isValue : BOOL;
		pcache : REFERENCE TO UDINT;
		endpos : UDINT;
		cache : STRING[JSMN_STRLEN_CACHE];
	END_STRUCT;
	jsmnerr : 
		(
		JSMN_ERROR_NOMEM := -1,
		JSMN_ERROR_INVAL := -2,
		JSMN_ERROR_PART := -3
		);
	json_token_type_enum : 
		(
		JSON_UNDEFINED,
		JSON_OBJECT,
		JSON_ARRAY,
		JSON_STRING,
		JSON_PRIMITIVE
		);
	jsmn_callback : 	STRUCT 
		pFunction : REFERENCE TO UDINT;
		pUserData : REFERENCE TO UDINT;
	END_STRUCT;
	jsmn_callback_advanced_data : 	STRUCT 
		pValue : REFERENCE TO UDINT;
		ValueLen : UDINT;
	END_STRUCT;
	jsmn_callback_data : 	STRUCT 
		Name : STRING[JSMN_STRLEN_CALLBACK_DATA];
		Type : json_token_type_enum;
		Value : STRING[JSMN_STRLEN_CALLBACK_DATA];
		Levels : USINT;
		Structure : ARRAY[0..JSMN_MAI_STRUCT_LEVEL]OF STRING[JSMN_STRLEN_CALLBACK_DATA];
		Size : UDINT;
		Advanced : jsmn_callback_advanced_data;
	END_STRUCT;
END_TYPE
