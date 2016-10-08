/*
 * Copyright (c) 2015 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef QHTTPENGINE_QOBJECTHANDLER_H
#define QHTTPENGINE_QOBJECTHANDLER_H

#include <QVariantMap>

#include <QHttpEngine/QHttpHandler>
#include <QHttpEngine/QHttpSocket>

#include "qhttpengine_global.h"

class QHTTPENGINE_EXPORT QObjectHandlerPrivate;

/**
 * @brief Handler for invoking slots
 * @headerfile qobjecthandler.h QHttpEngine/QObjectHandler
 *
 * This handler enables incoming requests to be processed by slots in a
 * QObject-derived class or functor. Methods are registered by providing a
 * name and slot to invoke. The slot may take a pointer to the QHttpSocket for
 * the request as an argument. For requests that include a body, the content
 * is parsed as a JSON object and provided as a second parameter to the slot.
 * The slot is expected to return a QVariantMap containing the response, which
 * will be encoded as a JSON-document.
 *
 * To use this class, simply create an instance and call the appropriate
 * registerMethod() overload. For example:
 *
 * @code
 * class Object : public QObject
 * {
 *     Q_OBJECT
 * public slots:
 *     QVariantMap something(QHttpSocket *socket);
 * };
 *
 * QObjectHandler handler;
 * Object object;
 * // Old connection syntax
 * handler.registerMethod("something", &object, SLOT(something(QHttpSocket*)));
 * // New connection syntax
 * handler.registerMethod("something", &object, &Object::something);
 * @endcode
 *
 * It is also possible to use this class with a functor, eliminating the need
 * to create a class and slot:
 *
 * @code
 * QObjectHandler handler;
 * handler.registerMethod("something", [](QHttpSocket *socket) {
 *     return QVariantMap();
 * });
 * @endcode
 */
class QHTTPENGINE_EXPORT QObjectHandler : public QHttpHandler
{
    Q_OBJECT

public:

    /**
     * @brief Create a new QObject handler
     */
    explicit QObjectHandler(QObject *parent = 0);

    /**
     * @brief Register a method
     *
     * This overload uses the traditional connection syntax with macros.
     */
    void registerMethod(const QString &name, QObject *receiver, const char *method, int acceptedStatusCodes = QHttpSocket::GET);

    /**
     * @brief Register a method
     *
     * This overload uses the new connection syntax with member pointers.
     */
    template <typename Func1>
    inline void registerMethod(const QString &name,
                               typename QtPrivate::FunctionPointer<Func1>::Object *receiver,
                               Func1 slot, int acceptedStatusCodes = QHttpSocket::GET) {

        typedef QtPrivate::FunctionPointer<Func1> SlotType;

        // Ensure the slot doesn't have too many parameters
        Q_STATIC_ASSERT_X(int(SlotType::ArgumentCount) <= 2,
                          "The slot must have no more than two arguments.");

        // Ensure the parameters are of the correct type
        Q_STATIC_ASSERT_X((QtPrivate::CheckCompatibleArguments<QtPrivate::List<QHttpSocket*, QVariantMap>, typename SlotType::Arguments>::value),
                          "The slot parameters do not match");

        // Ensure the return value is correct
        Q_STATIC_ASSERT_X((QtPrivate::AreArgumentsCompatible<typename SlotType::ReturnType, QVariantMap>::value),
                          "Return type of the slot is not compatible with the return type of the signal.");

        // Invoke the implementation
        registerMethodImpl(name, receiver,
                           new QtPrivate::QSlotObject<Func1, typename SlotType::Arguments, void>(slot),
                           acceptedStatusCodes);
    }

    /**
     * @brief Register a method
     *
     * This overload uses the new functor syntax (without context).
     */
    template <typename Func1>
    inline void registerMethod(const QString &name, Func1 slot, int acceptedStatusCodes = QHttpSocket::GET) {
        registerMethod(name, Q_NULLPTR, slot, acceptedStatusCodes);
    }

    /**
     * @brief Register a method
     *
     * This overload uses the new functor syntax (with context).
     */
    template <typename Func1>
    inline typename QtPrivate::QEnableIf<!QtPrivate::FunctionPointer<Func1>::IsPointerToMemberFunction &&
                                         !QtPrivate::is_same<const char*, Func1>::value, void>::Type
            registerMethod(const QString &name, QObject *context, Func1 slot, int acceptedStatusCodes = QHttpSocket::GET) {

        // There is an easier way to do this but then the header wouldn't
        // compile on non-C++11 compilers
        return registerMethod_functor(name, context, slot, &Func1::operator(), acceptedStatusCodes);
    }

protected:

    /**
     * @brief Reimplementation of QHttpHandler::process()
     */
    virtual void process(QHttpSocket *socket, const QString &path);

private:

    template <typename Func1, typename Func1Operator>
    inline void registerMethod_functor(const QString &name, QObject *context, Func1 slot, Func1Operator, int acceptedStatusCodes) {

        typedef QtPrivate::FunctionPointer<Func1Operator> SlotType;

        // Ensure the slot doesn't have too many parameters
        Q_STATIC_ASSERT_X(int(SlotType::ArgumentCount) <= 2,
                          "The slot must have no more than two arguments.");

        // Ensure the parameters are of the correct type
        Q_STATIC_ASSERT_X((QtPrivate::CheckCompatibleArguments<QtPrivate::List<QHttpSocket*, QVariantMap>, typename SlotType::Arguments>::value),
                          "The slot parameters do not match");

        // Ensure the return value is correct
        Q_STATIC_ASSERT_X((QtPrivate::AreArgumentsCompatible<typename SlotType::ReturnType, QVariantMap>::value),
                          "Return type of the slot is not compatible with the return type of the signal.");

        registerMethodImpl(name, context,
                           new QtPrivate::QFunctorSlotObject<Func1, SlotType::ArgumentCount,
                                typename QtPrivate::List_Left<QtPrivate::List<QHttpSocket*, QVariantMap>, SlotType::ArgumentCount>::Value, void>(slot),
                           acceptedStatusCodes);
    }

    void registerMethodImpl(const QString &name, QObject *receiver, QtPrivate::QSlotObjectBase *slotObj, int acceptedStatusCodes);

    QObjectHandlerPrivate *const d;
    friend class QObjectHandlerPrivate;
};

#endif // QHTTPENGINE_QOBJECTHANDLER_H
