//
// deals with gui logic
//

#ifndef PRODUCTIVITY_TIMER_TIMER_VIEW_H
#define PRODUCTIVITY_TIMER_TIMER_VIEW_H

#include <QMainWindow>

class QLabel;
class QPushButton;

class Timer_View : public QMainWindow
{
    Q_OBJECT

    public:
        void start_timer();
        void stop_timer();
        void pause_timer();
        explicit Timer_View(QWidget *parent = nullptr);

    private:
        QLabel *m_timer_label = nullptr;
        QPushButton* m_start_button = nullptr;
        QPushButton* m_stop_button = nullptr;
        QPushButton* m_pause_button = nullptr;
};




#endif //PRODUCTIVITY_TIMER_TIMER_VIEW_H