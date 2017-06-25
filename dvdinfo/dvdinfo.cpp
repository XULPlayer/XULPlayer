#include <stdio.h>
#include <string.h> /*memset() strcat() _strdup()*/
#include <windows.h>
#if 0
#include <dvdnav/dvd_reader.h>
#include <dvdnav/ifo_types.h>
#include <dvdnav/ifo_read.h>
#include <dvdnav/nav_read.h>
#else
#include "dvd_reader.h"
#include "ifo_types.h"
#include "ifo_read.h"
#include "nav_read.h"
#endif

#include "msg.h"
#include "dvdinfo.h"
#include "iso_lang.h"

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

#define FIRST_AC3_AID 128
#define FIRST_DTS_AID 136
#define FIRST_MPG_AID 0
#define FIRST_PCM_AID 160
#define FIRST_SID 0x20

const char * dvd_audio_stream_types[8] = { "ac3","unknown","mpeg1","mpeg2ext","lpcm","unknown","dts" };
const char * dvd_audio_stream_channels[6] = { "mono", "stereo", "unknown", "unknown", "5.1/6.1", "5.1" };

#define MAX_TITLE_NUM 50
video_info_t video_infos[MAX_TITLE_NUM];
int title_num = 0;

char *p_xml;

const char *get_langue(const char *psz_lang)
{
    const iso639_lang_t *pl;

    if( psz_lang == NULL || *psz_lang == '\0' )
        return "??";

	for( pl = p_languages; pl->psz_eng_name != NULL; pl++ )
    {
        if( !strcasecmp( pl->psz_iso639_1, psz_lang )) break;
    }

	if( pl->psz_eng_name != NULL )
		return pl->psz_eng_name;

    return "??";
}

static int dvdtimetomsec(dvd_time_t *dt)
{
	static int framerates[4] = {0, 2500, 0, 2997};
	int framerate = framerates[(dt->frame_u & 0xc0) >> 6];
	int msec = (((dt->hour & 0xf0) >> 3) * 5 + (dt->hour & 0x0f)) * 3600000;
	msec += (((dt->minute & 0xf0) >> 3) * 5 + (dt->minute & 0x0f)) * 60000;
	msec += (((dt->second & 0xf0) >> 3) * 5 + (dt->second & 0x0f)) * 1000;
	if(framerate > 0)
		msec += (((dt->frame_u & 0x30) >> 3) * 5 + (dt->frame_u & 0x0f)) * 100000 / framerate;
	return msec;
}

static int get_titleset_length(ifo_handle_t *vts_file, tt_srpt_t *tt_srpt, int title_no)
{
	int vts_ttn;  ///< title number within video title set
	int pgc_no;   ///< program chain number
	int msec;     ///< time length in milliseconds

	msec=0;
	if(!vts_file || !tt_srpt)
		return 0;

	if(vts_file->vtsi_mat && vts_file->vts_pgcit)
	{
		vts_ttn = tt_srpt->title[title_no].vts_ttn - 1;
		pgc_no = vts_file->vts_ptt_srpt->title[vts_ttn].ptt[0].pgcn - 1;
		msec = dvdtimetomsec(&vts_file->vts_pgcit->pgci_srp[pgc_no].pgc->playback_time);
	}
	return msec;
}

static int describe_titleset(dvd_reader_t *dvd, tt_srpt_t *tt_srpt, int vts_no)
{
	ifo_handle_t *vts_file;
	int title_no, msec=0;

	vts_file = ifoOpen(dvd, vts_no);
	if(!vts_file)
		return 0;

	if(!vts_file->vtsi_mat || !vts_file->vts_pgcit)
	{
		ifoClose(vts_file);
		return 0;
	}

	for(title_no = 0; title_no < tt_srpt->nr_of_srpts; title_no++)
	{
		if (tt_srpt->title[title_no].title_set_nr != vts_no)
			continue;
		msec = get_titleset_length(vts_file, tt_srpt, title_no);
		mp_msg("ID_DVD_TITLE_%d_LENGTH=%d.%03d\n", title_no + 1, msec / 1000, msec % 1000);
		sprintf(p_xml, "<len>%d</len>", msec);
		p_xml += strlen(p_xml);
	}
	ifoClose(vts_file);
	return 1;
}

static void list_chapters(pgc_t *pgc)
{
    int i, cell;
    unsigned int t=0, t2=0;

    if(pgc->nr_of_programs < 2)
       return;

    mp_msg("CHAPTERS: ");
    for(i=0; i<pgc->nr_of_programs-1; i++)
    {
        cell = pgc->program_map[i]; //here the cell is 1-based
        t2 = t/1000;
        mp_msg("%02d:%02d:%02d,", t2/3600, (t2/60)%60, t2%60);
        while(cell < pgc->program_map[i+1]) {
            if(!(pgc->cell_playback[cell-1].block_type == BLOCK_TYPE_ANGLE_BLOCK &&
                 pgc->cell_playback[cell-1].block_mode != BLOCK_MODE_FIRST_CELL)
            )
                t += dvdtimetomsec(&pgc->cell_playback[cell-1].playback_time);
            cell++;
        }
    }
    mp_msg("\n");
}

/*
*/
char *get_dvdifo_xml(const char*psz_path)
{
	char *psz_xml;
	int ttn, pgc_id, pgn, i, n;
	dvd_reader_t *dvd;
	ifo_handle_t *vmg_file;
	tt_srpt_t *tt_srpt;
	
	int dvd_title, dvd_chapter;
	ifo_handle_t *vts_file;
	pgc_t *pgc;
	
	int msec, sec;
	int max_len = -1, mainmovie_id = -1;

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	char psz_tmp[5];

	if(psz_path == NULL || psz_path[0] == '\0' || psz_path[1] != ':') {
		return NULL;
	}
	
	sprintf(psz_tmp, "%c:\\*", psz_path[0]);
	hFind = FindFirstFile(psz_tmp, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		return NULL;
	}
	/**
	* open dvd
	*/
	dvd = DVDOpen(psz_path);

	if(!dvd) {
		mp_msg(MSGTR_CantOpenDVD, psz_path);
		return NULL;
	}
	/**
	* get VMG
	**/
	vmg_file = ifoOpen(dvd, 0);
	if(!vmg_file) {
		mp_msg(MSGTR_DVDnoVMG);
		DVDClose(dvd);
		return NULL;
	}
	
	psz_xml = (char*)malloc(sizeof(char)*MAX_BUF_SIZE);
	if (psz_xml == NULL) return NULL;

	psz_xml[0] ='\0';
	p_xml = psz_xml;
	sprintf(p_xml, "<?xml version=\"1.0\"?><dvdifo>");
	p_xml += strlen(p_xml);
	
	tt_srpt = vmg_file->tt_srpt; /*!!*/

	sprintf(p_xml, "<vmg>");
	p_xml += strlen(p_xml);
	{
		unsigned char discid [16]; ///< disk ID, a 128 bit MD5 sum
		int vts_no;   ///< video title set number

		mp_msg("ID_DVD_TITLES=%d\n", tt_srpt->nr_of_srpts);
		sprintf(p_xml, "<titlenum id=\"titlenum\" num=\"%d\"/>", tt_srpt->nr_of_srpts);
		p_xml += strlen(p_xml);
#if 0
		for (title_no = 0; title_no < tt_srpt->nr_of_srpts; title_no++)
		{
			mp_msg("ID_DVD_TITLE_%d_CHAPTERS=%d\n", title_no + 1, tt_srpt->title[title_no].nr_of_ptts);
			mp_msg("ID_DVD_TITLE_%d_ANGLES=%d\n", title_no + 1, tt_srpt->title[title_no].nr_of_angles);
		}
#endif
		mp_msg("ID_DVD_VTSS=%d\n", vmg_file->vts_atrt->nr_of_vtss);

		for (vts_no = 1; vts_no <= vmg_file->vts_atrt->nr_of_vtss; vts_no++)
		{
			describe_titleset(dvd, tt_srpt, vts_no);
		}
		sprintf(p_xml, "<discID>");
		p_xml += strlen(p_xml);
		if (DVDDiscID(dvd, discid) >= 0)
		{
			int i;
			mp_msg("ID_DVD_DISC_ID=");
			for (i = 0; i < 16; i ++) {
				mp_msg("%02X", discid[i]);
				sprintf(p_xml, "%02X", discid[i]);
				p_xml += strlen(p_xml);
			}
			mp_msg("\n");
		}
		sprintf(p_xml, "</discID>");
		p_xml += strlen(p_xml);
	}
	sprintf(p_xml, "</vmg>");
	p_xml += strlen(p_xml);

	title_num = 0;
	
	sprintf(p_xml, "<titles>");
	p_xml += strlen(p_xml);
	for (dvd_title = 0; dvd_title < tt_srpt->nr_of_srpts; dvd_title++)
	{
		/**
		* Load the VTS information for the title set our title is in.
		*/
		static int framerates[4] = {0, 2500, 0, 2997};
		static int fpsnums[4] = {0, 25000, 0, 30000};
		static int fpsdens[4] = {0, 1000, 0, 1001};
		int framerate;
		video_attr_t *video;
		int width;
		int height;
		
		vts_file = ifoOpen( dvd, tt_srpt->title[dvd_title].title_set_nr );
		if(!vts_file) {
			mp_msg(MSGTR_DVDnoIFO, tt_srpt->title[dvd_title].title_set_nr );
			ifoClose( vmg_file );
			DVDClose( dvd );
			free(psz_xml);
			return NULL;
		}


		if (vts_file->vts_pgcit == NULL) {
			continue;
		}

		ttn = tt_srpt->title[dvd_title].vts_ttn - 1;

		/**
		* we can open vobs now, but do not use the function until now.
		title = DVDOpenFile(dvd, tt_srpt->title[dvd_title].title_set_nr, DVD_READ_TITLE_VOBS);
		*/
		pgc = vts_file->vts_pgcit ? vts_file->vts_pgcit->pgci_srp[ttn].pgc : NULL; /*PGC*/
		
		msec = dvdtimetomsec(&pgc->playback_time);
		sec = msec / 1000;
		if (msec > max_len) {
			max_len = msec;
			mainmovie_id = dvd_title + 1;
		}
		
		framerate = framerates[(pgc->playback_time.frame_u & 0xc0) >> 6];

		if (title_num > MAX_TITLE_NUM) title_num = MAX_TITLE_NUM - 1; /*FIXME*/
		
		video_infos[title_num].fpsnum =  fpsnums[(pgc->playback_time.frame_u & 0xc0) >> 6]; 
		video_infos[title_num].fpsden =  fpsdens[(pgc->playback_time.frame_u & 0xc0) >> 6]; 

		video  = &vts_file->vtsi_mat->vts_video_attr;

		if (video->display_aspect_ratio == 0)  {/*4:3*/
			video_infos[title_num].darw = 4;
			video_infos[title_num].darh = 3;
		}
		else if (video->display_aspect_ratio == 3){ /*16:9*/
			video_infos[title_num].darw = 16;
			video_infos[title_num].darh = 9;
		}
		else {
			video_infos[title_num].darw = 0;
			video_infos[title_num].darh = 0;
		}
		
		height = (video->video_format == 0)?480:576;
		switch(video->picture_size) {
		case 0:
			width = 720;
			break;
		case 1:
			width = 704;
			break;
		case 2:
			width = 352;
			break;
		case 3:
			width = 352;
			height /= 2;
			break;      
		default:
			width = 0;
			height = 0;
			break;
		}

		video_infos[title_num].width = width;
		video_infos[title_num].height = height;
		video_infos[title_num].video_format = (int)video->video_format;
		title_num++;
		
		sprintf(p_xml, "<title id=\"Track%d\" "
						"playbackTime=\"%d\" "
						"playbackTimeStr=\"%02d:%02d:%02d.%03d\" "
						"framerate=\"%d\" "
						"video_format=\"%d\" "
						"width=\"%d\" "
						"height=\"%d\" "
						"display_aspect_ratio=\"%d\">",
						dvd_title + 1,
						msec,
						sec/3600, (sec/60)%60, sec%60, msec % 1000,
						framerate,
						(int)video->video_format,
						width,
						height,
						(int)video->display_aspect_ratio);
		p_xml += strlen(p_xml);

		/**
		* Check number of audio channels and types
		*/
		sprintf(p_xml, "<audios>");
		p_xml += strlen(p_xml);
		n = 0;
		if(vts_file->vts_pgcit) {
			for(i=0;i<8;i++) {
				if(pgc->audio_control[i] & 0x8000) { /*present*/
					audio_attr_t * audio = &vts_file->vtsi_mat->vts_audio_attr[i];
					int language = 0;
					char tmp[] = "unknown";
					int id;

					if(audio->lang_type == 1) {
						language=audio->lang_code;
						tmp[0]=language>>8;
						tmp[1]=language&0xff;
						tmp[2]=0;
					}

					id=pgc->audio_control[i] >> 8 & 7;
					switch(audio->audio_format) {
					case 0: // ac3
						id+=FIRST_AC3_AID;
						break;
					case 6: // dts
						id+=FIRST_DTS_AID;
						break;
					case 2: // mpeg layer 1/2/3
					case 3: // mpeg2 ext
						id+=FIRST_MPG_AID;
						break;
					case 4: // lpcm
						id+=FIRST_PCM_AID;
						break;
					}

					// Pontscho: to my mind, tha channels:
					//  1 - stereo
					//  5 - 5.1
					sprintf(p_xml,
						"<audio id=\"audio%d\" "
						"aid=\"%d\" "
						"language=\"%s\" "
						"type=\"%s\" "
						"channels=\"%s\"/>",
						++n,
						id,
						get_langue(tmp),
						dvd_audio_stream_types[ audio->audio_format ],
						dvd_audio_stream_channels[ audio->channels ]);
					p_xml += strlen(p_xml);
				} /*if*/
			} /*for*/
		} /*if*/
		sprintf(p_xml, "</audios>");
		p_xml += strlen(p_xml);

		/**
		* Check number of subtitles and language
		*/
		sprintf(p_xml, "<subtitles>");
		p_xml += strlen(p_xml);
		n = 0;
		for(i=0;i<32;i++) {
			if(pgc->subp_control[i] & 0x80000000) {
				subp_attr_t * subtitle = &vts_file->vtsi_mat->vts_subp_attr[i];
				int language = 0;
				char tmp[] = "unknown";
				int id;
				
				if(subtitle->type == 1) {
					language=subtitle->lang_code;
					tmp[0]=language>>8;
					tmp[1]=language&0xff;
					tmp[2]=0;
				}

				if(video->display_aspect_ratio == 0) /* 4:3 */
					id = pgc->subp_control[i] >> 24 & 31;
				else if(video->display_aspect_ratio == 3) /* 16:9 */
					id = pgc->subp_control[i] >> 8 & 31;

				sprintf(p_xml,
					"<subtitle id=\"subtitle%d\" "
					"sid=\"%d\" " /*FIXME*/
					"language=\"%s\"/>",
					++n,
					id + FIRST_SID,
					get_langue(tmp));
				p_xml += strlen(p_xml);
			}
		}
		sprintf(p_xml, "</subtitles>");
		p_xml += strlen(p_xml);
		/**
		* chapters in title.
		*/
		sprintf(p_xml, "<chapters>");
		p_xml += strlen(p_xml);
		#define nr_of_ptts (tt_srpt->title[dvd_title].nr_of_ptts)
		n = 0;
		msec = 0;
		for(dvd_chapter=0;dvd_chapter<nr_of_ptts;dvd_chapter++) {
			dvd_chapter_t ptt;
			int dvd_angle = 1; /*FIXME*/
			int end_cell;
			int sec2;
			int msec2;

			pgc_id = vts_file->vts_ptt_srpt->title[ttn].ptt[dvd_chapter].pgcn; // local
			pgn  = vts_file->vts_ptt_srpt->title[ttn].ptt[dvd_chapter].pgn;  // local
			ptt.pgc_idx = pgc_id-1;
			ptt.pgc = vts_file->vts_pgcit->pgci_srp[pgc_id-1].pgc;
			ptt.start_cell = ptt.pgc->program_map[pgn-1] - 1;
			
			if (pgn < ptt.pgc->nr_of_programs - 1) {
				end_cell =  ptt.pgc->program_map[pgn] - 2;
			} else {
				end_cell = ptt.pgc->nr_of_cells - 1;
			}
			if(ptt.pgc->cell_playback[ptt.start_cell].block_type == BLOCK_TYPE_ANGLE_BLOCK ) 
				ptt.start_cell += dvd_angle;
			ptt.cur_pack = ptt.pgc->cell_playback[ptt.start_cell].first_sector;
			ptt.cell_last_pack=ptt.pgc->cell_playback[ptt.start_cell].last_sector;
			
			msec2 = msec;
			sec2 = msec2 / 1000;
			for (i = ptt.start_cell; i <= end_cell; i++) {
				msec += dvdtimetomsec(&ptt.pgc->cell_playback[i].playback_time);
			}
			sec = msec / 1000;
			sprintf(p_xml,
				"<chapter id=\"Chapter%d\" "
				"EntryCell=\"%d\" pack=\"0x%X-0x%X\" "
				"PlaybackTimeStart=\"%d\" "
				"PlaybackTimeEnd=\"%d\" "
				"playbackTimeStartStr=\"%02d:%02d:%02d\" "
				"playbackTimeEndStr=\"%02d:%02d:%02d\"/>",
				++n,
				ptt.start_cell + 1,
				ptt.cur_pack,ptt.cell_last_pack,
				msec2,
				msec,
				sec2/3600, (sec2/60)%60, sec2%60,
				sec/3600, (sec/60)%60, sec%60);
			p_xml += strlen(p_xml);
#if 0
			for(i=0; i<ptt.pgc->nr_of_cells; i++)
				ptt.cell_times_table[i] = dvdtimetomsec(&ptt.pgc->cell_playback[i].playback_time);
#endif			
			list_chapters(ptt.pgc);
		} /*chapters*/
		sprintf(p_xml, "</chapters>");
		p_xml += strlen(p_xml);

		sprintf(p_xml, "</title>");
		p_xml += strlen(p_xml);
		ifoClose(vts_file);
	}/*VTSs*/

	sprintf(p_xml, "</titles><mainmovie id=\"%d\"/></dvdifo>", mainmovie_id);

	ifoClose(vmg_file);
	DVDClose(dvd);

	psz_xml = (char*)realloc(psz_xml, strlen(psz_xml) + 1);
	return psz_xml;  
}

int get_dvd_videoinfo(int i_titleid, video_info_t *p_video_info)
{
	if (p_video_info == NULL || i_titleid <= 0 || i_titleid > title_num) return -1;
	memcpy(p_video_info, &video_infos[i_titleid-1], sizeof(video_info_t));

	return 0;
}

