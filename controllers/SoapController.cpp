//
// Created by jwc on 2/4/19.
//

#include <iostream>
#include <THttpRequest>
#include "SoapRequest.h"

SoapRequest::SoapRequest() {}

QString SoapRequest::getSoapMethod() {
    std::cerr <<  "SoapRequest::getSoapMethod: " << std::endl;
    return soapMethod;
}

QString SoapRequest::getErrorMessage() {
    std::cerr <<  "SoapRequest::getErrorMessage: " << std::endl;
    return errorString;
}


bool SoapRequest::initRequest(THttpRequest *tHttpRequest) {
    std::cerr <<  "SoapRequest::initRequest: " << std::endl;

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

bool SoapRequest::readDocument() {
    std::cerr <<  "SoapRequest::readDocument: " << std::endl;

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

bool SoapRequest::readEnvelope() {
    std::cerr <<  "SoapRequest::readEnvelope: " << std::endl;

    while (xmlReader.readNextStartElement()) {
        std::cerr << "readNextStartElement: " << xmlReader.name().toString().toStdString() << std::endl;
        if (xmlReader.name() == "Header") {
            xmlReader.skipCurrentElement();
        }
        else if (xmlReader.name() == "Body") {
            return readBody();
        }
        else {
            std::cerr << "Unexpected element in SOAP Envelope." << std::endl;
            xmlReader.raiseError(QObject::tr("Unexpected element in SOAP Envelope."));
        }

    }
    return !xmlReader.error();
}

bool SoapRequest::readBody() {
    std::cerr << "SoapRequest::readBody: " << std::endl;

    if (xmlReader.readNextStartElement()) {
        std::cerr << "readNextStartElement: " << xmlReader.name().toString().toStdString() << std::endl;
        soapMethod = xmlReader.name().toString();
    }
    return !xmlReader.error();
}


/*
 * Request body
 */
bool SoapRequest::processRequest() {
    std::cerr << "SoapRequest::processRequest: " << std::endl;

    if (xmlReader.isStartElement()) {
        std::cerr << "isStartElement: " << std::endl;
    }

    while (xmlReader.readNextStartElement()) {
        std::cerr << "readNextStartElement: " << xmlReader.name().toString().toStdString() << std::endl;
        if (xmlReader.name() == "Blog") {    // TODO: make generic
            return readObject();
        }
        else {
            xmlReader.raiseError(QObject::tr("Unexpected object."));
        }

    }
    return !xmlReader.error();
}

bool SoapRequest::readObject() {
    std::cerr << "SoapRequest::readObject: " << xmlReader.name().toString().toStdString() << std::endl;

    while (xmlReader.readNextStartElement()) {
        QString key =  xmlReader.name().toString();
        QString value = xmlReader.readElementText();

        std::cerr << "Key: " << key.toStdString() << " Value: " << value.toStdString() << std::endl;

        items.insert(xmlReader.name().toString(), QVariant(value));

    }
    return !xmlReader.error();
}

QVariantMap SoapRequest::getItems() {
    return items;
}
