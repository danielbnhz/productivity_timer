

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

QLineEdit, QComboBox , QDateTimeEdit, QSpinBox {
    background-color: #16263F;
    border: 1px solid #2F5FA8;
    border-radius: 8px;
    padding: 8px 10px;
    selection-background-color: #FF8A1F;
}
QLineEdit:focus, QComboBox:focus,
QDateTimeEdit:focus, QSpinBox:focus { border-color: #FF8A1F; }
QComboBox QAbstractItemView {
    background-color: #16263F;
    selection-background-color: #2F5FA8;
    border: 1px solid #2F5FA8;
}

QTabWidget::pane {
    border: none;
    border-top: 1px solid #1F3050;
}
QTabBar::tab {
    background: transparent;
    color: #9DB2D1;
    padding: 10px 22px;
    border: none;
    border-bottom: 2px solid transparent;
}
QTabBar::tab:hover    { color: #E6EDF7; }
QTabBar::tab:selected {
    color: #FF8A1F;
    border-bottom: 2px solid #FF8A1F;
    font-weight: 700;
}

QLabel#totalHoursLabel,
QLabel#productiveHoursLabel {
    font-size: 40px;
    font-weight: 700;
}
QLabel#totalHoursLabel      { color: #E6EDF7; }
QLabel#productiveHoursLabel { color: #FF8A1F; }

QTableWidget {
    background-color: #16263F;
    gridline-color: #1F3050;
    border: 1px solid #2F5FA8;
    border-radius: 8px;
}
QTableWidget::item { padding: 6px 10px; }
QHeaderView::section {
    background-color: #1B2F4D;
    color: #FF8A1F;
    border: none;
    border-bottom: 2px solid #FF8A1F;
    padding: 8px 10px;
    font-weight: 700;
}
QTableCornerButton::section {
    background-color: #1B2F4D;
    border: none;
}

QPushButton#rangeButton {
    padding: 6px 10px;
}
QPushButton#rangeButton:checked {
    background-color: #FF8A1F;
    border: none;
    color: #0F1B2D;
    font-weight: 700;
}
QPushButton#rangeButton:checked:hover {
    background-color: #FFA047;
}


)");
    }

}
#endif //PRODUCTIVITY_TIMER_THEME_H