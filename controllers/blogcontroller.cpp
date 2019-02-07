#include <iostream>
#include <QXmlInputSource>
#include <QMapIterator>
#include "blogcontroller.h"
#include "blog.h"
#include "SoapRequest.h"


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

    SoapRequest sr;
    if (!sr.initRequest(&httpRequest())) {
        renderText(sr.getErrorMessage());
        return;
    }

    QString soapMethod = sr.getSoapMethod();
    std::cerr <<  "sr.getSoapMethod: " << soapMethod.toStdString() << std::endl;

    if (soapMethod != "CreateBlog") {
        std::cerr <<  "SOAP method invalid" << std::endl;
    }

    if (sr.processRequest()) {
        auto model = Blog::create(sr.getItems());

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
    std::cerr <<  "BlogController::xmlGet" << std::endl;

    SoapRequest sr;
    if (!sr.initRequest(&httpRequest())) {
        std::cerr <<  "sr.getErrorMessage: " <<  sr.getErrorMessage().toStdString() << std::endl;
        renderText("SOAP Error:" + sr.getErrorMessage());
        return;
    }

    QString soapMethod = sr.getSoapMethod();
    std::cerr <<  "sr.getSoapMethod: " << soapMethod.toStdString() << std::endl;

    if (soapMethod != "GetBlog") {
        renderText("Invalid SOAP method:" + soapMethod);
        return;
    }

    /*
     *
     */

    setContentType("text/xml");
    setStatusCode(200);

    QDomDocument doc;
    QDomElement envelope = doc.createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Envelope");
    envelope.setPrefix("soap");
    QDomElement body = doc.createElement("soap:Body");
    QDomElement response = doc.createElementNS("http://localhost/Blog", "blog:GetBlogResponse");

    body.appendChild(response);
    envelope.appendChild(body);
    doc.appendChild(envelope);

    QDomDocumentFragment frag = Blog::getAllXml(doc);
    response.appendChild(frag);

    //std::cerr <<  "doc: " <<  doc.toString(4).toStdString()  << std::endl;

    renderXml(doc);
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
