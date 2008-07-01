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
#ifndef HTTPParsers_h
#define HTTPParsers_h
/**
 *  @file  HTTPParsers.t
 *  HTTPParsers description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

namespace OWBAL {

    class String;

    /**
     * parse HTTP refresh 
     * @param[in] : refresh
     * @param[in] : from http equivalent meta data
     * @param[in] : delay
     * @param[in] : url
     * @param[out] : status
     * @code
     * bool s = parseHTTPRefresh(r, f, d, u);
     * @endcode
     */
    bool parseHTTPRefresh(const String& refresh, bool fromHttpEquivMeta, double& delay, String& url);

    /**
     * filename from HTTP Content disposition
     * @param[in] : http content
     * @param[out] : file name
     * @code
     * String f = filenameFromHTTPContentDisposition(c);
     * @endcode
     */
    String filenameFromHTTPContentDisposition(const String&); 

    /**
     * extract MIMEType from media type
     * @param[in] : media type
     * @param[out] : mime type
     * @code
     * String m = extractMIMETypeFromMediaType(t);
     * @endcode
     */
    String extractMIMETypeFromMediaType(const String&);

    /**
     * extract charset from media type
     * @param[in] : media type
     * @param[out] : charset
     * @code
     * String c = extractCharsetFromMediaType(t);
     * @endcode
     */
    String extractCharsetFromMediaType(const String&); 
}

#endif




