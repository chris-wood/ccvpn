#include <sodium.h>

#define MESSAGE (const unsigned char *) "Ivan de Oliveira Nunes"
#define MESSAGE_LEN 22
#define CIPHERTEXT_LEN (crypto_box_SEALBYTES + MESSAGE_LEN)

int main(){

	/* Recipient creates a long-term key pair */
	unsigned char recipient_pk[crypto_box_PUBLICKEYBYTES];
	unsigned char recipient_sk[crypto_box_SECRETKEYBYTES];
	crypto_box_keypair(recipient_pk, recipient_sk);

    printf("Public key size %zu, private key size %zu\n", sizeof(recipient_pk), sizeof(recipient_sk));

	/* Anonymous sender encrypts a message using an ephemeral key pair
	 * and the recipient's public key */
	unsigned char ciphertext[CIPHERTEXT_LEN];
	crypto_box_seal(ciphertext, MESSAGE, MESSAGE_LEN, recipient_pk);

	/* Recipient decrypts the ciphertext */
	unsigned char decrypted[MESSAGE_LEN];
	if (crypto_box_seal_open(decrypted, ciphertext, CIPHERTEXT_LEN,
		                     recipient_pk, recipient_sk) != 0) {
		/* message corrupted or not intended for this recipient */
		printf("Not decyphered\n");
	}else{
		printf("Message: %s\n",decrypted);
	}
	
	return 0;
}
