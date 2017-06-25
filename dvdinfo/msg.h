#ifndef __MSG_H__
#define __MSG_H__

#define MSGTR_CantOpenDVD "Couldn't open DVD device: %s\n"
#define MSGTR_DVDspeedCantOpen "Couldn't open DVD device for writing, changing DVD speed needs write access.\n"
#define MSGTR_DVDrestoreSpeed "Restoring DVD speed... "
#define MSGTR_DVDlimitSpeed "Limiting DVD speed to %dKB/s... "
#define MSGTR_DVDlimitFail "failed\n"
#define MSGTR_DVDlimitOk "successful\n"
#define MSGTR_NoDVDSupport "MPlayer was compiled without DVD support, exiting.\n"
#define MSGTR_DVDnumTitles "There are %d titles on this DVD.\n"
#define MSGTR_DVDinvalidTitle "Invalid DVD title number: %d\n"
#define MSGTR_DVDnumChapters "There are %d chapters in this DVD title.\n"
#define MSGTR_DVDinvalidChapter "Invalid DVD chapter number: %d\n"
#define MSGTR_DVDinvalidChapterRange "Invalid chapter range specification %s\n"
#define MSGTR_DVDinvalidLastChapter "Invalid DVD last chapter number: %d\n"
#define MSGTR_DVDnumAngles "There are %d angles in this DVD title.\n"
#define MSGTR_DVDinvalidAngle "Invalid DVD angle number: %d\n"
#define MSGTR_DVDnoIFO "Cannot open the IFO file for DVD title %d.\n"
#define MSGTR_DVDnoVMG "Can't open VMG info!\n"
#define MSGTR_DVDnoVOBs "Cannot open title VOBS (VTS_%02d_1.VOB).\n"
#define MSGTR_DVDnoMatchingAudio "No matching DVD audio language found!\n"
#define MSGTR_DVDaudioChannel "Selected DVD audio channel: %d language: %c%c\n"
#define MSGTR_DVDaudioStreamInfo "audio stream: %d format: %s (%s) language: %s aid: %d.\n"
#define MSGTR_DVDnumAudioChannels "number of audio channels: %d.\n"
#define MSGTR_DVDnoMatchingSubtitle "No matching DVD subtitle language found!\n"
#define MSGTR_DVDsubtitleChannel "Selected DVD subtitle channel: %d language: %c%c\n"
#define MSGTR_DVDsubtitleLanguage "subtitle ( sid ): %d language: %s\n"
#define MSGTR_DVDnumSubtitles "number of subtitles: %d\n"

#ifdef DVDIFO_DEBUG
#define mp_msg printf /*FIXME*/
#else
#define mp_msg
#endif
#endif /*#ifndef __MSG_H__*/