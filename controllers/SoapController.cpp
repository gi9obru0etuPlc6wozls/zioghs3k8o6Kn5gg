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
    return items;
}

QXmlStreamReader::Error SoapController::soapRequest() {
    std::cerr << "SoapController::soapRequest: " << std::endl;

    if (httpRequest().method() != Tf::Post || httpRequest().header().contentType() != "text/xml") {
        tDebug("Method/contentType error");
        xmlReader.raiseError(QObject::tr("Method/contentType error."));
        return xmlReader.error();
    }
    qioDevice = httpRequest().rawBody();

    if (!qioDevice->isOpen()) {
        qioDevice->open(QIODevice::ReadOnly);
    }

    xmlReader.setDevice(qioDevice);
    return readDocument();
}

void SoapController::soapResponse(const QDomDocumentFragment &frag) {
    setContentType("text/xml");
    setStatusCode(200);

    QDomElement envelope = doc.createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Envelope");
    envelope.setPrefix("soap");
    QDomElement body = doc.createElement("soap:Body");

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
    std::cerr <<  "SoapController::readDocument: " << std::endl;

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
    std::cerr <<  "SoapController::readEnvelope: " << std::endl;

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
    std::cerr << "SoapController::readMethod: " << std::endl;

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

    items = readRequest().toMap(); // TODO: optimize?

    return xmlReader.error();
}

QVariant SoapController::readRequest() {
    tDebug("SoapController::readRequest:");

    bool inElement = false;

    QString name = xmlReader.name().toString();
    QString text;
    SoapMap xmlMap;

    while (!xmlReader.atEnd() && xmlReader.error() == QXmlStreamReader::NoError) {
        auto token = xmlReader.readNext();

        switch (token) {
            case QXmlStreamReader::TokenType::Characters:
                text = xmlReader.text().toString();
                tDebug("+QXmlStreamReader::TokenType::Characters: %s", text.toStdString().c_str());
                break;

            case QXmlStreamReader::TokenType::StartElement:
                tDebug("+QXmlStreamReader::TokenType::StartElement: %s", xmlReader.name().toString().toStdString().c_str());
                inElement = true;
                xmlMap.insert(xmlReader.name().toString(), readRequest());
                break;

            case QXmlStreamReader::TokenType::EndElement:
                tDebug("+QXmlStreamReader::TokenType::EndElement: %s", xmlReader.name().toString().toStdString().c_str());

                if (inElement) {
                    return QVariant(xmlMap);
                }

                return QVariant(text);
            default:
                tDebug("SOAP XML Error");
                xmlReader.raiseError(QObject::tr("SOAP XML Error."));
                return QVariant();
        }
    }

    tDebug("Unexpected end of SOAP XML");
    xmlReader.raiseError(QObject::tr("Unexpected end of SOAP XML."));
    return QVariant();
}

void SoapController::dumpMap(QVariant qVariant) {
    tDebug("--- SoapController::dumpMap");

    if (qVariant.canConvert(QMetaType::QVariantMap)) {
        tDebug("QMetaType::QVariantMap");

        SoapMap xmlMap = qVariant.toMap();

        for (QMap<QString, QVariant>::iterator xmlMap_it = xmlMap.begin(); xmlMap_it != xmlMap.end(); ++xmlMap_it) {
            tDebug("Key: %s ", xmlMap_it.key().toStdString().c_str());

            dumpMap(xmlMap_it.value());
        }
    } else if (qVariant.canConvert(QMetaType::QString)) {
        tDebug("QMetaType::QString");
        tDebug("value: %s", qVariant.toString().toStdString().c_str());
    } else {
        tDebug("Error in QMetaType");
    }
}
