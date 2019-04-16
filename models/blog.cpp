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

QDomElement Blog::toXml(QDomDocument &dom) const {
    QDomElement ret = dom.createElement(variableName() +':'+ objectName());
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

QDomDocumentFragment Blog::findXmlByCriteria(QDomDocument &dom,
    const TCriteria &cri,
    const QList<QPair<QString, Tf::SortOrder>> &sortColumns,
    int &count,
    int limit, int offset)
{
    QDomDocumentFragment ret = dom.createDocumentFragment();
    TSqlORMapper<BlogObject> mapper;
    if (! sortColumns.isEmpty()) {
        for (auto &p : sortColumns) {
            if (!p.first.isEmpty()) {
                tDebug("findXmlByCriteria order: %s", p.first.toStdString().c_str());
                mapper.setSortOrder(p.first, p.second);
            }
        }
    }

    count = mapper.findCount(cri);

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

QDomDocumentFragment Blog::findXmlByCriteria(QDomDocument &dom,
    const QVariant &filtersParam,
    const QVariant &sortColumnsParam,
    int &count,
    int limit, int offset)
{
    TCriteria cri = paramToCriteria(filtersParam);
    QList<QPair<QString, Tf::SortOrder>> sortColumns = paramToSortOrder(sortColumnsParam);

    return findXmlByCriteria(dom, cri, sortColumns, count, limit, offset);
}

TCriteria Blog::paramToCriteria(const QVariant &vm) {
    TCriteria cri;

    if (vm.canConvert(QMetaType::QVariantMap)) {
        tDebug("QMetaType::QVariantMap");

        int propertyIndex;
        TSql::ComparisonOperator comparisonOp;

        QMultiMap<QString, QVariant> filters = vm.toMap();

        for (auto &filter: filters) {
            if ((propertyIndex = propertyToIndex(filter.toMap().value("property").toString())) == -1) {
                tDebug("invalid property");
                continue;
            }
            if ((comparisonOp = comparisonOperator(filter.toMap().value("operator").toString())) == TSql::ComparisonOperator::Invalid) {
                tDebug("invalid operator");
                continue;
            }

            cri.add(propertyIndex, comparisonOp, filter.toMap().value("value"));
            //dumpMap(filter.value());
        }
    }
    return cri;
}

QList<QPair<QString, Tf::SortOrder>> Blog::paramToSortOrder(const QVariant &sortOrdersParam) {
    QList<QPair<QString, Tf::SortOrder>> sortColumns;

    if (sortOrdersParam.canConvert(QMetaType::QVariantMap)) {
        tDebug("QMetaType::QVariantMap");
        QMultiMap<QString, QVariant> sortOrders = sortOrdersParam.toMap();

        for (auto &sortOrder: sortOrders) {
            QString sortColumn = propertyToColumn(sortOrder.toMap().value("property").toString());
            Tf::SortOrder sortDirection = Blog::sortOrder(sortOrder.toMap().value("order").toString());
            sortColumns.append(QPair<QString, Tf::SortOrder>(sortColumn, sortDirection));
        }
    }
    return sortColumns;
}


QString Blog::variableName() const {
    return QString("blog");
}

QString Blog::objectName() const {
    return QString("Blog");
}

int Blog::propertyToIndex(const QString &propertyName) {
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

QString Blog::propertyToColumn(const QString &propertyName) {
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
    return map.value(propertyName, {});
}

QVector<QString> *Blog::variableNames() const
{
    static QVector<QString> vector{
            {""},
            {"blog:id"},
            {"blog:title"},
            {"blog:body"},
            {"blog:colString"},
            {"blog:colInteger"},
            {"blog:colFloat"},
            {"blog:colDouble"},
            {"blog:colNumeric"},
            {"blog:createdAt"},
            {"blog:updatedAt"},
            {"blog:lockRevision"}
    };
    return &vector;
}

TSql::ComparisonOperator Blog::comparisonOperator(const QString &op) {
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

Tf::SortOrder Blog::sortOrder(const QString &order) {
    static QMap<QString, Tf::SortOrder> map{
            {"DESC", Tf::SortOrder::DescendingOrder},
            {"ASC",  Tf::SortOrder::AscendingOrder}
    };

    return map.value(order, Tf::SortOrder::AscendingOrder);
}

// Don't remove below this line
T_REGISTER_STREAM_OPERATORS(Blog)
