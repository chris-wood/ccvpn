/*
 * Copyright (c) 2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX OR PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ################################################################################
 * #
 * # PATENT NOTICE
 * #
 * # This software is distributed under the BSD 2-clause License (see LICENSE
 * # file).  This BSD License does not make any patent claims and as such, does
 * # not act as a patent grant.  The purpose of this section is for each contributor
 * # to define their intentions with respect to intellectual property.
 * #
 * # Each contributor to this source code is encouraged to state their patent
 * # claims and licensing mechanisms for any contributions made. At the end of
 * # this section contributors may each make their own statements.  Contributor's
 * # claims and grants only apply to the pieces (source code, programs, text,
 * # media, etc) that they have contributed directly to this software.
 * #
 * # There is no guarantee that this section is complete, up to date or accurate. It
 * # is up to the contributors to maintain their portion of this section and up to
 * # the user of the software to verify any claims herein.
 * #
 * # Do not remove this header notification.  The contents of this section must be
 * # present in all distributions of the software.  You may only modify your own
 * # intellectual property statements.  Please provide contact information.
 *
 * - Palo Alto Research Center, Inc
 * This software distribution does not grant any rights to patents owned by Palo
 * Alto Research Center, Inc (PARC). Rights to these patents are available via
 * various mechanisms. As of January 2016 PARC has committed to FRAND licensing any
 * intellectual property used by its contributions to this software. You may
 * contact PARC at cipo@parc.com for more information or visit http://www.ccnx.org
 */
/**
 * @author Kevin Fox, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
/*
 * Athena Example Runtime implementation
 */

#include <config.h>
#include <pthread.h>
#include <unistd.h>
#include <sodium.h>
#include <string.h>

#include <ccnx/forwarder/athena/athena.h>
#include <ccnx/forwarder/athena/athena_Control.h>
#include <ccnx/forwarder/athena/athena_InterestControl.h>
#include <ccnx/forwarder/athena/athena_LRUContentStore.h>

#include <ccnx/common/ccnx_Interest.h>
#include <ccnx/common/ccnx_InterestReturn.h>
#include <ccnx/common/ccnx_ContentObject.h>
#include <ccnx/common/ccnx_Manifest.h>

#include <ccnx/common/validation/ccnxValidation_CRC32C.h>
#include <ccnx/common/codec/ccnxCodec_TlvPacket.h>

#include <parc/logging/parc_LogReporterTextStdout.h>

#include <parc/algol/parc_Clock.h>

// TIME MEASUREMENT FUNCTIONS  /////////////////////////

uint64_t time_stamp_before, time_stamp_after;

uint64_t
current_time()
{
    struct timeval currentTimeVal;
    gettimeofday(&currentTimeVal, NULL);
    uint64_t microseconds = currentTimeVal.tv_sec * 1000000 + currentTimeVal.tv_usec;
    return microseconds;
}

uint64_t 
updateAvg(uint64_t currentAvg, uint64_t nSamples, uint64_t newValue)
{
    return (currentAvg * nSamples + newValue) / (nSamples + 1) ;
}

void
printTimeCSV(Athena *athena, FILE *fp)
{   
    printf("\n");
    if (fp == NULL) {
        printf("intReg,intEncap,intDecap,contReg,contEnc,contDec\n");

        printf("%d,%d,%d,%d,%d,%d\n",
            (int)athena->time.avg_interest_time,
            (int)athena->time.avg_vpn_enc_interest_time,
            (int)athena->time.avg_vpn_dec_interest_time,
            (int)athena->time.avg_content_time,
            (int)athena->time.avg_vpn_enc_content_time,
            (int)athena->time.avg_vpn_dec_content_time
        );
    } else {
        //fprintf(fp, "intReg,intEncap,intDecap,contReg,contEnc,contDec\n");

        fprintf(fp, "%d,%d,%d,%d,%d,%d\n",
            (int)athena->time.avg_interest_time,
            (int)athena->time.avg_vpn_enc_interest_time,
            (int)athena->time.avg_vpn_dec_interest_time,
            (int)athena->time.avg_content_time,
            (int)athena->time.avg_vpn_enc_content_time,
            (int)athena->time.avg_vpn_dec_content_time
        );
    }
    printf("\n");
}

// END TIME MEASUREMENT /////////////////////////

static PARCLog *
_athena_logger_create(void) {

    PARCLogReporter *reporter = parcLogReporterTextStdout_Create();

    PARCLog *log = parcLog_Create("localhost", "athena", NULL, reporter);
    parcLogReporter_Release(&reporter);

    parcLog_SetLevel(log, PARCLogLevel_Info);
    return log;
}

static void
_removeLink(void *context, PARCBitVector *linkVector) {
    Athena *athena = (Athena *) context;

    const char *linkVectorString = parcBitVector_ToString(linkVector);

    // cleanup specified links from the FIB and PIT, these calls are currently presumed synchronous
    bool result = athenaFIB_RemoveLink(athena->athenaFIB, linkVector);
    assertTrue(result, "Failed to remove link from FIB %s", linkVectorString);

    result = athenaPIT_RemoveLink(athena->athenaPIT, linkVector);
    assertTrue(result, "Failed to remove link from PIT %s", linkVectorString);

    parcMemory_Deallocate(&linkVectorString);
}

static void
_athenaDestroy(Athena **athena) {
    ccnxName_Release(&((*athena)->athenaName));
    ccnxName_Release(&((*athena)->publicName));
    athenaTransportLinkAdapter_Destroy(&((*athena)->athenaTransportLinkAdapter));
    athenaContentStore_Release(&((*athena)->athenaContentStore));
    athenaPIT_Release(&((*athena)->athenaPIT));
    athenaFIB_Release(&((*athena)->athenaFIB));
    parcLog_Release(&((*athena)->log));

    if ((*athena)->publicKey) {
        parcBuffer_Release(&((*athena)->publicKey));
    }
    if ((*athena)->secretKey) {
        parcBuffer_Release(&((*athena)->secretKey));
    }

    if ((*athena)->configurationLog) {
        parcOutputStream_Release(&((*athena)->configurationLog));
    }
    if ((*athena)->secretKey != NULL) {
        parcBuffer_Release(&((*athena)->secretKey));
    }
    if ((*athena)->publicKey != NULL) {
        parcBuffer_Release(&((*athena)->publicKey));
    }

}

parcObject_ExtendPARCObject(Athena, _athenaDestroy, NULL, NULL, NULL, NULL, NULL, NULL);

Athena *
athena_CreateWithKeyPair(CCNxName *name, size_t contentStoreSizeInMB, PARCBuffer *secretKey, PARCBuffer *publicKey) {
    assertTrue(crypto_aead_aes256gcm_is_available() == 1, "AES-GCM-256 is not available");

    Athena *athena = parcObject_CreateAndClearInstance(Athena);

    athena->athenaName = ccnxName_CreateFromCString(CCNxNameAthena_Forwarder);
    athena->publicName = ccnxName_Acquire(name);
    assertNotNull(athena->athenaName, "Failed to create forwarder name (%s)", CCNxNameAthena_Forwarder);

    athena->athenaFIB = athenaFIB_Create();
    assertNotNull(athena->athenaFIB, "Failed to create FIB");

    athena->athenaPIT = athenaPIT_Create();
    assertNotNull(athena->athenaPIT, "Failed to create PIT");

    AthenaLRUContentStoreConfig storeConfig;
    storeConfig.capacityInMB = contentStoreSizeInMB;

    athena->athenaContentStore = athenaContentStore_Create(&AthenaContentStore_LRUImplementation, &storeConfig);
    assertNotNull(athena->athenaContentStore, "Failed to create Content Store");

    athena->athenaTransportLinkAdapter = athenaTransportLinkAdapter_Create(_removeLink, athena);
    assertNotNull(athena->athenaTransportLinkAdapter, "Failed to create Transport Link Adapter");

    athena->log = _athena_logger_create();
    athena->athenaState = Athena_Running;

    athena->secretKey = parcBuffer_Acquire(secretKey);
    athena->publicKey = parcBuffer_Acquire(publicKey);

    return athena;
}

Athena *
athena_Create(CCNxName *name, size_t contentStoreSizeInMB) {
    assertTrue(crypto_aead_aes256gcm_is_available() == 1, "AES-GCM-256 is not available");

    Athena *athena = parcObject_CreateAndClearInstance(Athena);

    athena->athenaName = ccnxName_CreateFromCString(CCNxNameAthena_Forwarder);
    athena->publicName = ccnxName_Acquire(name);
    assertNotNull(athena->athenaName, "Failed to create forwarder name (%s)", CCNxNameAthena_Forwarder);

    athena->athenaFIB = athenaFIB_Create();
    assertNotNull(athena->athenaFIB, "Failed to create FIB");

    athena->athenaPIT = athenaPIT_Create();
    assertNotNull(athena->athenaPIT, "Failed to create PIT");

    AthenaLRUContentStoreConfig storeConfig;
    storeConfig.capacityInMB = contentStoreSizeInMB;

    athena->athenaContentStore = athenaContentStore_Create(&AthenaContentStore_LRUImplementation, &storeConfig);
    assertNotNull(athena->athenaContentStore, "Failed to create Content Store");

    athena->athenaTransportLinkAdapter = athenaTransportLinkAdapter_Create(_removeLink, athena);
    assertNotNull(athena->athenaTransportLinkAdapter, "Failed to create Transport Link Adapter");

    athena->log = _athena_logger_create();
    athena->athenaState = Athena_Running;

    athena->secretKey = NULL;
    athena->publicKey = NULL;

    return athena;
}

parcObject_ImplementAcquire(athena, Athena);

parcObject_ImplementRelease(athena, Athena);

//static void
//_processInterestControl(Athena *athena, CCNxInterest *interest, PARCBitVector *ingressVector) {
//    //
//    // Management messages
//    //
//    athenaInterestControl(athena, interest, ingressVector);
//}

static CCNxMetaMessage *
_processControl(Athena *athena, CCNxControl *control, PARCBitVector *ingressVector) {
    //
    // Management messages
    //
    athenaControl(athena, control, ingressVector);
    return NULL;
}

static CCNxInterest *
_encryptInterestSym(Athena *athena, CCNxInterest *interest, PARCBuffer *symKeyBuffer, CCNxName *prefix, PARCBuffer *responseKeyAndNonce)
{
    // Get the wire format
    PARCBuffer *interestWireFormat = athenaTransportLinkModule_CreateMessageBuffer(interest);
    size_t interestSize = parcBuffer_Remaining(interestWireFormat);

    // Generate a random nonce
    int nonceLength = crypto_aead_aes256gcm_NPUBBYTES;
    unsigned char nonceBuffer[nonceLength];
    randombytes_buf(nonceBuffer, nonceLength);

    // Generate a random symmetric that will be used when encrypting the response
    int symmetricKeyLen = parcBuffer_Remaining(responseKeyAndNonce);

    // Create the interest and key buffer that will be encrypted
    PARCBuffer *interestKeyBuffer = parcBuffer_Allocate(symmetricKeyLen + interestSize);
    parcBuffer_PutArray(interestKeyBuffer, symmetricKeyLen, parcBuffer_Overlay(responseKeyAndNonce, 0));
    parcBuffer_PutBuffer(interestKeyBuffer, interestWireFormat);
    parcBuffer_Flip(interestKeyBuffer);
    size_t plaintextLength = parcBuffer_Remaining(interestKeyBuffer);
    PARCBuffer *encapsulatedInterest;

    // symmetric encapsulation
    encapsulatedInterest = parcBuffer_Allocate(plaintextLength + crypto_aead_aes256gcm_ABYTES);

    unsigned long long ciphertext_len;
    crypto_aead_aes256gcm_encrypt(parcBuffer_Overlay(encapsulatedInterest, 0), &ciphertext_len,
                                  parcBuffer_Overlay(interestKeyBuffer, 0), plaintextLength,
                                  NULL, 0,
                                  NULL,
                                  (unsigned char *) nonceBuffer,
                                  (unsigned char *) parcBuffer_Overlay(symKeyBuffer, 0));

    unsigned char hash[crypto_generichash_BYTES];
    size_t keySize = parcBuffer_Remaining(symKeyBuffer);

    crypto_generichash(hash, sizeof hash,
                   parcBuffer_Overlay(symKeyBuffer, 0), keySize,
                   NULL, 0);

    PARCBuffer *payload = parcBuffer_Allocate(1 + sizeof hash + crypto_aead_aes256gcm_NPUBBYTES + parcBuffer_Remaining(encapsulatedInterest));
    parcBuffer_PutUint8(payload, '0');
    parcBuffer_PutArray(payload, sizeof hash, hash);
    parcBuffer_PutArray(payload, crypto_aead_aes256gcm_NPUBBYTES, nonceBuffer);
    parcBuffer_PutBuffer(payload, encapsulatedInterest);
    parcBuffer_Flip(payload);

    // Create the new interest and add the ciphertext as the payload
    CCNxInterest *newInterest = ccnxInterest_CreateSimple(prefix);
    ccnxInterest_SetPayloadAndId(newInterest, payload);

    parcBuffer_Release(&interestWireFormat);
    parcBuffer_Release(&interestKeyBuffer);
    parcBuffer_Release(&encapsulatedInterest);
    parcBuffer_Release(&payload);

    return newInterest;
}

static CCNxInterest *
_encryptInterestPub(Athena *athena, CCNxInterest *interest, PARCBuffer *keyBuffer, CCNxName *prefix, PARCBuffer *responseKeyAndNonce)
{
    // Get the wire format
    PARCBuffer *interestWireFormat = athenaTransportLinkModule_CreateMessageBuffer(interest);
    size_t interestSize = parcBuffer_Remaining(interestWireFormat);

    // Generate a random symmetric that will be used when encrypting the response
    int keyAndNonceSize = parcBuffer_Remaining(responseKeyAndNonce);

    // Create the interest and key buffer that will be encrypted
    PARCBuffer *messagePlaintext = parcBuffer_Allocate(keyAndNonceSize + interestSize);
    parcBuffer_PutArray(messagePlaintext, keyAndNonceSize, parcBuffer_Overlay(responseKeyAndNonce, 0));
    parcBuffer_PutBuffer(messagePlaintext, interestWireFormat);
    parcBuffer_Flip(messagePlaintext);
    size_t plaintextLength = parcBuffer_Remaining(messagePlaintext);

    PARCBuffer *encapsulatedInterest = parcBuffer_Allocate(plaintextLength + crypto_box_SEALBYTES);
    crypto_box_seal(parcBuffer_Overlay(encapsulatedInterest, 0), parcBuffer_Overlay(messagePlaintext, 0),
                    plaintextLength, parcBuffer_Overlay(keyBuffer, 0));
 
    unsigned char hash[crypto_generichash_BYTES];
    size_t keySize = parcBuffer_Remaining(keyBuffer);

    crypto_generichash(hash, sizeof hash,
                   parcBuffer_Overlay(keyBuffer, 0), keySize,
                   NULL, 0);

    PARCBuffer *payload = parcBuffer_Allocate(1 + sizeof hash + parcBuffer_Remaining(encapsulatedInterest));
    parcBuffer_PutUint8(payload, '1');
    parcBuffer_PutArray(payload, sizeof hash, hash);
    parcBuffer_PutBuffer(payload, encapsulatedInterest);
    parcBuffer_Flip(payload);

    // Create the new interest and add the ciphertext as the payload
    CCNxInterest *newInterest = ccnxInterest_CreateSimple(prefix);
    ccnxInterest_SetPayloadAndId(newInterest, payload);

    parcBuffer_Release(&interestWireFormat);
    parcBuffer_Release(&messagePlaintext);
    parcBuffer_Release(&encapsulatedInterest);
    parcBuffer_Release(&payload);

    return newInterest;
}

static CCNxMetaMessage *
_processInterest(Athena *athena, CCNxInterest *interest, PARCBitVector *ingressVector) {
    // Start measuring time
    time_stamp_before = current_time();
    // Type of time measurement variable
    uint8_t type = 0;
    uint8_t isItPublicKey = 0;
    uint8_t hoplimit;
    //
    // *   (0) Hoplimit check, exclusively on interest messages
    //
    int linkId = parcBitVector_NextBitSet(ingressVector, 0);
    if (athenaTransportLinkAdapter_IsNotLocal(athena->athenaTransportLinkAdapter, linkId)) {
        hoplimit = ccnxInterest_GetHopLimit(interest);
        if (hoplimit == 0) {
            // We should never receive a message with a hoplimit of 0 from a non-local source.
//            parcLog_Error(athena->log,
//                          "Received a message with a hoplimit of zero from a non-local source (%s).",
//                          athenaTransportLinkAdapter_LinkIdToName(athena->athenaTransportLinkAdapter, linkId));
            return NULL;
        }
        ccnxInterest_SetHopLimit(interest, hoplimit - 1);
    }

    //
    // *   (1) if the interest is in the ContentStore, reply and return,
    //     assuming that other PIT entries were satisified when the content arrived.
    //
    CCNxMetaMessage *content = athenaContentStore_GetMatch(athena->athenaContentStore, interest);
    if (content) {
        const char *ingressVectorString = parcBitVector_ToString(ingressVector);
        //parcLog_Debug(athena->log, "Forwarding content from store to %s", ingressVectorString);
        parcMemory_Deallocate(&ingressVectorString);
        PARCBitVector *result = athenaTransportLinkAdapter_Send(athena->athenaTransportLinkAdapter, content,
                                                                ingressVector);
        if (result) { // failed channels - client will resend interest unless we wish to optimize things here
            parcBitVector_Release(&result);
        }
        return NULL;
    }

    CCNxInterest *newInterest = ccnxInterest_Acquire(interest);
    CCNxName *originalInterestName = ccnxInterest_GetName(newInterest);

    PARCBuffer *symKeyBuffer = NULL;

    // Check to see if this interest has a prefix that is meant for us (our public prefix)
    // If so, then it contains an encapsulated interest. So we must decrypt the interest.
    bool isPrefix = ccnxName_StartsWith(originalInterestName, athena->publicName);
    bool hasPayload = ccnxInterest_GetPayload(interest) != NULL;
    if (isPrefix && hasPayload) {
        type = 2;

        PARCBuffer *interestPayload = ccnxInterest_GetPayload(interest);
        PARCBuffer *secretKey = athena->secretKey;
        PARCBuffer *publicKey = athena->publicKey;

        // 1. Read the key flag
        uint8_t keyFlag = parcBuffer_GetUint8(interestPayload);

        // 2. Read the key ID
        uint8_t keyIdBuffer[crypto_generichash_BYTES];
        parcBuffer_GetBytes(interestPayload, crypto_generichash_BYTES, keyIdBuffer);

        // The plaintext buffer container
        PARCBuffer *decrypted = NULL;

        parcBuffer_SetPosition(publicKey, 1);
        parcBuffer_SetPosition(secretKey, 1);

        if (keyFlag == '1') {
            isItPublicKey = 1;
            int ciphertextSize = parcBuffer_Remaining(interestPayload);
            decrypted = parcBuffer_Allocate(ciphertextSize);

            if (0 != crypto_box_seal_open(
                    parcBuffer_Overlay(decrypted, 0),
                    parcBuffer_Overlay(interestPayload, 0),
                    ciphertextSize,
                    parcBuffer_Overlay(publicKey, 0),
                    parcBuffer_Overlay(secretKey, 0))) {
                /* message corrupted or not intended for this recipient */
                ccnxInterest_Release(&newInterest);
                return NULL;
            }
        } else {
            // 3. Read the nonce
            uint8_t nonceBuffer[crypto_aead_aes256gcm_NPUBBYTES];
            parcBuffer_GetBytes(interestPayload, crypto_aead_aes256gcm_NPUBBYTES, nonceBuffer);

            // XXX: the code below assumes that the symmetric key is the `secretKey` variable -- this should be changed

            int ciphertextSize = parcBuffer_Remaining(interestPayload);
            decrypted = parcBuffer_Allocate(ciphertextSize);
            unsigned long long decryptedLength;
            int success = crypto_aead_aes256gcm_decrypt(parcBuffer_Overlay(decrypted, 0), &decryptedLength,
                                          NULL, parcBuffer_Overlay(interestPayload, 0), ciphertextSize,
                                          NULL,
                                          0,
                                          nonceBuffer, parcBuffer_Overlay(secretKey, 0));
            if (success != 0) {
                ccnxInterest_Release(&newInterest);
                return NULL;
            }
        }

        parcBuffer_SetPosition(publicKey, 0);
        parcBuffer_SetPosition(secretKey, 0);

        // Suck in the key and then advance the buffer to point to the encapsulated interest
        symKeyBuffer = parcBuffer_Allocate(crypto_aead_aes256gcm_KEYBYTES + crypto_aead_aes256gcm_NPUBBYTES);
        for (size_t i = 0; i < crypto_aead_aes256gcm_KEYBYTES + crypto_aead_aes256gcm_NPUBBYTES; i++) {
            parcBuffer_PutUint8(symKeyBuffer, parcBuffer_GetUint8(decrypted));
        }
        parcBuffer_Flip(symKeyBuffer);

        uint8_t msb = ((uint8_t *) parcBuffer_Overlay(decrypted, 0)) [2];
        uint8_t lsb = ((uint8_t *) parcBuffer_Overlay(decrypted, 0)) [3];
        uint16_t size = (((uint16_t) msb) << 8) | lsb;

        PARCBuffer *interestBuffer = parcBuffer_Allocate(size);
        for (size_t i = 0; i < size; i++) {
            parcBuffer_PutUint8(interestBuffer, parcBuffer_GetUint8(decrypted));
        }
        parcBuffer_Flip(interestBuffer);

        // From the wire format, re-create the encapsulated interest
        CCNxMetaMessage *rawMessage = ccnxMetaMessage_CreateFromWireFormatBuffer(interestBuffer);

        ccnxInterest_Release(&newInterest);
        newInterest = ccnxInterest_Acquire(ccnxMetaMessage_GetInterest(rawMessage));
        ccnxMetaMessage_Release(&rawMessage);

        parcBuffer_Release(&interestBuffer);
        parcBuffer_Release(&decrypted);     

    }

    //
    // *   (3) if it's in the FIB, forward, then update the PIT expectedReturnVector so we can verify
    //         when the returned object arrives that it came from an interface it was expected from.
    //         Interest messages with a hoplimit of 0 will never be sent out by the link adapter to a
    //         non-local interface so we need not check that here.
    //
    CCNxName *ccnxName = ccnxInterest_GetName(newInterest);
    AthenaFIBValue *vector = athenaFIB_Lookup(athena->athenaFIB, ccnxName, ingressVector);
    PARCBitVector *egressVector = NULL;
    if (vector != NULL) {
        egressVector = athenaFIBValue_GetVector(vector);
    }

    if (egressVector != NULL) {
        // If no links are in the egress vector the FIB returned, return a no route interest message
        if (parcBitVector_NumberOfBitsSet(egressVector) == 0) {
            if (ccnxWireFormatMessage_ConvertInterestToInterestReturn(newInterest,
                                                                      CCNxInterestReturn_ReturnCode_NoRoute)) {
                // NOTE: The Interest has been modified in-place. It is now an InterestReturn.
                //parcLog_Debug(athena->log, "Returning Interest as InterestReturn (code: NoRoute)");
                PARCBitVector *failedLinks = athenaTransportLinkAdapter_Send(athena->athenaTransportLinkAdapter,
                                                                             newInterest, ingressVector);
                if (failedLinks != NULL) {
                    parcBitVector_Release(&failedLinks);
                }
            } else {
                if (ccnxName) {
                    const char *name = ccnxName_ToString(ccnxName);
                    //parcLog_Error(athena->log, "Unable to return Interest (%s) as InterestReturn (code: NoRoute).",
                    //              name);
                    parcMemory_Deallocate(&name);
                } else {
                    //parcLog_Error(athena->log, "Unable to return Interest () as InterestReturn (code: NoRoute).");
                }
            }
        } else {
            // If there is a a public key and target prefix associated with the matching FIB entry,
            // then we must encapsulate the interest
            PARCBuffer *keyBuffer = athenaFIBValue_GetKey(vector);
            CCNxName *targetPrefix = athenaFIBValue_GetOutputPrefix(vector);
            if (keyBuffer != NULL && targetPrefix != NULL) {
                type = 1;

                // XXX: figure out how this is being modified in place
                
                CCNxInterest *encryptedInterest;
                CCNxName *copyPrefix = ccnxName_Copy(targetPrefix);

                int responseKeyAndNonceLength = crypto_aead_aes256gcm_KEYBYTES + crypto_aead_aes256gcm_NPUBBYTES;
                unsigned char responseKeyAndNonce[responseKeyAndNonceLength];
                randombytes_buf(responseKeyAndNonce, responseKeyAndNonceLength);
                PARCBuffer *keyAndNonce = parcBuffer_CreateFromArray(responseKeyAndNonce, responseKeyAndNonceLength);
                parcBuffer_Flip(keyAndNonce);

                bool isPublicKey = parcBuffer_GetAtIndex(keyBuffer, 0) == '1';
                isItPublicKey = isPublicKey;
                parcBuffer_SetPosition(keyBuffer, 1);

                if (isPublicKey) {
                    encryptedInterest = _encryptInterestPub(athena, newInterest, keyBuffer, copyPrefix, keyAndNonce);
                } else {
                    encryptedInterest = _encryptInterestSym(athena, newInterest, keyBuffer, copyPrefix, keyAndNonce);
                }

                parcBuffer_SetPosition(keyBuffer, 0);

                ccnxInterest_Release(&newInterest);
                newInterest = encryptedInterest;

                if (symKeyBuffer != NULL) {
                    parcBuffer_Release(&symKeyBuffer);
                }

                symKeyBuffer = parcBuffer_CreateFromArray(responseKeyAndNonce, responseKeyAndNonceLength);
                parcBuffer_Flip(symKeyBuffer);
                parcBuffer_Release(&keyAndNonce);
                ccnxName_Release(&copyPrefix);
            }

            // debug
            char *interestString = ccnxInterest_ToString(newInterest);
            //parcLog_Info(athena->log, "Sent: %s", interestString);
            parcMemory_Deallocate(&interestString);

            char *nameString = ccnxName_ToString(originalInterestName);
            //parcLog_Info(athena->log, "Adding the interest name: %s", nameString);
            parcMemory_Deallocate(&nameString);

            PARCBitVector *expectedReturnVector;
            AthenaPITResolution result;
            if ((result = athenaPIT_AddInterest(athena->athenaPIT, newInterest, ingressVector, originalInterestName, symKeyBuffer,
                                                &expectedReturnVector)) != AthenaPITResolution_Forward) {
                if (result == AthenaPITResolution_Error) {
                    //parcLog_Error(athena->log, "PIT resolution error");
                }
                ccnxInterest_Release(&newInterest);
                return NULL;
            }
            
            PARCBitVector *failedLinks =
                    athenaTransportLinkAdapter_Send(athena->athenaTransportLinkAdapter, newInterest, egressVector);

            if (failedLinks) { // remove failed channels - client will resend interest unless we wish to optimize here
                parcBitVector_ClearVector(expectedReturnVector, failedLinks);
                parcBitVector_Release(&failedLinks);
            }
        }
        athenaFIBValue_Release(&vector);
    } else {
        // No FIB entry found, return a NoRoute interest return and remove the entry from the PIT.

        if (ccnxWireFormatMessage_ConvertInterestToInterestReturn(newInterest,
                                                                  CCNxInterestReturn_ReturnCode_NoRoute)) {
            // NOTE: The Interest has been modified in-place. It is now an InterestReturn.
            //parcLog_Debug(athena->log, "Returning Interest as InterestReturn (code: NoRoute)");
            PARCBitVector *failedLinks = athenaTransportLinkAdapter_Send(athena->athenaTransportLinkAdapter, newInterest,
                                                                         ingressVector);
            if (failedLinks != NULL) {
                parcBitVector_Release(&failedLinks);
            }
        } else {
            if (ccnxName) {
                const char *name = ccnxName_ToString(ccnxName);
                //parcLog_Error(athena->log, "Unable to return Interest (%s) as InterestReturn (code: NoRoute).", name);
                parcMemory_Deallocate(&name);
            } else {
                //parcLog_Error(athena->log, "Unable to return Interest () as InterestReturn (code: NoRoute).");
            }
        }

        if (ccnxName) {
            const char *name = ccnxName_ToString(ccnxName);
            //parcLog_Debug(athena->log, "Name (%s) not found in FIB and no default route. Message dropped.", name);
            parcMemory_Deallocate(&name);
        } else {
            //parcLog_Debug(athena->log, "Name () not found in FIB and no default route. Message dropped.");
        }
    }

    if (symKeyBuffer != NULL) {
        parcBuffer_Release(&symKeyBuffer);
    }


    // Compute total time
    time_stamp_after = current_time();
/*
    FILE* fp;
    switch (type) {
        case 1 :
            if(isItPublicKey) {
                fp = fopen("int_encap_pk.csv","a");
            } else {
                fp = fopen("int_encap_sk.csv","a");
            }
            fprintf(fp,"%d\n", time_stamp_after - time_stamp_before);
            fclose(fp);
            athena->time.avg_vpn_enc_interest_time = updateAvg( athena->time.avg_vpn_enc_interest_time, athena->time.n_vpn_enc_interest_time, time_stamp_after - time_stamp_before );
            athena->time.n_vpn_enc_interest_time++;
            //printf("Avg. VPN Encap. interest computation time: %d\n\n", (int)athena->time.avg_vpn_enc_interest_time);    
            break;
        case 2 :
            if(isItPublicKey) {
                fp = fopen("int_decap_pk.csv","a");
            } else {
                fp = fopen("int_decap_sk.csv","a");
            }
            fprintf(fp,"%d\n", time_stamp_after - time_stamp_before);
            fclose(fp);

            athena->time.avg_vpn_dec_interest_time = updateAvg(athena->time.avg_vpn_dec_interest_time, athena->time.n_vpn_dec_interest_time, time_stamp_after - time_stamp_before);
            athena->time.n_vpn_dec_interest_time++;
            //printf("Avg. VPN Decap. interest computation time: %d\n\n", (int)athena->time.avg_vpn_dec_interest_time);    
            break;
        default :
            athena->time.avg_interest_time = updateAvg(athena->time.avg_interest_time, athena->time.n_interest_time, time_stamp_after - time_stamp_before);
            athena->time.n_interest_time++;
            //printf("Avg. Regular interest  computation time: %d\n\n", (int)athena->time.avg_interest_time);   
    }
*/
    return newInterest;
}
static CCNxMetaMessage *
_processInterestReturn(Athena *athena, CCNxInterestReturn *interestReturn, PARCBitVector *ingressVector) {
    // We can ignore interest return messages and allow the PIT entry to timeout, or
    //
    // Verify the return came from the next-hop where the interest was originally sent to
    // if not, ignore
    // otherwise, may try another forwarding path or clear the PIT state and forward the interest return on the reverse path

    return NULL;
}

static PARCBuffer *
_createMessageHash(const CCNxMetaMessage *metaMessage) {
    // We need to interact with the content message as a WireFormatMessage to get to
    // the content hash API.
    CCNxWireFormatMessage *wireFormatMessage = (CCNxWireFormatMessage *) metaMessage;

    PARCCryptoHash *hash = ccnxWireFormatMessage_CreateContentObjectHash(wireFormatMessage);

    if (hash != NULL) {
        PARCBuffer *buffer = parcBuffer_Acquire(parcCryptoHash_GetDigest(hash));
        parcCryptoHash_Release(&hash);
        return buffer;
    } else {
        return NULL;
    }
}

static CCNxMetaMessage *
_processContentObject(Athena *athena, CCNxContentObject *contentObject, PARCBitVector *ingressVector) {
    // Start measuring time
    time_stamp_before = current_time();
    // Type of time measurement variable
    uint8_t type = 0;

    //
    // *   (1) If it does not match anything in the PIT, drop it
    //
    const CCNxName *name = ccnxContentObject_GetName(contentObject);
    PARCBuffer *keyId = ccnxContentObject_GetKeyId(contentObject);
    PARCBuffer *digest = _createMessageHash(contentObject);

    CCNxContentObject *returnContent = ccnxContentObject_Acquire(contentObject);

    AthenaPITValue *value = athenaPIT_Match(athena->athenaPIT, name, keyId, digest, ingressVector);
    PARCBitVector *egressVector = athenaPITValue_GetVector(value);
    PARCBuffer *encryptKey = athenaPITValue_GetKey(value);
    CCNxName *interestName = athenaPITValue_GetName(value);

    if (egressVector) {
        egressVector = parcBitVector_Acquire(egressVector);
        if (parcBitVector_NumberOfBitsSet(egressVector) > 0) {
            if (encryptKey != NULL && interestName != NULL) {
                encryptKey = parcBuffer_Acquire(encryptKey);
                interestName = ccnxName_Acquire(interestName);
//                ccnxName_Display(interestName, 0);

                PARCBuffer *contentWireFormat = athenaTransportLinkModule_CreateMessageBuffer(contentObject);
                size_t contentSize = parcBuffer_Remaining(contentWireFormat);

                PARCBuffer* symKeyBuffer = parcBuffer_Allocate(crypto_aead_aes256gcm_KEYBYTES);
                PARCBuffer* nonceBuffer = parcBuffer_Allocate(crypto_aead_aes256gcm_NPUBBYTES);

                for (size_t i = 0; i < crypto_aead_aes256gcm_KEYBYTES; i++) {
                    parcBuffer_PutUint8(symKeyBuffer, parcBuffer_GetUint8(encryptKey));
                }
                parcBuffer_Flip(symKeyBuffer);
                for (size_t i = 0; i < crypto_aead_aes256gcm_NPUBBYTES; i++) {
                    parcBuffer_PutUint8(nonceBuffer, parcBuffer_GetUint8(encryptKey));
                }
                parcBuffer_Flip(nonceBuffer);

                // Determine if we need to encrypt or decrypt the content by checking if the original interest
                // name matches our prefix. If so, then we decrypted the corresponding interest, which means here
                // we must encrypt the content.
                bool isPrefix = ccnxName_StartsWith(interestName, athena->publicName);
                if (!isPrefix) { // this content object's payload carries the encapsulated content object
                    PARCBuffer *payload = ccnxContentObject_GetPayload(contentObject);
                    contentSize = parcBuffer_Remaining(payload);

                    PARCBuffer* plaintext = parcBuffer_Allocate(contentSize - crypto_aead_aes256gcm_ABYTES);
	                unsigned long long plaintext_len;
                    
                    type = 2;

	                if (contentSize < crypto_aead_aes256gcm_ABYTES ||
		                crypto_aead_aes256gcm_decrypt(parcBuffer_Overlay(plaintext, 0), &plaintext_len,
		                                              NULL,
		                                              parcBuffer_Overlay(payload, 0), contentSize,
		                                              NULL, 0,
		                                              parcBuffer_Overlay(nonceBuffer, 0), parcBuffer_Overlay(symKeyBuffer, 0)) != 0)
                    {
		                // message forged!
                        parcBuffer_Release(&symKeyBuffer);
                        parcBuffer_Release(&nonceBuffer);
                        parcBuffer_Release(&contentWireFormat);
                        parcBuffer_Release(&plaintext);
                        athenaPITValue_Release(&value);
                        return NULL;
    
	                }

                    // Recover the serialized message
                    CCNxMetaMessage *rawMessage = ccnxMetaMessage_CreateFromWireFormatBuffer(plaintext);
                    ccnxContentObject_Release(&returnContent);
                    returnContent = ccnxContentObject_Acquire(ccnxMetaMessage_GetContentObject(rawMessage));
                    ccnxMetaMessage_Release(&rawMessage);
                    parcBuffer_Release(&plaintext);
                } else {
                    PARCBuffer* ciphertext = parcBuffer_Allocate(contentSize + crypto_aead_aes256gcm_ABYTES);
                    unsigned long long ciphertext_len;

                    type = 1;
                	crypto_aead_aes256gcm_encrypt(parcBuffer_Overlay(ciphertext, 0), &ciphertext_len,
		                                          parcBuffer_Overlay(contentWireFormat, 0), contentSize,
		                                          NULL, 0,
                                                  NULL,
                                                  (unsigned char *) parcBuffer_Overlay(nonceBuffer, 0), (unsigned char *) parcBuffer_Overlay(symKeyBuffer, 0));

                    ccnxContentObject_Release(&returnContent);
                    returnContent = ccnxContentObject_CreateWithNameAndPayload(interestName, ciphertext);
                    parcBuffer_Release(&ciphertext);
                }

                parcBuffer_Release(&symKeyBuffer);
                parcBuffer_Release(&nonceBuffer);
                parcBuffer_Release(&contentWireFormat);

                parcBuffer_Release(&encryptKey);
                ccnxName_Release(&interestName);
            }

            //
            // *   (2) Add to the Content Store
            //
            athenaContentStore_PutContentObject(athena->athenaContentStore, returnContent);

            //
            // *   (3) Reverse path forward it via PIT entries
            //
            const char *egressVectorString = parcBitVector_ToString(egressVector);
            //parcLog_Debug(athena->log, "Content Object forwarded to %s.", egressVectorString);
            parcMemory_Deallocate(&egressVectorString);
            PARCBitVector *result = athenaTransportLinkAdapter_Send(athena->athenaTransportLinkAdapter, returnContent, egressVector);

            if (result) {
                // if there are failed channels, client will resend interest unless we wish to retry here
                parcBitVector_Release(&result);
            }
        }
        parcBitVector_Release(&egressVector);
        athenaPITValue_Release(&value);
    }

    // Compute total time
    time_stamp_after = current_time();
/*
    FILE* fp;

    switch (type) {
        case 1 :
            fp = fopen("cont_encap_sk.csv","a");
            fprintf(fp,"%d\n", time_stamp_after - time_stamp_before);
            fclose(fp);

            athena->time.avg_vpn_enc_content_time = updateAvg(athena->time.avg_vpn_enc_content_time, athena->time.n_vpn_enc_content_time, time_stamp_after - time_stamp_before);
            athena->time.n_vpn_enc_content_time++;
            //printf("Avg. VPN Encrypt. content computation time: %d\n\n", (int)athena->time.avg_vpn_enc_content_time);
            break;
        case 2 :
            fp = fopen("cont_decap_sk.csv","a");
            fprintf(fp,"%d\n", time_stamp_after - time_stamp_before);
            fclose(fp);

            athena->time.avg_vpn_dec_content_time = updateAvg(athena->time.avg_vpn_dec_content_time, athena->time.n_vpn_dec_content_time, time_stamp_after - time_stamp_before);
            athena->time.n_vpn_dec_content_time++;
            //printf("Avg. VPN Decryp. content computation time: %d\n\n", (int)athena->time.avg_vpn_dec_content_time);    
            break;
        default :
            athena->time.avg_content_time = updateAvg(athena->time.avg_content_time, athena->time.n_content_time, time_stamp_after - time_stamp_before);
            athena->time.n_content_time++;
            //printf("Avg. Regular content computation time: %d\n\n", (int)athena->time.avg_content_time);    
    }
*/
    return returnContent;
}

static CCNxMetaMessage *
_processManifest(Athena *athena, CCNxManifest *manifest, PARCBitVector *ingressVector) {
    //
    // *   (1) If it does not match anything in the PIT, drop it
    //
    const CCNxName *name = ccnxManifest_GetName(manifest);
    PARCBuffer *digest = _createMessageHash(manifest);

    AthenaPITValue *value = athenaPIT_Match(athena->athenaPIT, name, NULL, digest, ingressVector);
    PARCBitVector *egressVector = athenaPITValue_GetVector(value);
    if (egressVector) {
        if (parcBitVector_NumberOfBitsSet(egressVector) > 0) {
            //
            // *   (2) Add to the Content Store
            //
            athenaContentStore_PutContentObject(athena->athenaContentStore, manifest);
            // _athenaPIT_RemoveInterestFromMap

            //
            // *   (3) Reverse path forward it via PIT entries
            //
            const char *egressVectorString = parcBitVector_ToString(egressVector);
            parcLog_Debug(athena->log, "Manifest forwarded to %s.", egressVectorString);
            parcMemory_Deallocate(&egressVectorString);
            PARCBitVector *result = athenaTransportLinkAdapter_Send(athena->athenaTransportLinkAdapter, manifest,
                                                                    egressVector);
            if (result) {
                // if there are failed channels, client will resend interest unless we wish to retry here
                parcBitVector_Release(&result);
            }
        }
        athenaPITValue_Release(&value);
    }

    return NULL;
}

CCNxMetaMessage *
athena_ProcessMessage(Athena *athena, CCNxMetaMessage *ccnxMessage, PARCBitVector *ingressVector) {
    CCNxMetaMessage *outputMessage = NULL;
    if (ccnxMetaMessage_IsInterest(ccnxMessage)) {
        const CCNxName *ccnxName = ccnxInterest_GetName(ccnxMessage);
        if (ccnxName) {
            const char *name = ccnxName_ToString(ccnxName);
            //parcLog_Debug(athena->log, "Processing Interest Message: %s", name);
            parcMemory_Deallocate(&name);
        } else {
            //parcLog_Debug(athena->log, "Received Interest Message without a name.");
        }
        CCNxInterest *interest = ccnxMetaMessage_GetInterest(ccnxMessage);
        outputMessage = _processInterest(athena, interest, ingressVector);
        athena->stats.numProcessedInterests++;
    } else if (ccnxMetaMessage_IsContentObject(ccnxMessage)) {
        const CCNxName *ccnxName = ccnxContentObject_GetName(ccnxMessage);
        if (ccnxName) {
            const char *name = ccnxName_ToString(ccnxName);
            //parcLog_Debug(athena->log, "Processing Content Object Message: %s", name);
            parcMemory_Deallocate(&name);
        } else {
            //parcLog_Debug(athena->log, "Received Content Object Message without a name.");
        }
        CCNxContentObject *contentObject = ccnxMetaMessage_GetContentObject(ccnxMessage);
        outputMessage = _processContentObject(athena, contentObject, ingressVector);
        athena->stats.numProcessedContentObjects++;
    } else if (ccnxMetaMessage_IsControl(ccnxMessage)) {
        //parcLog_Debug(athena->log, "Processing Control Message");
        CCNxControl *control = ccnxMetaMessage_GetControl(ccnxMessage);
        outputMessage = _processControl(athena, control, ingressVector);
        athena->stats.numProcessedControlMessages++;
    } else if (ccnxMetaMessage_IsInterestReturn(ccnxMessage)) {
        //parcLog_Debug(athena->log, "Processing Interest Return Message");

        CCNxInterestReturn *interestReturn = ccnxMetaMessage_GetInterestReturn(ccnxMessage);
        outputMessage = _processInterestReturn(athena, interestReturn, ingressVector);
        athena->stats.numProcessedInterestReturns++;
    } else if (ccnxMetaMessage_IsManifest(ccnxMessage)) {
        //parcLog_Debug(athena->log, "Processing Interest Return Message");

        CCNxManifest *manifest = ccnxMetaMessage_GetManifest(ccnxMessage);
        outputMessage =  _processManifest(athena, manifest, ingressVector);
        athena->stats.numProcessedManifests++;
    } else {
        trapUnexpectedState("Invalid CCNxMetaMessage type");
    }

    return outputMessage;
}

void
athena_EncodeMessage(CCNxMetaMessage *message) {
    PARCSigner *signer = ccnxValidationCRC32C_CreateSigner();
    CCNxCodecNetworkBufferIoVec *iovec = ccnxCodecTlvPacket_DictionaryEncode(message, signer);
    bool result = ccnxWireFormatMessage_PutIoVec(message, iovec);
    assertTrue(result, "ccnxWireFormatMessage_PutIoVec failed");
    ccnxCodecNetworkBufferIoVec_Release(&iovec);
    parcSigner_Release(&signer);
}

void *
athena_ForwarderEngine(void *arg) {
    Athena *athena = (Athena *) arg;

    if (athena) {
        while (athena->athenaState == Athena_Running) {

            CCNxMetaMessage *ccnxMessage;
            PARCBitVector *ingressVector;
            int receiveTimeout = -1; // block until message received
            ccnxMessage = athenaTransportLinkAdapter_Receive(athena->athenaTransportLinkAdapter,
                                                             &ingressVector, receiveTimeout);
            if (ccnxMessage) {
                CCNxMetaMessage *result = athena_ProcessMessage(athena, ccnxMessage, ingressVector);
                if (result != NULL) {

                    if (ccnxMetaMessage_IsInterest(result)) {
                            const CCNxName *ccnxName = ccnxInterest_GetName(result);
                            if (ccnxName) {
                                const char *name = ccnxName_ToString(ccnxName);
                                char namePrefix[20];
                                memcpy(namePrefix,name,19);
                                namePrefix[19] = '\0';
                                if (!strcmp(namePrefix,"ccnx:/producer/kill")) {
                                    //printf("kill command interest\n");
                                    parcMemory_Deallocate(&name);
                                    ccnxMetaMessage_Release(&result);
                                    parcBitVector_Release(&ingressVector);
                                    break;
                                }
                                parcMemory_Deallocate(&name);
                            }
                    }

                    ccnxMetaMessage_Release(&result);
                }
                parcBitVector_Release(&ingressVector);
//                ccnxMetaMessage_Release(&ccnxMessage);
                // Checks for kill interest
                if (ccnxMetaMessage_IsInterest(ccnxMessage)) {
                        const CCNxName *ccnxName = ccnxInterest_GetName(ccnxMessage);
                        if (ccnxName) {
                            const char *name = ccnxName_ToString(ccnxName);
                            char namePrefix[20];
                            memcpy(namePrefix,name,19);
                            namePrefix[19] = '\0';
                            if (!strcmp(namePrefix,"ccnx:/producer/kill")) {
                                //printf("\nKill command interest\n");
                                parcMemory_Deallocate(&name);
                                break;
                            }
                            parcMemory_Deallocate(&name);
                        }
                }

            }
        }
//        FILE* fp = fopen("./times.csv","a");
//        printTimeCSV(athena,fp);
//        fclose(fp);
        usleep(1000); // workaround for coordinating with test infrastructure
        athena_Release(&athena);
    }
    return NULL;
}
