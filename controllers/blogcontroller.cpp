#include <iostream>
#include <QXmlInputSource>
#include <QMapIterator>
#include "blogcontroller.h"
#include "blog.h"
#include "SoapXmlHandler.h"


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

void BlogController::xmlCreate(const QString &id)
{
    QString message = "Created successfully.";

    if (httpRequest().method() != Tf::Post || httpRequest().header().contentType() != "text/xml") {
        message = "Bad request";
    }

    auto *source = new QXmlInputSource(httpRequest().rawBody());
    auto *handler = new SoapXmlHandler("Blog");  // QXmlDefaultHandler
    QXmlSimpleReader reader;
    reader.setContentHandler(handler);
    if (!reader.parse(source)) {
        message = "XML parse error.";
    }

    auto items = handler->itemMap();
    auto model = Blog::create(items);

    if (model.isNull()) {
        message = "Failed to create.";
    }

    /*
     * - response
     */

    setContentType("text/xml");
    setStatusCode(200);

    QDomDocument doc;

    QDomElement envelope = doc.createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Envelope");
    envelope.setPrefix("soap");

    QDomElement body = doc.createElement("soap:Body");

    QDomElement response = doc.createElementNS("http://framesquared.com/Blog", "blog:GetBlogResponse");
    model.toXml(doc, response, "blog:");

    body.appendChild(response);
    envelope.appendChild(body);
    doc.appendChild(envelope);

    std::cerr <<  "doc: " <<  doc.toString(4).toStdString()  << std::endl;

    renderXml(doc);
}

void BlogController::xml(const QString &id)
{
    std::cerr <<  "BlogController::xml(const QString &id): " << std::endl;

    setContentType("text/xml");
    setStatusCode(200);

    QDomDocument doc;

    QDomElement envelope = doc.createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Envelope");
    envelope.setPrefix("soap");

    QDomElement body = doc.createElement("soap:Body");

    QDomDocumentFragment frag = Blog::getAllXml(doc, "blog:");
    QDomElement response = doc.createElementNS("http://framesquared.com/Blog", "blog:GetBlogResponse");

    response.appendChild(frag);
    body.appendChild(response);
    envelope.appendChild(body);
    doc.appendChild(envelope);

    std::cerr <<  "doc: " <<  doc.toString(4).toStdString()  << std::endl;

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
