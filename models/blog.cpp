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

int Blog::propertyIndexMap(const QString &propertyName) {
    static QMap<QString, int> map{
            {"id", BlogObject::PropertyIndex::Id},
            {"title", BlogObject::PropertyIndex::Title},
            {"body", BlogObject::PropertyIndex::Body},
            {"colString", BlogObject::PropertyIndex::ColString},
            {"colInteger", BlogObject::PropertyIndex::ColInteger},
            {"colFloat", BlogObject::PropertyIndex::ColFloat},
            {"colDouble", BlogObject::PropertyIndex::ColDouble},
            {"colNumeric", BlogObject::PropertyIndex::ColNumeric},
            {"createdAt", BlogObject::PropertyIndex::CreatedAt},
            {"updatedAt", BlogObject::PropertyIndex::UpdatedAt},
            {"lockRevision", BlogObject::PropertyIndex::LockRevision}
    };
    return map.value(propertyName, -1);
}

QMap<QString, QString> Blog::propertyColumnMap() {
    static QMap<QString, QString> map{
            {"id","id"},
            {"title", "title"},
            {"body", "body" },
            {"colString", "col_string"},
            {"colInteger", "col_integer" },
            {"colFloat", "col_float"},
            {"colDouble", "col_double"},
            {"colNumeric", "col_numeric"},
            {"createdAt", "created_at"},
            {"updatedAt", "updated_at" },
            {"lockRevision", "lock_revision"}
    };
    return map;
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

QDomDocumentFragment Blog::getXmlByCriteria(QDomDocument &dom,
    const TCriteria &cri,
    const QList<QPair<QString, Tf::SortOrder>> &sortColumns,
    int limit, int offset)
{
    QDomDocumentFragment ret = dom.createDocumentFragment();
    TSqlORMapper<BlogObject> mapper;
    if (! sortColumns.isEmpty()) {
        for (auto &p : sortColumns) {
            if (!p.first.isEmpty()) {
                tDebug("getXmlByCriteria order: %s", p.first.toStdString().c_str());
                mapper.setSortOrder(p.first, p.second);
            }
        }
    }
    if (limit > 0) {
        mapper.setLimit(limit);
    }
    if (offset > 0) {
        mapper.setOffset(offset);
    }

    if (mapper.find(cri) > 0) {
        for (auto &o : mapper) {
            ret.appendChild(Blog(o).toXml(dom));
        }
    }
    return ret;
}

QDomDocumentFragment Blog::getXmlByCriteria(QDomDocument &dom,
    const QVariant &filters,
    const QList<QPair<QString, Tf::SortOrder>> &sortColumns,
    int limit, int offset)
{
    TCriteria cri;



    QDomDocumentFragment ret = dom.createDocumentFragment();
    TSqlORMapper<BlogObject> mapper;
    if (! sortColumns.isEmpty()) {
        for (auto &p : sortColumns) {
            if (!p.first.isEmpty()) {
                tDebug("getXmlByCriteria order: %s", p.first.toStdString().c_str());
                mapper.setSortOrder(p.first, p.second);
            }
        }
    }
    if (limit > 0) {
        mapper.setLimit(limit);
    }
    if (offset > 0) {
        mapper.setOffset(offset);
    }

    if (mapper.find(cri) > 0) {
        for (auto &o : mapper) {
            ret.appendChild(Blog(o).toXml(dom));
        }
    }
    return ret;
}

//TCriteria Blog::getCriteria(const QMap<QString, int> &propertyMap) const {
TCriteria Blog::getCriteria(const QVariant &vm) const {
    TCriteria cri;

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

            //if ((propertyIndex = propertyMap.value((*xmlMap_it).toMap().value("property").toString(), -1)) == -1) {
            //if ((propertyIndex = propertyIndexMap((*xmlMap_it).toMap().value("property").toString()) == -1) {
            if ((propertyIndex = propertyIndexMap((*xmlMap_it).toMap().value("property").toString())) == -1) {
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

TSql::ComparisonOperator Blog::getComparisonOp(const QString &op) {
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

// Don't remove below this line
T_REGISTER_STREAM_OPERATORS(Blog)
