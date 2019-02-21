#include <iostream>
#include <QXmlInputSource>
#include <QMapIterator>
#include <TCriteria>
#include "blogcontroller.h"
#include "blog.h"
#include "SoapController.h"


void BlogController::index()
{
    auto blogList = Blog::getAll();
    texport(blogList);
    render();
}

void BlogController::show(const QString &id)
{
    auto blog = Blog::get(id.toInt());
    texport(blog);
    render();
}

void BlogController::xmlCreate()
{
    std::cerr <<  "BlogController::xmlCreate: " << std::endl;

    SoapController sr;
    if (!sr.soapRequest()) {
        renderText(sr.errorMessage());
        return;
    }


    QString soapMethod = sr.getSoapMethod();
    std::cerr <<  "sr.getSoapMethod: " << soapMethod.toStdString() << std::endl;

    if (soapMethod != "CreateBlog") {
        std::cerr <<  "SOAP method invalid" << std::endl;
    }

    if (sr.soapRequest() != QXmlStreamReader::NoError) {
        auto model = Blog::create(sr.soapParameters());

        if (model.isNull()) {
            std::cerr <<  "Failed to create record." << std::endl;
        }
    }
    else {
        std::cerr <<  "SOAP XML Parse error." << std::endl;
    }

    // Dummy response.
    QDomDocument doc;
    QDomElement response = doc.createElementNS("http://localhost/Blog", "blog:CreateBlogResponse");
    doc.appendChild(response);
    renderXml(doc);
}

void BlogController::xmlGet(const QString &id)
{
    tDebug("BlogController::xmlGet");

    if (soapRequest() != QXmlStreamReader::NoError) {
        tDebug("soapRequest error: %s %d", errorMessage().toStdString().c_str(), error());
        renderText(errorMessage());
        return;
    }

    QString soapMethod = getSoapMethod();
    std::cerr <<  "getSoapMethod: " << soapMethod.toStdString() << std::endl;

    if (soapMethod != "GetBlog") {
        renderText("Invalid SOAP method:" + soapMethod);
        return;
    }

//    tDebug("1 ===");
//    dumpMap(soapParameters());  // TODO: remove
//    tDebug("1 ===");

    //2019-02-18 09:16:54 DEBUG [10361] 1 ===
    //2019-02-18 09:16:54 DEBUG [10361] --- SoapController::dumpMap: QVariantMap
    //2019-02-18 09:16:54 DEBUG [10361] Key: GetBlog
    //2019-02-18 09:16:54 DEBUG [10361] --- SoapController::dumpMap: QVariantMap
    //2019-02-18 09:16:54 DEBUG [10361] Key: filters
    //2019-02-18 09:16:54 DEBUG [10361] --- SoapController::dumpMap: QVariantMap
    //2019-02-18 09:16:54 DEBUG [10361] Key: filter
    //2019-02-18 09:16:54 DEBUG [10361] --- SoapController::dumpMap: QString
    //2019-02-18 09:16:54 DEBUG [10361] value: like

    tDebug("1 ===");
    QString name = "filters";

    QVariant vm = getParameter(name);

    if (vm.canConvert(QMetaType::QVariantMap)) {
        tDebug("QMetaType::QVariantMap");

        SoapMap xmlMap = vm.toMap();

        TCriteria cri;

        for (QMap<QString, QVariant>::iterator xmlMap_it = xmlMap.begin(); xmlMap_it != xmlMap.end(); ++xmlMap_it) {
            tDebug("Key: %s ", xmlMap_it.key().toStdString().c_str());
            QString p = (*xmlMap_it).toMap().value("property").toString();
            QString v = (*xmlMap_it).toMap().value("value").toString();
            QString o = (*xmlMap_it).toMap().value("operator").toString();

            tDebug("property: %s value: %s operation: %s",
                    p.toStdString().c_str(),
                    v.toStdString().c_str(),
                    o.toStdString().c_str()
            );
            //dumpMap(xmlMap_it.value());
        }
    }

    QDomDocumentFragment frag = Blog::getAllXml(doc);

    soapResponse(frag);
}

void BlogController::create()
{
    switch (httpRequest().method()) {
    case Tf::Get:
        render();
        break;

    case Tf::Post: {
        auto blog = httpRequest().formItems("blog");
        auto model = Blog::create(blog);

        if (!model.isNull()) {
            QString notice = "Created successfully.";
            tflash(notice);
            redirect(urla("show", model.id()));
        } else {
            QString error = "Failed to create.";
            texport(error);
            texport(blog);
            render();
        }
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void BlogController::save(const QString &id)
{
    switch (httpRequest().method()) {
    case Tf::Get: {
        auto model = Blog::get(id.toInt());
        if (!model.isNull()) {
            session().insert("blog_lockRevision", model.lockRevision());
            auto blog = model.toVariantMap();
            texport(blog);
            render();
        }
        break; }

    case Tf::Post: {
        QString error;
        int rev = session().value("blog_lockRevision").toInt();
        auto model = Blog::get(id.toInt(), rev);

        if (model.isNull()) {
            error = "Original data not found. It may have been updated/removed by another transaction.";
            tflash(error);
            redirect(urla("save", id));
            break;
        }

        auto blog = httpRequest().formItems("blog");

        model.setProperties(blog);
        if (model.save()) {
            QString notice = "Updated successfully.";
            tflash(notice);
            redirect(urla("show", model.id()));
        } else {
            error = "Failed to update.";
            texport(error);
            texport(blog);
            render();
        }
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void BlogController::remove(const QString &id)
{
    if (httpRequest().method() != Tf::Post) {
        renderErrorResponse(Tf::NotFound);
        return;
    }

    auto blog = Blog::get(id.toInt());
    blog.remove();
    redirect(urla("index"));
}


// Don't remove below this line
T_DEFINE_CONTROLLER(BlogController)
