#include <TreeFrogModel>
#include "onetwo.h"
#include "onetwoobject.h"

OneTwo::OneTwo()
    : TAbstractModel(), d(new OneTwoObject())
{
    d->first_last = 0;
}

OneTwo::OneTwo(const OneTwo &other)
    : TAbstractModel(), d(new OneTwoObject(*other.d))
{ }

OneTwo::OneTwo(const OneTwoObject &object)
    : TAbstractModel(), d(new OneTwoObject(object))
{ }

OneTwo::~OneTwo()
{
    // If the reference count becomes 0,
    // the shared data object 'OneTwoObject' is deleted.
}

qulonglong OneTwo::firstLast() const
{
    return d->first_last;
}

QString OneTwo::fieldA() const
{
    return d->field_a;
}

void OneTwo::setFieldA(const QString &fieldA)
{
    d->field_a = fieldA;
}

QString OneTwo::filed1() const
{
    return d->filed_1;
}

void OneTwo::setFiled1(const QString &filed1)
{
    d->filed_1 = filed1;
}

OneTwo &OneTwo::operator=(const OneTwo &other)
{
    d = other.d;  // increments the reference count of the data
    return *this;
}

OneTwo OneTwo::create(const QString &fieldA, const QString &filed1)
{
    OneTwoObject obj;
    obj.field_a = fieldA;
    obj.filed_1 = filed1;
    if (!obj.create()) {
        return OneTwo();
    }
    return OneTwo(obj);
}

OneTwo OneTwo::create(const QVariantMap &values)
{
    OneTwo model;
    model.setProperties(values);
    if (!model.d->create()) {
        model.d->clear();
    }
    return model;
}

OneTwo OneTwo::get(qulonglong firstLast)
{
    TSqlORMapper<OneTwoObject> mapper;
    return OneTwo(mapper.findByPrimaryKey(firstLast));
}

int OneTwo::count()
{
    TSqlORMapper<OneTwoObject> mapper;
    return mapper.findCount();
}

QList<OneTwo> OneTwo::getAll()
{
    return tfGetModelListByCriteria<OneTwo, OneTwoObject>(TCriteria());
}

QJsonArray OneTwo::getAllJson()
{
    QJsonArray array;
    TSqlORMapper<OneTwoObject> mapper;

    if (mapper.find() > 0) {
        for (TSqlORMapperIterator<OneTwoObject> i(mapper); i.hasNext(); ) {
            array.append(QJsonValue(QJsonObject::fromVariantMap(OneTwo(i.next()).toVariantMap())));
        }
    }
    return array;
}

TModelObject *OneTwo::modelData()
{
    return d.data();
}

const TModelObject *OneTwo::modelData() const
{
    return d.data();
}

QDataStream &operator<<(QDataStream &ds, const OneTwo &model)
{
    auto varmap = model.toVariantMap();
    ds << varmap;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, OneTwo &model)
{
    QVariantMap varmap;
    ds >> varmap;
    model.setProperties(varmap);
    return ds;
}

// Don't remove below this line
T_REGISTER_STREAM_OPERATORS(OneTwo)
