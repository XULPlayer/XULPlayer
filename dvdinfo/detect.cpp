#include "dvd_reader.h"
#include "ifo_types.h"
#include "ifo_read.h"
#include "nav_read.h"

#include "cdrom.h"
#include "dvdinfo.h"

int detect_media_type(const char *psz_path)
{
	int i_type;
	char *psz_dup;
	char psz_tmp[20];
	dvd_reader_t *dvd;
	vcddev_t* p_cddev;

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	if (psz_path == NULL) return TYPE_UNKNOWN;

	psz_dup = _strdup(psz_path);
	if( psz_path[0] && psz_path[1] == ':' &&
		psz_path[2] == '\\' && psz_path[3] == '\0' ) {
			psz_dup[2] = '\0';
	}

	sprintf(psz_tmp, "%c:\\*", psz_path[0]);
	hFind = FindFirstFile(psz_tmp, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		free(psz_dup);
		return TYPE_NOMEDIA;
	}

	do {
		/*CD?*/
		sprintf(psz_tmp, "%c:\\Track*.cda", psz_path[0]);
		hFind = FindFirstFile(psz_tmp, &FindFileData);

		if (hFind != INVALID_HANDLE_VALUE) {
			i_type = TYPE_CD;
		}
		else {
			/*VCD?*/
			sprintf(psz_tmp, "%c:\\MPEGAV", psz_path[0]);
			hFind = FindFirstFile(psz_tmp, &FindFileData);
			if (hFind == INVALID_HANDLE_VALUE) break;
			i_type = TYPE_VCD;
		}

		/*go deeper*/
		p_cddev = ioctl_Open(psz_dup);
		if(p_cddev == NULL) break;
		ioctl_Close(p_cddev);
		free(psz_dup);
		return i_type;
	} while (0);

	i_type = TYPE_UNKNOWN;
	/*DVD?*/
	dvd = DVDOpen(psz_dup);

	if(dvd != NULL) {
		/*go deeper*/
		ifo_handle_t *vmg_file;
		vmg_file = ifoOpen(dvd, 0);
		if(vmg_file != NULL) {
			ifoClose(vmg_file);
			i_type = TYPE_DVD;
		}
		DVDClose(dvd);
	}

	free(psz_dup);
	return i_type;
}
