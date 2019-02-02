//
// Created by jwc on 2/1/19.
//

#ifndef APPNAME_BLOGXMLHANDLER_H
#define APPNAME_BLOGXMLHANDLER_H


#include <QtXml/QXmlDefaultHandler>
#include <iostream>

class SoapXmlHandler : public QXmlDefaultHandler {
public:
    SoapXmlHandler() {}

    explicit SoapXmlHandler(const QString &objectName) {
        this->objectName = objectName;
    }

    void setName(const QString &objectName) {
        this->objectName = objectName;
    }

    bool startDocument() override {
        return true;
    }

    bool startEntity(const QString &name) override {
        return true;
    }

    bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts) override {
        if (!inObject) {
            if (this->objectName == qName) {
                inObject = true;
            }
            return true;
        }
        inElement = true;
        elementName = qName;
        return true;
    }

    bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName) override {
        if (!inObject) {
            if (this->objectName == qName) {
                inObject = false;
            }
            return true;
        }
        inElement = false;
        map.insert(elementName, QVariant(elementValue));
        return true;
    }

    bool characters(const QString &value) override {
        if (inObject && inElement) {
            elementValue = value;
        }
        return true;
    }

    QVariantMap itemMap() {
        return map;
    }


private:
    QString objectName;
    bool inObject = false;
    bool inElement = false;
    QString elementName;
    QString elementValue;
    QVariantMap map;
};

#endif //APPNAME_BLOGXMLHANDLER_H
