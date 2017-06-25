#ifndef __CDHELPER_H__
#define __CDHELPER_H__

#include "cdrom.h"

/*
 * WAV file header format
 */
#pragma pack(1)
typedef struct
{
  BYTE	riff[4];			/* must be "RIFF"				 */
  DWORD len;				/* #bytes + 44 - 8				 */
  BYTE	cWavFmt[8]; 		/* must be "WAVEfmt"			 */
  DWORD dwHdrLen;
  WORD	wFormat;
  WORD	wNumChannels;
  DWORD dwSampleRate;
  DWORD dwBytesPerSec;
  WORD	wBlockAlign;
  WORD	wBitsPerSample;
  BYTE	cData[4];			 /* must be "data"				 */
  DWORD dwDataLen;			 /* #bytes						 */
} WAVHDR;
#pragma pack()

vcddev_t *cd_open(const char *psz_path);
void cd_close(vcddev_t *p_cddev);
int cd_getTracksMap( const vcddev_t *p_cddev, int **pp_sectors );
int cd_read(const vcddev_t *p_cddev,int i_sector, byte_t * p_buffer, int i_nb);

void cd_build_waveheader(WAVHDR *waveheader);

#endif
