#ifndef BLOG_H
#define BLOG_H

#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QSharedDataPointer>
#include <TGlobal>
#include <TAbstractModel>
#include <sqlobjects/blogobject.h>

class TModelObject;
class BlogObject;
class QJsonArray;


class T_MODEL_EXPORT Blog : public TAbstractModel
{
public:
    Blog();
    Blog(const Blog &other);
    Blog(const BlogObject &object);
    ~Blog();

    int id() const;
    QString title() const;
    void setTitle(const QString &title);
    QString body() const;
    void setBody(const QString &body);
    QString colString() const;
    void setColString(const QString &colString);
    int colInteger() const;
    void setColInteger(int colInteger);
    double colFloat() const;
    void setColFloat(double colFloat);
    double colDouble() const;
    void setColDouble(double colDouble);
    double colNumeric() const;
    void setColNumeric(double colNumeric);
    QDateTime createdAt() const;
    void setCreatedAt(const QDateTime &createdAt);
    QDateTime updatedAt() const;
    void setUpdatedAt(const QDateTime &updatedAt);
    int lockRevision() const;
    Blog &operator=(const Blog &other);

    bool create() override { return TAbstractModel::create(); }
    bool update() override { return TAbstractModel::update(); }
    bool save()   override { return TAbstractModel::save(); }
    bool remove() override { return TAbstractModel::remove(); }

    static Blog create(const QString &title, const QString &body, const QString &colString, int colInteger, double colFloat, double colDouble, double colNumeric, const QDateTime &createdAt, const QDateTime &updatedAt);
    static Blog create(const QVariantMap &values);
    static Blog get(int id);
    static Blog get(int id, int lockRevision);
    static int count();
    static QList<Blog> getAll();
    static QList<Blog> getAll(const int limit,const int offset);
    static QJsonArray getAllJson();

    static QDomDocumentFragment getAllXml(QDomDocument &dom);
    QDomElement toXml(QDomDocument &dom) const;

    static BlogObject::PropertyIndex getPropertyIndex(const QString &propertyName);

private:
    QSharedDataPointer<BlogObject> d;

    TModelObject *modelData() override;
    const TModelObject *modelData() const override;
    friend QDataStream &operator<<(QDataStream &ds, const Blog &model);
    friend QDataStream &operator>>(QDataStream &ds, Blog &model);

    QVector<QString> *variableNames() const;

    QString *tableNameToVariableName() const;
    QString *tableNameToObjectName() const;
};

Q_DECLARE_METATYPE(Blog)
Q_DECLARE_METATYPE(QList<Blog>)

#endif // BLOG_H
