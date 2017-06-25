#include <stdio.h> /*fprintf fopen*/
#include <stdlib.h> /*free*/

#include "dvdinfo.h"
#include "cdrom.h"
#include "cdinfo.h"

#define READ_ONCE 20

int main(int argc, char *argv[])
{
	char * p_buf;
	char p_path[3];
	char *p;
	int i_ret;
	CDLIST cdlist;

	i_ret = GetCDList( &cdlist );
	
	if (i_ret != 0) {
		fprintf(stderr, "Can not open any CDROM\n");
		return -1;
	}
	
	p_buf = get_cdrom_xml();
	printf("%s\n", p_buf);
	free(p_buf);

	p_path[0] = cdlist.cd[1].c_drive;
	p_path[1] = ':';
	p_path[2] = '\0';
	
	printf("First CDROM PATH:%s\n", p_path);
	
	if (argc > 1 && argv[1][0] == 'v') {
		p_buf = get_vcdifo_xml(p_path);
		p = "VCD";
	} else if (argc > 1 && argv[1][0] == 'c') {
		p_buf = get_cdifo_xml(p_path);
		p = "CD";
	} else { /*default DVD*/
		p_buf = get_dvdifo_xml(p_path);
		p = "DVD";
	} 


	if (p_buf == NULL) {
		fprintf(stderr, "Can not open %s as %s\n", p_path, p);
		return -2;
	}
	printf("%s\n", p_buf);
	free(p_buf);

	if (argc > 2 && argv[1][0] == 'c') {
		int i_track = atoi(argv[2]);
		char psz_out[20];
		FILE *fp;
		WAVHDR waveheader; /*test: just for play*/
		vcddev_t *p_cddev;
		int *p_sectors; /*need free me*/
		int i_sectors;
		int i_size = READ_ONCE * CDDA_DATA_SIZE;
		int i;
		DWORD i_len;
		unsigned char p_buf[READ_ONCE * CDDA_DATA_SIZE];

		sprintf(psz_out, "track%d.wav", i_track);
		
		p_cddev = cd_open(p_path);
		if (p_cddev == NULL) return -1;
		
		fp = fopen(psz_out, "wb");
		if (fp == NULL) return -1;

		i_sectors = cd_getTracksMap(p_cddev, &p_sectors);

		if (i_sectors <= 0 || i_track > i_sectors) {
			fclose(fp);
			if (i_sectors > 0) free(p_sectors);
			return -1;
		}


		/*write a header*/
		i_len = (DWORD)( p_sectors[i_track+1] - p_sectors[i_track] ) * CDDA_DATA_SIZE;
		cd_build_waveheader(&waveheader, i_len);
		fwrite(&waveheader, 1, sizeof(WAVHDR), fp);	


		for (i = p_sectors[i_track-1]; i<p_sectors[i_track]; i += READ_ONCE) {
			if (cd_read(p_cddev, i, p_buf, READ_ONCE) < 0) {
				fprintf(stderr, "Can not read sector[offset:%d, block:%d]", i, READ_ONCE);
				fclose(fp);
				return -1;
			}
			fwrite(p_buf, 1, i_size, fp);
		}

		fclose(fp);
		
	}
	
	return 0;
}
