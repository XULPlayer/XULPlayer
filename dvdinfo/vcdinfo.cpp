/*
* filename: vcdinfo.c
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#include "cdrom.h"
#include "dvdinfo.h"

char *get_vcdifo_xml(const char* psz_path)
{
	char psz_xml[MAX_BUF_SIZE], *psz_dup;
	char *p_xml = psz_xml;
	vcddev_t* p_vcddev;
	int *p_sectors; /*need free me*/
	int i_tracks, i;
	if (psz_path == NULL) return NULL;
	
	psz_dup = _strdup(psz_path);
	if( psz_path[0] && psz_path[1] == ':' &&
		psz_path[2] == '\\' && psz_path[3] == '\0' ) {
			psz_dup[2] = '\0';
	}

{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	char psz_tmp[20];

	sprintf(psz_tmp, "%c:\\MPEGAV", psz_path[0]);
	hFind = FindFirstFile(psz_tmp, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		return NULL;
	}
}

	/* Open VCD */
	if(!(p_vcddev = ioctl_Open(psz_dup))) {
		free(psz_dup);
		return NULL;
	}

	sprintf(p_xml, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><vcdinfo>");
	p_xml += strlen(p_xml);

	i_tracks = ioctl_GetTracksMap(p_vcddev, &p_sectors);
	
	if (i_tracks <= 1)
	{ /* i_tracks == 0 "unable to count tracks"
	   * i_tracks == 1 "no movie tracks found" */
		goto theEnd;
	}

	sprintf(p_xml, "<tracks>");
	p_xml += strlen(p_xml);
	for (i = 1; i < i_tracks; i++) {
		__int64 i_size = ( p_sectors[i+1] - p_sectors[i] ) * (__int64)VCD_DATA_SIZE;
		__int64 i_length = i_size * 8 / (1150+224);
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
	ioctl_Close(p_vcddev);
	if (i_tracks > 0) {	
		free(p_sectors);
	}
	free(psz_dup);

	sprintf(p_xml, "</vcdinfo>");
	return _strdup(psz_xml);
}