/*
 * Copyright (c) 2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @author Nacho Solis, Christopher A. Wood, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#ifndef ccnxVPN_Stats_h
#define ccnxVPN_Stats_h

/**
 * Structure to collect and display the performance statistics.
 */
struct vpn_stats;
typedef struct vpn_stats CCNxVPNStats;

/**
 * Create an empty `CCNxVPNStats` instance.
 *
 * The returned result must be freed via {@link ccnxVPNStats_Release}
 *
 * @return A newly allocated `CCNxVPNStats`.
 *
 * Example
 * @code
 * {
 *     CCNxVPNStats *stats = ccnxVPNStats_Create();
 * }
 * @endcode
 */
CCNxVPNStats *ccnxVPNStats_Create(void);

/**
 * Increase the number of references to a `CCNxVPNStats`.
 *
 * Note that new `CCNxVPNStats` is not created,
 * only that the given `CCNxVPNStats` reference count is incremented.
 * Discard the reference by invoking `ccnxVPNStats_Release`.
 *
 * @param [in] clock A pointer to a `CCNxVPNStats` instance.
 *
 * @return The input `CCNxVPNStats` pointer.
 *
 * Example:
 * @code
 * {
 *     CCNxVPNStats *stats = ccnxVPNStats_Create();
 *     CCNxVPNStats *copy = ccnxVPNStats_Acquire(stats);
 *     ccnxVPNStats_Release(&stats);
 *     ccnxVPNStats_Release(&copy);
 * }
 * @endcode
 */
CCNxVPNStats *ccnxVPNStats_Acquire(const CCNxVPNStats *stats);

/**
 * Release a previously acquired reference to the specified instance,
 * decrementing the reference count for the instance.
 *
 * The pointer to the instance is set to NULL as a side-effect of this function.
 *
 * If the invocation causes the last reference to the instance to be released,
 * the instance is deallocated and the instance's implementation will perform
 * additional cleanup and release other privately held references.
 *
 * @param [in,out] clockPtr A pointer to a pointer to the instance to release.
 *
 * Example:
 * @code
 * {
 *     CCNxVPNStats *stats = ccnxVPNStats_Create();
 *     CCNxVPNStats *copy = ccnxVPNStats_Acquire(stats);
 *     ccnxVPNStats_Release(&stats);
 *     ccnxVPNStats_Release(&copy);
 * }
 * @endcode
 */
void ccnxVPNStats_Release(CCNxVPNStats **statsPtr);

/**
 * Record the name and time for a request (e.g., interest).
 *
 * @param [in] stats The `CCNxVPNStats` instance.
 * @param [in] name The `CCNxName` name structure.
 * @param [in] timeInUs The send time (in microseconds).
 */
void ccnxVPNStats_RecordRequest(CCNxVPNStats *stats, CCNxName *name, uint64_t timeInUs);

/**
 * Record the name and time for a response (e.g., content object).
 *
 * @param [in] stats The `CCNxVPNStats` instance.
 * @param [in] name The `CCNxName` name structure.
 * @param [in] timeInUs The send time (in microseconds).
 * @param [in] message The response `CCNxMetaMessage`.
 *
 * @return The delta between the request and response (in microseconds).
 */
size_t ccnxVPNStats_RecordResponse(CCNxVPNStats *stats, CCNxName *name, uint64_t timeInUs, CCNxMetaMessage *message);

/**
 * Display the average statistics stored in this `CCNxVPNStats` instance.
 *
 * @param [in] stats The `CCNxVPNStats` instance from which to draw the average data.
 *
 * @retval true If the stats were displayed correctly
 * @retval false Otherwise
 */
bool ccnxVPNStats_Display(CCNxVPNStats *stats);
#endif // ccnxVPN_Stats_h
