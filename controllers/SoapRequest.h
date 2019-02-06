//
// Created by jwc on 2/4/19.
//

#ifndef APPNAME_SOAPREQUEST_H
#define APPNAME_SOAPREQUEST_H


#include <QtCore/QIODevice>
#include <QtCore/QXmlStreamReader>

class SoapRequest {
public:

    SoapRequest();
    SoapRequest(QIODevice *qioDevice);

    void setDevice(QIODevice *qioDevice);
    QString getSoapMethod();
    bool processRequest();

private:
    QIODevice *qioDevice;
    QXmlStreamReader xmlReader;

    bool readDocument();
    bool readEnvelope();
    bool readBody();

    QString soapMethod;
    QString errorString;

    bool readObject();
};


#endif //APPNAME_SOAPREQUEST_H
