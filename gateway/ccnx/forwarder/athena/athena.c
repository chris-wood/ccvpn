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
_athena_logger_create(void)
{

    PARCLogReporter *reporter = parcLogReporterTextStdout_Create();

    PARCLog *log = parcLog_Create("localhost", "athena", NULL, reporter);
    parcLogReporter_Release(&reporter);

    parcLog_SetLevel(log, PARCLogLevel_Info);
    return log;
}

static void
_removeLink(void *context, PARCBitVector *linkVector)
{
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
_athenaDestroy(Athena **athena)
{
    ccnxName_Release(&((*athena)->athenaName));
    athenaTransportLinkAdapter_Destroy(&((*athena)->athenaTransportLinkAdapter));
    athenaContentStore_Release(&((*athena)->athenaContentStore));
    athenaPIT_Release(&((*athena)->athenaPIT));
    athenaFIB_Release(&((*athena)->athenaFIB));
    parcLog_Release(&((*athena)->log));
    if ((*athena)->configurationLog) {
        parcOutputStream_Release(&((*athena)->configurationLog));
    }
}

parcObject_ExtendPARCObject(Athena, _athenaDestroy, NULL, NULL, NULL, NULL, NULL, NULL);

Athena *
athena_Create(size_t contentStoreSizeInMB)
{
	if(sodium_init()==-1){
		printf("Crypto lib sodium not available");
		exit(1);
	}

    Athena *athena = parcObject_CreateAndClearInstance(Athena);

    athena->athenaName = ccnxName_CreateFromCString(CCNxNameAthena_Forwarder);
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

    return athena;
}

parcObject_ImplementAcquire(athena, Athena);

parcObject_ImplementRelease(athena, Athena);

static void
_processInterestControl(Athena *athena, CCNxInterest *interest, PARCBitVector *ingressVector)
{
    //
    // Management messages
    //
    athenaInterestControl(athena, interest, ingressVector);
}

static void
_processControl(Athena *athena, CCNxControl *control, PARCBitVector *ingressVector)
{
    //
    // Management messages
    //
    athenaControl(athena, control, ingressVector);
}

static void
_processInterest(Athena *athena, CCNxInterest *interest, PARCBitVector *ingressVector)
{
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
        PARCBitVector *result = athenaTransportLinkAdapter_Send(athena->athenaTransportLinkAdapter, content, ingressVector);
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
    PARCBitVector *expectedReturnVector;
    AthenaPITResolution result;
    if ((result = athenaPIT_AddInterest(athena->athenaPIT, interest, ingressVector, NULL, &expectedReturnVector)) != AthenaPITResolution_Forward) {
        if (result == AthenaPITResolution_Error) {
            parcLog_Error(athena->log, "PIT resolution error");
        }
        return;
    }

    // Divert interests destined to the forwarder, we assume these are control messages
    CCNxName *ccnxName = ccnxInterest_GetName(interest);
    if (ccnxName && (ccnxName_StartsWith(ccnxName, athena->athenaName) == true)) {
        _processInterestControl(athena, interest, ingressVector);
        return;
    }

    //
    // *   (3) if it's in the FIB, forward, then update the PIT expectedReturnVector so we can verify
    //         when the returned object arrives that it came from an interface it was expected from.
    //         Interest messages with a hoplimit of 0 will never be sent out by the link adapter to a
    //         non-local interface so we need not check that here.
    //
    ccnxName = ccnxInterest_GetName(interest);
    AthenaFIBValue *vector = athenaFIB_Lookup(athena->athenaFIB, ccnxName, ingressVector);
    PARCBitVector *egressVector = NULL;
    if (vector != NULL) {
        egressVector = athenaFIBValue_GetVector(vector);
    }

    PARCBuffer *keyBuffer;
    char *interestByteStream;
    CCNxName *ccnx_newName;
    CCNxInterest *newInterest;

    if (egressVector != NULL) {
        // If no links are in the egress vector the FIB returned, return a no route interest message
        if (parcBitVector_NumberOfBitsSet(egressVector) == 0) {
            if (ccnxWireFormatMessage_ConvertInterestToInterestReturn(interest,
                                                                      CCNxInterestReturn_ReturnCode_NoRoute)) {
                // NOTE: The Interest has been modified in-place. It is now an InterestReturn.
                parcLog_Debug(athena->log, "Returning Interest as InterestReturn (code: NoRoute)");
                PARCBitVector *failedLinks = athenaTransportLinkAdapter_Send(athena->athenaTransportLinkAdapter,
                                                                             interest, ingressVector);
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
        } else {
            parcBitVector_SetVector(expectedReturnVector, egressVector);

            // Retrieving the recipient (Gateway 2) public key

            keyBuffer = athenaFIBValue_GetKey(vector);

            if (keyBuffer == NULL) {
                printf("Key buffer is NULL!\n");
            }

            if (1) {//(keyBuffer != NULL) {

                unsigned char recipient_pk[crypto_box_PUBLICKEYBYTES];
                unsigned char recipient_sk[crypto_box_SECRETKEYBYTES];

                crypto_box_keypair(recipient_pk, recipient_sk);

                //char* recipient_pk = parcBuffer_ToString(keyBuffer);
                printf("key pair generated\n");

                // TODO: Corvert the interest to wire format.
                interestByteStream = ccnxName_ToString(ccnxName);

                printf("Interest name: %s\n", interestByteStream);
                int interestLen = strlen(interestByteStream);
            
                if(!strncmp("ccnx:/foo/bar/baz", interestByteStream, strlen("ccnx:/foo/bar/baz\0"))) {  // Lookup table instead of hardcoded

                    //Generating Random Symmetric Key
                    if (crypto_aead_aes256gcm_is_available() == 0) {
                        printf("aead_aes256gcm not available!");
                        exit(1);
                    }

                    unsigned char symmetricKey[crypto_aead_aes256gcm_KEYBYTES];
                    int symmetricKeyLen = crypto_aead_aes256gcm_KEYBYTES;
                    randombytes_buf(symmetricKey, sizeof symmetricKey);

                    //Converting symmetric key to Hex format
                    int i;
                    char symmHexBuf[symmetricKeyLen * 2];
                    for (i=0; i < symmetricKeyLen; i++) {
                        sprintf(symmHexBuf + i, "%02X", symmetricKey[i]);
                    }
                    printf("Created symmetric key for AEAD!\n");
                    printf("Symmetric key:\n0x%s\n\n", symmHexBuf);	

                    //Creating a plaintext to be encrypted: "interestBytes|SymKeyBytes"
                    int plainTextLen = interestLen + symmetricKeyLen * 2 + 1;
                    unsigned char plainText[plainTextLen];
                    memcpy(plainText, interestByteStream, interestLen);
                    memcpy(plainText + interestLen, "/", 1);
                    memcpy(plainText+interestLen + 1, symmHexBuf, symmetricKeyLen * 2);
                    printf("Generated new plaintext!\n");
                    printf("Plaintext:\n%s\n\n", plainText);

                    // ciphertext buffer
                    int ciphertextLen = crypto_box_SEALBYTES + plainTextLen;
                    unsigned char ciphertext[ciphertextLen];
                    //Encrypting with gateway 2 public key.
                    crypto_box_seal(ciphertext, plainText, plainTextLen, recipient_pk);

                    //Creating the new interest, something like: ("ccnx:/domain/2/"|ciphertext);
                    char cipherHexBuf[ciphertextLen * 2];
                    for (i=0; i < ciphertextLen; i++) {
                        sprintf(cipherHexBuf + i, "%02X", ciphertext[i]);
                    }
                    printf("Generated ciphertext:\n%s\n\n", cipherHexBuf);

                    //Use athenaFIBValue_GetOutputPrefix(AthenaFIBValue *vector);
                    //to get the new name prefix
                    char namePrefix[] = "ccnx:/domain/2/";
                    int namePrefixLen = strlen(namePrefix);
                    printf("newName prefix:\n%s\n\n", namePrefix);

                    //Creating new interest name
                    int newNameLen = ciphertextLen * 2 + namePrefixLen;
                    char newName[newNameLen];
                    memcpy(newName, namePrefix, namePrefixLen);
                    memcpy(newName + namePrefixLen, cipherHexBuf, ciphertextLen * 2);
                    printf("New interest name:\n%s\n\n", newName);

                    //Creating the interest by name and adding it to PIT
                    printf("Creating the name from CString...\n");
                    ccnx_newName =  ccnxName_CreateFromCString(newName);
                    printf("Done!\n");

                    printf("Creating the new interest by the name...\n");
                    newInterest = ccnxInterest_CreateSimple(ccnx_newName);
                    printf("Done!\n");

                    printf("Adding interest to PIT...\n");
                    PARCBitVector *expectedReturnVector;
                    AthenaPITResolution result;
                    if ((result = athenaPIT_AddInterest(athena->athenaPIT, newInterest, ingressVector, NULL, &expectedReturnVector)) != AthenaPITResolution_Forward) {
                        if (result == AthenaPITResolution_Error) {
                            parcLog_Error(athena->log, "PIT resolution error");
                        }
                        return;
                    }
                    printf("Done!\n");

                    ccnxInterest_Release(&newInterest);
                    ccnxName_Release(&ccnx_newName);				

                    //TODO: Forward Interest

                    printf("Succeful execution\n\n\n");

				}
                parcMemory_Deallocate(&interestByteStream);
			}

            



            // XXX caw: if the vector's key is not null, encrypt the interest under that key
            // XXX caw: we also need to move the PIT insertion to *after* this step. (it's above at line 222 right now.)

            PARCBitVector *failedLinks =
                athenaTransportLinkAdapter_Send(athena->athenaTransportLinkAdapter, interest, egressVector);

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
}

static void
_processInterestReturn(Athena *athena, CCNxInterestReturn *interestReturn, PARCBitVector *ingressVector)
{
    // We can ignore interest return messages and allow the PIT entry to timeout, or
    //
    // Verify the return came from the next-hop where the interest was originally sent to
    // if not, ignore
    // otherwise, may try another forwarding path or clear the PIT state and forward the interest return on the reverse path
}

static PARCBuffer *
_createMessageHash(const CCNxMetaMessage *metaMessage)
{
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
_processContentObject(Athena *athena, CCNxContentObject *contentObject, PARCBitVector *ingressVector)
{
    //
    // *   (1) If it does not match anything in the PIT, drop it
    //
    const CCNxName *name = ccnxContentObject_GetName(contentObject);
    PARCBuffer *keyId = ccnxContentObject_GetKeyId(contentObject);
    PARCBuffer *digest = _createMessageHash(contentObject);

    PARCBitVector *egressVector = athenaPIT_Match(athena->athenaPIT, name, keyId, digest, ingressVector);
    if (egressVector) {
        if (parcBitVector_NumberOfBitsSet(egressVector) > 0) {
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
            PARCBitVector *result = athenaTransportLinkAdapter_Send(athena->athenaTransportLinkAdapter, contentObject, egressVector);
            if (result) {
                // if there are failed channels, client will resend interest unless we wish to retry here
                parcBitVector_Release(&result);
            }
        }
        parcBitVector_Release(&egressVector);
    }
}

static void
_processManifest(Athena *athena, CCNxManifest *manifest, PARCBitVector *ingressVector)
{
    //
    // *   (1) If it does not match anything in the PIT, drop it
    //
    const CCNxName *name = ccnxManifest_GetName(manifest);
    PARCBuffer *digest = _createMessageHash(manifest);

    PARCBitVector *egressVector = athenaPIT_Match(athena->athenaPIT, name, NULL, digest, ingressVector);
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
            PARCBitVector *result = athenaTransportLinkAdapter_Send(athena->athenaTransportLinkAdapter, manifest, egressVector);
            if (result) {
                // if there are failed channels, client will resend interest unless we wish to retry here
                parcBitVector_Release(&result);
            }
        }
        parcBitVector_Release(&egressVector);
    }
}

void
athena_ProcessMessage(Athena *athena, CCNxMetaMessage *ccnxMessage, PARCBitVector *ingressVector)
{
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
athena_EncodeMessage(CCNxMetaMessage *message)
{
    PARCSigner *signer = ccnxValidationCRC32C_CreateSigner();
    CCNxCodecNetworkBufferIoVec *iovec = ccnxCodecTlvPacket_DictionaryEncode(message, signer);
    bool result = ccnxWireFormatMessage_PutIoVec(message, iovec);
    assertTrue(result, "ccnxWireFormatMessage_PutIoVec failed");
    ccnxCodecNetworkBufferIoVec_Release(&iovec);
    parcSigner_Release(&signer);
}

void *
athena_ForwarderEngine(void *arg)
{
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
