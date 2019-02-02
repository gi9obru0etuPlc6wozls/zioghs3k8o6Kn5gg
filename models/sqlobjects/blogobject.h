#ifndef BLOGOBJECT_H
#define BLOGOBJECT_H

#include <TSqlObject>
#include <QSharedData>


class T_MODEL_EXPORT BlogObject : public TSqlObject, public QSharedData
{
public:
    int id {0};
    QString title;
    QString body;
    QString col_string;
    int col_integer {0};
    double col_float {0};
    double col_double {0};
    double col_numeric {0};
    QDateTime created_at;
    QDateTime updated_at;
    int lock_revision {0};

    enum PropertyIndex {
        Id = 0,
        Title,
        Body,
        ColString,
        ColInteger,
        ColFloat,
        ColDouble,
        ColNumeric,
        CreatedAt,
        UpdatedAt,
        LockRevision,
    };

    int primaryKeyIndex() const override { return Id; }
    int autoValueIndex() const override { return Id; }
    QString tableName() const override { return QLatin1String("blog"); }

private:    /*** Don't modify below this line ***/
    Q_OBJECT
    Q_PROPERTY(int id READ getid WRITE setid)
    T_DEFINE_PROPERTY(int, id)
    Q_PROPERTY(QString title READ gettitle WRITE settitle)
    T_DEFINE_PROPERTY(QString, title)
    Q_PROPERTY(QString body READ getbody WRITE setbody)
    T_DEFINE_PROPERTY(QString, body)
    Q_PROPERTY(QString col_string READ getcol_string WRITE setcol_string)
    T_DEFINE_PROPERTY(QString, col_string)
    Q_PROPERTY(int col_integer READ getcol_integer WRITE setcol_integer)
    T_DEFINE_PROPERTY(int, col_integer)
    Q_PROPERTY(double col_float READ getcol_float WRITE setcol_float)
    T_DEFINE_PROPERTY(double, col_float)
    Q_PROPERTY(double col_double READ getcol_double WRITE setcol_double)
    T_DEFINE_PROPERTY(double, col_double)
    Q_PROPERTY(double col_numeric READ getcol_numeric WRITE setcol_numeric)
    T_DEFINE_PROPERTY(double, col_numeric)
    Q_PROPERTY(QDateTime created_at READ getcreated_at WRITE setcreated_at)
    T_DEFINE_PROPERTY(QDateTime, created_at)
    Q_PROPERTY(QDateTime updated_at READ getupdated_at WRITE setupdated_at)
    T_DEFINE_PROPERTY(QDateTime, updated_at)
    Q_PROPERTY(int lock_revision READ getlock_revision WRITE setlock_revision)
    T_DEFINE_PROPERTY(int, lock_revision)
};

#endif // BLOGOBJECT_H
