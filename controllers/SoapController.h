//
// Created by jwc on 2/4/19.
//

#ifndef APPNAME_SOAPCONTROLLER_H
#define APPNAME_SOAPCONTROLLER_H


#include <QtCore/QIODevice>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QMap>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>

typedef QMultiMap<QString, QVariant> XMLMap;

class SoapController {
public:

    SoapController();

    bool initRequest(THttpRequest *tHttpRequest);
    bool processRequest();

    QString getSoapMethod();
    QString getErrorMessage();
    QVariantMap getItems();

private:
    THttpRequest *tHttpRequest;
    QIODevice *qioDevice;
    QXmlStreamReader xmlReader;

    bool readDocument();
    bool readEnvelope();
    bool readMethod();
    QVariant readRequest();

    QString soapMethod;
    QString errorString;

    QVariantMap *items;

    QVariant dumpMap(QVariant x);
};


#endif //APPNAME_SOAPCONTROLLER_H
