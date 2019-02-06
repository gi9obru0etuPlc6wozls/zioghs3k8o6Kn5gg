//
// Created by jwc on 2/4/19.
//

#include <iostream>
#include "SoapRequest.h"

SoapRequest::SoapRequest() {}

SoapRequest::SoapRequest(QIODevice *qioDevice) {
    this->setDevice(qioDevice);
}

void SoapRequest::setDevice(QIODevice *qioDevice) {
    this->qioDevice = qioDevice;
    if (!this->qioDevice->isOpen()) {
        this->qioDevice->open(QIODevice::ReadOnly);
    }
    xmlReader.setDevice(qioDevice);
}

QString SoapRequest::getSoapMethod() {

    std::cerr <<  "SoapRequest::getSoapMethod: " << std::endl;

    if (readDocument())
        return soapMethod;

    errorString = xmlReader.errorString();
    return "";
}

bool SoapRequest::readDocument() {
    QString errorMsg = "";
    QString s;

    std::cerr <<  "SoapRequest::readDocument: " << std::endl;

    if (xmlReader.readNextStartElement()) {
        std::cerr << "readNextStartElement: " << xmlReader.name().toString().toStdString() << std::endl;
        if (xmlReader.name() == "Envelope") {
            return readEnvelope();
        }
        else {
            xmlReader.raiseError(QObject::tr("SOAP Envelope not found."));
        }
    }
    return !xmlReader.error();
}

bool SoapRequest::readEnvelope() {

    while (xmlReader.readNextStartElement()) {
        std::cerr << "readNextStartElement: " << xmlReader.name().toString().toStdString() << std::endl;
        if (xmlReader.name() == "Header") {
            xmlReader.skipCurrentElement();
        }
        else if (xmlReader.name() == "Body") {
            return readBody();
        }
        else {
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




bool SoapRequest::processRequest() {
    std::cerr << "SoapRequest::processRequest: " << std::endl;

    if (xmlReader.isStartElement()) {
        std::cerr << "isStartElement: " << std::endl;
    }

    while (xmlReader.readNextStartElement()) {
        std::cerr << "readNextStartElement: " << xmlReader.name().toString().toStdString() << std::endl;
        if (xmlReader.name() == "Blog") {
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


