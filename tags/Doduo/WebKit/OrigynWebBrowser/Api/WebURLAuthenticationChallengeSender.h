/*
 * Copyright (C) 2008 Pleyo.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Pleyo nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PLEYO AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PLEYO OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebURLAuthenticationChallengeSender_h
#define WebURLAuthenticationChallengeSender_h


/**
 *  @file  WebURLAuthenticationChallengeSender.h
 *  WebURLAuthenticationChallengeSender description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"
#include <wtf/Forward.h>
#include <wtf/Forward.h>
#include <wtf/RefPtr.h>

namespace WebCore {
    class ResourceHandle;
}
class WebURLAuthenticationChallenge;
class WebURLCredential;

class WebURLAuthenticationChallengeSender
{
public:

    /**
     *  createInstance description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    static WebURLAuthenticationChallengeSender* createInstance(PassRefPtr<WebCore::ResourceHandle>);
private:

    /**
     *  WebURLAuthenticationChallengeSender description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    WebURLAuthenticationChallengeSender(PassRefPtr<WebCore::ResourceHandle>);
public:

    /**
     *  ~WebURLAuthenticationChallengeSender description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual ~WebURLAuthenticationChallengeSender();

    /**
     *  cancelAuthenticationChallenge description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void cancelAuthenticationChallenge(WebURLAuthenticationChallenge* challenge);

    /**
     *  continueWithoutCredentialForAuthenticationChallenge description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void continueWithoutCredentialForAuthenticationChallenge(WebURLAuthenticationChallenge* challenge);

    /**
     *  useCredential description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    virtual void useCredential(WebURLCredential* credential, WebURLAuthenticationChallenge* challenge);

    /**
     * resourceHandle description
     * @param[in]: description
     * @param[out]: description
     * @code
     * @endcode
     */
    WebCore::ResourceHandle* resourceHandle() const;

private:

    RefPtr<WebCore::ResourceHandle> m_handle;
};

#endif
