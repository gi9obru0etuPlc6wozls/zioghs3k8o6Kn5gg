//
// Created by jwc on 2/4/19.
//

#ifndef APPNAME_SOAPREQUEST_H
#define APPNAME_SOAPREQUEST_H


#include <QtCore/QIODevice>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QMap>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>

class SoapRequest {
public:

    SoapRequest();
    SoapRequest(QIODevice *qioDevice);

    void setDevice(QIODevice *qioDevice);
    QString getSoapMethod();
    bool processRequest();

    QVariantMap getItems();

private:
    QIODevice *qioDevice;
    QXmlStreamReader xmlReader;

    bool readDocument();
    bool readEnvelope();
    bool readBody();
    bool readObject();

    QString soapMethod;
    QString errorString;

    QVariantMap items;
};


#endif //APPNAME_SOAPREQUEST_H
