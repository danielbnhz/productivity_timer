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
    private:
        QLabel *m_timer_label = nullptr;
        QPushButton* m_start_button = nullptr;
        QPushButton* m_stop_button = nullptr;
        QPushButton* m_pause_button = nullptr;



    public:


        void set_time_text(const QString& text);

        Timer_View(QWidget *parent = nullptr)
        {
            m_timer_label = new QLabel(this);
            m_start_button = new QPushButton(this);
            m_stop_button = new QPushButton(this);
            m_pause_button = new QPushButton(this);

        }


};

