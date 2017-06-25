#ifdef __cplusplus
extern "C" {
#endif

typedef struct cpucaps_s {
	int cpuType;
	int cpuModel;
	int cpuStepping;
	int hasMMX;
	int hasMMX2;
	int has3DNow;
	int has3DNowExt;
	int hasSSE;
	int hasSSE2;
	int isX86;
	unsigned cl_size; /* size of cache line */
	int hasAltiVec;
	int hasTSC;
	int cpuCores;
	int cpuFreq;
	char vendorName[20];
	char friendlyName[256];
} CpuCaps;

void GetCpuCaps( CpuCaps *caps);
unsigned long MeasureCPUSpeed(int fStartMeasure);

#ifdef __cplusplus
}
#endif
