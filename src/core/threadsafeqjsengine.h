/*

https://github.com/nst1911/thread-safe-qjsengine

MIT License

Copyright (c) 2022 Nikita Stelmashenko

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef THREADSAFEQJSENGINE_H
#define THREADSAFEQJSENGINE_H

#include <QJSEngine>
#include <QThread>

namespace ThreadSafeQJSEngine {

template <typename F>
auto call(QJSEngine* engine, F&& f) -> typename std::enable_if<!std::is_void<decltype(f())>::value, decltype(f())>::type
{
    if (QThread::currentThread() != engine->thread())
    {
        decltype(f()) result;
        QMetaObject::invokeMethod(engine, std::forward<F>(f), Qt::BlockingQueuedConnection, &result);
        return result;
    }
    else
    {
        return std::forward<F>(f)();
    }
}

template <typename F>
auto call(QJSEngine* engine, F&& f) -> typename std::enable_if<std::is_void<decltype(f())>::value, void>::type
{
    if (QThread::currentThread() != engine->thread())
    {
        QMetaObject::invokeMethod(engine, std::forward<F>(f), Qt::BlockingQueuedConnection);
    }
    else
    {
        std::forward<F>(f)();
    }
}

};

#endif // THREADSAFEQJSENGINE_H
