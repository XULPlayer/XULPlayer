#ifndef STRING_PRO_MACRO
#define STRING_PRO_MACRO

#ifdef   STRPRO_STATIC
	#define  MC_STRPRO_EXT  
#else
	#ifdef   STRPRO_EXPORTS
	#define  MC_STRPRO_EXT   __declspec(dllexport)
	#else
	#define  MC_STRPRO_EXT   __declspec(dllimport)
	#endif
#endif

#define _MC_STRPRO_BEGIN   namespace StrPro {
#define _MC_STRPRO_END     }

#endif

