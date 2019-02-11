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

QString SoapController::getErrorMessage() {
    std::cerr <<  "SoapController::getErrorMessage: " << std::endl;
    return errorString;
}


bool SoapController::initRequest(THttpRequest *tHttpRequest) {
    std::cerr <<  "SoapController::initRequest: " << std::endl;

    if (tHttpRequest->method() != Tf::Post || tHttpRequest->header().contentType() != "text/xml") {
        errorString = "Method/contentType error";
        return false;
    }

    qioDevice = tHttpRequest->rawBody();

    if (!qioDevice->isOpen()) {
        qioDevice->open(QIODevice::ReadOnly);
    }

    xmlReader.setDevice(qioDevice);

    return readDocument();
}

bool SoapController::readDocument() {
    std::cerr <<  "SoapController::readDocument: " << std::endl;

    if (xmlReader.readNextStartElement()) {
        std::cerr << "readNextStartElement: " << xmlReader.name().toString().toStdString() << std::endl;
        if (xmlReader.name() == "Envelope") {
            return readEnvelope();
        }
        else {
            std::cerr << "SOAP Envelope not found." << std::endl;
            xmlReader.raiseError(QObject::tr("SOAP Envelope not found."));
        }
    }
    return !xmlReader.error();
}

bool SoapController::readEnvelope() {
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
            xmlReader.raiseError(QObject::tr("Unexpected element in SOAP Envelope."));
        }

    }
    return !xmlReader.error();
}

bool SoapController::readMethod() {
    std::cerr << "SoapController::readMethod: " << std::endl;

    if (xmlReader.readNextStartElement()) {
        std::cerr << "readNextStartElement: " << xmlReader.name().toString().toStdString() << std::endl;
        soapMethod = xmlReader.name().toString();
    }

    if (xmlReader.error()) return !xmlReader.error();

    tDebug("Token: %s Name: %s Text: %s ",
           xmlReader.tokenString().toStdString().c_str(),
           xmlReader.name().toString().toStdString().c_str(),
           xmlReader.text().toString().toStdString().c_str()
    );

    QVariant x = readRequest();

    tDebug("------ dumpMap Start");
    dumpMap(x);

    tDebug("------ dumpMap End");

}

QVariant SoapController::dumpMap(QVariant qVariant) {
    tDebug("--- SoapController::dumpMap");

    if (qVariant.canConvert(QMetaType::QVariantMap)) {
        tDebug("QMetaType::QVariantMap");

        XMLMap xmlMap = qVariant.toMap();

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


QVariant SoapController::readRequest() {
    tDebug("--- SoapController::readRequest:");

    bool inElement = false;


    QString name = xmlReader.name().toString();
    QString text;
    XMLMap xmlMap;

    while (!xmlReader.atEnd()) {
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
                xmlReader.raiseError("SOAP XML Error");
                std::cerr << "SOAP XML Error " << std::endl;
                break;
        }
    }

    tDebug("***RETURN***");
    std::cerr << "***RETURN***" << std::endl;
    return QVariant();
}