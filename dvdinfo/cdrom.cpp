/*
* filename:cdrominfo.cpp 
* Come from VLC
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef HAVE_SYS_TYPES_H
#   include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#   include <sys/stat.h>
#endif
#ifdef HAVE_FCNTL_H 
#   include <fcntl.h>
#endif

#if defined( SYS_BSDI )
#   include <dvd.h>
#elif defined ( __APPLE__ )
#   include <CoreFoundation/CFBase.h>
#   include <IOKit/IOKitLib.h>
#   include <IOKit/storage/IOCDTypes.h>
#   include <IOKit/storage/IOCDMedia.h>
#   include <IOKit/storage/IOCDMediaBSDClient.h>
#elif defined( HAVE_SCSIREQ_IN_SYS_SCSIIO_H )
#   include <sys/inttypes.h>
#   include <sys/cdio.h>
#   include <sys/scsiio.h>
#elif defined( HAVE_IOC_TOC_HEADER_IN_SYS_CDIO_H )
#   include <sys/cdio.h>
#   include <sys/cdrio.h>
#elif defined( WIN32 )
#   include <windows.h>
#   include <winioctl.h>
#   include <io.h>
#   include "myaspi32.h"
#elif defined (__linux__)
#   include <sys/ioctl.h>
#   include <linux/cdrom.h>
#else
#   error FIXME
#endif

#include "cdrom.h"
#include "msg.h"
/*****************************************************************************
* The vcddev structure
*****************************************************************************/
struct vcddev_s
{
	char   *psz_dev;                                      /* vcd device name */

	/* Section used in vcd image mode */
	int    i_vcdimage_handle;                   /* vcd image file descriptor */
	int    i_tracks;                          /* number of tracks of the vcd */
	int    *p_sectors;                           /* tracks layout on the vcd */

	/* Section used in vcd device mode */

#ifdef WIN32
	HANDLE h_device_handle;                         /* vcd device descriptor */
	HMODULE  hASPI;
	short i_sid;
	DWORD (*lpSendCommand)(LPSRB);

#else
	int    i_device_handle;                         /* vcd device descriptor */
#endif

};


/*****************************************************************************
* Misc. Macros
*****************************************************************************/
/* LBA = msf.frame + 75 * ( msf.second + 60 * msf.minute ) */
#define MSF_TO_LBA(min, sec, frame) ((int)frame + 75 * (sec + 60 * min))
/* LBA = msf.frame + 75 * ( msf.second - 2 + 60 * msf.minute ) */
#define MSF_TO_LBA2(min, sec, frame) ((int)frame + 75 * (sec -2 + 60 * min))

#ifndef O_BINARY
#   define O_BINARY 0
#endif

#define VCDDEV_T 1

/*****************************************************************************
* Platform specifics
*****************************************************************************/
#if defined( __APPLE__ )
#define darwin_freeTOC( p ) free( (void*)p )
#define CD_MIN_TRACK_NO 01
#define CD_MAX_TRACK_NO 99
#endif

#if defined( WIN32 )

/* Win32 DeviceIoControl specifics */
#ifndef MAXIMUM_NUMBER_TRACKS
#    define MAXIMUM_NUMBER_TRACKS 100
#endif
typedef struct _TRACK_DATA {
	UCHAR Reserved;
	UCHAR Control : 4;
	UCHAR Adr : 4;
	UCHAR TrackNumber;
	UCHAR Reserved1;
	UCHAR Address[4];
} TRACK_DATA, *PTRACK_DATA;
typedef struct _CDROM_TOC {
	UCHAR Length[2];
	UCHAR FirstTrack;
	UCHAR LastTrack;
	TRACK_DATA TrackData[MAXIMUM_NUMBER_TRACKS];
} CDROM_TOC, *PCDROM_TOC;
typedef enum _TRACK_MODE_TYPE {
	YellowMode2,
	XAForm2,
	CDDA
} TRACK_MODE_TYPE, *PTRACK_MODE_TYPE;
typedef struct __RAW_READ_INFO {
	LARGE_INTEGER DiskOffset;
	ULONG SectorCount;
	TRACK_MODE_TYPE TrackMode;
} RAW_READ_INFO, *PRAW_READ_INFO;

#ifndef IOCTL_CDROM_BASE
#    define IOCTL_CDROM_BASE FILE_DEVICE_CD_ROM
#endif
#ifndef IOCTL_CDROM_READ_TOC
#    define IOCTL_CDROM_READ_TOC CTL_CODE(IOCTL_CDROM_BASE, 0x0000, \
	METHOD_BUFFERED, FILE_READ_ACCESS)
#endif
#ifndef IOCTL_CDROM_RAW_READ
#define IOCTL_CDROM_RAW_READ CTL_CODE(IOCTL_CDROM_BASE, 0x000F, \
	METHOD_OUT_DIRECT, FILE_READ_ACCESS)
#endif

#define SCSI_INQUIRY    0x12    // Inquiry (MANDATORY)

/* Win32 aspi specific */
#define WIN_NT               ( GetVersion() < 0x80000000 )
#define ASPI_HAID           0
#define ASPI_TARGET         0
#define DTYPE_CDROM         0x05

#define SC_GET_DEV_TYPE     0x01
#define SC_EXEC_SCSI_CMD    0x02
#define SC_GET_DISK_INFO    0x06
#define SS_COMP             0x01
#define SS_PENDING          0x00
#define SS_NO_ADAPTERS      0xE8
#define SRB_DIR_IN          0x08
#define SRB_DIR_OUT         0x10
#define SRB_EVENT_NOTIFY    0x40

#define READ_CD 0xbe
#define SECTOR_TYPE_MODE2_FORM2 0x14
#define SECTOR_TYPE_CDDA 0x04
#define READ_CD_RAW_MODE2 0xF0
#define READ_CD_USERDATA 0x10

#define READ_TOC 0x43
#define READ_TOC_FORMAT_TOC 0x0


DWORD (*pfnGetASPI32SupportInfo)(void);
DWORD (*pfnSendASPI32Command)(LPSRB);
BOOL  (*pfnGetASPI32Buffer)(PASPI32BUFF);
BOOL  (*pfnFreeASPI32Buffer)(PASPI32BUFF);
BOOL  (*pfnTranslateASPI32Address)(PDWORD,PDWORD);
DWORD (*pfnGetASPI32DLLVersion)(void);

static HMODULE hinstWNASPI32 = NULL;

static HMODULE load_Aspi( void );
static void unload_Aspi( HMODULE );
int GetNumAdapters( void );

#endif /* WIN32 */

/*****************************************************************************
* Local Prototypes
*****************************************************************************/
static int    OpenVCDImage( const char *, struct vcddev_s * );
static void   CloseVCDImage( struct vcddev_s * );

#if defined( __APPLE__ )
static CDTOC *darwin_getTOC( const struct vcddev_s * );
static int    darwin_getNumberOfTracks( CDTOC *, int );

#elif defined( WIN32 )
static int    win32_vcd_open( const char *, struct vcddev_s *);
#endif

/*****************************************************************************
* ioctl_Open: Opens a VCD device or file and returns an opaque handle
*****************************************************************************/
vcddev_t *ioctl_Open( const char *psz_dev )
{
	int i_ret;
	int b_is_file;
	vcddev_t *p_vcddev;
#ifndef WIN32
	struct stat fileinfo;
#endif

	if( !psz_dev ) return NULL;

	/*
	*  Initialize structure with default values
	*/
	p_vcddev = (vcddev_t *)malloc( sizeof(vcddev_t) );
	if( p_vcddev == NULL )
	{
		mp_msg( "out of memory" );
		return NULL;
	}
	p_vcddev->i_vcdimage_handle = -1;
	p_vcddev->psz_dev = NULL;
	b_is_file = 1;

	/*
	*  Check if we are dealing with a device or a file (vcd image)
	*/
#ifdef WIN32
	if( (strlen( psz_dev ) == 2 && psz_dev[1] == ':') )
	{
		b_is_file = 0;
	}

#else
	if( stat( psz_dev, &fileinfo ) < 0 )
	{
		free( p_vcddev );
		return NULL;
	}

	/* Check if this is a block/char device */
	if( S_ISBLK( fileinfo.st_mode ) || S_ISCHR( fileinfo.st_mode ) )
		b_is_file = 0;
#endif

	if( b_is_file )
	{
		i_ret = OpenVCDImage( psz_dev, p_vcddev );
	}
	else
	{
		/*
		*  open the vcd device
		*/

#ifdef WIN32
		i_ret = win32_vcd_open( psz_dev, p_vcddev );
#else
		p_vcddev->i_device_handle = -1;
		p_vcddev->i_device_handle = open( psz_dev, O_RDONLY | O_NONBLOCK );
		i_ret = (p_vcddev->i_device_handle == -1) ? -1 : 0;
#endif
	}

	if( i_ret == 0 )
	{
		p_vcddev->psz_dev = (char *)strdup( psz_dev );
	}
	else
	{
		free( p_vcddev );
		p_vcddev = NULL;
	}

	return p_vcddev;
}

/*****************************************************************************
* ioctl_Close: Closes an already opened VCD device or file.
*****************************************************************************/
void ioctl_Close( vcddev_t *p_vcddev )
{
	if( p_vcddev->psz_dev ) free( p_vcddev->psz_dev );

	if( p_vcddev->i_vcdimage_handle != -1 )
	{
		/*
		*  vcd image mode
		*/

		CloseVCDImage( p_vcddev );
		return;
	}

	/*
	*  vcd device mode
	*/

#ifdef WIN32
	if( p_vcddev->h_device_handle )
		CloseHandle( p_vcddev->h_device_handle );
	if( p_vcddev->hASPI )
		unload_Aspi( p_vcddev->hASPI );
#else
	if( p_vcddev->i_device_handle != -1 )
		close( p_vcddev->i_device_handle );
#endif
}

/*****************************************************************************
* ioctl_GetTracksMap: Read the Table of Content, fill in the pp_sectors map
*                     if pp_sectors is not null and return the number of
*                     tracks available.
*****************************************************************************/
int ioctl_GetTracksMap( const vcddev_t *p_vcddev, int **pp_sectors )
{
	int i_tracks = 0;

	if( p_vcddev->i_vcdimage_handle != -1 )
	{
		/*
		*  vcd image mode
		*/

		i_tracks = p_vcddev->i_tracks;

		if( pp_sectors )
		{
			*pp_sectors = (int*)malloc( (i_tracks + 1) * sizeof(int) );
			if( *pp_sectors == NULL )
			{
				mp_msg( "out of memory" );
				return 0;
			}
			memcpy( *pp_sectors, p_vcddev->p_sectors,
				(i_tracks + 1) * sizeof(int) );
		}

		return i_tracks;
	}
	else
	{

		/*
		*  vcd device mode
		*/

#if defined( WIN32 )
		if( p_vcddev->hASPI )
		{
			HANDLE hEvent;
			SRB_ExecSCSICmd ssc;
			byte_t p_tocheader[ 4 ];

			/* Create the transfer completion event */
			hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
			if( hEvent == NULL )
			{
				return -1;
			}

			memset( &ssc, 0, sizeof( ssc ) );

			ssc.SRB_Cmd         = SC_EXEC_SCSI_CMD;
			ssc.SRB_Flags       = SRB_DIR_IN | SRB_EVENT_NOTIFY;
			ssc.SRB_HaId        = LOBYTE( p_vcddev->i_sid );
			ssc.SRB_Target      = HIBYTE( p_vcddev->i_sid );
			ssc.SRB_SenseLen    = SENSE_LEN;

			ssc.SRB_PostProc = (LPVOID) hEvent;
			ssc.SRB_CDBLen      = 10;

			/* Operation code */
			ssc.CDBByte[ 0 ] = READ_TOC;

			/* Format */
			ssc.CDBByte[ 2 ] = READ_TOC_FORMAT_TOC;

			/* Starting track */
			ssc.CDBByte[ 6 ] = 0;

			/* Allocation length and buffer */
			ssc.SRB_BufLen = sizeof( p_tocheader );
			ssc.SRB_BufPointer  = p_tocheader;
			ssc.CDBByte[ 7 ] = (unsigned char)(( ssc.SRB_BufLen >>  8 ) & 0xff);
			ssc.CDBByte[ 8 ] = (unsigned char)(( ssc.SRB_BufLen       ) & 0xff);

			/* Initiate transfer */
			ResetEvent( hEvent );
			p_vcddev->lpSendCommand( (LPSRB) &ssc );

			/* If the command has still not been processed, wait until it's
			* finished */
			if( ssc.SRB_Status == SS_PENDING )
				WaitForSingleObject( hEvent, INFINITE );

			/* check that the transfer went as planned */
			if( ssc.SRB_Status != SS_COMP )
			{
				CloseHandle( hEvent );
				return 0;
			}

			i_tracks = p_tocheader[3] - p_tocheader[2] + 1;

			if( pp_sectors )
			{
				int i, i_toclength;
				byte_t *p_fulltoc;

				i_toclength = 4 /* header */ + p_tocheader[0] +
					((unsigned int)p_tocheader[1] << 8);

				p_fulltoc = (byte_t*)malloc( i_toclength );
				*pp_sectors = (int*)malloc( (i_tracks + 1) * sizeof(**pp_sectors) );

				if( *pp_sectors == NULL || p_fulltoc == NULL )
				{
					if( *pp_sectors ) free( *pp_sectors );
					if( p_fulltoc ) free( p_fulltoc );
					mp_msg( "out of memory" );
					CloseHandle( hEvent );
					return 0;
				}

				/* Allocation length and buffer */
				ssc.SRB_BufLen = i_toclength;
				ssc.SRB_BufPointer  = p_fulltoc;
				ssc.CDBByte[ 7 ] = (unsigned char)(( ssc.SRB_BufLen >>  8 ) & 0xff);
				ssc.CDBByte[ 8 ] = (unsigned char)(( ssc.SRB_BufLen       ) & 0xff);

				/* Initiate transfer */
				ResetEvent( hEvent );
				p_vcddev->lpSendCommand( (LPSRB) &ssc );

				/* If the command has still not been processed, wait until it's
				* finished */
				if( ssc.SRB_Status == SS_PENDING )
					WaitForSingleObject( hEvent, INFINITE );

				/* check that the transfer went as planned */
				if( ssc.SRB_Status != SS_COMP )
					i_tracks = 0;

				for( i = 0 ; i <= i_tracks ; i++ )
				{
					int i_index = 8 + 8 * i;
					(*pp_sectors)[ i ] = ((int)p_fulltoc[ i_index ] << 24) +
						((int)p_fulltoc[ i_index+1 ] << 16) +
						((int)p_fulltoc[ i_index+2 ] << 8) +
						(int)p_fulltoc[ i_index+3 ];

					mp_msg( "p_sectors: %i, %i", i, (*pp_sectors)[i]);
				}

				free( p_fulltoc );
			}

			CloseHandle( hEvent );

		}
		else
		{
			DWORD dwBytesReturned;
			CDROM_TOC cdrom_toc;

			if( DeviceIoControl( p_vcddev->h_device_handle,
				IOCTL_CDROM_READ_TOC,
				NULL, 0, &cdrom_toc, sizeof(CDROM_TOC),
				&dwBytesReturned, NULL ) == 0 )
			{
				mp_msg( "could not read TOCHDR" );
				return 0;
			}

			i_tracks = cdrom_toc.LastTrack - cdrom_toc.FirstTrack + 1;

			if( pp_sectors )
			{
				int i;

				*pp_sectors = (int*)malloc( (i_tracks + 1) * sizeof(**pp_sectors) );
				if( *pp_sectors == NULL )
				{
					mp_msg( "out of memory" );
					return 0;
				}

				for( i = 0 ; i <= i_tracks ; i++ )
				{
					(*pp_sectors)[ i ] = MSF_TO_LBA2(
						cdrom_toc.TrackData[i].Address[1],
						cdrom_toc.TrackData[i].Address[2],
						cdrom_toc.TrackData[i].Address[3] );
					mp_msg( "p_sectors: %i, %i", i, (*pp_sectors)[i]);
				}
			}
		}

#elif defined( HAVE_IOC_TOC_HEADER_IN_SYS_CDIO_H ) \
	|| defined( HAVE_SCSIREQ_IN_SYS_SCSIIO_H )
		struct ioc_toc_header tochdr;
		struct ioc_read_toc_entry toc_entries;

		if( ioctl( p_vcddev->i_device_handle, CDIOREADTOCHEADER, &tochdr )
			== -1 )
		{
			mp_msg( "could not read TOCHDR" );
			return 0;
		}

		i_tracks = tochdr.ending_track - tochdr.starting_track + 1;

		if( pp_sectors )
		{
			int i;

			*pp_sectors = malloc( (i_tracks + 1) * sizeof(int) );
			if( *pp_sectors == NULL )
			{
				mp_msg( "out of memory" );
				return NULL;
			}

			toc_entries.address_format = CD_LBA_FORMAT;
			toc_entries.starting_track = 0;
			toc_entries.data_len = ( i_tracks + 1 ) *
				sizeof( struct cd_toc_entry );
			toc_entries.data = (struct cd_toc_entry *)
				malloc( toc_entries.data_len );
			if( toc_entries.data == NULL )
			{
				mp_msg( "out of memory" );
				free( *pp_sectors );
				return 0;
			}

			/* Read the TOC */
			if( ioctl( p_vcddev->i_device_handle, CDIOREADTOCENTRYS,
				&toc_entries ) == -1 )
			{
				mp_msg( "could not read the TOC" );
				free( *pp_sectors );
				free( toc_entries.data );
				return 0;
			}

			/* Fill the p_sectors structure with the track/sector matches */
			for( i = 0 ; i <= i_tracks ; i++ )
			{
#if defined( HAVE_SCSIREQ_IN_SYS_SCSIIO_H )
				/* FIXME: is this ok? */
				(*pp_sectors)[ i ] = toc_entries.data[i].addr.lba;
#else
				(*pp_sectors)[ i ] = ntohl( toc_entries.data[i].addr.lba );
#endif
			}
		}
#else
		struct cdrom_tochdr   tochdr;
		struct cdrom_tocentry tocent;

		/* First we read the TOC header */
		if( ioctl( p_vcddev->i_device_handle, CDROMREADTOCHDR, &tochdr )
			== -1 )
		{
			mp_msg( "could not read TOCHDR" );
			return 0;
		}

		i_tracks = tochdr.cdth_trk1 - tochdr.cdth_trk0 + 1;

		if( pp_sectors )
		{
			int i;

			*pp_sectors = malloc( (i_tracks + 1) * sizeof(int) );
			if( *pp_sectors == NULL )
			{
				mp_msg( "out of memory" );
				return 0;
			}

			/* Fill the p_sectors structure with the track/sector matches */
			for( i = 0 ; i <= i_tracks ; i++ )
			{
				tocent.cdte_format = CDROM_LBA;
				tocent.cdte_track =
					( i == i_tracks ) ? CDROM_LEADOUT : tochdr.cdth_trk0 + i;

				if( ioctl( p_vcddev->i_device_handle, CDROMREADTOCENTRY,
					&tocent ) == -1 )
				{
					mp_msg( "could not read TOCENTRY" );
					free( *pp_sectors );
					return 0;
				}

				(*pp_sectors)[ i ] = tocent.cdte_addr.lba;
			}
		}
#endif

		return i_tracks;
	}
}

/****************************************************************************
* ioctl_ReadSector: Read VCD or CDDA sectors
****************************************************************************/
int ioctl_ReadSectors( const vcddev_t *p_vcddev,
					  int i_sector, byte_t * p_buffer, int i_nb, int i_type )
{
	byte_t *p_block;
	int i;

	if( i_type == VCD_TYPE ) p_block = (byte_t*)malloc( VCD_SECTOR_SIZE * i_nb );
	else p_block = p_buffer;

	if( p_vcddev->i_vcdimage_handle != -1 )
	{
		/*
		*  vcd image mode
		*/
		if( lseek( p_vcddev->i_vcdimage_handle, i_sector * VCD_SECTOR_SIZE,
			SEEK_SET ) == -1 )
		{
			mp_msg( "Could not lseek to sector %d", i_sector );
			if( i_type == VCD_TYPE ) free( p_block );
			return -1;
		}

		if( read( p_vcddev->i_vcdimage_handle, p_block, VCD_SECTOR_SIZE * i_nb)
			== -1 )
		{
			mp_msg( "Could not read sector %d", i_sector );
			if( i_type == VCD_TYPE ) free( p_block );
			return -1;
		}

	}
	else
	{

		/*
		*  vcd device mode
		*/

#if defined( WIN32 )
		if( p_vcddev->hASPI )
		{
			HANDLE hEvent;
			SRB_ExecSCSICmd ssc;

			/* Create the transfer completion event */
			hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
			if( hEvent == NULL )
			{
				if( i_type == VCD_TYPE ) free( p_block );
				return -1;
			}

			memset( &ssc, 0, sizeof( ssc ) );

			ssc.SRB_Cmd         = SC_EXEC_SCSI_CMD;
			ssc.SRB_Flags       = SRB_DIR_IN | SRB_EVENT_NOTIFY;
			ssc.SRB_HaId        = LOBYTE( p_vcddev->i_sid );
			ssc.SRB_Target      = HIBYTE( p_vcddev->i_sid );
			ssc.SRB_SenseLen    = SENSE_LEN;

			ssc.SRB_PostProc = (LPVOID) hEvent;
			ssc.SRB_CDBLen      = 12;

			/* Operation code */
			ssc.CDBByte[ 0 ] = READ_CD;

			/* Sector type */
			ssc.CDBByte[ 1 ] = i_type == VCD_TYPE ? SECTOR_TYPE_MODE2_FORM2 :
			SECTOR_TYPE_CDDA;

			/* Start of LBA */
			ssc.CDBByte[ 2 ] = ( i_sector >> 24 ) & 0xff;
			ssc.CDBByte[ 3 ] = ( i_sector >> 16 ) & 0xff;
			ssc.CDBByte[ 4 ] = ( i_sector >>  8 ) & 0xff;
			ssc.CDBByte[ 5 ] = ( i_sector       ) & 0xff;

			/* Transfer length */
			ssc.CDBByte[ 6 ] = ( i_nb >> 16 ) & 0xff;
			ssc.CDBByte[ 7 ] = ( i_nb >> 8  ) & 0xff;
			ssc.CDBByte[ 8 ] = ( i_nb       ) & 0xff;

			/* Data selection */
			ssc.CDBByte[ 9 ] = i_type == VCD_TYPE ? READ_CD_RAW_MODE2 :
			READ_CD_USERDATA;

			/* Result buffer */
			ssc.SRB_BufPointer  = p_block;
			ssc.SRB_BufLen = VCD_SECTOR_SIZE * i_nb;

			/* Initiate transfer */
			ResetEvent( hEvent );
			p_vcddev->lpSendCommand( (LPSRB) &ssc );

			/* If the command has still not been processed, wait until it's
			* finished */
			if( ssc.SRB_Status == SS_PENDING )
			{
				WaitForSingleObject( hEvent, INFINITE );
			}
			CloseHandle( hEvent );

			/* check that the transfer went as planned */
			if( ssc.SRB_Status != SS_COMP )
			{
				if( i_type == VCD_TYPE ) free( p_block );
				return -1;
			}
		}
		else
		{
			DWORD dwBytesReturned;
			RAW_READ_INFO cdrom_raw;

			/* Initialize CDROM_RAW_READ structure */
			cdrom_raw.DiskOffset.QuadPart = CD_SECTOR_SIZE * i_sector;
			cdrom_raw.SectorCount = i_nb;
			cdrom_raw.TrackMode =  i_type == VCD_TYPE ? XAForm2 : CDDA;

			if( DeviceIoControl( p_vcddev->h_device_handle,
				IOCTL_CDROM_RAW_READ, &cdrom_raw,
				sizeof(RAW_READ_INFO), p_block,
				VCD_SECTOR_SIZE * i_nb, &dwBytesReturned,
				NULL ) == 0 )
			{
				if( i_type == VCD_TYPE )
				{
					/* Retry in YellowMode2 */
					cdrom_raw.TrackMode = YellowMode2;
					if( DeviceIoControl( p_vcddev->h_device_handle,
						IOCTL_CDROM_RAW_READ, &cdrom_raw,
						sizeof(RAW_READ_INFO), p_block,
						VCD_SECTOR_SIZE * i_nb, &dwBytesReturned,
						NULL ) == 0 )
					{
						free( p_block );
						return -1;
					}
				}
				else return -1;
			}
		}

#elif defined( HAVE_SCSIREQ_IN_SYS_SCSIIO_H )
		struct scsireq  sc;
		int i_ret;

		memset( &sc, 0, sizeof(sc) );
		sc.cmd[0] = 0xBE;
		sc.cmd[1] = i_type == VCD_TYPE ? SECTOR_TYPE_MODE2_FORM2:
		SECTOR_TYPE_CDDA;
		sc.cmd[2] = (i_sector >> 24) & 0xff;
		sc.cmd[3] = (i_sector >> 16) & 0xff;
		sc.cmd[4] = (i_sector >>  8) & 0xff;
		sc.cmd[5] = (i_sector >>  0) & 0xff;
		sc.cmd[6] = (i_nb >> 16) & 0xff;
		sc.cmd[7] = (i_nb >>  8) & 0xff;
		sc.cmd[8] = (i_nb      ) & 0xff;
		sc.cmd[9] = i_type == VCD_TYPE ? READ_CD_RAW_MODE2 : READ_CD_USERDATA;
		sc.cmd[10] = 0; /* sub channel */
		sc.cmdlen = 12;
		sc.databuf = (caddr_t)p_block;
		sc.datalen = VCD_SECTOR_SIZE * i_nb;
		sc.senselen = sizeof( sc.sense );
		sc.flags = SCCMD_READ;
		sc.timeout = 10000;

		i_ret = ioctl( i_fd, SCIOCCOMMAND, &sc );
		if( i_ret == -1 )
		{
			mp_msg( "SCIOCCOMMAND failed" );
			if( i_type == VCD_TYPE ) free( p_block );
			return -1;
		}
		if( sc.retsts || sc.error )
		{
			mp_msg( "SCSI command failed: status %d error %d\n",
				sc.retsts, sc.error );
			if( i_type == VCD_TYPE ) free( p_block );
			return -1;
		}

#elif defined( HAVE_IOC_TOC_HEADER_IN_SYS_CDIO_H )
		int i_size = VCD_SECTOR_SIZE;

		if( ioctl( p_vcddev->i_device_handle, CDRIOCSETBLOCKSIZE, &i_size )
			== -1 )
		{
			mp_msg( "Could not set block size" );
			if( i_type == VCD_TYPE ) free( p_block );
			return( -1 );
		}

		if( lseek( p_vcddev->i_device_handle,
			i_sector * VCD_SECTOR_SIZE, SEEK_SET ) == -1 )
		{
			mp_msg( "Could not lseek to sector %d", i_sector );
			if( i_type == VCD_TYPE ) free( p_block );
			return( -1 );
		}

		if( read( p_vcddev->i_device_handle,
			p_block, VCD_SECTOR_SIZE * i_nb ) == -1 )
		{
			mp_msg( "Could not read sector %d", i_sector );
			if( i_type == VCD_TYPE ) free( p_block );
			return( -1 );
		}

#else
		for( i = 0; i < i_nb; i++ )
		{
			int i_dummy = i_sector + i + 2 * CD_FRAMES;

#define p_msf ((struct cdrom_msf0 *)(p_block + i * VCD_SECTOR_SIZE))
			p_msf->minute =   i_dummy / (CD_FRAMES * CD_SECS);
			p_msf->second = ( i_dummy % (CD_FRAMES * CD_SECS) ) / CD_FRAMES;
			p_msf->frame =  ( i_dummy % (CD_FRAMES * CD_SECS) ) % CD_FRAMES;
#undef p_msf

			if( ioctl( p_vcddev->i_device_handle, CDROMREADRAW,
				p_block + i * VCD_SECTOR_SIZE ) == -1 )
			{
				mp_msg( "could not read block %i from disc",
					i_sector );

				if( i == 0 )
				{
					if( i_type == VCD_TYPE ) free( p_block );
					return( -1 );
				}
				else break;
			}
		}
#endif
	}

	/* For VCDs, we don't want to keep the header and footer of the
	* sectors read */
	if( i_type == VCD_TYPE )
	{
		for( i = 0; i < i_nb; i++ )
		{
			memcpy( p_buffer + i * VCD_DATA_SIZE,
				p_block + i * VCD_SECTOR_SIZE + VCD_DATA_START,
				VCD_DATA_SIZE );
		}
		free( p_block );
	}

	return( 0 );
}

/****************************************************************************
* Private functions
****************************************************************************/

/****************************************************************************
* OpenVCDImage: try to open a vcd image from a .cue file
****************************************************************************/
static int OpenVCDImage( const char *psz_dev,
						vcddev_t *p_vcddev )
{
#if FIXME
	int i_ret = -1;
	char *p_pos;
	char *psz_vcdfile = NULL;
	char *psz_cuefile = NULL;
	FILE *cuefile     = NULL;
	char line[1024];

	/* Check if we are dealing with a .cue file */
	p_pos = strrchr( psz_dev, '.' );
	if( p_pos && !strcmp( p_pos, ".cue" ) )
	{
		/* psz_dev must be the cue file. Let's assume there's a .bin
		* file with the same filename */
		if( p_pos )
		{
			psz_vcdfile = malloc( p_pos - psz_dev + 5 /* ".bin" */ );
			strncpy( psz_vcdfile, psz_dev, p_pos - psz_dev );
			strcpy( psz_vcdfile + (p_pos - psz_dev), ".bin");
		}
		else
		{
			psz_vcdfile = malloc( strlen(psz_dev) + 5 /* ".bin" */ );
			sprintf( psz_vcdfile, "%s.bin", psz_dev );
		}
		psz_cuefile = strdup( psz_dev );
	}
	else
	{
		/* psz_dev must be the actual vcd file. Let's assume there's a .cue
		* file with the same filename */
		if( p_pos )
		{
			psz_cuefile = malloc( p_pos - psz_dev + 5 /* ".cue" */ );
			strncpy( psz_cuefile, psz_dev, p_pos - psz_dev );
			strcpy( psz_cuefile + (p_pos - psz_dev), ".cue");
		}
		else
		{
			psz_cuefile = malloc( strlen(psz_dev) + 5 /* ".cue" */ );
			sprintf( psz_cuefile, "%s.cue", psz_dev );
		}
		/* If we need to look up the .cue file, then we don't have to look for the vcd */
		psz_vcdfile = strdup( psz_dev );
	}

	/* Open the cue file and try to parse it */
	mp_msg( "trying .cue file: %s", psz_cuefile );
	cuefile = utf8_fopen( psz_cuefile, "rt" );
	if( cuefile == NULL )
	{
		i_ret = -1;
		mp_msg( "could not find .cue file" );
		goto error;
	}

	mp_msg( "using vcd image file: %s", psz_vcdfile );
	p_vcddev->i_vcdimage_handle = open( psz_vcdfile,
		O_RDONLY | O_NONBLOCK | O_BINARY, 0666 );

	if( p_vcddev->i_vcdimage_handle == -1 && 
		fscanf( cuefile, "FILE %c", line ) &&
		fgets( line, 1024, cuefile ) )
	{
		/* We have a cue file, but no valid vcd file yet */
		free( psz_vcdfile );
		p_pos = strchr( line, '"' );
		if( p_pos )
		{
			*p_pos = 0;

			/* Take care of path standardization */
			if( *line != '/' && ((p_pos = strrchr( psz_cuefile, '/' ))
				|| (p_pos = strrchr( psz_cuefile, '\\' ) )) )
			{
				psz_vcdfile = malloc( strlen(line) +
					(p_pos - psz_cuefile + 1) + 1 );
				strncpy( psz_vcdfile, psz_cuefile, (p_pos - psz_cuefile + 1) );
				strcpy( psz_vcdfile + (p_pos - psz_cuefile + 1), line );
			}
			else psz_vcdfile = strdup( line );
		}
		mp_msg( "using vcd image file: %s", psz_vcdfile );
		p_vcddev->i_vcdimage_handle = utf8_open( psz_vcdfile,
			O_RDONLY | O_NONBLOCK | O_BINARY, 0666 );
	}

	if( p_vcddev->i_vcdimage_handle == -1)
	{
		i_ret = -1;
		goto error;
	}
	else i_ret = 0;

	/* Try to parse the i_tracks and p_sectors info so we can just forget
	* about the cuefile */
	if( i_ret == 0 )
	{
		int p_sectors[100];
		int i_tracks = 0;
		int i_num;
		char psz_dummy[10];

		while( fgets( line, 1024, cuefile ) )
		{
			/* look for a TRACK line */
			if( !sscanf( line, "%9s", psz_dummy ) ||
				strcmp(psz_dummy, "TRACK") )
				continue;

			/* look for an INDEX line */
			while( fgets( line, 1024, cuefile ) )
			{
				int i_min, i_sec, i_frame;

				if( (sscanf( line, "%9s %2u %2u:%2u:%2u", psz_dummy, &i_num,
					&i_min, &i_sec, &i_frame ) != 5) || (i_num != 1) )
					continue;

				i_tracks++;
				p_sectors[i_tracks - 1] = MSF_TO_LBA(i_min, i_sec, i_frame);
				mp_msg( "vcd track %i begins at sector:%i",
					i_tracks - 1, p_sectors[i_tracks - 1] );
				break;
			}
		}

		/* fill in the last entry */
		p_sectors[i_tracks] = lseek(p_vcddev->i_vcdimage_handle, 0, SEEK_END)
			/ VCD_SECTOR_SIZE;
		mp_msg( "vcd track %i, begins at sector:%i",
			i_tracks, p_sectors[i_tracks] );
		p_vcddev->i_tracks = i_tracks;
		p_vcddev->p_sectors = malloc( (i_tracks + 1) * sizeof(int) );
		memcpy( p_vcddev->p_sectors, p_sectors, (i_tracks + 1) * sizeof(int) );

	}

error:
	if( cuefile ) fclose( cuefile );
	if( psz_cuefile ) free( psz_cuefile );
	if( psz_vcdfile ) free( psz_vcdfile );

	return i_ret;
#else
	return -1;
#endif
}

/****************************************************************************
* CloseVCDImage: closes a vcd image opened by OpenVCDImage
****************************************************************************/
static void CloseVCDImage( vcddev_t *p_vcddev )
{
	if( p_vcddev->i_vcdimage_handle != -1 )
		close( p_vcddev->i_vcdimage_handle );
	else
		return;

	if( p_vcddev->p_sectors )
		free( p_vcddev->p_sectors );
}

#if defined( WIN32 )
/*****************************************************************************
* win32_vcd_open: open vcd drive
*****************************************************************************
* Load and use aspi if it is available, otherwise use IOCTLs on WinNT/2K/XP.
*****************************************************************************/
static int win32_vcd_open( const char *psz_dev, vcddev_t *p_vcddev )
{
	/* Initializations */
	p_vcddev->h_device_handle = NULL;
	p_vcddev->i_sid = 0;
	p_vcddev->hASPI = NULL;
	p_vcddev->lpSendCommand = 0;

	if( WIN_NT ) /*WIN_NT*/
	{
		char psz_win32_drive[7];

		mp_msg( "using winNT/2K/XP ioctl layer" );

		sprintf( psz_win32_drive, "\\\\.\\%c:", psz_dev[0] );

		p_vcddev->h_device_handle = CreateFile( psz_win32_drive, GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING,
			FILE_FLAG_NO_BUFFERING |
			FILE_FLAG_RANDOM_ACCESS, NULL );
		return (p_vcddev->h_device_handle == NULL) ? -1 : 0;
	}
	else
	{
		HMODULE hASPI = NULL;
		int i, j, i_hostadapters;
		char c_drive = psz_dev[0];

		hASPI = load_Aspi();

		if( hASPI == NULL )
		{
			mp_msg( "unable to load aspi or get aspi function pointers" );
			return -1;
		}

		/* ASPI support seems to be there */

		i_hostadapters = GetNumAdapters();
		if( i_hostadapters <= 0 )
		{
			unload_Aspi( hASPI );
			return -1;
		}

		c_drive = c_drive > 'Z' ? c_drive - 'a' : c_drive - 'A';

		for( i = 0; i < i_hostadapters; i++ )
		{
			for( j = 0; j < 15; j++ )
			{
				SRB_GetDiskInfo srbDiskInfo;

				srbDiskInfo.SRB_Cmd         = SC_GET_DISK_INFO;
				srbDiskInfo.SRB_HaId        = i;
				srbDiskInfo.SRB_Flags       = 0;
				srbDiskInfo.SRB_Hdr_Rsvd    = 0;
				srbDiskInfo.SRB_Target      = j;
				srbDiskInfo.SRB_Lun         = 0;

				pfnSendASPI32Command( (LPSRB) &srbDiskInfo );

				if( (srbDiskInfo.SRB_Status == SS_COMP) &&
					(srbDiskInfo.SRB_Int13HDriveInfo == c_drive) )
				{
					/* Make sure this is a cdrom device */
					SRB_GDEVBlock   srbGDEVBlock;

					memset( &srbGDEVBlock, 0, sizeof(SRB_GDEVBlock) );
					srbGDEVBlock.SRB_Cmd    = SC_GET_DEV_TYPE;
					srbGDEVBlock.SRB_HaId   = i;
					srbGDEVBlock.SRB_Target = j;

					pfnSendASPI32Command( (LPSRB) &srbGDEVBlock );
					printf("%d %d\n", srbGDEVBlock.SRB_Status, srbGDEVBlock.SRB_DeviceType);
					if( ( srbGDEVBlock.SRB_Status == SS_COMP ) &&
						( srbGDEVBlock.SRB_DeviceType == DTYPE_CDROM ) )
					{
						p_vcddev->i_sid = MAKEWORD( i, j );
						p_vcddev->hASPI = hASPI;
						p_vcddev->lpSendCommand = pfnSendASPI32Command;
						mp_msg( "using aspi layer" );

						return 0;
					}
					else
					{
						unload_Aspi( hASPI );
						mp_msg( "%c: is not a cdrom drive",
							psz_dev[0] );
						return -1;
					}
				}
			}
		}

		unload_Aspi( hASPI );
		mp_msg( "unable to get haid and target (aspi)" );

	}

	return -1;
}

#endif /* WIN32 */


/***************************************************************************
* HMODULE load_Aspi( void )
***************************************************************************/
static HMODULE load_Aspi( void )
{
	if (hinstWNASPI32 != NULL) {
		return hinstWNASPI32;
	}
    
	hinstWNASPI32 = LoadLibrary( "WNASPI32.DLL" );

#ifdef _DEBUG
	printf( " loadAspi()\n" );
#endif

	if (hinstWNASPI32)
	{
		pfnGetASPI32SupportInfo =
			(DWORD(*)(void))GetProcAddress( hinstWNASPI32, "GetASPI32SupportInfo" );
		pfnSendASPI32Command =
			(DWORD(*)(LPSRB))GetProcAddress( hinstWNASPI32, "SendASPI32Command" );
		pfnGetASPI32Buffer =
			(BOOL(*)(PASPI32BUFF))GetProcAddress( hinstWNASPI32, "GetASPI32Buffer" );
		pfnFreeASPI32Buffer =
			(BOOL(*)(PASPI32BUFF))GetProcAddress( hinstWNASPI32, "FreeASPI32Buffer" );
		pfnTranslateASPI32Address =
			(BOOL(*)(PDWORD,PDWORD))GetProcAddress( hinstWNASPI32, "TranslateASPI32Address" );
		pfnGetASPI32DLLVersion =
			(DWORD(*)(void))GetProcAddress( hinstWNASPI32, "GetASPI32DLLVersion" );

		if ( !pfnGetASPI32SupportInfo || !pfnSendASPI32Command )
		{
			unload_Aspi(hinstWNASPI32);
			return NULL;
		}
	}
	else
	{

#ifdef _DEBUG
		DWORD dwErr;
		dwErr = GetLastError();
		printf( "Unable to load WNASPI32.DLL\n" );
		printf( " GetLastError() -> %d (%04X)\n", dwErr, dwErr );
#endif
		return NULL;
	}

#ifdef _DEBUG
	printf( " loadAspi() successful\n" );
#endif
	return hinstWNASPI32; /*OK*/
}

/***************************************************************************
* void unload_Aspi(HMODULE)
***************************************************************************/
static void unload_Aspi(HMODULE hASPI)
{
	if (hASPI != NULL) {
		FreeLibrary(hASPI);
		if (hASPI == hinstWNASPI32) {
			hinstWNASPI32 = NULL;
		}
	}
}

/***************************************************************************
* GetNumAdapters
***************************************************************************/
int GetNumAdapters( void )
{
	DWORD d;
	BYTE bHACount;
	BYTE bASPIStatus;

	d = pfnGetASPI32SupportInfo();
	bASPIStatus = HIBYTE(LOWORD(d));
	bHACount    = LOBYTE(LOWORD(d));

	if ( bASPIStatus != SS_COMP && bASPIStatus != SS_NO_ADAPTERS )
	{
		return -1;
	}
	return (int)bHACount;
}

/***************************************************************************
* GetDriveInfo
***************************************************************************/
DWORD GetDriveInfo( BYTE ha, BYTE tgt, BYTE lun, LPCDREC cdrec )
{
	DWORD dwStatus;
	HANDLE heventSRB;
	SRB_ExecSCSICmd s;
	BYTE buf[100];
	char outBuf[101];
	CDREC cdrecTmp;
	SRB_GetDiskInfo srbDiskInfo;

	heventSRB = CreateEvent( NULL, TRUE, FALSE, NULL );

	if ( cdrec )
		cdrecTmp = *cdrec;

	memset( &cdrecTmp.info, 0, sizeof(CDINFO) );
	memset( &s, 0, sizeof( s ) );
	memset( buf, 0, 100 );
	s.SRB_Cmd        = SC_EXEC_SCSI_CMD;
	s.SRB_HaId       = ha;
	s.SRB_Target     = tgt;
	s.SRB_Lun        = lun;
	s.SRB_Flags      = SRB_DIR_IN | SRB_EVENT_NOTIFY;
	s.SRB_BufLen     = 100;
	s.SRB_BufPointer = buf;
	s.SRB_SenseLen   = SENSE_LEN;
	s.SRB_CDBLen     = 6;
	s.SRB_PostProc   = (LPVOID)heventSRB;
	s.CDBByte[0]     = SCSI_INQUIRY;
	s.CDBByte[4]     = 100;

	ResetEvent( heventSRB );
	dwStatus = pfnSendASPI32Command( (LPSRB)&s );
	if ( dwStatus == SS_PENDING )
	{
		WaitForSingleObject( heventSRB, INFINITE );
	}
	CloseHandle( heventSRB );

	if ( s.SRB_Status != SS_COMP )
	{
		return SS_ERR;
	}

#ifdef _DEBUG
	{
		FILE *fp = fopen( "inquiry.dat", "wb" );
		fwrite( buf, 1, 100, fp );
		fclose( fp );
	}
#endif

	memcpy( cdrecTmp.info.vendor, &buf[8], 8 );
	memcpy( cdrecTmp.info.prodId, &buf[16], 16 );
	memcpy( cdrecTmp.info.rev, &buf[32], 4 );
	memcpy( cdrecTmp.info.vendSpec, &buf[36], 20 );
	wsprintf( outBuf, "%s, %s v%s (%d:%d:%d)\n", 
		cdrecTmp.info.vendor, cdrecTmp.info.prodId, cdrecTmp.info.rev,
		ha, tgt, lun );
#ifdef _DEBUG
	printf( " GetDriveInfo: %s\n", outBuf );
#endif

	strncpy( cdrecTmp.id, outBuf, MAXIDLEN );
	cdrecTmp.id[MAXIDLEN] = 0;

	memset(&srbDiskInfo, 0, sizeof(SRB_GetDiskInfo));
	
	srbDiskInfo.SRB_Cmd 		= SC_GET_DISK_INFO;
	srbDiskInfo.SRB_HaId		= ha;
	srbDiskInfo.SRB_Target		= tgt;
	srbDiskInfo.SRB_Lun 		= lun;
	pfnSendASPI32Command( (LPSRB) &srbDiskInfo );

	if (srbDiskInfo.SRB_Status != SS_COMP) {
#ifdef _DEBUG
		printf(" GetDriveInfo: SC_GET_DISK_INFO failed\n");
#endif
		return SS_ERR;
	}
	cdrecTmp.c_drive =srbDiskInfo.SRB_Int13HDriveInfo + 'A';

	if ( cdrec )
		*cdrec = cdrecTmp;

	return SS_COMP;
}

/******************************************************************
* GetCDList
*
* Scans all host adapters for CD-ROM units, and stores information
* for all units located
******************************************************************/
int GetCDList( LPCDLIST cd )
{
	SRB_HAInquiry sh;
	SRB_GDEVBlock sd;
	int numAdapters, i, j, k;
	int maxTgt;
	HMODULE hASPI;

	/* load Aspi*/
	hASPI = load_Aspi();
	
	if (hASPI == NULL) {
#ifdef _DEBUG
		printf( " GetCDList(): Can not load ASPI\n" );
#endif
		return -1;
	}

	/* initialize cd list */
	memset( cd, 0, sizeof(*cd) );

	numAdapters = GetNumAdapters();
#ifdef _DEBUG
	printf( " GetCDList(): numAdapters == %d\n", numAdapters );
#endif
	if ( numAdapters == 0 )
	{
		unload_Aspi(hASPI);
		return -1;
	}

	for( i = 0; i < numAdapters; i++ )
	{
		memset( &sh, 0, sizeof( sh ) );
		sh.SRB_Cmd   = SC_HA_INQUIRY;
		sh.SRB_HaId  = i;
		pfnSendASPI32Command( (LPSRB)&sh );

		/* on error skip to next adapter */
		if ( sh.SRB_Status != SS_COMP )
			continue;

		maxTgt = (int)sh.HA_Unique[3];

		if ( maxTgt == 0 )
			maxTgt = 8;

		for( j = 0; j < maxTgt; j++ )
		{
			for( k = 0; k < 8; k++ )
			{
				memset( &sd, 0, sizeof( sd ) );
				sd.SRB_Cmd   = SC_GET_DEV_TYPE;
				sd.SRB_HaId  = i;
				sd.SRB_Target = j;
				sd.SRB_Lun   = k;
				pfnSendASPI32Command( (LPSRB)&sd );
				if ( sd.SRB_Status == SS_COMP )
				{
					if ( sd.SRB_DeviceType == DTYPE_CDROM )
					{
						cd->cd[cd->num].ha = i;
						cd->cd[cd->num].tgt = j;
						cd->cd[cd->num].lun = k;
						memset( cd->cd[cd->num].id, 0, MAXIDLEN+1 );
						GetDriveInfo( (BYTE)i, (BYTE)j, (BYTE)k, &(cd->cd[cd->num]) );
						cd->num++;
					}
#ifdef _DEBUG
					else
					{
						printf( "       : sd.SRB_DeviceType == %d\n", sd.SRB_DeviceType );
						GetDriveInfo( (BYTE)i, (BYTE)j, (BYTE)k, NULL );
					}
#endif
				}
			}
		}
	}

	unload_Aspi(hASPI);

	if ( cd->num == 0 ) {
        return -1;
    }

	return 0;
}

#if 0
const int iAtoZnum = 26;

char *get_cdrom_xml()
{  
	char *psz_xml;
	char *p_xml;

	psz_xml = p_xml = (char*)malloc(sizeof(char)*1024);

	if (psz_xml == NULL) {
		return NULL;
	}

	sprintf(p_xml, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><cdroms>");
	p_xml += strlen(p_xml);

#if 0
	{
		DWORD   dwDrivers;  
		char psz_driver[3];

		dwDrivers=GetLogicalDrives();  
		for (int i = 0; i < iAtoZnum; i++) {  
			if ((dwDrivers & (1<<i))!=0) {
				sprintf(psz_driver,"%c:", 'A'+i);
				if (GetDriveType(psz_driver) == DRIVE_CDROM) {
					sprintf(p_xml, "<cdrom path=\"%s\\\"/>", psz_driver);
					p_xml += strlen(p_xml);
				}
			}  
		}
	}
#else
	{
		char drive_letter;
		for(drive_letter = 'A'; drive_letter <= 'Z'; ++drive_letter ) {
			char psz_driver[3] = {drive_letter, ':', 0};
			if(GetDriveTypeA(psz_driver) == DRIVE_CDROM) {
				sprintf(p_xml, "<cdrom path=\"%s\\\"/>", psz_driver);
				p_xml += strlen(p_xml);
			}
		}
	}
#endif
	sprintf(p_xml, "</cdroms>");
	p_xml += strlen(p_xml);

	psz_xml = (char*)realloc(psz_xml, strlen(psz_xml) + 1);
	return psz_xml;
}
#else
char *get_cdrom_xml()
{  
	char *psz_xml;
	char *p_xml;
	int i;
	CDLIST cdlist;
	
	if (GetCDList( &cdlist ) != 0) { /*ret 0 is OK*/
		fprintf(stderr, "Can not open any CDROM\n");
		return NULL;
	}

	psz_xml = p_xml = (char*)malloc(sizeof(char)*1024);

	if (psz_xml == NULL) {
		return NULL;
	}

	sprintf(p_xml, "<cdroms>");
	p_xml += strlen(p_xml);

	for (i = 0; i < cdlist.num; i++) {
		sprintf(p_xml, 
			"<cdrom path=\"%c:\\\" info=\"%s, %s v%s (%d:%d:%d)\"/>",
			cdlist.cd[i].c_drive,
			cdlist.cd[i].info.vendor,
			cdlist.cd[i].info.prodId,
			cdlist.cd[i].info.rev,
			cdlist.cd[i].ha,
			cdlist.cd[i].tgt,
			cdlist.cd[i].lun);
		p_xml += strlen(p_xml);
	}

	sprintf(p_xml, "</cdroms>");
	p_xml += strlen(p_xml);

	psz_xml = (char*)realloc(psz_xml, strlen(psz_xml) + 1);
	return psz_xml;
}
#endif

int get_first_cdrom(char *p_buf)
{
	char drive_letter;

	p_buf[1] = ':';
	p_buf[2] = '\0';

	for(drive_letter = 'A'; drive_letter <= 'Z'; ++drive_letter ) {
		p_buf[0] = drive_letter;
		if(GetDriveTypeA(p_buf) == DRIVE_CDROM) {
			return 0;
		}
	}

	return -1;
}
