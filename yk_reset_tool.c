#include <stdio.h>
#include <winscard.h>
#include <ykpiv.h>
#include <internal.h>
#include "clogger\clogger.h"


static ykpiv_rc _send_data(ykpiv_state *state, APDU *apdu,
	unsigned char *data, unsigned long *recv_len, int *sw) {
	long rc;
	unsigned int send_len = (unsigned int)apdu->st.lc + 5;

	LogInfo("_send_data");
	if (1) {
		LogInfo("Data Sent:");
		LogBuffer(apdu->raw, send_len);
	}
	rc = SCardTransmit(state->card, SCARD_PCI_T1, apdu->raw, send_len, NULL, data, recv_len);
	if (rc != SCARD_S_SUCCESS) {
		if (1) {
			LogInfo("error: SCardTransmit failed, rc=%08lx\n", rc);
		}
		return YKPIV_PCSC_ERROR;
	}

	if (1) {
		LogInfo("Data Received:");
		LogBuffer(data, *recv_len);
	}
	if (*recv_len >= 2) {
		*sw = (data[*recv_len - 2] << 8) | data[*recv_len - 1];
	}
	else {
		*sw = 0;
	}
	return YKPIV_OK;
}


int main(void)
{
	ykpiv_rc	rc = 0;
	ykpiv_state	*state_a;
	int			verbosity = 0;
	int			retries = 0;
	char		buf[1024 * 2];

	printf("ykpiv_init - state_a\n");
	if (ykpiv_init(&state_a, verbosity) != YKPIV_OK) {
		fprintf(stderr, "Failed initializing library.\n");
		return 1;
	}

	printf("ykpiv_connect - state_a\n");
	rc = ykpiv_connect(state_a, NULL);//"Yubico Yubikey 4 CCID 0"
	if (rc != YKPIV_OK) {
		fprintf(stderr, "Failed ykpiv_connect.  rc=%d\n", rc);
		return 1;
	}

#if 1
	unsigned char key[24] = "aaaaaaaaaaaaaaaaaaaaaaaa";
	printf("ykpiv_authenticate - state_a\n");
	retries = 0;
	rc = ykpiv_authenticate(state_a, key);
	if (rc != YKPIV_OK) {
		fprintf(stderr, "Failed ykpiv_authenticate. rc=%d\n", rc);
		return 1;
	}

#define	YKPIV_KEY_MSMD_DIRROOT_ID		0xa0
#define	YKPIV_KEY_MSMD_DIRROOT_CF		0xa1
#define	YKPIV_KEY_MSMD_DIRROOT_APPS		0xa2
#define	YKPIV_KEY_MSMD_DIRMSCP_CMAPFILE	0xa3

#define	YKPIV_OBJ_MSMD			0x5fd000
#define	YKPIV_OBJ_MSMDCARDID	(YKPIV_OBJ_MSMD + 1) // Fixed Size: 16 bytes
#define	YKPIV_OBJ_MSMDCARDCF	(YKPIV_OBJ_MSMD + 2) // Variable Size:  6 bytes - 8KB or more
#define	YKPIV_OBJ_MSMDCARDAPPS	(YKPIV_OBJ_MSMD + 3) // Fixed Size:  8 bytes
#define	YKPIV_OBJ_MSMDCMAPFILE	(YKPIV_OBJ_MSMD + 4) // Variable Size:  6 bytes - 8KB or more


	for (int j = 1; j <= 4; j++) {
		for (int i = 0; i < sizeof(buf); i++) {
			buf[i] = j+0x60;
		}
		//YKPIV_OBJ_MSMD + j
		rc = ykpiv_save_object(state_a, YKPIV_OBJ_MSMD + j, buf, sizeof(buf));
		if (rc != YKPIV_OK) {
			fprintf(stderr, "Failed ykpiv_save_object. rc=%d\n", rc);
			return 1;
		}
		int buf_len = sizeof(buf);
		memset(buf, 0, sizeof(buf));
		rc = ykpiv_fetch_object(state_a, YKPIV_OBJ_MSMD + j, buf, &buf_len);
		if (rc != YKPIV_OK) {
			fprintf(stderr, "Failed ykpiv_fetch_object. rc=%d\n", rc);
			return 1;
		}
		printf("ykpiv_fetch_object[YKPIV_OBJ_MSMD + %d]: buf_len=%d\n", j, buf_len);
	}

#else
	printf("_change_pin\n");
	retries = 0;
	const char* pwd_a = "aaaaaaaa";
	const char* pwd_b = "bbbbbbbb";
	rc = ykpiv_change_pin(state_a, pwd_b, 8, pwd_a, 8, &retries);
	if (rc != YKPIV_OK) {
		fprintf(stderr, "Failed ykpiv_change_pin. retries=%d rc=%d\n", retries, rc);
		return 1;
	}
#endif

	printf("\n\nALL PASSED\n");
	ykpiv_done(state_a);

	return 0;
}