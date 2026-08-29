//
// model c++ file
//

#include "timer.h"
#include <QElapsedTimer>
#include <string>
QT_BEGIN_NAMESPACE
namespace productivity_timer
{
    class Timer
    {
        QElapsedTimer timer;
        qint64 ms;
        std::string task_name;
        bool completed;

    public:

        Timer();



        void start_timer(qint64 ms){

            timer.start();

            while (!timer.hasExpired(ms))
            {
                slowOperation1();
            }
    };
        void stop_timer();
        bool is_done();



    };
}
QT_END_NAMESPACE