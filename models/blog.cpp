#include <TreeFrogModel>
#include <QtXml/QDomDocument>
#include "blog.h"
#include "blogobject.h"

Blog::Blog()
    : TAbstractModel(), d(new BlogObject())
{
    d->id = 0;
    d->col_integer = 0;
    d->col_float = 0;
    d->col_double = 0;
    d->col_numeric = 0;
    d->lock_revision = 0;
}

Blog::Blog(const Blog &other)
    : TAbstractModel(), d(new BlogObject(*other.d))
{ }

Blog::Blog(const BlogObject &object)
    : TAbstractModel(), d(new BlogObject(object))
{ }

Blog::~Blog()
{
    // If the reference count becomes 0,
    // the shared data object 'BlogObject' is deleted.
}

int Blog::id() const
{
    return d->id;
}

QString Blog::title() const
{
    return d->title;
}

void Blog::setTitle(const QString &title)
{
    d->title = title;
}

QString Blog::body() const
{
    return d->body;
}

void Blog::setBody(const QString &body)
{
    d->body = body;
}

QString Blog::colString() const
{
    return d->col_string;
}

void Blog::setColString(const QString &colString)
{
    d->col_string = colString;
}

int Blog::colInteger() const
{
    return d->col_integer;
}

void Blog::setColInteger(int colInteger)
{
    d->col_integer = colInteger;
}

double Blog::colFloat() const
{
    return d->col_float;
}

void Blog::setColFloat(double colFloat)
{
    d->col_float = colFloat;
}

double Blog::colDouble() const
{
    return d->col_double;
}

void Blog::setColDouble(double colDouble)
{
    d->col_double = colDouble;
}

double Blog::colNumeric() const
{
    return d->col_numeric;
}

void Blog::setColNumeric(double colNumeric)
{
    d->col_numeric = colNumeric;
}

QDateTime Blog::createdAt() const
{
    return d->created_at;
}

void Blog::setCreatedAt(const QDateTime &createdAt)
{
    d->created_at = createdAt;
}

QDateTime Blog::updatedAt() const
{
    return d->updated_at;
}

void Blog::setUpdatedAt(const QDateTime &updatedAt)
{
    d->updated_at = updatedAt;
}

int Blog::lockRevision() const
{
    return d->lock_revision;
}

Blog &Blog::operator=(const Blog &other)
{
    d = other.d;  // increments the reference count of the data
    return *this;
}

Blog Blog::create(const QString &title, const QString &body, const QString &colString, int colInteger, double colFloat, double colDouble, double colNumeric, const QDateTime &createdAt, const QDateTime &updatedAt)
{
    BlogObject obj;
    obj.title = title;
    obj.body = body;
    obj.col_string = colString;
    obj.col_integer = colInteger;
    obj.col_float = colFloat;
    obj.col_double = colDouble;
    obj.col_numeric = colNumeric;
    obj.created_at = createdAt;
    obj.updated_at = updatedAt;
    if (!obj.create()) {
        return Blog();
    }
    return Blog(obj);
}

Blog Blog::create(const QVariantMap &values)
{
    Blog model;
    model.setProperties(values);
    if (!model.d->create()) {
        model.d->clear();
    }
    return model;
}

Blog Blog::get(int id)
{
    TSqlORMapper<BlogObject> mapper;
    return Blog(mapper.findByPrimaryKey(id));
}

Blog Blog::get(int id, int lockRevision)
{
    TSqlORMapper<BlogObject> mapper;
    TCriteria cri;
    cri.add(BlogObject::Id, id);
    cri.add(BlogObject::LockRevision, lockRevision);
    return Blog(mapper.findFirst(cri));
}

int Blog::count()
{
    TSqlORMapper<BlogObject> mapper;
    return mapper.findCount();
}

QList<Blog> Blog::getAll()
{
    return tfGetModelListByCriteria<Blog, BlogObject>(TCriteria());
}

QString *Blog::tableNameToVariableName() const
{
    static QString *pointer;
    if (!pointer) {
        pointer = new QString(fieldNameToVariableName(d->tableName()));
    }
    return pointer;
}

QString *Blog::tableNameToObjectName() const
{
    static QString *pointer;
    if (!pointer) {
        pointer = new QString(fieldNameToVariableName(d->tableName()));
    }

    if (!pointer->isEmpty()) {
        (*pointer)[0] = (*pointer)[0].toUpper();
    }
    return pointer;
}

QJsonArray Blog::getAllJson()
{
    QJsonArray array;
    TSqlORMapper<BlogObject> mapper;

    if (mapper.find() > 0) {
        for (TSqlORMapperIterator<BlogObject> i(mapper); i.hasNext(); ) {
            array.append(QJsonValue(QJsonObject::fromVariantMap(Blog(i.next()).toVariantMap())));
        }
    }
    return array;
}

QDomDocumentFragment Blog::getAllXml(QDomDocument &dom)
{
    QDomDocumentFragment ret = dom.createDocumentFragment();
    TSqlORMapper<BlogObject> mapper;

    if (mapper.find() > 0) {
        for (TSqlORMapperIterator<BlogObject> i(mapper); i.hasNext(); ) {
            ret.appendChild(Blog(i.next()).toXml(dom));
        }
    }
    return ret;
}

QVector<QString> *Blog::variableNames() const
{
    static QVector<QString> *pointer;
    if (!pointer) {
        pointer = new QVector<QString>;
        const QMetaObject *metaObj = modelData()->metaObject();
        pointer->fill(nullptr, modelData()->metaObject()->propertyCount());

        for (int i = metaObj->propertyOffset(); i < metaObj->propertyCount(); ++i) {
            QString n(metaObj->property(i).name());

            if (!n.isEmpty()) {
                (*pointer)[i] = *tableNameToVariableName() + ":" + fieldNameToVariableName(n);
            }
        }
    }
    return pointer;
}

QDomElement Blog::toXml(QDomDocument &dom) const {
    QDomElement ret = dom.createElement(*tableNameToVariableName() +':'+ *tableNameToObjectName());
    QVector<QString> *varNames = variableNames();

    const TModelObject *md = modelData();
    const QMetaObject *metaObj = md->metaObject();

    for (int i = metaObj->propertyOffset(); i < metaObj->propertyCount(); ++i) {
        const char *propName = metaObj->property(i).name();
        QString n(propName);
        if (!n.isEmpty()) {
            QDomElement tag = dom.createElement((*varNames)[i]);
            ret.appendChild(tag);

            QDomText text = dom.createTextNode(md->property(propName).toString());
            tag.appendChild(text);
        }
    }
    return ret;
}

TModelObject *Blog::modelData()
{
    return d.data();
}

const TModelObject *Blog::modelData() const
{
    return d.data();
}

QDataStream &operator<<(QDataStream &ds, const Blog &model)
{
    auto varmap = model.toVariantMap();
    ds << varmap;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Blog &model)
{
    QVariantMap varmap;
    ds >> varmap;
    model.setProperties(varmap);
    return ds;
}

// Don't remove below this line
T_REGISTER_STREAM_OPERATORS(Blog)
