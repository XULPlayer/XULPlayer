/*
* filename: cdhelper.c
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#include "cdrom.h"
#include "cdinfo.h"

/*psz_path must as "I:" not as "I:\" */
vcddev_t *cd_open(const char *psz_path)
{
	return ioctl_Open(psz_path);
}

void cd_close(vcddev_t *p_cddev)
{
	ioctl_Close(p_cddev);
}

int cd_getTracksMap( const vcddev_t *p_cddev, int **pp_sectors )
{
	return ioctl_GetTracksMap(p_cddev, pp_sectors);

}

/*
	i_nb is block number, not buffer size;
*/
int cd_read(const vcddev_t *p_cddev,int i_sector, byte_t * p_buffer, int i_nb)
{
	if (p_cddev == NULL) return -1;
	
	return ioctl_ReadSectors( p_cddev, i_sector, p_buffer, i_nb, CDDA_TYPE );

}

/* Build a WAV header for the output data */
void cd_build_waveheader(WAVHDR *wav, DWORD len)
{
	memcpy( wav->riff, "RIFF", 4 );
	wav->len = len + 44 - 8;
	memcpy( wav->cWavFmt, "WAVEfmt ", 8 );
	wav->dwHdrLen = 16;
	wav->wFormat = 1;
	wav->wNumChannels = 2;
	wav->dwSampleRate = 44100;
	wav->dwBytesPerSec = 44100*2*2;
	wav->wBlockAlign = 4;
	wav->wBitsPerSample = 16;
	memcpy( wav->cData, "data", 4 );
	wav->dwDataLen = len;
}

#ifndef MAX_BUF_SIZE
#define MAX_BUF_SIZE 400*1024 /*400KB*/
#endif

char *get_cdifo_xml(const char* psz_path)
{
	char *psz_xml, *p_xml, *psz_dup;
	vcddev_t* p_cddev;
	int *p_sectors; /*need free me*/
	int i_tracks, i;
	if (psz_path == NULL) return NULL;
	
	psz_xml = p_xml = (char*)malloc(sizeof(char)*MAX_BUF_SIZE);
	
	if (psz_xml == NULL) {
		return NULL;
	}

	psz_dup = _strdup(psz_path);
	if( psz_path[0] && psz_path[1] == ':' &&
		psz_path[2] == '\\' && psz_path[3] == '\0' ) {
			psz_dup[2] = '\0';
	}
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	char psz_tmp[20];

	sprintf(psz_tmp, "%c:\\Track*.cda", psz_path[0]);
	hFind = FindFirstFile(psz_tmp, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		return NULL;
	}
}
	/* Open CD */
	if(!(p_cddev = ioctl_Open(psz_dup))) {
		free(psz_dup);
		free(psz_xml);
		return NULL;
	}

	sprintf(p_xml, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><cdinfo>");
	p_xml += strlen(p_xml);

	i_tracks = ioctl_GetTracksMap(p_cddev, &p_sectors);
	
	if (i_tracks <= 0)
	{	/* i_tracks == 0 "no audio tracks found" */
		goto theEnd;
	}

	sprintf(p_xml, "<tracks>");
	p_xml += strlen(p_xml);
	for (i = 0; i < i_tracks; i++) {
		__int64 i_size = ( p_sectors[i+1] - p_sectors[i] ) * (__int64)CDDA_DATA_SIZE;
		__int64 i_length = (1000i64) * i_size / 44100 / 4; /*msec*/

//		__int64 i_length = i_size / 44100 / 4; /*sec*/
		int h = (int)(i_length / 1000 / 3600);
		int m = (int)(i_length / 1000 % 3600 / 60);
		int s = (int)(i_length / 1000 % 60);
		int r = (int)(i_length % 1000);

		sprintf(p_xml, 
			"<track id=\"%d\" startsector=\"%d\" size=\"%I64i\" length=\"%02d:%02d:%02d.%03d\"/>",
			i+1, p_sectors[i], i_size, h, m, s, r);

		p_xml += strlen(p_xml);
	}
	sprintf(p_xml, "</tracks>");
	p_xml += strlen(p_xml);

theEnd:
	ioctl_Close(p_cddev);
	if (i_tracks > 0) {	
		free(p_sectors);
	}
	free(psz_dup);

	sprintf(p_xml, "</cdinfo>");

	psz_xml = (char*)realloc(psz_xml, strlen(psz_xml)+1);

	return psz_xml;
}