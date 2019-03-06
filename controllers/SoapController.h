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
#include <TCriteria>
#include "applicationcontroller.h"

typedef QMultiMap<QString, QVariant> SoapMap;

class T_CONTROLLER_EXPORT SoapController : public ApplicationController {
    Q_OBJECT
public:

    SoapController();

    QXmlStreamReader::Error soapRequest();
    void soapResponse(const QDomDocumentFragment &frag, const QVariantMap &qvm);
    void soapError(const QString &messageString);

    QVariantMap soapParameters();
    QVariant getSoapParameter(const QString &name) const;
    QPair<int, int> getPagination() const;

    QString getSoapMethod();
    QString errorMessage();
    QXmlStreamReader::Error error();

    void dumpMap(QVariant x);

protected:
    QDomDocument doc;

    QIODevice *qioDevice;
    QXmlStreamReader xmlReader;

    QXmlStreamReader::Error readDocument();
    QXmlStreamReader::Error readEnvelope();
    QXmlStreamReader::Error readMethod();
    QVariant readRequest();

    QString soapMethod;

    QVariantMap requestParameters; // TODO: optimize?
private:

};


#endif //APPNAME_SOAPCONTROLLER_H
