/*
 * File: JsonGetValue.c
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


/* pass in Json string, token arrays, and token name will return the value */
signed short JsonGetValue(unsigned long pJsonString, unsigned long pTokenArray, unsigned long pName, unsigned long pStrValue, unsigned long StrLength)
{
	if (pJsonString == 0 || pTokenArray==0 || pName == 0)
	{
		return -1;
	}
	
	BOOL ValueFound;
	int Offset;
	STRING	TempString[StrLength];
	STRING NotFoundStr[15] = "No Value Found";
	
	json_token *t2;
	t2 = (json_token*)pTokenArray;

	Offset = JsonTokenEqual(pJsonString,pTokenArray,pName);
	//check if a value exist for found token
	if(t2[Offset].Size > 0){
		ValueFound = 1;
		//check if value string pointer exists
		if (pStrValue !=0){
			memset(&TempString,0,sizeof(TempString));
			//check if value is larger than given value string
			if ((t2[Offset+1].End - t2[Offset+1].Start) > StrLength){//value too large for string, copy what we can
				strncpy( TempString, (char*)(pJsonString+t2[Offset+1].Start),StrLength );
				}else{//value fits in value string
				strncpy( TempString, (char*)(pJsonString+t2[Offset+1].Start),(t2[Offset+1].End - t2[Offset+1].Start) );
			}
		}
		//value doesn't exist for found token
		}else{
		ValueFound = 0;
		//check if value string pointer exists
		if (pStrValue !=0){
			if (StrLength < strlen(NotFoundStr)){//value too large for string, copy what we can
				strncpy( TempString, NotFoundStr,StrLength );
			}else{//value fits in value string
				strncpy( TempString, NotFoundStr,sizeof(NotFoundStr));
			}
		}
	}
	//copy string to user pointer
	memset((void*)pStrValue,0,StrLength);
	strncpy((char*)pStrValue,TempString,StrLength);
	
	//return token index of the given value
	if(ValueFound){
		return Offset+1;
	} else{
		return -1;
	}
}
