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

void BlogController::xmlTest1()
{
    if (httpRequest().method() != Tf::Post || httpRequest().header().contentType() != "text/xml") {
        std::cerr <<  "Method/contentType error" << std::endl;
        return;
    }

    QXmlInputSource xmlSource(httpRequest().rawBody());
    QXmlStreamReader xmlReader(xmlSource.data());  // <--- Works, but does lots of extra data copies.

    while (!xmlReader.atEnd() && !xmlReader.hasError()) {
        QXmlStreamReader::TokenType token = xmlReader.readNext();

        if (token == QXmlStreamReader::StartDocument) {
            std::cerr <<  "QXmlStreamReader::StartDocument" << std::endl;
        }

        if (token == QXmlStreamReader::StartElement) {
            std::cerr <<  "QXmlStreamReader::StartElement" << std::endl;
        }

        if (token == QXmlStreamReader::EndDocument) {
            std::cerr <<  "QXmlStreamReader::EndDocument" << std::endl;
        }

        if (token == QXmlStreamReader::EndElement) {
            std::cerr <<  "QXmlStreamReader::EndElement" << std::endl;
        }

        std::cerr <<  "Name: " << xmlReader.name().toString().toStdString() << std::endl;
    }

    if (xmlReader.hasError()) {
        std::cerr << "errorString: " << xmlReader.errorString().toStdString() << std::endl;
    }

    // Dummy response.
    QDomDocument doc;
    QDomElement response = doc.createElementNS("http://framesquared.com/Blog", "blog:GetBlogResponse");
    doc.appendChild(response);
    renderXml(doc);
}

void BlogController::xmlTest2()
{
    if (httpRequest().method() != Tf::Post || httpRequest().header().contentType() != "text/xml") {
        std::cerr <<  "Method/contentType error" << std::endl;
        return;
    }

    QXmlStreamReader xmlReader;
    QIODevice *qioDevice = httpRequest().rawBody();
    qioDevice->open(QIODevice::ReadOnly);
    xmlReader.setDevice(qioDevice);  // <--- Not working.

    while (!xmlReader.atEnd() && !xmlReader.hasError()) {
        QXmlStreamReader::TokenType token = xmlReader.readNext();

        if (token == QXmlStreamReader::StartDocument) {
            std::cerr <<  "QXmlStreamReader::StartDocument" << std::endl;
        }

        if (token == QXmlStreamReader::StartElement) {
            std::cerr <<  "QXmlStreamReader::StartElement" << std::endl;
        }

        if (token == QXmlStreamReader::EndDocument) {
            std::cerr <<  "QXmlStreamReader::EndDocument" << std::endl;
        }

        if (token == QXmlStreamReader::EndElement) {
            std::cerr <<  "QXmlStreamReader::EndElement" << std::endl;
        }

        std::cerr <<  "Name: " << xmlReader.name().toString().toStdString() << std::endl;
    }

    if (xmlReader.hasError()) {
        std::cerr << "errorString: " << xmlReader.errorString().toStdString() << std::endl;
    }

    // Dummy response.
    QDomDocument doc;
    QDomElement response = doc.createElementNS("http://framesquared.com/Blog", "blog:GetBlogResponse");
    doc.appendChild(response);
    renderXml(doc);
}

void BlogController::xmlTest3()
{
    if (httpRequest().method() != Tf::Post || httpRequest().header().contentType() != "text/xml") {
        std::cerr <<  "Method/contentType error" << std::endl;
        return;
    }

    QByteArray qba("<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
                   "<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
                   "    <soap:Body>\n"
                   "        <CreateBlog xmlns=\"http://framesquared.com/Blog\">\n"
                   "            <Blog>\n"
                   "                <id>Blog-1</id>\n"
                   "                <title>Johnnie</title>\n"
                   "                <body>Carson</body>\n"
                   "                <colString>Seven</colString>\n"
                   "                <colInteger>7</colInteger>\n"
                   "                <colFloat>7</colFloat>\n"
                   "                <colDouble>7</colDouble>\n"
                   "                <colNumeric>7</colNumeric>\n"
                   "                <createAt></createAt>\n"
                   "                <updateAt></updateAt>\n"
                   "                <lockRevision>0</lockRevision>\n"
                   "            </Blog>\n"
                   "        </CreateBlog>\n"
                   "    </soap:Body>\n"
                   "</soap:Envelope>");

    QBuffer qb(&qba);

    std::cerr << "qba: " << qba.toStdString() << std::endl;

    qb.open(QBuffer::ReadOnly);
    std::cerr << "qb.readAll: " << qb.readAll().toStdString() << std::endl;

//    QBuffer inputFile(httpRequest().rawBody()); // <-- Not working
//    if (inputFile.open(QIODevice::ReadOnly))
//    {
//        QTextStream in(&inputFile);
//        while (!in.atEnd())
//        {
//            QString line = in.readLine();
//            std::cerr << "Line: " << line.toStdString() << std::endl;
//        }
//        inputFile.close();
//    }

    // Dummy response.
    QDomDocument doc;
    QDomElement response = doc.createElementNS("http://framesquared.com/Blog", "blog:GetBlogResponse");
    doc.appendChild(response);
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
