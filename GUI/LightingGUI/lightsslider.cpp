/*!
 * RGB-LED-Routines
 * LightingGUI
 *
 * A Qt-based GUI for controlling LED arrays.
 *
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#include "lightsslider.h"

#include <QtCore>
#include <QtGui>
#include <QStyleOption>


LightsSlider::LightsSlider(QWidget *parent) : QWidget(parent) {
    this->setAutoFillBackground(true);
    // init the slider
    slider = new QSlider(Qt::Horizontal, this);
    slider->setAutoFillBackground(true);
    slider->setGeometry(this->rect());
    setMinimumPossible(false, 0);
    setSnapToNearestTick(false);
    slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(slider, SIGNAL(valueChanged(int)),
            this, SLOT(receivedValue(int)));
}


void LightsSlider::setSliderColorBackground(DataLayer::Color color) {
    // compute a darker version for our gradient
    DataLayer::Color darkColor = {(uint8_t)(color.r / 5),
                                  (uint8_t)(color.g / 5),
                                  (uint8_t)(color.b / 5)};

    // generate a stylesheet based off of the color with a gradient
    QString styleSheetString = QString("QSlider::sub-page:horizontal{ " \
                                       " background:qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(%1, %2, %3), stop: 1 rgb(%4, %5, %6));" \
                                       " background: qlineargradient(x1: 0, y1: 0.2, x2: 1, y2: 1, stop: 0 rgb(%1, %2, %3), stop: 1 rgb(%4, %5, %6));" \
                                       "}").arg(QString::number(darkColor.r),
                                                QString::number(darkColor.g),
                                                QString::number(darkColor.b),
                                                QString::number(color.r),
                                                QString::number(color.g),
                                                QString::number(color.b));
    slider->setStyleSheet(styleSheetString);
}


void LightsSlider::receivedValue(int value) {
    value = jumpSliderToPosition(slider, value);
    emit valueChanged(value);
}


/*!
 * solution based on this stack overflow response:
 * http://stackoverflow.com/a/15321654
 */
int LightsSlider::jumpSliderToPosition(QSlider *slider, int newPos) {
    Qt::MouseButtons btns = QApplication::mouseButtons();
    QPoint localMousePos = slider->mapFromGlobal(QCursor::pos());
    bool clickOnSlider = (btns & Qt::LeftButton)
                          && (localMousePos.x() >= 0
                          && localMousePos.y() >= 0
                          && localMousePos.x() < slider->size().width()
                          && localMousePos.y() < slider->size().height());
    if (clickOnSlider) {
        float posRatio = localMousePos.x() / (float)slider->size().width();
        int sliderRange = slider->maximum() - slider->minimum();
        int sliderPosUnderMouse = slider->minimum() + sliderRange * posRatio;
        if (sliderPosUnderMouse != newPos) {
            if (mShouldSnap) {
                sliderPosUnderMouse = snapSliderToNearestTick(slider, sliderPosUnderMouse);
            }
            if (mUseMinimumPossible && (sliderPosUnderMouse < mMinimumPossible)) {
                sliderPosUnderMouse = mMinimumPossible;
            }
            slider->setValue(sliderPosUnderMouse);
           return sliderPosUnderMouse;
        }
    }

    if (mShouldSnap) {
        newPos = snapSliderToNearestTick(slider, newPos);
    }
    if (mUseMinimumPossible && (newPos < mMinimumPossible)) {
        newPos = mMinimumPossible;
    }
    return newPos;
}


int LightsSlider::snapSliderToNearestTick(QSlider *slider, int pos) {
    int numberOfFullTicks = pos / slider->tickInterval();
    int leftTick = slider->minimum() + numberOfFullTicks * slider->tickInterval();
    int rightTick = slider->minimum()  + (numberOfFullTicks + 1) * slider->tickInterval();
    if ((leftTick - pos) < (pos - rightTick)) {
        pos = rightTick;
    } else {
        pos = leftTick;
    }
    return pos;
}


void LightsSlider::setMinimumPossible(bool useMinimumPossible, int minimumPossible) {
    mUseMinimumPossible = useMinimumPossible;
    mMinimumPossible = minimumPossible;
    if (mUseMinimumPossible && (slider->value() < mMinimumPossible)) {
        jumpSliderToPosition(slider, mMinimumPossible);
    }
}


void LightsSlider::resizeEvent(QResizeEvent *event) {
    Q_UNUSED (event);
    slider->setGeometry(this->rect());
}


void LightsSlider::paintEvent(QPaintEvent *event) {
    Q_UNUSED (event);
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QColor(255, 255, 255, 60));

    painter.fillRect(this->rect(), QBrush(QColor(48, 47, 47)));

    // draw tick marks
    // do this manually because they are very badly behaved with style sheets
    int interval = slider->tickInterval();
    if (interval == 0) {
        interval = slider->pageStep();
    }

    if (slider->tickPosition() != QSlider::NoTicks) {
        for (int i = slider->minimum(); i <= slider->maximum(); i += interval) {
            int x = round((double)((double)((double)(i - slider->minimum()) / (double)(slider->maximum() - slider->minimum())) * (double)(slider->width()))) - 1;
            int h = 2;
            if (slider->tickPosition() == QSlider::TicksBothSides || slider->tickPosition() == QSlider::TicksAbove) {
                int y = slider->rect().top();
                painter.drawLine(x, y, x, y / 2 + h);
            }
            if (slider->tickPosition() == QSlider::TicksBothSides || slider->tickPosition() == QSlider::TicksBelow) {
                int y = this->rect().bottom();
                painter.drawLine(x, y, x, y / 2 - h);
            }
        }
    }
}


void LightsSlider::setSnapToNearestTick(bool shouldSnap) {
    mShouldSnap = shouldSnap;
}
