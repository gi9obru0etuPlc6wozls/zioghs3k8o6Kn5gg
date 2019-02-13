#ifndef BLOGCONTROLLER_H
#define BLOGCONTROLLER_H

#include "applicationcontroller.h"
#include "SoapController.h"
#include <QDomDocument>


class T_CONTROLLER_EXPORT BlogController : public SoapController
{
    Q_OBJECT
public:
    BlogController() : SoapController() {}

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
