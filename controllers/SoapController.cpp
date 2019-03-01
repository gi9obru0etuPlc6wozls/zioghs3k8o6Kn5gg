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

QVariant SoapController::getParameter(const QString &name) const {
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

//QVariant SoapController::readRequest() {
//    tDebug("=== SoapController::readRequest:");
//    QString text;
//
//    while (!xmlReader.atEnd() && xmlReader.error() == QXmlStreamReader::NoError) {
//        auto token = xmlReader.readNext();
//        token = xmlReader.tokenType();
//        const QString name = xmlReader.name().toString();
//
//        switch (token) {
//            case QXmlStreamReader::TokenType::Characters:
//                text = xmlReader.text().toString();
//                tDebug("+QXmlStreamReader::TokenType::Characters: %s", text.trimmed().toStdString().c_str());
//                break;
//
//            case QXmlStreamReader::TokenType::StartElement:
//                tDebug("+QXmlStreamReader::TokenType::StartElement: %s", name.toStdString().c_str());
//                break;
//
//            case QXmlStreamReader::TokenType::EndElement:
//                tDebug("+QXmlStreamReader::TokenType::EndElement: %s", name.toStdString().c_str());
//                break;
//            case QXmlStreamReader::TokenType::NoToken:
//                tDebug("+QXmlStreamReader::TokenType::NoToken");
//                break;
//
//            case QXmlStreamReader::TokenType::Invalid:
//                tDebug("+QXmlStreamReader::TokenType::Invalid");
//                break;
//
//            case QXmlStreamReader::TokenType::StartDocument:
//                tDebug("+QXmlStreamReader::TokenType::StartDocument");
//                break;
//
//            case QXmlStreamReader::TokenType::EndDocument:
//                tDebug("+QXmlStreamReader::TokenType::EndDocument");
//                break;
//
//            case QXmlStreamReader::TokenType::Comment:
//                tDebug("+QXmlStreamReader::TokenType::Comment");
//                break;
//
//            case QXmlStreamReader::TokenType::DTD:
//                tDebug("+QXmlStreamReader::TokenType::DTD");
//                break;
//
//            case QXmlStreamReader::TokenType::EntityReference:
//                tDebug("+QXmlStreamReader::TokenType::EntityReference");
//                break;
//
//            case QXmlStreamReader::TokenType::ProcessingInstruction:
//                tDebug("+QXmlStreamReader::TokenType::ProcessingInstruction");
//                break;
//
////            default:
////                tDebug("SOAP XML Error");
////                xmlReader.raiseError(QObject::tr("SOAP XML Error."));
////                break;
//        }
//    }
//
//    tDebug("Unexpected end of SOAP XML");
//    xmlReader.raiseError(QObject::tr("Unexpected end of SOAP XML."));
//    return {};
//}

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
    QVariant pageMap = getParameter("pagination");

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

TCriteria SoapController::getCriteria(const QMap<QString, int> &propertyMap) const {
    TCriteria cri;
    QVariant vm = getParameter("filters");

    if (vm.canConvert(QMetaType::QVariantMap)) {
        tDebug("QMetaType::QVariantMap");

        int propertyIndex;
        TSql::ComparisonOperator comparisonOp;

        QMultiMap<QString, QVariant> xmlMap = vm.toMap();

        for (QMap<QString, QVariant>::iterator xmlMap_it = xmlMap.begin(); xmlMap_it != xmlMap.end(); ++xmlMap_it) {
//            tDebug("Key: %s ", xmlMap_it.key().toStdString().c_str());
//            QString p = (*xmlMap_it).toMap().value("property").toString();
//            QString v = (*xmlMap_it).toMap().value("value").toString();
//            QString o = (*xmlMap_it).toMap().value("operator").toString();
//
//            tDebug("property: %s value: %s operation: %s",
//                   p.toStdString().c_str(),
//                   v.toStdString().c_str(),
//                   o.toStdString().c_str()
//            );

            if ((propertyIndex = propertyMap.value((*xmlMap_it).toMap().value("property").toString(), -1)) == -1) {
                tDebug("invalid property");
                continue;
            }
            if ((comparisonOp = getComparisonOp((*xmlMap_it).toMap().value("operator").toString())) == TSql::ComparisonOperator::Invalid) {
                tDebug("invalid operator");
                continue;
            }

            cri.add(propertyIndex, comparisonOp, (*xmlMap_it).toMap().value("value"));
            //dumpMap(xmlMap_it.value());
        }
    }
    return cri;
}

QList<QPair<QString, Tf::SortOrder>> SoapController::getSortOrder(const QMap<QString, QString> &propertyMap) const {
    QList<QPair<QString, Tf::SortOrder>> sortColumns;
    QVariant vm = getParameter("sortValues");

    if (vm.canConvert(QMetaType::QVariantMap)) {
        tDebug("QMetaType::QVariantMap");
        SoapMap xmlMap = vm.toMap();

        Tf::SortOrder sortOrder;

        for (QMap<QString, QVariant>::iterator xmlMap_it = xmlMap.begin(); xmlMap_it != xmlMap.end(); ++xmlMap_it) {
            tDebug("Key: %s ", xmlMap_it.key().toStdString().c_str());
            QString p = (*xmlMap_it).toMap().value("property").toString();
            QString o = (*xmlMap_it).toMap().value("order").toString();
            tDebug("property: %s order: %s",
                   p.toStdString().c_str(),
                   o.toStdString().c_str()
            );

            QString sortColumn = propertyMap.value((*xmlMap_it).toMap().value("property").toString());
            sortOrder = SoapController::getSortDirection((*xmlMap_it).toMap().value("order").toString());
            sortColumns.append(QPair<QString, Tf::SortOrder>(sortColumn, sortOrder));
        }
    }
    return sortColumns;
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

TSql::ComparisonOperator SoapController::getComparisonOp(const QString &op) {
    static QMap<QString, TSql::ComparisonOperator> map{
        {"lt",      TSql::ComparisonOperator::LessThan},
        {"le",      TSql::ComparisonOperator::LessEqual},
        {"eq",      TSql::ComparisonOperator::Equal},
        {"ge",      TSql::ComparisonOperator::GreaterEqual},
        {"gt",      TSql::ComparisonOperator::GreaterThan},
        {"ne",      TSql::ComparisonOperator::NotEqual},
        {"in",      TSql::ComparisonOperator::In},
        {"notin",   TSql::ComparisonOperator::NotIn},
        {"like",    TSql::ComparisonOperator::Like}
    };

    return map.value(op, TSql::ComparisonOperator::Invalid);
}

Tf::SortOrder SoapController::getSortDirection(const QString &order) {
    static QMap<QString, Tf::SortOrder> map{
        {"DESC", Tf::SortOrder::DescendingOrder},
        {"ASC",  Tf::SortOrder::AscendingOrder}
    };

    return map.value(order, Tf::SortOrder::AscendingOrder);
}
