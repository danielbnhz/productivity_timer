//
// deals with gui logic
//

#include "timer_view.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMainWindow>

class Timer_View : public QMainWindow
{
    Q_OBJECT
    Private:
        QLabel* m_timer_label;
        QPushButton* m_start_button;
        QPushButton* m_stop_button;


    public:


        void set_time_text(const QString& text);


    explicit Timer_View(QWidget* parent)
        : QWidget(parent)
        , m_timer_label(new QLabel("25:00", this))
        , m_start_button(new QPushButton("Start", this))
        , m_stop_button(new QPushButton("Stop", this))

};

