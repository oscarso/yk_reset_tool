#include <stdio.h>
#include <winscard.h>
#include <ykpiv.h>
#include <internal.h>
#include "clogger\clogger.h"


#define	YKPIV_OBJ_MSMD			0x5fd000
#define YKPIV_OBJ_MSMDMSROOTS	(YKPIV_OBJ_MSMD + 1)
#define	YKPIV_OBJ_MSMDCARDID	(YKPIV_OBJ_MSMD + 2) // Fixed Size: 16 bytes
#define	YKPIV_OBJ_MSMDCARDCF	(YKPIV_OBJ_MSMD + 3) // Variable Size:  6 bytes - 8KB or more
#define	YKPIV_OBJ_MSMDCARDAPPS	(YKPIV_OBJ_MSMD + 4) // Fixed Size:  8 bytes
#define	YKPIV_OBJ_MSMDCMAPFILE	(YKPIV_OBJ_MSMD + 5) // Variable Size:  6 bytes - 8KB or more


void clear_all_pages(ykpiv_state*	state) {
	ykpiv_rc		ykrc = YKPIV_OK;
	int				objID;
	unsigned char	buf[1024 * 2];
	
	memset(buf, 0, sizeof(buf));

	printf("Clearing cardcf ...");
	objID = YKPIV_OBJ_MSMDCARDCF;
	ykrc = ykpiv_save_object(state, objID, buf, sizeof(buf));
	if (ykrc == YKPIV_OK) { printf("Done.\n"); }
	else { printf("Failed.\n"); }

	printf("Clearing cardid ...");
	objID = YKPIV_OBJ_MSMDCARDID;
	ykrc = ykpiv_save_object(state, objID, buf, sizeof(buf));
	if (ykrc == YKPIV_OK) { printf("Done.\n"); }
	else { printf("Failed.\n"); }

	printf("Clearing cmapfile ...");
	objID = YKPIV_OBJ_MSMDCMAPFILE;
	ykrc = ykpiv_save_object(state, objID, buf, sizeof(buf));
	if (ykrc == YKPIV_OK) { printf("Done.\n"); }
	else { printf("Failed.\n"); }

	printf("Clearing msroots ...");
	objID = YKPIV_OBJ_MSMDMSROOTS;
	ykrc = ykpiv_save_object(state, objID, buf, sizeof(buf));
	if (ykrc == YKPIV_OK) { printf("Done.\n"); }
	else { printf("Failed.\n"); }

	return;
}


int main(void)
{
	ykpiv_rc		ykrc = YKPIV_OK;
	ykpiv_state*	ykState;
	int				verbosity = 0;
	int				retries = 0;
	char			key[24] = { 0 };
	char			pin[9] = { 0 };


	printf("ykpiv_init\n");
	if (ykpiv_init(&ykState, verbosity) != YKPIV_OK) {
		fprintf(stderr, "Failed initializing library.\n");
		return 1;
	}

	printf("ykpiv_connect\n");
	ykrc = ykpiv_connect(ykState, NULL);//"Yubico Yubikey 4 CCID 0"
	if (ykrc != YKPIV_OK) {
		fprintf(stderr, "Failed ykpiv_connect.  ykrc=%d\n", ykrc);
		return 1;
	}

	memcpy(pin, (const char *)"aaaaaaaa", 8);
	memcpy(key, pin, 8);
	memcpy(&key[8], pin, 8);
	memcpy(&key[16], pin, 8);

	printf("Verifying PIN ...");
	ykrc = ykpiv_verify(ykState, (const char *)pin, &retries);
	if (ykrc == YKPIV_OK) { printf("Done.\n"); }
	else { printf("Failed.\n"); }

	printf("Verifying Management Key ...");
	ykrc = ykpiv_authenticate(ykState, (const unsigned char *)key);
	if (ykrc == YKPIV_OK) { printf("Done.\n"); }
	else { printf("Failed.\n"); }

	//clear all pages
	clear_all_pages(ykState);

	printf("All Done\n");
	ykpiv_done(ykState);

	return 0;
}