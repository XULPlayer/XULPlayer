#include <stdio.h>
#include <string.h> /*memset*/
#include <dvdnav/dvd_reader.h>
#include "common.h"
#include "stream_dvd.h"


#define FIRST_AC3_AID 128
#define FIRST_DTS_AID 136
#define FIRST_MPG_AID 0
#define FIRST_PCM_AID 160

const char * dvd_audio_stream_types[8] = { "ac3","unknown","mpeg1","mpeg2ext","lpcm","unknown","dts" };
const char * dvd_audio_stream_channels[6] = { "mono", "stereo", "unknown", "unknown", "5.1/6.1", "5.1" };

void usage(const char *program_name)
{
	fprintf(stderr, "Usage: %s <dvd path>\n", program_name);
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

dvd_info_t *get_dvdifo(const char * p_dvd_device)
{
	int ttn, pgc_id, pgn, i;
	dvd_reader_t *dvd;
	ifo_handle_t *vmg_file;
	tt_srpt_t *tt_srpt;

	dvd_info_t *d;

	int dvd_title, dvd_chapter;
	ifo_handle_t *vts_file;
	pgc_t *pgc;

	/**
	* open dvd
	*/
	dvd = DVDOpen(p_dvd_device);

	if(!dvd) {
		mp_msg(MSGTR_CantOpenDVD,p_dvd_device);
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

	tt_srpt = vmg_file->tt_srpt; /*!!*/

	{
		int title_no; ///< title number
		unsigned char discid [16]; ///< disk ID, a 128 bit MD5 sum
		int vts_no;   ///< video title set number

		mp_msg("ID_DVD_TITLES=%d\n", tt_srpt->nr_of_srpts);
		for (title_no = 0; title_no < tt_srpt->nr_of_srpts; title_no++)
		{
			mp_msg("ID_DVD_TITLE_%d_CHAPTERS=%d\n", title_no + 1, tt_srpt->title[title_no].nr_of_ptts);
			mp_msg("ID_DVD_TITLE_%d_ANGLES=%d\n", title_no + 1, tt_srpt->title[title_no].nr_of_angles);
		}

		mp_msg("ID_DVD_VTSS=%d\n", vmg_file->vts_atrt->nr_of_vtss);

		for (vts_no = 1; vts_no <= vmg_file->vts_atrt->nr_of_vtss; vts_no++)
		{
			describe_titleset(dvd, tt_srpt, vts_no);
		}
		if (DVDDiscID(dvd, discid) >= 0)
		{
			int i;
			mp_msg("ID_DVD_DISC_ID=");
			for (i = 0; i < 16; i ++)
				mp_msg("%02X", discid[i]);
			mp_msg("\n");
		}
	}

	// store data
	d = malloc(sizeof(dvd_info_t));
	memset(d, 0, sizeof(dvd_info_t));
	d->dvd=dvd;
	d->vmg_file=vmg_file;
	d->tt_srpt=tt_srpt;

	d->vtss = malloc(sizeof(dvd_vts_t)*tt_srpt->nr_of_srpts);
	memset(d->vtss, 0, sizeof(dvd_vts_t)*tt_srpt->nr_of_srpts);

	for (dvd_title = 0; dvd_title < tt_srpt->nr_of_srpts; dvd_title++)
	{
		dvd_vts_t *p_vts = &d->vtss[dvd_title];
		ttn = tt_srpt->title[dvd_title].vts_ttn - 1;
		
		mp_msg("\ntitle:%d\n", dvd_title+1);
		/**
		* Load the VTS information for the title set our title is in.
		*/
		vts_file = ifoOpen( dvd, tt_srpt->title[dvd_title].title_set_nr );
		if(!vts_file) {
			mp_msg(MSGTR_DVDnoIFO, tt_srpt->title[dvd_title].title_set_nr );
			ifoClose( vmg_file );
			DVDClose( dvd );
			free(d);
			return NULL;
		}
		p_vts->vts_file = vts_file;
		/**
		* we can open vobs now, but do not use the function until now.
		title = DVDOpenFile(dvd, tt_srpt->title[dvd_title].title_set_nr, DVD_READ_TITLE_VOBS);
		*/
		pgc = vts_file->vts_pgcit ? vts_file->vts_pgcit->pgci_srp[ttn].pgc : NULL; /*PGC*/
		/**
		* Check number of audio channels and types
		*/
		p_vts->nr_of_channels = 0;
		if(vts_file->vts_pgcit) {
			for(i=0;i<8;i++) {
				if(pgc->audio_control[i] & 0x8000) { /*present*/
					audio_attr_t * audio = &vts_file->vtsi_mat->vts_audio_attr[i];
					int language = 0;
					char tmp[] = "unknown";
					language_t *audio_stream = &p_vts->audio_streams[p_vts->nr_of_channels];

					if(audio->lang_type == 1) {
						language=audio->lang_code;
						tmp[0]=language>>8;
						tmp[1]=language&0xff;
						tmp[2]=0;
					}

					audio_stream->language=language;
					audio_stream->id=pgc->audio_control[i] >> 8 & 7;
					switch(audio->audio_format) {
					case 0: // ac3
						audio_stream->id+=FIRST_AC3_AID;
						break;
					case 6: // dts
						audio_stream->id+=FIRST_DTS_AID;
						break;
					case 2: // mpeg layer 1/2/3
					case 3: // mpeg2 ext
						audio_stream->id+=FIRST_MPG_AID;
						break;
					case 4: // lpcm
						audio_stream->id+=FIRST_PCM_AID;
						break;
					}

					audio_stream->type=audio->audio_format;
					// Pontscho: to my mind, tha channels:
					//  1 - stereo
					//  5 - 5.1
					audio_stream->channels=audio->channels;
					mp_msg(MSGTR_DVDaudioStreamInfo,
						p_vts->nr_of_channels,
						dvd_audio_stream_types[ audio->audio_format ],
						dvd_audio_stream_channels[ audio->channels ],
						tmp,
						audio_stream->id
						);
					mp_msg("ID_AUDIO_ID=%d\n", audio_stream->id);
					if(language && tmp[0])
						mp_msg("ID_AID_%d_LANG=%s\n", audio_stream->id, tmp);

					p_vts->nr_of_channels++;
				} /*if*/
			} /*for*/
		} /*if*/
		mp_msg(MSGTR_DVDnumAudioChannels,p_vts->nr_of_channels );

		/**
		* Check number of subtitles and language
		*/
		p_vts->nr_of_subtitles = 0;
		for(i=0;i<32;i++) {
			if(pgc->subp_control[i] & 0x80000000) {
				subp_attr_t * subtitle = &vts_file->vtsi_mat->vts_subp_attr[i];
				video_attr_t *video = &vts_file->vtsi_mat->vts_video_attr;
				int language = 0;
				char tmp[] = "unknown";
				language_t *sub_stream = &p_vts->subtitles[p_vts->nr_of_subtitles];

				if(subtitle->type == 1) {
					language=subtitle->lang_code;
					tmp[0]=language>>8;
					tmp[1]=language&0xff;
					tmp[2]=0;
				}

				sub_stream->language=language;
				sub_stream->id=p_vts->nr_of_subtitles;
				if(video->display_aspect_ratio == 0) /* 4:3 */
					sub_stream->id = pgc->subp_control[i] >> 24 & 31;
				else if(video->display_aspect_ratio == 3) /* 16:9 */
					sub_stream->id = pgc->subp_control[i] >> 8 & 31;

				mp_msg(MSGTR_DVDsubtitleLanguage, p_vts->nr_of_subtitles, tmp);
				mp_msg("ID_SUBTITLE_ID=%d\n", p_vts->nr_of_subtitles);
				if(language && tmp[0])
					mp_msg("ID_SID_%d_LANG=%s\n", p_vts->nr_of_subtitles, tmp);
				p_vts->nr_of_subtitles++;
			}
		}
		mp_msg(MSGTR_DVDnumSubtitles,p_vts->nr_of_subtitles);
		/**
		* chapters in title.
		*/
		#define nr_of_ptts (tt_srpt->title[dvd_title].nr_of_ptts)
		p_vts->ptts = 
			malloc(sizeof(dvd_chapter_t)*nr_of_ptts);
		memset(p_vts->ptts, 0, sizeof(dvd_chapter_t)*nr_of_ptts);

		for(dvd_chapter=0;dvd_chapter<nr_of_ptts;dvd_chapter++) {
			dvd_chapter_t *p_ptt = &p_vts->ptts[dvd_chapter];
			int dvd_angle = 1; /*FIXME*/
			pgc_id = vts_file->vts_ptt_srpt->title[ttn].ptt[dvd_chapter].pgcn; // local
			pgn  = vts_file->vts_ptt_srpt->title[ttn].ptt[dvd_chapter].pgn;  // local
			p_ptt->pgc_idx = pgc_id-1;
			p_ptt->pgc = vts_file->vts_pgcit->pgci_srp[pgc_id-1].pgc;
			p_ptt->start_cell = p_ptt->pgc->program_map[pgn-1] - 1;

			if(p_ptt->pgc->cell_playback[p_ptt->start_cell].block_type == BLOCK_TYPE_ANGLE_BLOCK ) 
				p_ptt->start_cell += dvd_angle;
			p_ptt->cur_pack = p_ptt->pgc->cell_playback[p_ptt->start_cell].first_sector;
			p_ptt->cell_last_pack=p_ptt->pgc->cell_playback[p_ptt->start_cell].last_sector;
			mp_msg("chapter %d: start cell: %d  pack: 0x%X-0x%X\n",
				dvd_chapter+1,
				p_ptt->start_cell,p_ptt->cur_pack,p_ptt->cell_last_pack);

			//assign cell_times_table
			p_ptt->cell_times_table = malloc(sizeof(unsigned int) * p_ptt->pgc->nr_of_cells);
			if(p_ptt->cell_times_table == NULL) {
				mp_msg("have no memory!\n");
				goto error;
			}
			for(i=0; i<p_ptt->pgc->nr_of_cells; i++)
				p_ptt->cell_times_table[i] = dvdtimetomsec(&p_ptt->pgc->cell_playback[i].playback_time);
			list_chapters(p_ptt->pgc);
		} /*chapters*/
	}/*VTSs*/
	
	return d;
error:
	/*free all*/
	return NULL;  
}

/*
*/
#define MAX_BUF_SIZE 10*1000 /*10KB*/
char *p_xml;
char *get_dvdifo_xml(const char*psz_path)
{
	char psz_xml[MAX_BUF_SIZE];
	int ttn, pgc_id, pgn, i, n;
	dvd_reader_t *dvd;
	ifo_handle_t *vmg_file;
	tt_srpt_t *tt_srpt;
	
	int dvd_title, dvd_chapter;
	ifo_handle_t *vts_file;
	pgc_t *pgc;

	if(psz_path == NULL || *psz_path == '\0') {
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

	psz_xml[0] ='\0';
	p_xml = psz_xml;
	sprintf(p_xml, "<dvdifo>");
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

	sprintf(p_xml, "<titles>");
	p_xml += strlen(p_xml);
	for (dvd_title = 0; dvd_title < tt_srpt->nr_of_srpts; dvd_title++)
	{
		sprintf(p_xml, "<title id=\"title%d\">", dvd_title + 1);
		p_xml += strlen(p_xml);

		ttn = tt_srpt->title[dvd_title].vts_ttn - 1;
		
		mp_msg("\ntitle:%d\n", dvd_title+1);
		/**
		* Load the VTS information for the title set our title is in.
		*/
		vts_file = ifoOpen( dvd, tt_srpt->title[dvd_title].title_set_nr );
		if(!vts_file) {
			mp_msg(MSGTR_DVDnoIFO, tt_srpt->title[dvd_title].title_set_nr );
			ifoClose( vmg_file );
			DVDClose( dvd );
			return NULL;
		}
		/**
		* we can open vobs now, but do not use the function until now.
		title = DVDOpenFile(dvd, tt_srpt->title[dvd_title].title_set_nr, DVD_READ_TITLE_VOBS);
		*/
		pgc = vts_file->vts_pgcit ? vts_file->vts_pgcit->pgci_srp[ttn].pgc : NULL; /*PGC*/
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
						tmp,
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
				video_attr_t *video = &vts_file->vtsi_mat->vts_video_attr;
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
					id,
					tmp);
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
		for(dvd_chapter=0;dvd_chapter<nr_of_ptts;dvd_chapter++) {
			dvd_chapter_t ptt;
			int dvd_angle = 1; /*FIXME*/

			pgc_id = vts_file->vts_ptt_srpt->title[ttn].ptt[dvd_chapter].pgcn; // local
			pgn  = vts_file->vts_ptt_srpt->title[ttn].ptt[dvd_chapter].pgn;  // local
			ptt.pgc_idx = pgc_id-1;
			ptt.pgc = vts_file->vts_pgcit->pgci_srp[pgc_id-1].pgc;
			ptt.start_cell = ptt.pgc->program_map[pgn-1] - 1;

			if(ptt.pgc->cell_playback[ptt.start_cell].block_type == BLOCK_TYPE_ANGLE_BLOCK ) 
				ptt.start_cell += dvd_angle;
			ptt.cur_pack = ptt.pgc->cell_playback[ptt.start_cell].first_sector;
			ptt.cell_last_pack=ptt.pgc->cell_playback[ptt.start_cell].last_sector;
			sprintf(p_xml,
				"<chapter id=\"chapter%d\" "
				"EntryCell=\"%d\" pack=\"0x%X-0x%X\" "
				"PlaybackTime=\"%d\"/>",
				++n,
				ptt.start_cell + 1,
				ptt.cur_pack,ptt.cell_last_pack,
				dvdtimetomsec(&ptt.pgc->cell_playback[ptt.start_cell].playback_time));
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
	sprintf(p_xml, "</titles></dvdifo>");
	ifoClose(vmg_file);
	DVDClose(dvd);
	return _strdup(psz_xml);  
}

int main()
{
	char *p_ret = get_dvdifo_xml("G:\\");
	FILE *fp = fopen("ifo.xml", "w");
	if (p_ret == NULL) {
		printf("error!\n");
		return -1;
	}

	fprintf(fp, "%s", p_ret);
	
	free(p_ret);
	fclose(fp);
	getchar();
	return 0;
}
