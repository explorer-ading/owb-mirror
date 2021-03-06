/*
 * Copyright (c) 2008, 2009, Google Inc. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "XBMImageDecoder.h"

#include "ASCIICType.h"

#include <algorithm>
#include <cstdio>

namespace WebCore {

XBMImageDecoder::XBMImageDecoder()
    : m_decodeOffset(0)
    , m_allDataReceived(false)
    , m_decodedHeader(false)
    , m_dataType(Unknown)
    , m_bitsDecoded(0)
{
}

void XBMImageDecoder::setData(SharedBuffer* data, bool allDataReceived)
{
    ImageDecoder::setData(data, allDataReceived);
    m_xbmString = data->buffer();
    m_xbmString.append('\0');

    m_allDataReceived = allDataReceived;
}

bool XBMImageDecoder::isSizeAvailable()
{
    if (!ImageDecoder::isSizeAvailable() && !failed())
        decode(true);

    return ImageDecoder::isSizeAvailable();
}

RGBA32Buffer* XBMImageDecoder::frameBufferAtIndex(size_t index)
{
    if (index)
        return 0;

    if (m_frameBufferCache.isEmpty())
        m_frameBufferCache.resize(1);

    // Attempt to get the size if we don't have it yet.
    if (!ImageDecoder::isSizeAvailable())
        decode(true);
    
    // Initialize the framebuffer if needed.
    RGBA32Buffer& buffer = m_frameBufferCache[0];
    if (!failed() && ImageDecoder::isSizeAvailable()
        && (buffer.status() == RGBA32Buffer::FrameEmpty)) {
        if (!buffer.setSize(size().width(), size().height())) {
            setFailed();
            return 0;
        }
        buffer.setStatus(RGBA32Buffer::FramePartial);

        // For XBMs, the frame always fills the entire image.
        buffer.setRect(IntRect(IntPoint(), size()));
    }
        
    // Keep trying to decode until we've got the entire image.
    if (buffer.status() == RGBA32Buffer::FramePartial)
        decode(false);

    return &buffer;
}

bool XBMImageDecoder::decodeHeader()
{
    ASSERT(m_decodeOffset <= m_xbmString.size());
    ASSERT(!m_decodedHeader);

    const char* input = m_xbmString.data();

    // At least 2 "#define <string> <unsigned>" sequences are required. These
    // specify the width and height of the image.
    int width, height;
    if (!ImageDecoder::isSizeAvailable()) {
        int count;
        if (sscanf(&input[m_decodeOffset], "#define %*s %i #define %*s %i%n",
                   &width, &height, &count) != 2)
            return false;

        // The width and height need to follow some rules.
        if (width < 0 || width > maxDimension || height < 0 || height > maxDimension) {
            // If this happens, decoding should not continue.
            setFailed();
            return false;
        }

        if (!setSize(width, height)) {
            setFailed();
            return false;
        }
        m_decodeOffset += count;
        ASSERT(m_decodeOffset <= m_xbmString.size());
    }

    ASSERT(ImageDecoder::isSizeAvailable());

    // Now we're looking for something that tells us that we've seen all of the
    // "#define <string> <unsigned>" sequences that we're going to. Mozilla
    // just looks for " char " or " short ". We'll do the same.
    if (m_dataType == Unknown) {
        const char* x11hint = " char ";
        const char* x11HintLocation = strstr(&input[m_decodeOffset], x11hint);
        if (x11HintLocation) {
            m_dataType = X11;
            m_decodeOffset += ((x11HintLocation - &input[m_decodeOffset]) + strlen(x11hint));
        } else {
            const char* x10hint = " short ";
            const char* x10HintLocation = strstr(&input[m_decodeOffset], x10hint);
            if (x10HintLocation) {
                m_dataType = X10;
                m_decodeOffset += ((x10HintLocation - &input[m_decodeOffset]) + strlen(x10hint));
            } else
                return false;
        }
        ASSERT(m_decodeOffset <= m_xbmString.size());
    }

    // Find the start of the data. Again, we do what mozilla does and just
    // look for a '{' in the input.
    const char* found = strchr(&input[m_decodeOffset], '{');
    if (!found)
        return false;

    // Advance to character after the '{'
    m_decodeOffset += ((found - &input[m_decodeOffset]) + 1);
    ASSERT(m_decodeOffset <= m_xbmString.size());
    m_decodedHeader = true;

    return true;
}

// The data in an XBM file is provided as an array of either "char" or "short"
// values. These values are decoded one at a time using strtoul() and the bits
// are used to set the alpha value for the image.
//
// The value for the color is always set to RGB(0,0,0), the alpha value takes
// care of whether or not the pixel shows up.
//
// Since the data may arrive in chunks, and many prefixes of valid numbers are
// themselves valid numbers, this code needs to check to make sure that the
// value is not truncated. This is done by consuming space after the value
// read until a ',' or a '}' occurs. In a valid XBM, one of these characters
// will occur after each value.
//
// The checks after strtoul are based on Mozilla's nsXBMDecoder.cpp.
bool XBMImageDecoder::decodeDatum(uint16_t* result)
{
    const char* input = m_xbmString.data();
    char* endPtr;
    const uint16_t value = strtoul(&input[m_decodeOffset], &endPtr, 0);

    // End of input or couldn't decode anything, can't go any further.
    if (endPtr == &input[m_decodeOffset] || !*endPtr)
        return false;

    // Possibly a hex value truncated at "0x". Need more data.
    if (value == 0 && (*endPtr == 'x' || *endPtr == 'X'))
        return false;

    // Skip whitespace
    while (*endPtr && isASCIISpace(*endPtr))
        ++endPtr;

    // Out of input, don't know what comes next.
    if (!*endPtr)
        return false;

    // If the next non-whitespace character is not one of these, it's an error.
    // Every valid entry in the data array needs to be followed by ',' or '}'.
    if (*endPtr != ',' && *endPtr != '}') {
        setFailed();
        return false;
    }

    // At this point we have a value.
    *result = value;

    // Skip over the decoded value plus the delimiter (',' or '}').
    m_decodeOffset += ((endPtr - &input[m_decodeOffset]) + 1);
    ASSERT(m_decodeOffset <= m_xbmString.size());

    return true;
}

bool XBMImageDecoder::decodeData()
{
    ASSERT(m_decodeOffset <= m_xbmString.size());
    ASSERT(m_decodedHeader && !m_frameBufferCache.isEmpty());

    RGBA32Buffer& frame = m_frameBufferCache[0];

    ASSERT(frame.status() == RGBA32Buffer::FramePartial);

    const int bitsPerRow = size().width();

    ASSERT(m_dataType != Unknown);

    while (m_bitsDecoded < (size().width() * size().height())) {
        uint16_t value;
        if (!decodeDatum(&value))
            return false;

        int x = m_bitsDecoded % bitsPerRow;
        const int y = m_bitsDecoded / bitsPerRow;

        // How many bits will be written?
        const int bits = std::min(bitsPerRow - x, (m_dataType == X11) ? 8 : 16);

        // Only the alpha channel matters here, so the color values are always
        // set to 0.
        for (int i = 0; i < bits; ++i)
            frame.setRGBA(x++, y, 0, 0, 0, value & (1 << i) ? 255 : 0);

        m_bitsDecoded += bits;
    }

    frame.setStatus(RGBA32Buffer::FrameComplete);

    return true;
}

// Decode as much as we can of the XBM file.
void XBMImageDecoder::decode(bool sizeOnly)
{
    if (failed())
        return;

    bool decodeResult = false;

    if (!m_decodedHeader)
        decodeResult = decodeHeader();

    if (m_decodedHeader && !sizeOnly)
        decodeResult = decodeData();

    // The header or the data could not be decoded, but there is no more
    // data: decoding has failed.
    if (!decodeResult && m_allDataReceived)
        setFailed();
}

} // namespace WebCore
