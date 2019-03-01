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
    void soapResponse(const QDomDocumentFragment &frag);

    QVariantMap soapParameters();
    QVariant getParameter(const QString &name) const;
    QPair<int, int> getPagination() const;
    QList<QPair<QString, Tf::SortOrder>> getSortOrder(const QMap<QString, QString> &propertyMap) const;

    QString getSoapMethod();
    QString errorMessage();
    QXmlStreamReader::Error error();

    static Tf::SortOrder getSortDirection(const QString &order);

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
