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
	return (YKPIV_OBJ_MS_MSROOTS + 1) + bContainerIndex;
}


void clear_all_pages(ykpiv_state*	state) {
	ykpiv_rc	ykrc = YKPIV_OK;
	int			objID;
	char		buf[1];

	memset(buf, 0, sizeof(buf));

	objID = YKPIV_OBJ_MS_CARDCF;
	printf("Clearing cardcf (0x%x) ...", objID);
	ykrc = ykpiv_save_object(state, objID, buf, sizeof(buf));
	if (ykrc == YKPIV_OK) { printf("Done.\n"); }
	else { printf("Failed.\n"); }

	objID = YKPIV_OBJ_MS_CMAPFILE;
	printf("Clearing cmapfile (0x%x) ...", objID);
	ykrc = ykpiv_save_object(state, objID, buf, sizeof(buf));
	if (ykrc == YKPIV_OK) { printf("Done.\n"); }
	else { printf("Failed.\n"); }

	objID = YKPIV_OBJ_MS_MSROOTS;
	printf("Clearing msroots (0x%x) ...", objID);
	ykrc = ykpiv_save_object(state, objID, buf, sizeof(buf));
	if (ykrc == YKPIV_OK) { printf("Done.\n"); }
	else { printf("Failed.\n"); }

	objID = YKPIV_OBJ_AUTHENTICATION;
	printf("Clearing YKPIV_OBJ_AUTHENTICATION (0x%x) ...", objID);
	ykrc = ykpiv_save_object(state, objID, buf, sizeof(buf));
	if (ykrc == YKPIV_OK) { printf("Done.\n"); }
	else { printf("Failed.\n"); }

	objID = YKPIV_OBJ_SIGNATURE;
	printf("Clearing YKPIV_OBJ_SIGNATURE (0x%x) ...", objID);
	ykrc = ykpiv_save_object(state, objID, buf, sizeof(buf));
	if (ykrc == YKPIV_OK) { printf("Done.\n"); }
	else { printf("Failed.\n"); }

#if 0
	for (int cntrIndex = 0; cntrIndex < 5; cntrIndex++) {
		objID = getDataOffset(cntrIndex);
		printf("Clearing at ContainerIndex %d in 0x%x ...", cntrIndex, objID);
		ykrc = ykpiv_save_object(state, objID, buf, sizeof(buf));
		if (ykrc == YKPIV_OK) { printf("Done.\n"); }
		else { printf("Failed.\n"); }
	}
#endif

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
	char			key[] = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
		                      0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
		                      0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08 };
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

	//sign at 9c
	/*
	unsigned char	pt[256];
	size_t			ptLen = sizeof(pt);
	unsigned char	sig[256];
	size_t			sigLen = sizeof(sig);
	ykrc = ykpiv_sign_data(
			ykState,
			pt, ptLen,
			&sig[0], &sigLen,
			YKPIV_ALGO_RSA2048,
			0x9c
			);
	*/

	printf("All Done\n");
	ykpiv_done(ykState);

	return 0;
}