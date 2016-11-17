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

static void
_processInterestControl(Athena *athena, CCNxInterest *interest, PARCBitVector *ingressVector) {
    //
    // Management messages
    //
    athenaInterestControl(athena, interest, ingressVector);
}

static void
_processControl(Athena *athena, CCNxControl *control, PARCBitVector *ingressVector) {
    //
    // Management messages
    //
    athenaControl(athena, control, ingressVector);
}

static CCNxInterest *
_encryptInterest(Athena *athena, CCNxInterest *interest, PARCBuffer *keyBuffer, CCNxName *prefix, unsigned char* symmetricKey)
{
    // Get the wire format
    PARCBuffer *interestWireFormat = athenaTransportLinkModule_CreateMessageBuffer(interest);
    size_t interestSize = parcBuffer_Remaining(interestWireFormat);

    // Generate a random symmetric that will be used when encrypting the response
//    unsigned char symmetricKey[crypto_aead_aes256gcm_KEYBYTES];
    int symmetricKeyLen = crypto_aead_aes256gcm_KEYBYTES;
//    randombytes_buf(symmetricKey, sizeof(symmetricKey));

    // Create the interest and key buffer that will be encrypted
    PARCBuffer *interestKeyBuffer = parcBuffer_Allocate(symmetricKeyLen + interestSize);
    parcBuffer_PutArray(interestKeyBuffer, symmetricKeyLen, symmetricKey);
    parcBuffer_PutBuffer(interestKeyBuffer, interestWireFormat);
    parcBuffer_Flip(interestKeyBuffer);
    size_t plaintextLength = parcBuffer_Remaining(interestKeyBuffer);

    // Encrypt the encapsulated interest
    PARCBuffer *encapsulatedInterest = parcBuffer_Allocate(plaintextLength + crypto_box_SEALBYTES);
    crypto_box_seal(parcBuffer_Overlay(encapsulatedInterest, 0), parcBuffer_Overlay(interestKeyBuffer, 0),
                    plaintextLength, parcBuffer_Overlay(keyBuffer, 0));

    // Create the new interest and add the ciphertext as the payload
    CCNxInterest *newInterest = ccnxInterest_CreateSimple(prefix);
    ccnxInterest_SetPayloadAndId(newInterest, encapsulatedInterest);
/*
    for (size_t i = 0; i < parcBuffer_Remaining(interestWireFormat); i++) {
        printf("%02x ", ((uint8_t *) parcBuffer_Overlay(interestWireFormat, 0))[i]);
    }
    printf("\n");
*/
    parcBuffer_Release(&interestWireFormat);
    parcBuffer_Release(&interestKeyBuffer);
    parcBuffer_Release(&encapsulatedInterest);

    return newInterest;
}



static void
_processInterest(Athena *athena, CCNxInterest *interest, PARCBitVector *ingressVector) {
    uint8_t hoplimit;

    //
    // *   (0) Hoplimit check, exclusively on interest messages
    //
    int linkId = parcBitVector_NextBitSet(ingressVector, 0);
    if (athenaTransportLinkAdapter_IsNotLocal(athena->athenaTransportLinkAdapter, linkId)) {
        hoplimit = ccnxInterest_GetHopLimit(interest);
        if (hoplimit == 0) {
            // We should never receive a message with a hoplimit of 0 from a non-local source.
            parcLog_Error(athena->log,
                          "Received a message with a hoplimit of zero from a non-local source (%s).",
                          athenaTransportLinkAdapter_LinkIdToName(athena->athenaTransportLinkAdapter, linkId));
            return;
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
        parcLog_Debug(athena->log, "Forwarding content from store to %s", ingressVectorString);
        parcMemory_Deallocate(&ingressVectorString);
        PARCBitVector *result = athenaTransportLinkAdapter_Send(athena->athenaTransportLinkAdapter, content,
                                                                ingressVector);
        if (result) { // failed channels - client will resend interest unless we wish to optimize things here
            parcBitVector_Release(&result);
        }
        return;
    }

    //
    // *   (2) add it to the PIT, if it was aggregated or there was an error we're done, otherwise we
    //         forward the interest.  The expectedReturnVector is populated with information we get from
    //         the FIB and used to verify content objects ingress ports when they arrive.
    //
    /*
    PARCBitVector *expectedReturnVector
    AthenaPITResolution result;
    if ((result = athenaPIT_AddInterest(athena->athenaPIT, interest, ingressVector, NULL, &expectedReturnVector)) !=
        AthenaPITResolution_Forward) {
        if (result == AthenaPITResolution_Error) {
            parcLog_Error(athena->log, "PIT resolution error");
        }
        return;
    }
    */

    CCNxInterest *newInterest = ccnxInterest_Acquire(interest);

    PARCBuffer *symKeyBuffer = NULL;
    CCNxName *ccnxName = ccnxInterest_GetName(interest);
    bool isPrefix = ccnxName_StartsWith(ccnxName, athena->publicName);
    bool hasPayload = ccnxInterest_GetPayload(interest) != NULL;
    if (isPrefix && hasPayload) {
        printf("Decapsulating...\n");
        PARCBuffer *interestPayload = ccnxInterest_GetPayload(interest);
        size_t interestPayloadSize = parcBuffer_Remaining(interestPayload);
        PARCBuffer *secretKey = athena->secretKey;
        PARCBuffer *publicKey = athena->publicKey;
        PARCBuffer *decrypted = parcBuffer_Allocate(interestPayloadSize);

        if (0 != crypto_box_seal_open(
                                 parcBuffer_Overlay(decrypted, 0),
                                 parcBuffer_Overlay(interestPayload, 0),
                                 interestPayloadSize,
                                 parcBuffer_Overlay(publicKey, 0),
                                 parcBuffer_Overlay(secretKey, 0))
                                 )
        {
		    /* message corrupted or not intended for this recipient */
		    printf("Not decyphered\n");
            return;
        }


        // Suck in the key and then advance the buffer to point to the encapsulated interest
        symKeyBuffer = parcBuffer_Allocate(crypto_aead_aes256gcm_KEYBYTES);
        for (size_t i = 0; i < crypto_aead_aes256gcm_KEYBYTES; i++) {
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
    ccnxName = ccnxInterest_GetName(newInterest);
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
                parcLog_Debug(athena->log, "Returning Interest as InterestReturn (code: NoRoute)");
                PARCBitVector *failedLinks = athenaTransportLinkAdapter_Send(athena->athenaTransportLinkAdapter,
                                                                             newInterest, ingressVector);
                if (failedLinks != NULL) {
                    parcBitVector_Release(&failedLinks);
                }
            } else {
                if (ccnxName) {
                    const char *name = ccnxName_ToString(ccnxName);
                    parcLog_Error(athena->log, "Unable to return Interest (%s) as InterestReturn (code: NoRoute).",
                                  name);
                    parcMemory_Deallocate(&name);
                } else {
                    parcLog_Error(athena->log, "Unable to return Interest () as InterestReturn (code: NoRoute).");
                }
            }
        } else {
            // Retrieving the recipient public key
            PARCBuffer *keyBuffer = athenaFIBValue_GetKey(vector);
            CCNxName *prefixBuffer = athenaFIBValue_GetOutputPrefix(vector);
            if (keyBuffer != NULL && prefixBuffer != NULL) {
                printf("Encapsulating...\n");
                assertTrue(keyBuffer != NULL && prefixBuffer != NULL, "Either the key or prefix was NULL.");

                unsigned char symmetricKey[crypto_aead_aes256gcm_KEYBYTES];
                int symmetricKeyLen = crypto_aead_aes256gcm_KEYBYTES;
                randombytes_buf(symmetricKey, sizeof(symmetricKey));
                
                printf("Original SymmKey: ");
                int i;
                for (i=0;i<symmetricKeyLen;i++){
                    printf("%c",symmetricKey[i]);
                }
                printf("\n");

                CCNxInterest *encryptedInterest = _encryptInterest(athena, newInterest, keyBuffer, prefixBuffer, symmetricKey);
                ccnxInterest_Release(&newInterest);
                newInterest = encryptedInterest;

                if (symKeyBuffer != NULL) {
                    parcBuffer_Release(&symKeyBuffer);
                }

                symKeyBuffer = parcBuffer_Allocate(crypto_aead_aes256gcm_KEYBYTES);
                parcBuffer_PutArray(symKeyBuffer, symmetricKeyLen, symmetricKey);
                parcBuffer_Flip(symKeyBuffer);
            }

            // debug
            char *interestString = ccnxInterest_ToString(newInterest);
            parcLog_Info(athena->log, "Sent: %s", interestString);
            parcMemory_Deallocate(&interestString);

            PARCBitVector *expectedReturnVector;
            AthenaPITResolution result;
            if ((result = athenaPIT_AddInterest(athena->athenaPIT, newInterest, ingressVector, NULL, symKeyBuffer,
                                                &expectedReturnVector)) != AthenaPITResolution_Forward) {
                if (result == AthenaPITResolution_Error) {
                    parcLog_Error(athena->log, "PIT resolution error");
                }
                return;
            }

            if (symKeyBuffer!=NULL) {
                char* test = parcBuffer_ToString(symKeyBuffer);
                printf("Stored SymmKey: %s\n",test);
                parcMemory_Deallocate(&test);
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

        if (ccnxWireFormatMessage_ConvertInterestToInterestReturn(interest,
                                                                  CCNxInterestReturn_ReturnCode_NoRoute)) {
            // NOTE: The Interest has been modified in-place. It is now an InterestReturn.
            parcLog_Debug(athena->log, "Returning Interest as InterestReturn (code: NoRoute)");
            PARCBitVector *failedLinks = athenaTransportLinkAdapter_Send(athena->athenaTransportLinkAdapter, interest,
                                                                         ingressVector);
            if (failedLinks != NULL) {
                parcBitVector_Release(&failedLinks);
            }
        } else {
            if (ccnxName) {
                const char *name = ccnxName_ToString(ccnxName);
                parcLog_Error(athena->log, "Unable to return Interest (%s) as InterestReturn (code: NoRoute).", name);
                parcMemory_Deallocate(&name);
            } else {
                parcLog_Error(athena->log, "Unable to return Interest () as InterestReturn (code: NoRoute).");
            }
        }

        if (athenaPIT_RemoveInterest(athena->athenaPIT, interest, ingressVector) != true) {
            if (ccnxName) {
                const char *name = ccnxName_ToString(ccnxName);
                parcLog_Error(athena->log, "Unable to remove interest (%s) from the PIT.", name);
                parcMemory_Deallocate(&name);
            } else {
                parcLog_Error(athena->log, "Unable to remove interest () from the PIT.");
            }
        }
        if (ccnxName) {
            const char *name = ccnxName_ToString(ccnxName);
            parcLog_Debug(athena->log, "Name (%s) not found in FIB and no default route. Message dropped.", name);
            parcMemory_Deallocate(&name);
        } else {
            parcLog_Debug(athena->log, "Name () not found in FIB and no default route. Message dropped.");
        }
    }

    if (symKeyBuffer != NULL) {
        parcBuffer_Release(&symKeyBuffer);
    }
    ccnxInterest_Release(&newInterest);
}
static void
_processInterestReturn(Athena *athena, CCNxInterestReturn *interestReturn, PARCBitVector *ingressVector) {
    // We can ignore interest return messages and allow the PIT entry to timeout, or
    //
    // Verify the return came from the next-hop where the interest was originally sent to
    // if not, ignore
    // otherwise, may try another forwarding path or clear the PIT state and forward the interest return on the reverse path
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

static void
_processContentObject(Athena *athena, CCNxContentObject *contentObject, PARCBitVector *ingressVector) {
    //
    // *   (1) If it does not match anything in the PIT, drop it
    //
    const CCNxName *name = ccnxContentObject_GetName(contentObject);
    PARCBuffer *keyId = ccnxContentObject_GetKeyId(contentObject);
    PARCBuffer *digest = _createMessageHash(contentObject);

    AthenaPITValue *value = athenaPIT_Match(athena->athenaPIT, name, keyId, digest, ingressVector);
    PARCBitVector *egressVector = athenaPITValue_GetVector(value);
    if (egressVector) {
        if (parcBitVector_NumberOfBitsSet(egressVector) > 0) {

            PARCBuffer *encryptKey = athenaPITValue_GetKey(value);
            if (encryptKey != NULL) {
                PARCBuffer *contentWireFormat = athenaTransportLinkModule_CreateMessageBuffer(contentObject);
                size_t contentSize = parcBuffer_Remaining(contentWireFormat);
                printf("content size = %zu\n", contentSize);

                // XXX
            }

            //
            // *   (2) Add to the Content Store
            //
            athenaContentStore_PutContentObject(athena->athenaContentStore, contentObject);

            //
            // *   (3) Reverse path forward it via PIT entries
            //
            const char *egressVectorString = parcBitVector_ToString(egressVector);
            parcLog_Debug(athena->log, "Content Object forwarded to %s.", egressVectorString);
            parcMemory_Deallocate(&egressVectorString);
            PARCBitVector *result = athenaTransportLinkAdapter_Send(athena->athenaTransportLinkAdapter, contentObject,
                                                                    egressVector);
            if (result) {
                // if there are failed channels, client will resend interest unless we wish to retry here
                parcBitVector_Release(&result);
            }
        }
        athenaPITValue_Release(&value);
    }
}

static void
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
}

void
athena_ProcessMessage(Athena *athena, CCNxMetaMessage *ccnxMessage, PARCBitVector *ingressVector) {
    if (ccnxMetaMessage_IsInterest(ccnxMessage)) {
        const CCNxName *ccnxName = ccnxInterest_GetName(ccnxMessage);
        if (ccnxName) {
            const char *name = ccnxName_ToString(ccnxName);
            parcLog_Debug(athena->log, "Processing Interest Message: %s", name);
            parcMemory_Deallocate(&name);
        } else {
            parcLog_Debug(athena->log, "Received Interest Message without a name.");
        }
        CCNxInterest *interest = ccnxMetaMessage_GetInterest(ccnxMessage);
        _processInterest(athena, interest, ingressVector);
        athena->stats.numProcessedInterests++;
    } else if (ccnxMetaMessage_IsContentObject(ccnxMessage)) {
        const CCNxName *ccnxName = ccnxContentObject_GetName(ccnxMessage);
        if (ccnxName) {
            const char *name = ccnxName_ToString(ccnxName);
            parcLog_Debug(athena->log, "Processing Content Object Message: %s", name);
            parcMemory_Deallocate(&name);
        } else {
            parcLog_Debug(athena->log, "Received Content Object Message without a name.");
        }
        CCNxContentObject *contentObject = ccnxMetaMessage_GetContentObject(ccnxMessage);
        _processContentObject(athena, contentObject, ingressVector);
        athena->stats.numProcessedContentObjects++;
    } else if (ccnxMetaMessage_IsControl(ccnxMessage)) {
        parcLog_Debug(athena->log, "Processing Control Message");

        CCNxControl *control = ccnxMetaMessage_GetControl(ccnxMessage);
        _processControl(athena, control, ingressVector);
        athena->stats.numProcessedControlMessages++;
    } else if (ccnxMetaMessage_IsInterestReturn(ccnxMessage)) {
        parcLog_Debug(athena->log, "Processing Interest Return Message");

        CCNxInterestReturn *interestReturn = ccnxMetaMessage_GetInterestReturn(ccnxMessage);
        _processInterestReturn(athena, interestReturn, ingressVector);
        athena->stats.numProcessedInterestReturns++;
    } else if (ccnxMetaMessage_IsManifest(ccnxMessage)) {
        parcLog_Debug(athena->log, "Processing Interest Return Message");

        CCNxManifest *manifest = ccnxMetaMessage_GetManifest(ccnxMessage);
        _processManifest(athena, manifest, ingressVector);
        athena->stats.numProcessedManifests++;
    } else {
        trapUnexpectedState("Invalid CCNxMetaMessage type");
    }
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
                athena_ProcessMessage(athena, ccnxMessage, ingressVector);

                parcBitVector_Release(&ingressVector);
                ccnxMetaMessage_Release(&ccnxMessage);
            }
        }
        usleep(1000); // workaround for coordinating with test infrastructure
        athena_Release(&athena);
    }
    return NULL;
}
