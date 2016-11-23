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

#include "../athena.c"

#include <LongBow/unit-test.h>

#include <errno.h>

#include <parc/algol/parc_SafeMemory.h>
#include <ccnx/common/ccnx_NameSegmentNumber.h>
#include <ccnx/common/internal/ccnx_InterestDefault.h>

#include <stdio.h>
#include <sodium.h>

LONGBOW_TEST_RUNNER(athena)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);

    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Static);

    LONGBOW_RUN_TEST_FIXTURE(Misc);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(athena)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(athena)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, athena_CreateRelease);
    LONGBOW_RUN_TEST_CASE(Global, athena_Create_KeyRelease);
    LONGBOW_RUN_TEST_CASE(Global, athena_ProcessInterest);
    LONGBOW_RUN_TEST_CASE(Global, athena_ProcessInterestEncapsulation);
//    LONGBOW_RUN_TEST_CASE(Global, athena_ProcessInterestDecapsulation);
//    LONGBOW_RUN_TEST_CASE(Global, athena_ProcessContentObject);
//    LONGBOW_RUN_TEST_CASE(Global, athena_ProcessControl);
//    LONGBOW_RUN_TEST_CASE(Global, athena_ProcessInterestReturn);
//    LONGBOW_RUN_TEST_CASE(Global, athena_ForwarderEngine);
//    LONGBOW_RUN_TEST_CASE(Global, athena_ProcessControl_CPI_REGISTER_PREFIX);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    __attribute__((unused)) int result = sodium_init();
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDOUT_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, athena_CreateRelease)
{
    CCNxName *testName = ccnxName_CreateFromCString("ccnx:/foo");
    Athena *athena = athena_Create(testName, 100);
    ccnxName_Release(&testName);
    athena_Release(&athena);
}

LONGBOW_TEST_CASE(Global, athena_Create_KeyRelease)
{
    unsigned char recipient_pk[crypto_box_PUBLICKEYBYTES];
    unsigned char recipient_sk[crypto_box_SECRETKEYBYTES];

    FILE* sk = fopen("/tmp/key.sec","r");
    fread(recipient_sk,sizeof(char),crypto_box_SECRETKEYBYTES,sk);
    fclose(sk);
    FILE* pk = fopen("/tmp/key.pub","r");
    fread(recipient_pk,sizeof(char),crypto_box_PUBLICKEYBYTES,pk);
    fclose(pk);
    PARCBuffer *secretKey = parcBuffer_WrapCString((char*)recipient_sk);
    PARCBuffer *publicKey = parcBuffer_WrapCString((char*)recipient_pk);

    CCNxName *testName = ccnxName_CreateFromCString("ccnx:/foo");
    Athena *athena = athena_CreateWithKeyPair(testName, 100, secretKey, publicKey);
    ccnxName_Release(&testName);

    parcBuffer_Release(&secretKey);
    parcBuffer_Release(&publicKey);

    athena_Release(&athena);
}

LONGBOW_TEST_CASE(Global, athena_ProcessInterest)
{
    CCNxName *testName = ccnxName_CreateFromCString("ccnx:/foo");
    Athena *athena = athena_Create(testName, 100);
    ccnxName_Release(&testName);

    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar/baz");
    CCNxInterest *interest = ccnxInterest_CreateSimple(name);

    uint64_t chunkNum = 0;
    CCNxNameSegment *chunkSegment = ccnxNameSegmentNumber_Create(CCNxNameLabelType_CHUNK, chunkNum);
    ccnxName_Append(name, chunkSegment);
    ccnxNameSegment_Release(&chunkSegment);

    PARCBuffer *payload = parcBuffer_WrapCString("this is a payload");
    CCNxContentObject *contentObject = ccnxContentObject_CreateWithNameAndPayload(name, payload);
    parcBuffer_Release(&payload);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t nowInMillis = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    ccnxContentObject_SetExpiryTime(contentObject, nowInMillis + 100000); // expire in 100 seconds

    PARCURI *connectionURI = parcURI_Parse("tcp://localhost:50100/listener/name=TCPListener");
    const char *result = athenaTransportLinkAdapter_Open(athena->athenaTransportLinkAdapter, connectionURI);
    assertTrue(result != NULL, "athenaTransportLinkAdapter_Open failed(%s)", strerror(errno));
    parcURI_Release(&connectionURI);

    connectionURI = parcURI_Parse("tcp://localhost:50100/name=TCP_0/local=false");
    result = athenaTransportLinkAdapter_Open(athena->athenaTransportLinkAdapter, connectionURI);
    assertTrue(result != NULL, "athenaTransportLinkAdapter_Open failed (%s)", strerror(errno));
    parcURI_Release(&connectionURI);

    connectionURI = parcURI_Parse("tcp://localhost:50100/name=TCP_1/local=false");
    result = athenaTransportLinkAdapter_Open(athena->athenaTransportLinkAdapter, connectionURI);
    assertTrue(result != NULL, "athenaTransportLinkAdapter_Open failed (%s)", strerror(errno));
    parcURI_Release(&connectionURI);

    int linkId = athenaTransportLinkAdapter_LinkNameToId(athena->athenaTransportLinkAdapter, "TCP_0");
    PARCBitVector *interestIngressVector = parcBitVector_Create();
    parcBitVector_Set(interestIngressVector, linkId);

    linkId = athenaTransportLinkAdapter_LinkNameToId(athena->athenaTransportLinkAdapter, "TCP_1");
    PARCBitVector *contentObjectIngressVector = parcBitVector_Create();
    parcBitVector_Set(contentObjectIngressVector, linkId);

    athena_EncodeMessage(interest);
    athena_EncodeMessage(contentObject);

    CCNxInterest *returnInterest = NULL;

    // Before FIB entry interest should not be forwarded
    returnInterest = athena_ProcessMessage(athena, interest, interestIngressVector);
    if (returnInterest != NULL) {
        ccnxInterest_Release(&returnInterest);
    }

    // Add route for interest, it should now be forwarded
    athenaFIB_AddRoute(athena->athenaFIB, name, contentObjectIngressVector);
    CCNxName *defaultName = ccnxName_CreateFromCString("lci:/");
    athenaFIB_AddRoute(athena->athenaFIB, defaultName, contentObjectIngressVector);
    ccnxName_Release(&defaultName);

    // Process exact interest match
    returnInterest = athena_ProcessMessage(athena, interest, interestIngressVector);
    if (returnInterest != NULL) {
        ccnxInterest_Release(&returnInterest);
    }

    // Process a super-interest match
    CCNxName *superName = ccnxName_CreateFromCString("lci:/foo/bar/baz/unmatched");
    CCNxInterest *superInterest = ccnxInterest_CreateSimple(superName);
    athena_EncodeMessage(superInterest);
    returnInterest = athena_ProcessMessage(athena, superInterest, interestIngressVector);
    if (returnInterest != NULL) {
        ccnxInterest_Release(&returnInterest);
    }
    ccnxName_Release(&superName);
    ccnxInterest_Release(&superInterest);

    // Process no-match/default route interest
    CCNxName *noMatchName = ccnxName_CreateFromCString("lci:/buggs/bunny");
    CCNxInterest *noMatchInterest = ccnxInterest_CreateSimple(noMatchName);
    athena_EncodeMessage(noMatchInterest);
    returnInterest = athena_ProcessMessage(athena, noMatchInterest, interestIngressVector);
    if (returnInterest != NULL) {
        ccnxInterest_Release(&returnInterest);
    }

    ccnxName_Release(&noMatchName);
    ccnxInterest_Release(&noMatchInterest);

    // Create a matching content object that the store should retain and reply to the following interest with
    CCNxContentObject *returnContent = athena_ProcessMessage(athena, contentObject, contentObjectIngressVector);
    if (returnContent != NULL) {
        ccnxContentObject_Release(&returnContent);
    }
    returnInterest = athena_ProcessMessage(athena, interest, interestIngressVector);
    if (returnInterest != NULL) {
        ccnxInterest_Release(&returnInterest);
    }

    parcBitVector_Release(&interestIngressVector);
    parcBitVector_Release(&contentObjectIngressVector);

    ccnxName_Release(&name);
    ccnxInterest_Release(&interest);
    ccnxContentObject_Release(&contentObject);
    athena_Release(&athena);
}

LONGBOW_TEST_CASE(Global, athena_ProcessInterestEncapsulation)
{
//    assertTrue(sodium_init()!=-1,"Crypto lib sodium not available");

    PARCURI *connectionURI;
    CCNxName *testName = ccnxName_CreateFromCString("ccnx:/athena");
    Athena *athena = athena_Create(testName, 100);
    ccnxName_Release(&testName);
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar/baz");
    CCNxName *otherName = ccnxName_CreateFromCString("lci:/foo/bar/other");
    CCNxInterest *interest = ccnxInterest_CreateSimple(name);
    CCNxInterest *otherInterest = ccnxInterest_CreateSimple(otherName);

    uint64_t chunkNum = 0;
    CCNxNameSegment *chunkSegment = ccnxNameSegmentNumber_Create(CCNxNameLabelType_CHUNK, chunkNum);
    ccnxName_Append(name, chunkSegment);
    ccnxNameSegment_Release(&chunkSegment);

    PARCBuffer *payload = parcBuffer_WrapCString("this is a payload");
    CCNxContentObject *contentObject = ccnxContentObject_CreateWithNameAndPayload(name, payload);
    parcBuffer_Release(&payload);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t nowInMillis = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    ccnxContentObject_SetExpiryTime(contentObject, nowInMillis + 100000); // expire in 100 seconds

    connectionURI = parcURI_Parse("tcp://localhost:50100/listener/name=TCPListener");
    const char *result = athenaTransportLinkAdapter_Open(athena->athenaTransportLinkAdapter, connectionURI);
    assertTrue(result != NULL, "athenaTransportLinkAdapter_Open failed(%s)", strerror(errno));
    parcURI_Release(&connectionURI);

    connectionURI = parcURI_Parse("tcp://localhost:50100/name=TCP_0/local=false");
    result = athenaTransportLinkAdapter_Open(athena->athenaTransportLinkAdapter, connectionURI);
    assertTrue(result != NULL, "athenaTransportLinkAdapter_Open failed (%s)", strerror(errno));
    parcURI_Release(&connectionURI);

    connectionURI = parcURI_Parse("tcp://localhost:50100/name=TCP_1/local=false");
    result = athenaTransportLinkAdapter_Open(athena->athenaTransportLinkAdapter, connectionURI);
    assertTrue(result != NULL, "athenaTransportLinkAdapter_Open failed (%s)", strerror(errno));
    parcURI_Release(&connectionURI);

    int linkId = athenaTransportLinkAdapter_LinkNameToId(athena->athenaTransportLinkAdapter, "TCP_0");
    PARCBitVector *interestIngressVector = parcBitVector_Create();
    parcBitVector_Set(interestIngressVector, linkId);

    linkId = athenaTransportLinkAdapter_LinkNameToId(athena->athenaTransportLinkAdapter, "TCP_1");
    PARCBitVector *contentObjectIngressVector = parcBitVector_Create();
    parcBitVector_Set(contentObjectIngressVector, linkId);

    athena_EncodeMessage(interest);
    athena_EncodeMessage(contentObject);

    CCNxInterest* returnInterest = NULL;

    // Before FIB entry interest should not be forwarded
    returnInterest = athena_ProcessMessage(athena, interest, interestIngressVector);
    if (returnInterest != NULL) {
        ccnxInterest_Release(&returnInterest);
    }

    // Creating public/secret keys for gateway2
    unsigned char recipient_pk[crypto_box_PUBLICKEYBYTES+1];
    unsigned char recipient_sk[crypto_box_SECRETKEYBYTES+1];
    unsigned char sym_key[crypto_aead_aes256gcm_KEYBYTES+crypto_aead_aes256gcm_NPUBBYTES+1];
    // Reading the key pair from /tmp
    // should run keygen before running this code.
    FILE* sk = fopen("/tmp/key.sec","r");
    assertNotNull(sk, "Could not open secret key file for reading");
    fread(recipient_sk,sizeof(char),crypto_box_SECRETKEYBYTES+1,sk);
    fclose(sk);
    FILE* pk = fopen("/tmp/key.pub","r");
    assertNotNull(pk, "Could not open public key file for reading");
    fread(recipient_pk,sizeof(char),crypto_box_PUBLICKEYBYTES+1,pk);
    fclose(pk);
    FILE* ek = fopen("/tmp/key.sym","r");
    assertNotNull(pk, "Could not open public key file for reading");
    fread(sym_key,sizeof(char),crypto_aead_aes256gcm_KEYBYTES+crypto_aead_aes256gcm_NPUBBYTES+1,ek);
    fclose(ek);

    PARCBuffer *secretKey = parcBuffer_WrapCString((char*)recipient_sk);
    PARCBuffer *publicKey = parcBuffer_WrapCString((char*)recipient_pk);
    PARCBuffer *symmetricKey = parcBuffer_WrapCString((char*)sym_key);

   
    // Creating the translation prefix
    CCNxName *gw2Name = ccnxName_CreateFromCString("lci:/domain/2");

    // Adding translation route so that the encryption data path is taken
//    athenaFIB_AddTranslationRoute(athena->athenaFIB, name, gw2Name, publicKey, contentObjectIngressVector);
    athenaFIB_AddTranslationRoute(athena->athenaFIB, otherName, gw2Name, symmetricKey, contentObjectIngressVector);
    CCNxName *defaultName = ccnxName_CreateFromCString("lci:/");
    athenaFIB_AddRoute(athena->athenaFIB, defaultName, contentObjectIngressVector);

    // Encapsulated interest with public key
    returnInterest = athena_ProcessMessage(athena, interest, interestIngressVector);
    if (returnInterest != NULL) {
        ccnxInterest_Release(&returnInterest);
    }
    // Encapsulated interest with shared secret key
    returnInterest = athena_ProcessMessage(athena, otherInterest, interestIngressVector);
    if (returnInterest != NULL) {
        ccnxInterest_Release(&returnInterest);
    }

////////SYMMETRIC DECRYPTION TEST STARTS HERE ///////////////////////////////////////////
/*
    CCNxName *encapName = ccnxInterest_GetName(returnInterest);
    
    PARCBuffer *symBuffer = NULL;
    PARCBuffer *interestPayload = ccnxInterest_GetPayload(returnInterest);
    size_t interestPayloadSize = parcBuffer_Remaining(interestPayload);
    PARCBuffer *decrypted = parcBuffer_Allocate(interestPayloadSize);

    if (0 != crypto_box_seal_open(
                             parcBuffer_Overlay(decrypted, 0),
                             parcBuffer_Overlay(interestPayload, 0),
                             interestPayloadSize,
                             recipient_pk,
                             recipient_sk)
                             )
    {
	    // message corrupted or not intended for this recipient
	    printf("Not decyphered\n");
        return;
    }
    // Suck in the key and then advance the buffer to point to the encapsulated interest
    symBuffer = parcBuffer_Allocate(crypto_aead_aes256gcm_KEYBYTES+crypto_aead_aes256gcm_NPUBBYTES);
    for (size_t i = 0; i < crypto_aead_aes256gcm_KEYBYTES+crypto_aead_aes256gcm_NPUBBYTES; i++) {
        parcBuffer_PutUint8(symBuffer, parcBuffer_GetUint8(decrypted));
    }

    parcBuffer_Flip(symBuffer);
    parcBuffer_Release(&decrypted);

    // This should trigger the symmetric decryption
    ccnxContentObject_Release(&contentObject);
    payload = parcBuffer_WrapCString("this is a payload");

    size_t contentSize = parcBuffer_Remaining(payload);

    PARCBuffer* symKeyBuffer = parcBuffer_Allocate(crypto_aead_aes256gcm_KEYBYTES);
    PARCBuffer* nonceBuffer = parcBuffer_Allocate(crypto_aead_aes256gcm_NPUBBYTES);

    for (size_t i = 0; i < crypto_aead_aes256gcm_KEYBYTES+crypto_aead_aes256gcm_NPUBBYTES; i++) {
        if (i<crypto_aead_aes256gcm_KEYBYTES){
            parcBuffer_PutUint8(symKeyBuffer, parcBuffer_GetUint8(symBuffer));
        }else{
            parcBuffer_PutUint8(nonceBuffer, parcBuffer_GetUint8(symBuffer));
        }
    }
    parcBuffer_Flip(symKeyBuffer);
    parcBuffer_Flip(nonceBuffer);

    PARCBuffer* ciphertext = parcBuffer_Allocate(contentSize + crypto_aead_aes256gcm_ABYTES);
    unsigned long long ciphertext_len;
    crypto_aead_aes256gcm_encrypt(parcBuffer_Overlay(ciphertext, 0), &ciphertext_len,
		                          parcBuffer_Overlay(payload, 0), contentSize,
		                          NULL, 0,
                                  NULL,
                                  parcBuffer_Overlay(nonceBuffer, 0), parcBuffer_Overlay(symKeyBuffer, 0));

    parcBuffer_Release(&symKeyBuffer);
    parcBuffer_Release(&nonceBuffer);

    parcBuffer_Release(&symBuffer);

    contentObject = ccnxContentObject_CreateWithNameAndPayload(encapName, ciphertext);
    parcBuffer_Release(&payload);
    parcBuffer_Release(&ciphertext);

    returnInterest = athena_ProcessMessage(athena, contentObject, contentObjectIngressVector);

    if (returnInterest!=NULL){
        ccnxInterest_Release(&returnInterest);
    }
/////////////SYMMETRIC DECRYPTION TEST ENDS HERE/////////////////////////////////////////////////////////////////
*/
    ccnxName_Release(&defaultName);
    ccnxName_Release(&gw2Name);
    parcBuffer_Release(&publicKey);
    parcBuffer_Release(&secretKey);
    parcBuffer_Release(&symmetricKey);

    parcBitVector_Release(&interestIngressVector);
    parcBitVector_Release(&contentObjectIngressVector);

    ccnxName_Release(&name);
    ccnxInterest_Release(&interest);
    ccnxName_Release(&otherName);
    ccnxInterest_Release(&otherInterest);


    ccnxInterest_Release(&contentObject);
    athena_Release(&athena);

}

LONGBOW_TEST_CASE(Global, athena_ProcessInterestDecapsulation)
{
//    assertTrue(sodium_init()!=-1,"Crypto lib sodium not available");

    unsigned char recipient_pk[crypto_box_PUBLICKEYBYTES];
    unsigned char recipient_sk[crypto_box_SECRETKEYBYTES];

    // Reading the key pair from /tmp
    // should run keygen before running this code.
    FILE* sk = fopen("/tmp/key.sec","r");
    assertNotNull(sk, "Could not open secret key file for reading");
    fread(recipient_sk,sizeof(char),crypto_box_SECRETKEYBYTES,sk);
    fclose(sk);
    FILE* pk = fopen("/tmp/key.pub","r");
    assertNotNull(pk, "Could not open public key file for reading");
    fread(recipient_pk,sizeof(char),crypto_box_PUBLICKEYBYTES,pk);
    fclose(pk);
    PARCBuffer *secretKey = parcBuffer_WrapCString((char*)recipient_sk);
    PARCBuffer *publicKey = parcBuffer_WrapCString((char*)recipient_pk);

    PARCURI *connectionURI;

    CCNxName *domainName = ccnxName_CreateFromCString("ccnx:/domain/2");
    Athena *athena = athena_CreateWithKeyPair(domainName, 100, secretKey, publicKey);

    CCNxName *name = ccnxName_CreateFromCString("ccnx:/foo");

    CCNxInterest *interest = ccnxInterest_CreateSimple(name);

    uint64_t chunkNum = 0;
    CCNxNameSegment *chunkSegment = ccnxNameSegmentNumber_Create(CCNxNameLabelType_CHUNK, chunkNum);
    ccnxName_Append(name, chunkSegment);
    ccnxNameSegment_Release(&chunkSegment);

    PARCBuffer *payload = parcBuffer_WrapCString("this is a payload");
    CCNxContentObject *contentObject = ccnxContentObject_CreateWithNameAndPayload(name, payload);
    parcBuffer_Release(&payload);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t nowInMillis = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    ccnxContentObject_SetExpiryTime(contentObject, nowInMillis + 100000); // expire in 100 seconds

    connectionURI = parcURI_Parse("tcp://localhost:50100/listener/name=TCPListener");
    const char *result = athenaTransportLinkAdapter_Open(athena->athenaTransportLinkAdapter, connectionURI);
    assertTrue(result != NULL, "athenaTransportLinkAdapter_Open failed(%s)", strerror(errno));
    parcURI_Release(&connectionURI);

    connectionURI = parcURI_Parse("tcp://localhost:50100/name=TCP_0/local=false");
    result = athenaTransportLinkAdapter_Open(athena->athenaTransportLinkAdapter, connectionURI);
    assertTrue(result != NULL, "athenaTransportLinkAdapter_Open failed (%s)", strerror(errno));
    parcURI_Release(&connectionURI);

    connectionURI = parcURI_Parse("tcp://localhost:50100/name=TCP_1/local=false");
    result = athenaTransportLinkAdapter_Open(athena->athenaTransportLinkAdapter, connectionURI);
    assertTrue(result != NULL, "athenaTransportLinkAdapter_Open failed (%s)", strerror(errno));
    parcURI_Release(&connectionURI);

    int linkId = athenaTransportLinkAdapter_LinkNameToId(athena->athenaTransportLinkAdapter, "TCP_0");
    PARCBitVector *interestIngressVector = parcBitVector_Create();
    parcBitVector_Set(interestIngressVector, linkId);

    linkId = athenaTransportLinkAdapter_LinkNameToId(athena->athenaTransportLinkAdapter, "TCP_1");
    PARCBitVector *contentObjectIngressVector = parcBitVector_Create();
    parcBitVector_Set(contentObjectIngressVector, linkId);

    athena_EncodeMessage(interest);
    athena_EncodeMessage(contentObject);

    CCNxInterest* returnInterest = NULL;

    // Before FIB entry interest should not be forwarded
    returnInterest = athena_ProcessMessage(athena, interest, interestIngressVector);
    if (returnInterest!=NULL){
        ccnxInterest_Release(&returnInterest);
    }


    // Add route for the decapsulated (original) interest
    athenaFIB_AddRoute(athena->athenaFIB, name, contentObjectIngressVector);
    if (returnInterest!=NULL){
        ccnxInterest_Release(&returnInterest);
    }

    // Creating encapsulated interest
    unsigned char symmetricKey[crypto_aead_aes256gcm_KEYBYTES+crypto_aead_aes256gcm_NPUBBYTES];
    randombytes_buf(symmetricKey, sizeof(symmetricKey));

    CCNxInterest *encryptedInterest = _encryptInterestPub(athena, interest, publicKey, domainName, symmetricKey);
    assertNotNull(encryptedInterest, "Failed to encapsulate the interest");

    // Process encapsulated interest. The result is the forwarding of the original decapsulated interest. Symmetric key is stored in the PIT.
    returnInterest = athena_ProcessMessage(athena, encryptedInterest, interestIngressVector);
    if (returnInterest!=NULL){
        ccnxInterest_Release(&returnInterest);
    }


    // Should encrypt and forward the content using the symmetric key
    returnInterest = athena_ProcessMessage(athena, contentObject, contentObjectIngressVector);
    if (returnInterest!=NULL){
        ccnxInterest_Release(&returnInterest);
    }

    ccnxInterest_Release(&encryptedInterest);
    ccnxInterest_Release(&interest);
    ccnxContentObject_Release(&contentObject);

    parcBitVector_Release(&interestIngressVector);
    parcBitVector_Release(&contentObjectIngressVector);

    ccnxName_Release(&domainName);
    ccnxName_Release(&name);

//    parcBuffer_Release(&targetPublicKey);
    parcBuffer_Release(&secretKey);
    parcBuffer_Release(&publicKey);

    athena_Release(&athena);
}

LONGBOW_TEST_CASE(Global, athena_ProcessContentObject)
{
    PARCURI *connectionURI;
    CCNxName *testName = ccnxName_CreateFromCString("ccnx:/foo");
    Athena *athena = athena_Create(testName, 100);
    ccnxName_Release(&testName);

    CCNxName *name = ccnxName_CreateFromCString("lci:/cakes/and/pies");
    uint64_t chunkNum = 0;
    CCNxNameSegment *chunkSegment = ccnxNameSegmentNumber_Create(CCNxNameLabelType_CHUNK, chunkNum);
    ccnxName_Append(name, chunkSegment);
    ccnxNameSegment_Release(&chunkSegment);

    PARCBuffer *payload = parcBuffer_WrapCString("this is a payload");
    CCNxContentObject *contentObject = ccnxContentObject_CreateWithNameAndPayload(name, payload);

    ccnxName_Release(&name);
    parcBuffer_Release(&payload);

    connectionURI = parcURI_Parse("tcp://localhost:50100/listener/name=TCPListener");
    const char *result = athenaTransportLinkAdapter_Open(athena->athenaTransportLinkAdapter, connectionURI);
    assertTrue(result != NULL, "athenaTransportLinkAdapter_Open failed (%s)", strerror(errno));
    parcURI_Release(&connectionURI);

    connectionURI = parcURI_Parse("tcp://localhost:50100/name=TCP_0");
    result = athenaTransportLinkAdapter_Open(athena->athenaTransportLinkAdapter, connectionURI);
    assertTrue(result != NULL, "athenaTransportLinkAdapter_Open failed (%s)", strerror(errno));
    parcURI_Release(&connectionURI);

    int linkId = athenaTransportLinkAdapter_LinkNameToId(athena->athenaTransportLinkAdapter, "TCP_0");
    PARCBitVector *ingressVector = parcBitVector_Create();
    parcBitVector_Set(ingressVector, linkId);

    athena_EncodeMessage(contentObject);

    athena_ProcessMessage(athena, contentObject, ingressVector);

    parcBitVector_Release(&ingressVector);

    // Make sure we recover processing a "nameless" content object
    payload = parcBuffer_WrapCString("Hello World");
    CCNxContentObject *reply = ccnxContentObject_CreateWithPayload(payload);
    parcBuffer_Release(&payload);

    CCNxMetaMessage *response = ccnxMetaMessage_CreateFromContentObject(reply);
    ccnxContentObject_Release(&reply);
    athena_EncodeMessage(response);

    athena_ProcessMessage(athena, response, ingressVector);

    assertNull(ingressVector, "Processing nameless content object didn't fail.");

    ccnxInterest_Release(&contentObject);
    ccnxInterest_Release(&response);
    athena_Release(&athena);
}



LONGBOW_TEST_CASE(Global, athena_ProcessControl)
{
    PARCURI *connectionURI;
    CCNxName *testName = ccnxName_CreateFromCString("ccnx:/foo");
    Athena *athena = athena_Create(testName, 100);
    ccnxName_Release(&testName);

    CCNxControl *control = ccnxControl_CreateFlushRequest();

    connectionURI = parcURI_Parse("tcp://localhost:50100/listener/name=TCPListener");
    const char *result = athenaTransportLinkAdapter_Open(athena->athenaTransportLinkAdapter, connectionURI);
    assertTrue(result != NULL, "athenaTransportLinkAdapter_Open failed (%s)", strerror(errno));
    parcURI_Release(&connectionURI);

    connectionURI = parcURI_Parse("tcp://localhost:50100/name=TCP_0");
    result = athenaTransportLinkAdapter_Open(athena->athenaTransportLinkAdapter, connectionURI);
    assertTrue(result != NULL, "athenaTransportLinkAdapter_Open failed (%s)", strerror(errno));
    parcURI_Release(&connectionURI);

    int linkId = athenaTransportLinkAdapter_LinkNameToId(athena->athenaTransportLinkAdapter, "TCP_0");
    PARCBitVector *ingressVector = parcBitVector_Create();
    parcBitVector_Set(ingressVector, linkId);

    athena_ProcessMessage(athena, control, ingressVector);

    parcBitVector_Release(&ingressVector);

    ccnxInterest_Release(&control);
    athena_Release(&athena);
}

LONGBOW_TEST_CASE(Global, athena_ProcessControl_CPI_REGISTER_PREFIX)
{
    PARCURI *connectionURI;
    CCNxName *testName = ccnxName_CreateFromCString("ccnx:/foo");
    Athena *athena = athena_Create(testName, 100);
    ccnxName_Release(&testName);

    CCNxName *name = ccnxName_CreateFromCString("ccnx:/foo/bar");
    CCNxControl *control = ccnxControl_CreateAddRouteToSelfRequest(name); // CPI_REGISTER_PREFIX
    CCNxMetaMessage *registerPrefixCommand = ccnxMetaMessage_CreateFromControl(control);
    ccnxControl_Release(&control);

    control = ccnxControl_CreateRemoveRouteToSelfRequest(name); // CPI_UNREGISTER_PREFIX
    CCNxMetaMessage *unregisterPrefixCommand = ccnxMetaMessage_CreateFromControl(control);
    ccnxControl_Release(&control);
    ccnxName_Release(&name);

    connectionURI = parcURI_Parse("tcp://localhost:50100/listener/name=TCPListener");
    const char *result = athenaTransportLinkAdapter_Open(athena->athenaTransportLinkAdapter, connectionURI);
    assertTrue(result != NULL, "athenaTransportLinkAdapter_Open failed (%s)", strerror(errno));
    parcURI_Release(&connectionURI);

    connectionURI = parcURI_Parse("tcp://localhost:50100/name=TCP_0");
    result = athenaTransportLinkAdapter_Open(athena->athenaTransportLinkAdapter, connectionURI);
    assertTrue(result != NULL, "athenaTransportLinkAdapter_Open failed (%s)", strerror(errno));
    parcURI_Release(&connectionURI);

    int linkId = athenaTransportLinkAdapter_LinkNameToId(athena->athenaTransportLinkAdapter, "TCP_0");
    PARCBitVector *ingressVector = parcBitVector_Create();
    parcBitVector_Set(ingressVector, linkId);

    // Call _Receive() once to prime the link. Messages are dropped until _Receive() is called once.
    PARCBitVector *linksRead = NULL;
    CCNxMetaMessage *msg = athenaTransportLinkAdapter_Receive(athena->athenaTransportLinkAdapter, &linksRead, -1);
    assertNull(msg, "Expected to NOT receive a message after the first call to _Receive()");

    CCNxMetaMessage *cpiMessages[2];
    cpiMessages[0] = registerPrefixCommand;    // CPI_REGISTER_PREFIX
    cpiMessages[1] = unregisterPrefixCommand;  // CPI_UNREGISTER_PREFIX

    for (int i = 0; i < 2; i++) {
        CCNxMetaMessage *cpiMessageToSend = cpiMessages[i];
        athena_ProcessMessage(athena, cpiMessageToSend, ingressVector);
        ccnxMetaMessage_Release(&cpiMessageToSend);

        CCNxMetaMessage *ack = athenaTransportLinkAdapter_Receive(athena->athenaTransportLinkAdapter, &linksRead, -1);
        assertNotNull(ack, "Expected a CPI_ACK message back");
        assertTrue(ccnxMetaMessage_IsControl(ack), "Expected a control message back");
        parcBitVector_Release(&linksRead);

        PARCJSON *json = ccnxControl_GetJson(ack);
        const PARCJSONValue *cpiAckResult = parcJSON_GetByPath(json, "CPI_ACK/REQUEST/RESULT");
        bool commandResult = parcJSONValue_GetBoolean(cpiAckResult);
        assertTrue(commandResult, "Expected the ACK to contain RESULT=true");

        ccnxMetaMessage_Release(&ack);
    }

    parcBitVector_Release(&ingressVector);
    athena_Release(&athena);
}


LONGBOW_TEST_CASE(Global, athena_ProcessInterestReturn)
{
    PARCURI *connectionURI;
    CCNxName *testName = ccnxName_CreateFromCString("ccnx:/foo");
    Athena *athena = athena_Create(testName, 100);
    ccnxName_Release(&testName);

    CCNxName *name = ccnxName_CreateFromCString("lci:/boose/roo/pie");

    CCNxInterest *interest =
        ccnxInterest_CreateWithImpl(&CCNxInterestFacadeV1_Implementation,
                                    name,
                                    CCNxInterestDefault_LifetimeMilliseconds,
                                    NULL,
                                    NULL,
                                    CCNxInterestDefault_HopLimit);
    ccnxName_Release(&name);
    CCNxInterestReturn *interestReturn = ccnxInterestReturn_Create(interest, CCNxInterestReturn_ReturnCode_Congestion);
    ccnxInterest_Release(&interest);

    connectionURI = parcURI_Parse("tcp://localhost:50100/listener/name=TCPListener");
    const char *result = athenaTransportLinkAdapter_Open(athena->athenaTransportLinkAdapter, connectionURI);
    assertTrue(result != NULL, "athenaTransportLinkAdapter_Open failed (%s)", strerror(errno));
    parcURI_Release(&connectionURI);

    connectionURI = parcURI_Parse("tcp://localhost:50100/name=TCP_0");
    result = athenaTransportLinkAdapter_Open(athena->athenaTransportLinkAdapter, connectionURI);
    assertTrue(result != NULL, "athenaTransportLinkAdapter_Open failed (%s)", strerror(errno));
    parcURI_Release(&connectionURI);

    int linkId = athenaTransportLinkAdapter_LinkNameToId(athena->athenaTransportLinkAdapter, "TCP_0");
    PARCBitVector *ingressVector = parcBitVector_Create();
    parcBitVector_Set(ingressVector, linkId);

    athena_EncodeMessage(interestReturn);

    athena_ProcessMessage(athena, interestReturn, ingressVector);

    parcBitVector_Release(&ingressVector);

    ccnxInterest_Release(&interestReturn);
    athena_Release(&athena);
}

LONGBOW_TEST_CASE(Global, athena_ForwarderEngine)
{
    // Create a new athena instance
    CCNxName *testName = ccnxName_CreateFromCString("ccnx:/foo");
    Athena *newAthena = athena_Create(testName, AthenaDefaultContentStoreSize);
    ccnxName_Release(&testName);
    assertNotNull(newAthena, "Could not create a new Athena instance");

    // Add a link
    PARCURI *connectionURI = parcURI_Parse("tcp://localhost:50100/listener");
    const char *result = athenaTransportLinkAdapter_Open(newAthena->athenaTransportLinkAdapter, connectionURI);
    assertTrue(result != NULL, "athenaTransportLinkAdapter_Open failed\n");
    parcURI_Release(&connectionURI);

    pthread_t thread;
    // Passing in a reference that will be released by the new thread as the thread may not
    // have time to acquire a reference itself before we release our reference.
    int ret = pthread_create(&thread, NULL, athena_ForwarderEngine, (void *) athena_Acquire(newAthena));
    assertTrue(ret == 0, "pthread_create failed");
    athena_Release(&newAthena);

    // Create a new local instance we can send a quit message from
    testName = ccnxName_CreateFromCString("ccnx:/foo");
    Athena *athena = athena_Create(testName, AthenaDefaultContentStoreSize);
    ccnxName_Release(&testName);
    assertNotNull(athena, "Could not create a new Athena instance");

    connectionURI = parcURI_Parse("tcp://localhost:50100/name=TCP_1");
    result = athenaTransportLinkAdapter_Open(athena->athenaTransportLinkAdapter, connectionURI);
    assertTrue(result != NULL, "athenaTransportLinkAdapter_Open failed (%s)", strerror(errno));
    parcURI_Release(&connectionURI);

    PARCBitVector *linkVector = parcBitVector_Create();

    int linkId = athenaTransportLinkAdapter_LinkNameToId(athena->athenaTransportLinkAdapter, "TCP_1");
    parcBitVector_Set(linkVector, linkId);

    CCNxName *name = ccnxName_CreateFromCString(CCNxNameAthenaCommand_Quit);
    CCNxMetaMessage *interest = ccnxInterest_CreateSimple(name);
    ccnxName_Release(&name);

    athena_EncodeMessage(interest);

    PARCBitVector
    *resultVector = athenaTransportLinkAdapter_Send(athena->athenaTransportLinkAdapter, interest, linkVector);
    assertNull(resultVector, "athenaTransportLinkAdapter_Send failed");
    ccnxMetaMessage_Release(&interest);
    parcBitVector_Release(&linkVector);

    CCNxMetaMessage
    *response = athenaTransportLinkAdapter_Receive(athena->athenaTransportLinkAdapter, &resultVector, -1);
    assertNotNull(resultVector, "athenaTransportLinkAdapter_Receive failed");
    assertTrue(parcBitVector_NumberOfBitsSet(resultVector) > 0, "athenaTransportLinkAdapter_Receive failed");
    parcBitVector_Release(&resultVector);
    ccnxMetaMessage_Release(&response);

    athenaTransportLinkAdapter_CloseByName(athena->athenaTransportLinkAdapter, "TCP_1");

    pthread_join(thread, NULL); // Wait for the child athena to actually finish

    athena_Release(&athena);
}

LONGBOW_TEST_FIXTURE(Static)
{
}

LONGBOW_TEST_FIXTURE_SETUP(Static)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Static)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDOUT_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

// Misc. tests

LONGBOW_TEST_FIXTURE(Misc)
{
}

LONGBOW_TEST_FIXTURE_SETUP(Misc)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Misc)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDOUT_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(athena);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
