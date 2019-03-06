#ifndef BLOG_H
#define BLOG_H

#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QSharedDataPointer>
#include <TGlobal>
#include <TCriteria>
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
    static QDomDocumentFragment findXmlByCriteria(QDomDocument &dom,
                                                  const TCriteria &cri,
                                                  const QList<QPair<QString, Tf::SortOrder>> &sortColumns,
                                                  int &count,
                                                  int limit = 0, int offset = 0);
    static QDomDocumentFragment findXmlByCriteria(QDomDocument &dom,
                                                  const QVariant &filtersParam, const QVariant &sortColumnsParam,
                                                  int &count,
                                                  int limit = 0, int offset = 0);


    static TCriteria paramToCriteria(const QVariant &vm);
    static QList<QPair<QString, Tf::SortOrder>> paramToSortOrder(const QVariant &sortOrdersParam);
    static TSql::ComparisonOperator comparisonOperator(const QString &op);
    static Tf::SortOrder sortOrder(const QString &order);

    QDomElement toXml(QDomDocument &dom) const;

    static int propertyToIndex(const QString &propertyName);
    static QString propertyToColumn(const QString &propertyName);

private:
    QSharedDataPointer<BlogObject> d;

    TModelObject *modelData() override;
    const TModelObject *modelData() const override;
    friend QDataStream &operator<<(QDataStream &ds, const Blog &model);
    friend QDataStream &operator>>(QDataStream &ds, Blog &model);

    QVector<QString> *variableNames() const;

    QString objectName() const;
    QString variableName() const;

//    QString *tableNameToVariableName() const;
//    QString *tableNameToObjectName() const;
};

Q_DECLARE_METATYPE(Blog)
Q_DECLARE_METATYPE(QList<Blog>)

#endif // BLOG_H
