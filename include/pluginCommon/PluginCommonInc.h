#ifndef PLUGIN_COMMON_INC
#define PLUGIN_COMMON_INC

#include "PCMacro.h"
#include "processwrapper.h"
#include "mplayerwrapper.h"

#ifdef ENABLE_MEDIALIB
#include "FileSeeker.h"
#include "imgProcess.h"
#endif

#ifdef ENABLE_MLNET
#include "mlnet.h"
#endif

#ifdef ENABLE_MCODER
#include "common.h"
#include "McodeFiles.h"
#include "MCodeWrapper.h"
#endif

#ifdef ENABLE_VIS
#include "MVisulation.h"
#endif
#endif
