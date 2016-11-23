#include <sodium.h>

int
main(int argc, char *argv[argc])
{
	unsigned char recipient_pk[crypto_box_PUBLICKEYBYTES];
	unsigned char recipient_sk[crypto_box_SECRETKEYBYTES];
	crypto_box_keypair(recipient_pk, recipient_sk);

    printf("Public key size %zu, private key size %zu\n", sizeof(recipient_pk), sizeof(recipient_sk));

    FILE* pk = fopen("/tmp/key.pub","w");
    fprintf(pk,"1");
    fwrite(recipient_pk,sizeof(char),crypto_box_PUBLICKEYBYTES,pk);
    fclose(pk);

    FILE* sk = fopen("/tmp/key.sec","w");
    fprintf(sk,"1");
    fwrite(recipient_sk,sizeof(char),crypto_box_SECRETKEYBYTES,sk);
    fclose(sk);

    unsigned char sym_key[crypto_aead_aes256gcm_KEYBYTES+crypto_aead_aes256gcm_NPUBBYTES];
	randombytes_buf(sym_key, sizeof sym_key);
    sk = fopen("/tmp/key.sym","w");
    fprintf(sk,"0");
    fwrite(sym_key,sizeof(char),crypto_aead_aes256gcm_KEYBYTES+crypto_aead_aes256gcm_NPUBBYTES,sk);
    fclose(sk);

	return 0;
}
