//
// Created by jwc on 2/4/19.
//

#include <iostream>
#include <THttpRequest>
#include <QtCore/QUuid>
#include "SoapController.h"

SoapController::SoapController() {}

QString SoapController::getSoapMethod() {
    std::cerr <<  "SoapController::getSoapMethod: " << std::endl;
    return soapMethod;
}

QString SoapController::errorMessage() {
    std::cerr <<  "SoapController::errorMessage: " << std::endl;
    return xmlReader.errorString();
}

QXmlStreamReader::Error SoapController::error() {
    return xmlReader.error();
}

QVariantMap SoapController::soapParameters() {
    return requestParameters;
}

QVariant SoapController::getSoapParameter(const QString &name) const {
    QVariant vm = requestParameters.value(name);
    return vm;
}

QXmlStreamReader::Error SoapController::soapRequest() {
    tDebug("SoapController::soapRequest");

    if (httpRequest().method() != Tf::Post || httpRequest().header().contentType() != "text/xml") {
        tDebug("Method/contentType error");
        xmlReader.raiseError(QObject::tr("Method/contentType error."));
        return xmlReader.error();
    }
    qioDevice = httpRequest().rawBody();

    if (!qioDevice->isOpen()) {
        if (!qioDevice->open(QIODevice::ReadOnly)) {
            xmlReader.raiseError(QObject::tr("Unable to open device."));
            return xmlReader.error();
        }
    }

    xmlReader.setDevice(qioDevice);
    return readDocument();
}

void SoapController::soapResponse(const QDomDocumentFragment &frag, const QVariantMap &map) {
    setContentType("text/xml");
    setStatusCode(200);

    QVariantMap statusMap{
            {"success", "true"},
            {"error", ""}
    };

    QDomElement envelope = doc.createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Envelope");
    envelope.setPrefix("soap");
    QDomElement body = doc.createElement("soap:Body");

    for (QVariantMap::const_iterator iter = map.begin(); iter != map.end(); ++iter) {
        qDebug() << iter.key() << iter.value();
        QDomElement e = doc.createElement(iter.key());
        e.appendChild(doc.createTextNode(iter.value().toString()));
        body.appendChild(e);
    }

    for (QVariantMap::const_iterator iter = statusMap.begin(); iter != statusMap.end(); ++iter) {
        qDebug() << iter.key() << iter.value();
        QDomElement e = doc.createElement(iter.key());
        e.appendChild(doc.createTextNode(iter.value().toString()));
        body.appendChild(e);
    }

    QDomElement response = doc.createElementNS("http://localhost/Blog", "blog:GetBlogResponse");
    body.appendChild(response);
    envelope.appendChild(body);
    doc.appendChild(envelope);

    response.appendChild(frag);

    //std::cerr <<  "doc: " <<  doc.toString(4).toStdString()  << std::endl;

    renderXml(doc);
}

QXmlStreamReader::Error SoapController::readDocument() {
    tDebug("SoapController::readDocument");

    if (xmlReader.readNextStartElement()) {
        std::cerr << "readNextStartElement: " << xmlReader.name().toString().toStdString() << std::endl;
        if (xmlReader.name() == "Envelope") {
            return readEnvelope();
        }
        else {
            tDebug("SOAP Envelope not found.");
            xmlReader.raiseError(QObject::tr("SOAP Envelope not found."));
        }
    }
    return xmlReader.error();
}

QXmlStreamReader::Error SoapController::readEnvelope() {
    tDebug("SoapController::readEnvelope");

    while (xmlReader.readNextStartElement()) {
        std::cerr << "readNextStartElement: " << xmlReader.name().toString().toStdString() << std::endl;
        if (xmlReader.name() == "Header") {
            xmlReader.skipCurrentElement();
        }
        else if (xmlReader.name() == "Body") {
            return readMethod();
        }
        else {
            std::cerr << "Unexpected element in SOAP Envelope." << std::endl;
            tDebug("Unexpected element in SOAP Envelope.");
            xmlReader.raiseError(QObject::tr("Unexpected element in SOAP Envelope."));
        }

    }
    return xmlReader.error();
}

QXmlStreamReader::Error SoapController::readMethod() {
    tDebug("SoapController::readMethod");

    if (xmlReader.readNextStartElement()) {
        std::cerr << "readNextStartElement: " << xmlReader.name().toString().toStdString() << std::endl;
        soapMethod = xmlReader.name().toString();
    }

    if (xmlReader.error()) return xmlReader.error();

    tDebug("Token: %s Name: %s Text: %s ",
           xmlReader.tokenString().toStdString().c_str(),
           xmlReader.name().toString().toStdString().c_str(),
           xmlReader.text().toString().toStdString().c_str()
    );

    requestParameters = readRequest().toMap(); // TODO: optimize?

    return xmlReader.error();
}

QVariant SoapController::readRequest() {
    tDebug("=== SoapController::readRequest:");

    bool inElement = false;
    QString text;
    SoapMap xmlMap;

    while (!xmlReader.atEnd() && xmlReader.error() == QXmlStreamReader::NoError) {
        const auto token = xmlReader.readNext();
        const QString name = xmlReader.name().toString();

        switch (token) {
            case QXmlStreamReader::TokenType::Characters:
                text = xmlReader.text().toString();
                tDebug("+QXmlStreamReader::TokenType::Characters Text: %s", text.trimmed().toStdString().c_str());
                break;

            case QXmlStreamReader::TokenType::StartElement:
                tDebug("+QXmlStreamReader::TokenType::StartElement: %s", name.toStdString().c_str());
                inElement = true;
                xmlMap.insert(name, readRequest());
                break;

            case QXmlStreamReader::TokenType::EndElement:
                tDebug("+QXmlStreamReader::TokenType::EndElement: %s %d", name.toStdString().c_str(), inElement);

                if (inElement) {
                    return xmlMap;
                }

                return text;
            default:
                tDebug("SOAP XML Error");
                xmlReader.raiseError(QObject::tr("SOAP XML Error."));
                return {};
        }
    }

    tDebug("Unexpected end of SOAP XML");
    xmlReader.raiseError(QObject::tr("Unexpected end of SOAP XML."));
    return {};
}

QPair<int, int> SoapController::getPagination() const {
    QVariant pageMap = getSoapParameter("pagination");

    int offset = 0;
    int limit = 0;

    if (pageMap.canConvert(QMetaType::QVariantMap)) {
        tDebug("QMetaType::QVariantMap");

        SoapMap xmlMap = pageMap.toMap();


        offset = xmlMap.value("start").toInt();
        limit = xmlMap.value("limit").toInt();
        tDebug("offset: %d ", offset);
        tDebug("limit: %d ", limit);

    }

    QPair<int,int> page{limit,offset};
    return page;
}

void SoapController::dumpMap(QVariant qVariant) {
    tDebug("--- SoapController::dumpMap: %s", qVariant.typeName());

    if (qVariant.canConvert(QMetaType::QVariantMap)) {
//        tDebug("QMetaType::QVariantMap");

        SoapMap xmlMap = qVariant.toMap();

        for (QMap<QString, QVariant>::iterator xmlMap_it = xmlMap.begin(); xmlMap_it != xmlMap.end(); ++xmlMap_it) {
            tDebug("Key: %s ", xmlMap_it.key().toStdString().c_str());

            dumpMap(xmlMap_it.value());
        }
    } else if (qVariant.canConvert(QMetaType::QString)) {
//        tDebug("QMetaType::QString");
        tDebug("value: %s", qVariant.toString().toStdString().c_str());
    } else {
        tDebug("Error in QMetaType: %s", qVariant.typeName());
        tDebug("value: %s", qVariant.toString().toStdString().c_str());
    }
}

void SoapController::soapError(const QString &messageString) {
    setContentType("text/xml");
    setStatusCode(200);

    QDomElement envelope = doc.createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Envelope");
    envelope.setPrefix("soap");
    QDomElement body = doc.createElement("soap:Body");

    QDomElement success = doc.createElement("success");
    success.appendChild(doc.createTextNode("false"));
    body.appendChild(success);

    QDomElement message = doc.createElement("error");
    message.appendChild(doc.createTextNode(messageString));
    body.appendChild(message);

    envelope.appendChild(body);
    doc.appendChild(envelope);

    renderXml(doc);
}
