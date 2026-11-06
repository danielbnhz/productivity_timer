#ifndef PRODUCTIVITY_TIMER_TREND_CHART_H
#define PRODUCTIVITY_TIMER_TREND_CHART_H

#include <QWidget>
#include <QList>

#include "../model/trend_point.h"

namespace productivity_timer
{
    class TrendChart : public QWidget
    {
    public:
        explicit TrendChart(QWidget* parent = nullptr);



        void set_points(const QList<TrendPoint>& points);

    protected:
        void paintEvent(QPaintEvent* event) override;

    private:
        QList<TrendPoint> m_points;
        productivity_timer::TrendChart* m_trend_chart = nullptr;

    };
}



#endif // PRODUCTIVITY_TIMER_TREND_CHART_H