/*
 * Copyright (C) 2007 Apple Inc. All rights reserved.
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
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "Threading.h"

#include "Logging.h"
#include "Page.h"
#include <windows.h>

namespace WebCore {

typedef Vector<void(*)()> FunctionQueue;

static HWND threadingWindowHandle = 0;
static UINT threadingFiredMessage = 0;
const LPCWSTR kThreadingWindowClassName = L"ThreadingWindowClass";
static bool processingCustomThreadingMessage = false;

static Mutex& functionQueueMutex()
{
    static Mutex staticFunctionQueueMutex;
    return staticFunctionQueueMutex;
}

static FunctionQueue& functionQueue()
{
    static FunctionQueue staticFunctionQueue;
    return staticFunctionQueue;
}

static void callFunctionsOnMainThread()
{
    FunctionQueue queueCopy;
    {
        MutexLocker locker(functionQueueMutex());
        queueCopy.swap(functionQueue());
    }

    LOG(Threading, "Calling %u functions on the main thread", queueCopy.size());
    for (unsigned i = 0; i < queueCopy.size(); ++i)
        queueCopy[i]();
}

LRESULT CALLBACK ThreadingWindowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == threadingFiredMessage) {
        processingCustomThreadingMessage = true;
        callFunctionsOnMainThread();
        processingCustomThreadingMessage = false;
    } else
        return DefWindowProc(hWnd, message, wParam, lParam);
    return 0;
}

void initializeThreading()
{
    if (threadingWindowHandle)
        return;
    
    WNDCLASSEX wcex;
    memset(&wcex, 0, sizeof(WNDCLASSEX));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.lpfnWndProc    = ThreadingWindowWndProc;
    wcex.hInstance      = Page::instanceHandle();
    wcex.lpszClassName  = kThreadingWindowClassName;
    RegisterClassEx(&wcex);

    threadingWindowHandle = CreateWindow(kThreadingWindowClassName, 0, 0,
       CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, HWND_MESSAGE, 0, Page::instanceHandle(), 0);
    threadingFiredMessage = RegisterWindowMessage(L"com.apple.WebKit.MainThreadFired");
}
    
void callOnMainThread(void(*function)())
{
    ASSERT(function);
    ASSERT(threadingWindowHandle);

    if (processingCustomThreadingMessage)
        LOG(Threading, "callOnMainThread() called recursively.  Beware of nested PostMessage()s");

    {
        MutexLocker locker(functionQueueMutex());
        functionQueue().append(function);
    }

    PostMessage(threadingWindowHandle, threadingFiredMessage, 0, 0);
}

} // namespace WebCore