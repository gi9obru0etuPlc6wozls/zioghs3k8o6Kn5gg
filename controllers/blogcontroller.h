#ifndef BLOGCONTROLLER_H
#define BLOGCONTROLLER_H

#include "applicationcontroller.h"
#include <QDomDocument>


class T_CONTROLLER_EXPORT BlogController : public ApplicationController
{
    Q_OBJECT
public:
    BlogController() : ApplicationController() {}

public slots:
    void index();
    void show(const QString &id);
    void xmlGet(const QString &id);
    void xmlCreate();
    void create();
    void save(const QString &id);
    void remove(const QString &id);
};

#endif // BLOGCONTROLLER_H
