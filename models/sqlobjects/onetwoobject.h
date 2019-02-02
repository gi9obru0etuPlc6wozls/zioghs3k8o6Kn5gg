#ifndef ONETWOOBJECT_H
#define ONETWOOBJECT_H

#include <TSqlObject>
#include <QSharedData>


class T_MODEL_EXPORT OneTwoObject : public TSqlObject, public QSharedData
{
public:
    qulonglong first_last {0};
    QString field_a;
    QString filed_1;

    enum PropertyIndex {
        FirstLast = 0,
        FieldA,
        Filed1,
    };

    int primaryKeyIndex() const override { return FirstLast; }
    int autoValueIndex() const override { return FirstLast; }
    QString tableName() const override { return QLatin1String("one_two"); }

private:    /*** Don't modify below this line ***/
    Q_OBJECT
    Q_PROPERTY(qulonglong first_last READ getfirst_last WRITE setfirst_last)
    T_DEFINE_PROPERTY(qulonglong, first_last)
    Q_PROPERTY(QString field_a READ getfield_a WRITE setfield_a)
    T_DEFINE_PROPERTY(QString, field_a)
    Q_PROPERTY(QString filed_1 READ getfiled_1 WRITE setfiled_1)
    T_DEFINE_PROPERTY(QString, filed_1)
};

#endif // ONETWOOBJECT_H
