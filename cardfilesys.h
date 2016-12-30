#define	SZ_MAX_PAGE				2048 //max size in bytes per flash page
#define	SZ_MAX_LEN				sizeof(DWORD) //max size in bytes to store the length of write data
#define	MAX_KEY_PER_CONTAINER	2

// Move into ykpiv.h later
#define	szCARD_APPS						"cardapps"
#define	YKPIV_OBJ_MS_CARDCF				0x5fff10
#define	YKPIV_OBJ_MS_CMAPFILE			(YKPIV_OBJ_MS_CARDCF + 1)
#define	YKPIV_OBJ_MS_MSROOTS			(YKPIV_OBJ_MS_CARDCF + 2)

