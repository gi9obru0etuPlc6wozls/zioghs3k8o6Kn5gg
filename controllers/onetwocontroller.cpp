#include "onetwocontroller.h"
#include "onetwo.h"


void OneTwoController::index()
{
    auto oneTwoList = OneTwo::getAll();
    texport(oneTwoList);
    render();
}

void OneTwoController::show(const QString &firstLast)
{
    auto oneTwo = OneTwo::get(firstLast.toULongLong());
    texport(oneTwo);
    render();
}

void OneTwoController::create()
{
    switch (httpRequest().method()) {
    case Tf::Get:
        render();
        break;

    case Tf::Post: {
        auto oneTwo = httpRequest().formItems("oneTwo");
        auto model = OneTwo::create(oneTwo);

        if (!model.isNull()) {
            QString notice = "Created successfully.";
            tflash(notice);
            redirect(urla("show", model.firstLast()));
        } else {
            QString error = "Failed to create.";
            texport(error);
            texport(oneTwo);
            render();
        }
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void OneTwoController::save(const QString &firstLast)
{
    switch (httpRequest().method()) {
    case Tf::Get: {
        auto model = OneTwo::get(firstLast.toULongLong());
        if (!model.isNull()) {
            auto oneTwo = model.toVariantMap();
            texport(oneTwo);
            render();
        }
        break; }

    case Tf::Post: {
        QString error;
        auto model = OneTwo::get(firstLast.toULongLong());
        
        if (model.isNull()) {
            error = "Original data not found. It may have been updated/removed by another transaction.";
            tflash(error);
            redirect(urla("save", firstLast));
            break;
        }

        auto oneTwo = httpRequest().formItems("oneTwo");
        model.setProperties(oneTwo);
        if (model.save()) {
            QString notice = "Updated successfully.";
            tflash(notice);
            redirect(urla("show", model.firstLast()));
        } else {
            error = "Failed to update.";
            texport(error);
            texport(oneTwo);
            render();
        }
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void OneTwoController::remove(const QString &firstLast)
{
    if (httpRequest().method() != Tf::Post) {
        renderErrorResponse(Tf::NotFound);
        return;
    }

    auto oneTwo = OneTwo::get(firstLast.toULongLong());
    oneTwo.remove();
    redirect(urla("index"));
}


// Don't remove below this line
T_DEFINE_CONTROLLER(OneTwoController)
