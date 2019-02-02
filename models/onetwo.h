#ifndef ONETWO_H
#define ONETWO_H

#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QSharedDataPointer>
#include <TGlobal>
#include <TAbstractModel>

class TModelObject;
class OneTwoObject;
class QJsonArray;


class T_MODEL_EXPORT OneTwo : public TAbstractModel
{
public:
    OneTwo();
    OneTwo(const OneTwo &other);
    OneTwo(const OneTwoObject &object);
    ~OneTwo();

    qulonglong firstLast() const;
    QString fieldA() const;
    void setFieldA(const QString &fieldA);
    QString filed1() const;
    void setFiled1(const QString &filed1);
    OneTwo &operator=(const OneTwo &other);

    bool create() override { return TAbstractModel::create(); }
    bool update() override { return TAbstractModel::update(); }
    bool save()   override { return TAbstractModel::save(); }
    bool remove() override { return TAbstractModel::remove(); }

    static OneTwo create(const QString &fieldA, const QString &filed1);
    static OneTwo create(const QVariantMap &values);
    static OneTwo get(qulonglong firstLast);
    static int count();
    static QList<OneTwo> getAll();
    static QJsonArray getAllJson();

private:
    QSharedDataPointer<OneTwoObject> d;

    TModelObject *modelData() override;
    const TModelObject *modelData() const override;
    friend QDataStream &operator<<(QDataStream &ds, const OneTwo &model);
    friend QDataStream &operator>>(QDataStream &ds, OneTwo &model);
};

Q_DECLARE_METATYPE(OneTwo)
Q_DECLARE_METATYPE(QList<OneTwo>)

#endif // ONETWO_H
