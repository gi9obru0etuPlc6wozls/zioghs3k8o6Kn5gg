#ifndef ONETWOCONTROLLER_H
#define ONETWOCONTROLLER_H

#include "applicationcontroller.h"


class T_CONTROLLER_EXPORT OneTwoController : public ApplicationController
{
    Q_OBJECT
public:
    OneTwoController() : ApplicationController() {}

public slots:
    void index();
    void show(const QString &firstLast);
    void create();
    void save(const QString &firstLast);
    void remove(const QString &firstLast);
};

#endif // ONETWOCONTROLLER_H
