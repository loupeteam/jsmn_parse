2.0.1 - Fix uninitialized callback/pcache in jsmn_init() causing intermittent
         page faults in JsonParse() (wild callback call / wild cache write)
        Skip jsmn_cache() when no streaming callback is registered, so
         JsonParse() token Start/End stay valid against the JSON string
        Fix out-of-bounds read in JsonGetValue() when the key is not found

2.0.0 - Update to AS6

1.5.0 - Add Support for 64 bit build

1.4.0 - Add pValue and ValueLen to callback data
         - Fix memory overflow on json values greater than 320 characters

1.3.0 - Fix incorrect array indices in callback data
		Garbage collect after commas during array and objects
		Increase string length in callback data strings
		Fix build warnings

1.2.0 - Fix bug in generateStructure()
		Move from itoa to brsitoa
		
1.1.0 - Add external JsmnInit() function

1.0.0 - First version
