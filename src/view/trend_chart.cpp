#include "trend_chart.h"

#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QFontMetrics>

#include <algorithm>
#include <cmath>

namespace productivity_timer
{
    TrendChart::TrendChart(QWidget* parent)
        : QWidget(parent)
    {
        setMinimumHeight(260);
        setMinimumWidth(350);
        setSizePolicy(
            QSizePolicy::Expanding,
            QSizePolicy::Expanding
        );
    }

    void TrendChart::set_points(
        const QList<TrendPoint>& points)
    {
        m_points = points;
        update();
    }

    void TrendChart::paintEvent(QPaintEvent* event)
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const QRect area = rect();

        painter.fillRect(area, palette().window());

        if (m_points.isEmpty())
        {
            painter.setPen(palette().text().color());

            painter.drawText(
                area,
                Qt::AlignCenter,
                "No trend data available."
            );

            return;
        }

        const int left = 65;
        const int right = 20;
        const int top = 25;
        const int bottom = 45;

        const QRectF plot(
            left,
            top,
            width() - left - right,
            height() - top - bottom
        );

        if (plot.width() <= 0 || plot.height() <= 0)
        {
            return;
        }

        qint64 maximum_ms = 0;

        for (const TrendPoint& point : m_points)
        {
            maximum_ms = std::max(
                maximum_ms,
                point.focused_ms
            );
        }


        const double maximum_hours =
            static_cast<double>(maximum_ms)
            / 3600000.0;

        const double scale_max =
            maximum_hours > 0
                ? std::ceil(maximum_hours)
                : 1.0;

        const QColor text_color =
            palette().text().color();

        const QColor grid_color =
            palette().mid().color();

        const QColor line_color =
            palette().highlight().color();

        painter.setFont(font());
        painter.setPen(text_color);


        for (int i = 0; i <= 4; ++i)
        {
            const double fraction = i / 4.0;

            const double y =
                plot.bottom() - fraction * plot.height();

            painter.setPen(QPen(grid_color, 1));

            painter.drawLine(
                QPointF(plot.left(), y),
                QPointF(plot.right(), y)
            );

            const double label_value =
                scale_max * fraction;

            painter.setPen(text_color);

            painter.drawText(
                QRectF(
                    0,
                    y - 10,
                    left - 10,
                    20
                ),
                Qt::AlignRight | Qt::AlignVCenter,
                QString::number(label_value, 'f', 1) + "h"
            );
        }

        QPainterPath line_path;

        const int count = m_points.size();

        for (int i = 0; i < count; ++i)
        {
            const double x =
                count == 1
                    ? plot.center().x()
                    : plot.left()
                        + (static_cast<double>(i)
                           / (count - 1))
                        * plot.width();

            const double hours =
                static_cast<double>(
                    m_points[i].focused_ms
                ) / 3600000.0;

            const double y =
                plot.bottom()
                - (hours / scale_max) * plot.height();

            const QPointF point(x, y);

            if (i == 0)
            {
                line_path.moveTo(point);
            }
            else
            {
                line_path.lineTo(point);
            }

            painter.setPen(Qt::NoPen);
            painter.setBrush(line_color);

            painter.drawEllipse(point, 4, 4);
        }

            painter.setPen(
                QPen(line_color, 2.5)
            );

            painter.setBrush(Qt::NoBrush);
            painter.drawPath(line_path);


            painter.setPen(text_color);

            const bool compact = count > 7;
            const int label_interval = compact ? 4 : 1;

            QFont date_font = font();
            date_font.setPointSize(compact ? 9 : 10);
            painter.setFont(date_font);

            for (int i = 0; i < count; ++i)
            {
                if (i % label_interval != 0 &&
                    i != count - 1)
                {
                    continue;
                }

                const double x =
                    count == 1
                        ? plot.center().x()
                        : plot.left()
                            + (static_cast<double>(i) / (count - 1))
                            * plot.width();

                const QString label =
                    m_points[i].date.toString("M/d");

                painter.drawText(
                    QRectF(
                        x - 25,
                        plot.bottom() + 10,
                        50,
                        25
                    ),
                    Qt::AlignCenter,
                    label
                );
            }
        }
    }
