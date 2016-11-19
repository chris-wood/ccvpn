#include <sodium.h>

#define MESSAGE (const unsigned char *) "Ivan de Oliveira Nunes"
#define MESSAGE_LEN sizeof("Ivan de Oliveira Nunes")
#define ADDITIONAL_DATA (const unsigned char *) ""
#define ADDITIONAL_DATA_LEN 0

/*
#define ADDITIONAL_DATA (const unsigned char *) "123456"
#define ADDITIONAL_DATA_LEN 6
*/
unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES];
unsigned char key[crypto_aead_aes256gcm_KEYBYTES];
unsigned char ciphertext[MESSAGE_LEN + crypto_aead_aes256gcm_ABYTES];
unsigned long long ciphertext_len;

int main(){
	if(sodium_init()==-1){
		return 1;
	}
	if (crypto_aead_aes256gcm_is_available() == 0) {
		abort(); /* Not available on this CPU */
	}

	randombytes_buf(key, sizeof key);
	randombytes_buf(nonce, sizeof nonce);

    printf("Key size: %zu, Nonce size: %zu\n", sizeof(key), sizeof(nonce));

	crypto_aead_aes256gcm_encrypt(ciphertext, &ciphertext_len,
		                          MESSAGE, MESSAGE_LEN,
		                          ADDITIONAL_DATA, ADDITIONAL_DATA_LEN,
		                          NULL, nonce, key);

	unsigned char decrypted[MESSAGE_LEN];
	unsigned long long decrypted_len;
	if (ciphertext_len < crypto_aead_aes256gcm_ABYTES ||
		crypto_aead_aes256gcm_decrypt(decrypted, &decrypted_len,
		                              NULL,
		                              ciphertext, ciphertext_len,
		                              ADDITIONAL_DATA,
		                              ADDITIONAL_DATA_LEN,
		                              nonce, key) != 0) {
		/* message forged! */
	}else{
		printf("Message ok!\n");
		printf("Content: %s\n",decrypted);
	}

	return 0;
}
