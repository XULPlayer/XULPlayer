#ifndef PLUGIN_COMMON_MACRO
#define PLUGIN_COMMON_MACRO

#ifdef  PLUGINCOMMON_STATIC
	#define  MC_PC_EXT  
#else
	#ifdef   PLUGINCOMMON_EXPORTS
	#define  MC_PC_EXT   __declspec(dllexport)
	#else
	#define  MC_PC_EXT   __declspec(dllimport)
	#endif
#endif

#define _MC_PC_BEGIN   namespace PluginCommon {
#define _MC_PC_END     }

#endif
