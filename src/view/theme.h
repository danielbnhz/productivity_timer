

#ifndef PRODUCTIVITY_TIMER_THEME_H
#define PRODUCTIVITY_TIMER_THEME_H
#pragma once

#include <QString>

namespace productivity_timer {

    inline QString theme_stylesheet()
    {
        return QString::fromUtf8(R"(
QWidget {
    background-color: #0F1B2D;
    color: #E6EDF7;
    font-family: "Segoe UI";
    font-size: 14px;
}
QLabel#timerLabel {
    font-size: 84px;
    font-weight: 700;
    color: #FF8A1F;
}
QLabel#statusLabel, QLabel#currentTaskLabel { color: #9DB2D1; }

QPushButton {
    background-color: #1B2F4D;
    border: 1px solid #2F5FA8;
    border-radius: 8px;
    padding: 10px 14px;
}
QPushButton:hover   { background-color: #234070; }
QPushButton:pressed { background-color: #2F5FA8; }
QPushButton:disabled {
    background-color: #131F33;
    border-color: #1F3050;
    color: #566B8C;
}
QPushButton#startButton {
    background-color: #FF8A1F;
    border: none;
    color: #0F1B2D;
    font-weight: 700;
}
QPushButton#startButton:hover    { background-color: #FFA047; }
QPushButton#startButton:disabled { background-color: #3A2A18; color: #7A6850; }

QLineEdit, QComboBox {
    background-color: #16263F;
    border: 1px solid #2F5FA8;
    border-radius: 8px;
    padding: 8px 10px;
    selection-background-color: #FF8A1F;
}
QLineEdit:focus, QComboBox:focus { border-color: #FF8A1F; }
QComboBox QAbstractItemView {
    background-color: #16263F;
    selection-background-color: #2F5FA8;
    border: 1px solid #2F5FA8;
}
)");
    }

}
#endif //PRODUCTIVITY_TIMER_THEME_H