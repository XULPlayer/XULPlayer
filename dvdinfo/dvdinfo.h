#ifndef __DVD_INFO_H__
#define __DVD_INFO_H__ 

#define MAX_BUF_SIZE 400*1024 /*400KB*/

typedef struct {
	int width;
	int height;
	int fpsnum;
	int fpsden;
	int darw;
	int darh;
	int video_format;
} video_info_t;

enum {
	TYPE_NOMEDIA = 0,
	TYPE_DVD,
	TYPE_VCD,
	TYPE_SVCD,
	TYPE_CD,
	TYPE_UNKNOWN,
};

/*type detect*/
int detect_media_type(const char *psz_path);

/*CDROM*/
char *get_cdrom_xml();
int get_first_cdrom(char *p_buf);

/*DVD*/
char *get_dvdifo_xml(const char*psz_path);
int get_dvd_videoinfo(int i_titleid, video_info_t *p_video_info);

/*VCD*/
char *get_vcdifo_xml(const char*psz_path);

/*CD*/
char *get_cdifo_xml(const char* psz_path);

#endif
