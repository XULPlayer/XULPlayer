#ifndef __STREAM_DVD_H__
#define __STREAM_DVD_H__

#include <dvdnav/dvd_reader.h>
#include <dvdnav/ifo_types.h>
#include <dvdnav/ifo_read.h>
#include <dvdnav/nav_read.h>

typedef struct {
	int id; // 0 - 31 mpeg; 128 - 159 ac3; 160 - 191 pcm
	int language; 
	int type;
	int channels;
} language_t;

typedef struct {
	int pgc_idx;
	pgc_t *pgc;
	int start_cell;
	int cur_pack;
	int cell_last_pack;
	unsigned int *cell_times_table; /*free me*/
} dvd_chapter_t;

typedef struct {
	ifo_handle_t *vts_file;
	vts_ptt_srpt_t *vts_ptt_srpt;
	pgc_t *pgc;
	// audio datas
	int nr_of_channels;
	language_t audio_streams[32];
	// subtitles
	int nr_of_subtitles;
	language_t subtitles[32];

	dvd_chapter_t *ptts; /*free me*/
} dvd_vts_t;

typedef struct {
	dvd_reader_t *dvd;
	//  dvd_file_t *title;
	ifo_handle_t *vmg_file;
	tt_srpt_t *tt_srpt;
	dvd_vts_t *vtss; /*free me*/
} dvd_info_t;

#endif
