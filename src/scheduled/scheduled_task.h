#pragma once

#include <QString>
#include <QtGlobal>

struct ScheduledTask
{
    qint64 id = -1;
    qint64 archetype_id = -1;

    QString title;

    bool recurring = false;
};