#include <stdio.h>
#include <winscard.h>
#include <ykpiv.h>
#include <internal.h>
#include "clogger\clogger.h"
#include "cardfilesys.h"


int	getDataOffset(const BYTE bContainerIndex) {
	/*
	One key container can hold only 1 key pairs (AT_SIGNATURE or AT_KEYEXCHANGE)
	*/
	return YKPIV_OBJ_RSAPUBKEYBLOB_OFFSET + bContainerIndex;
}


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

	for (int cntrIndex = 0; cntrIndex < 5; cntrIndex++) {
		objID = getDataOffset(cntrIndex);
		printf("Clearing at ContainerIndex %d in %x ...", cntrIndex, objID);
		ykrc = ykpiv_save_object(state, objID, buf, sizeof(buf));
		if (ykrc == YKPIV_OK) { printf("Done.\n"); }
		else { printf("Failed.\n"); }
	}

	printf("\n\n\nPress Any Key to Continue\n");
	getchar();

	return;
}


int main(void)
{
	ykpiv_rc		ykrc = YKPIV_OK;
	ykpiv_state*	ykState;
	int				verbosity = 0;
	int				retries = 0;
	char			key[] = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08 };
	char			pin[9] = { "aaaaaaaa" };


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