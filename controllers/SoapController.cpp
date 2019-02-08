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


    readRequest();

}

struct {
    QString name;
    QString text;


};

QVariant SoapController::readRequest() {
    std::cerr << "SoapController::readRequest: " << std::endl;

    QVariantMap map;
    QString text;
    QXmlStreamReader::TokenType token;



    while (!xmlReader.atEnd()) {
        token = xmlReader.readNext();

        tDebug("Token: %s Name: %s Text: %s ",
              xmlReader.tokenString().toStdString().c_str(),
             xmlReader.name().toString().toStdString().c_str(),
              xmlReader.text().toString().toStdString().c_str()
        );

        switch (token) {
            case QXmlStreamReader::StartElement:
                text = xmlReader.readElementText();
                tDebug("Text: %s", text.toStdString().c_str());
                break;

            case QXmlStreamReader::Characters:
                text = xmlReader.text().toString().trimmed();
                if (text.isEmpty()) {
                    tDebug("Skipping");
                    continue;
                }
                break;

        }

//        std::cerr << "Token: " << xmlReader.tokenString().toStdString()
//        << " Name: " << xmlReader.name().toString().toStdString()
//        << " Text: " << xmlReader.text().toString().toStdString()
//        << std::endl;

    }


    std::cerr << "return! " << std::endl;
    return map;
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
