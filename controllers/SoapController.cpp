//
// Created by jwc on 2/4/19.
//

#include <iostream>
#include <THttpRequest>
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


    XMLMap x;
    readRequest(x);
//    std::cerr << "x: " << x.toString().toStdString() << std::endl;

    dumpMap(QVariant(x));

//    QMap<QString, QVariant> map = x.toMap();
//
//    for (QMap<QString, QVariant>::iterator i = map.begin(); i != map.end(); ++i) {
//        tDebug("1. Key: %s", i.key().toStdString().c_str());
//
//        QMap<QString, QVariant> map1 = i->toMap();
//        for (QMap<QString, QVariant>::iterator j = map1.begin(); j != map1.end(); ++j) {
//            tDebug("2. Key: %s", j.key().toStdString().c_str());
//        }
//    }

}

QVariant SoapController::dumpMap(QVariant x) {
    QMap<QString, QVariant> map = x.toMap();

    for (QMap<QString, QVariant>::iterator i = map.begin(); i != map.end(); ++i) {
        tDebug("Key: %s", i.key().toStdString().c_str());

        QVariant y = i.value();
        tDebug("y: %s", y.toString().toStdString().c_str());
        dumpMap(y);
    }

}


QVariant SoapController::readRequest(XMLMap &xmlMap) {
    std::cerr << "SoapController::readRequest: " << std::endl;
    tDebug("---");

    bool inElement = false;


    QString name = xmlReader.name().toString();
    QString text;

    while (!xmlReader.atEnd()) {
        auto token = xmlReader.readNext();

        tDebug("Token: %s Name: %s Text: %s ",
                xmlReader.tokenString().toStdString().c_str(),
                xmlReader.name().toString().toStdString().c_str(),
                xmlReader.text().toString().toStdString().c_str()
        );

        switch (token) {
            case QXmlStreamReader::TokenType::Characters:
                tDebug("+QXmlStreamReader::TokenType::Characters");
                text = xmlReader.text().toString();
                break;
            case QXmlStreamReader::TokenType::StartElement:
                tDebug("+QXmlStreamReader::TokenType::StartElement: %s", xmlReader.name().toString().toStdString().c_str());
                inElement = true;
                xmlMap.insertMulti(xmlReader.name().toString(), readRequest());

                break;
            case QXmlStreamReader::TokenType::EndElement:
                tDebug("+QXmlStreamReader::TokenType::EndElement: %s", xmlReader.name().toString().toStdString().c_str());
//                if (xmlReader.name().toString() != parentName) {
//                    tDebug("Element name mismatch");
//                }

                if (inElement) {
                    tDebug("inElement: True");
                    //return QVariant(xmlMap);
                    return xmlMap;
                }

                tDebug("inElement: False");
                return text;
                break;
            default:
                xmlReader.raiseError("SOAP XML Error");
                std::cerr << "SOAP XML Error " << std::endl;
                break;
        }
    }
    tDebug("***RETURN***");

    std::cerr << "return! " << std::endl;
    return xmlMap;
}


/*
 * Request body
 */
//bool SoapController::processRequest() {
//    std::cerr << "SoapController::processRequest: " << std::endl;
//
//    if (xmlReader.isStartElement()) {
//        std::cerr << "isStartElement: " << std::endl;
//    }
//
//    while (xmlReader.readNextStartElement()) {
//        std::cerr << "readNextStartElement: " << xmlReader.name().toString().toStdString() << std::endl;
//        if (xmlReader.name() == "Blog") {    // TODO: make generic
//            return readObject();
//        }
//        else {
//            xmlReader.raiseError(QObject::tr("Unexpected object."));
//        }
//
//    }
//    return !xmlReader.error();
//}

//bool SoapController::readObject() {
//    std::cerr << "SoapController::readObject: " << xmlReader.name().toString().toStdString() << std::endl;
//
//    while (xmlReader.readNextStartElement()) {
//        QString key =  xmlReader.name().toString();
//        QString value = xmlReader.readElementText();
//
//        std::cerr << "Key: " << key.toStdString() << " Value: " << value.toStdString() << std::endl;
//
//        items.insert(xmlReader.name().toString(), QVariant(value));
//
//    }
//    return !xmlReader.error();
//}
//
//QVariantMap SoapController::getItems() {
//    return items;
//}
