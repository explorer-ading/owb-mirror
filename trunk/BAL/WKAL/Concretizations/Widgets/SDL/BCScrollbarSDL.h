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


#ifndef PlatformScrollBar_h
#define PlatformScrollBar_h

#include "Scrollbar.h"
#include "BALBase.h"
#include "Timer.h"
#include <wtf/PassRefPtr.h>

namespace WKAL {

class ScrollbarSDL : public Scrollbar {
public:
    static PassRefPtr<ScrollbarSDL> create(ScrollbarClient* client, ScrollbarOrientation orientation, ScrollbarControlSize size)
    {
        return adoptRef(new ScrollbarSDL(client, orientation, size));
    }
    virtual ~ScrollbarSDL();

    virtual void setFrameRect(const IntRect&);

    virtual bool handleMouseMoveEvent(const PlatformMouseEvent&);
    virtual bool handleMouseOutEvent(const PlatformMouseEvent&);
    virtual bool handleMousePressEvent(const PlatformMouseEvent&);
    virtual bool handleMouseReleaseEvent(const PlatformMouseEvent&);

    virtual void setEnabled(bool);

    // Required by ScrollbarThemeSDL.
    ScrollbarPart hitTest(const PlatformMouseEvent&);
protected:
    ScrollbarSDL(ScrollbarClient*, ScrollbarOrientation, ScrollbarControlSize);
    virtual void updateThumbPosition();
    virtual void updateThumbProportion();
    virtual void frameRectsChanged();

private:
    IntRect trackRect() const;
    bool hasButtons() const;
    IntRect forwardButtonRect() const;
    IntRect backButtonRect() const;
    bool hasThumb() const;
    void splitTrack(const IntRect& trackRect, IntRect& beforeThumbRect, IntRect& thumbRect, IntRect& afterThumbRect) const;
    int thumbLength() const;
    int thumbPosition() const;
    int trackLength() const;
    void invalidatePart(ScrollbarPart);
    bool thumbUnderMouse();
    IntRect thumbRect() const;
    static void balValueChanged(BalAdjustment*, ScrollbarSDL*);
    
    BalAdjustment* m_adjustment;
};

}

#endif // PlatformScrollBar_h