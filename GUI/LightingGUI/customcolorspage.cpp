/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "CustomColorsPage.h"
#include "ui_CustomColorsPage.h"
#include "icondata.h"

#include <QSignalMapper>

CustomColorsPage::CustomColorsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomColorsPage) {
    ui->setupUi(this);

    // --------------
    // Setup Array Count Slider
    // --------------

    int customArraySize = 10;
    connect(ui->arraySlider, SIGNAL(valueChanged(int)), this, SLOT(customColorCountChanged(int)));
    ui->arraySlider->setSliderColorBackground(QColor(0,255,0));
    ui->arraySlider->slider->setMaximum(customArraySize * 10);
    ui->arraySlider->slider->setTickInterval(10);
    ui->arraySlider->slider->setTickPosition(QSlider::TicksBelow);
    ui->arraySlider->setSnapToNearestTick(true);
    ui->arraySlider->setMinimumPossible(true, 20);
    ui->arraySlider->setSliderHeight(0.6f);

    // --------------
    // Setup Color Array Array (array array  ... array array...)
    // --------------

    mArrayColorsButtons = std::shared_ptr<std::vector<QToolButton*> >(new std::vector<QToolButton*>(customArraySize, nullptr));
    mIconData = IconData(80,80);

    QSignalMapper *arrayButtonsMapper = new QSignalMapper(this);
    for (int i = 0; i < customArraySize; ++i) {
        (*mArrayColorsButtons.get())[i] = new QToolButton;
        (*mArrayColorsButtons.get())[i]->setIcon(mIconData.renderAsQPixmap());
        ui->arrayColorsLayout->addWidget((*mArrayColorsButtons.get())[i], 0 , i);
        connect((*mArrayColorsButtons.get())[i], SIGNAL(clicked(bool)), arrayButtonsMapper, SLOT(map()));
        arrayButtonsMapper->setMapping((*mArrayColorsButtons.get())[i], i);
    }
    connect(arrayButtonsMapper, SIGNAL(mapped(int)), this, SLOT(selectArrayColor(int)));

    mGreyIcon = IconData(80,80);
    mGreyIcon.setSolidColor(QColor(140,140,140));

    // --------------
    // Setup Color Picker
    // --------------

    ui->colorPicker->chooseLayout(ELayoutColorPicker::eCondensedLayout);
    connect(ui->colorPicker, SIGNAL(colorUpdate(QColor)), this, SLOT(colorChanged(QColor)));
    mCurrentColorPickerIndex = 0;
}

CustomColorsPage::~CustomColorsPage() {
    delete ui;
}

void CustomColorsPage::setupButtons() {
    // --------------
    // Setup Routine Buttons
    // --------------
    std::vector<std::string> labels = {"Glimmer",
                                       "Fade",
                                       "Random Solid",
                                       "Random Individual",
                                       "Bars Solid",
                                       "Bars Moving"};

    std::vector<LightsButton *> buttons = {ui->glimmerButton,
                                           ui->fadeButton,
                                           ui->randomIndividualButton,
                                           ui->randomSolidButton,
                                           ui->barsSolidButton,
                                           ui->barsMovingButton};

    mRoutineButtons = std::shared_ptr<std::vector<LightsButton*> >(new std::vector<LightsButton*>(6, nullptr));
    int routineIndex = (int)ELightingRoutine::eMultiGlimmer;
    for (int i = 0; i < 6; ++i) {
        (*mRoutineButtons.get())[i] = buttons[i];
        (*mRoutineButtons.get())[i]->setupAsLabeledButton(QString::fromStdString(labels[i]), (ELightingRoutine)(routineIndex + i), mData, EColorGroup::eCustom);
        connect((*mRoutineButtons.get())[i], SIGNAL(buttonClicked(int, int)), this, SLOT(routineButtonClicked(int, int)));
   }
}

void CustomColorsPage::highlightRoutineButton(ELightingRoutine routine) {

    for (uint i = 0; i < mRoutineButtons->size(); i++) {
        QPushButton* button = (*mRoutineButtons.get())[i]->button;
        button->setChecked(false);
    }

    if (routine == ELightingRoutine::eMultiFade) {
        ui->fadeButton->button->setChecked(true);
    } else if (routine == ELightingRoutine::eMultiGlimmer) {
        ui->glimmerButton->button->setChecked(true);
    } else if (routine == ELightingRoutine::eMultiRandomSolid) {
        ui->randomSolidButton->button->setChecked(true);
    } else if (routine == ELightingRoutine::eMultiRandomIndividual) {
        ui->randomIndividualButton->button->setChecked(true);
    } else if (routine == ELightingRoutine::eMultiBarsSolid) {
        ui->barsSolidButton->button->setChecked(true);
    } else if (routine == ELightingRoutine::eMultiBarsMoving) {
        ui->barsMovingButton->button->setChecked(true);
    }

    if (mData->currentColorGroup() == EColorGroup::eCustom) {
        for (int i = (int)ELightingRoutine::eSingleGlimmer + 1; i < (int)ELightingRoutine::eLightingRoutine_MAX; i++) {
            mIconData.setLightingRoutine((ELightingRoutine)i, EColorGroup::eCustom);
            (*mRoutineButtons.get())[i - (int)ELightingRoutine::eSingleGlimmer - 1]->button->setIcon(mIconData.renderAsQPixmap());
        }
    }
}

void CustomColorsPage::selectArrayColor(int index) {
    mCurrentColorPickerIndex = index;
    ui->colorPicker->chooseColor(mData->colorGroup(EColorGroup::eCustom)[mCurrentColorPickerIndex], false);
    updateColorArray();
    for (int i = 0; i < mData->groupSize(EColorGroup::eCustom); ++i) {
        (*mArrayColorsButtons.get())[i]->setChecked(false);
        (*mArrayColorsButtons.get())[i]->setStyleSheet("border:none");
    }

    (*mArrayColorsButtons.get())[mCurrentColorPickerIndex]->setChecked(true);
    (*mArrayColorsButtons.get())[mCurrentColorPickerIndex]->setStyleSheet("border: 2px solid white");
}

// ----------------------------
// Slots
// ----------------------------

void CustomColorsPage::customColorCountChanged(int newCount) {
    int colorCount = newCount / 10;
    if (colorCount != mData->groupSize(EColorGroup::eCustom)) {
        mData->customColorsUsed(colorCount);
        updateColorArray();
        updateIcons();
        mComm->sendCustomArrayCount(mData->groupSize(EColorGroup::eCustom));
        emit updateMainIcons();
    }
}


void CustomColorsPage::modeChanged(int newMode) {
    mData->currentRoutine((ELightingRoutine)newMode);
    mData->currentColorGroup(EColorGroup::eCustom);
    mComm->sendRoutineChange(mData->currentRoutine(), (int)EColorGroup::eCustom);
    highlightRoutineButton(mData->currentRoutine());
    emit updateMainIcons();
}

void CustomColorsPage::colorChanged(QColor color) {
    mData->customColor(mCurrentColorPickerIndex, color);
    mComm->sendArrayColorChange(mCurrentColorPickerIndex, color);
    ui->arraySlider->setSliderColorBackground(mData->colorsAverage(EColorGroup::eCustom));
    updateIcons();
    emit updateMainIcons();
}


void CustomColorsPage::routineButtonClicked(int newRoutine, int newMode) {
    mData->currentRoutine((ELightingRoutine)newRoutine);
    mData->currentColorGroup(EColorGroup::eCustom);
    mComm->sendRoutineChange(mData->currentRoutine(), (int)EColorGroup::eCustom);
    highlightRoutineButton(mData->currentRoutine());
    emit updateMainIcons();
}

// ----------------------------
// Protected
// ----------------------------

void CustomColorsPage::showEvent(QShowEvent *event) {
  Q_UNUSED(event);
  // update all the mode icons to the current colors.
  updateIcons();

  // highlight the current mode, if its in this page
  highlightRoutineButton(mData->currentRoutine());

  // slider can't be set until mData is set up,
  // TODO: remove this dependency. root cause is probably
  // a signal sent out down the line.
  if (ui->arraySlider->slider->value() < 20) {
    ui->arraySlider->slider->setValue(20);
  }

  selectArrayColor(mCurrentColorPickerIndex);
}

// ----------------------------
// Private
// ----------------------------

void CustomColorsPage::updateIcons() {
    for (int i = 0; i < mData->groupSize(EColorGroup::eCustom); ++i) {
        mIconData.setSolidColor(mData->colorGroup(EColorGroup::eCustom)[i]);
        (*mArrayColorsButtons.get())[i]->setEnabled(true);
        (*mArrayColorsButtons.get())[i]->setIcon(mIconData.renderAsQPixmap());
    }

    for (int i = mData->groupSize(EColorGroup::eCustom); i < mData->maxColorGroupSize(); ++i) {
        (*mArrayColorsButtons.get())[i]->setIcon(mGreyIcon.renderAsQPixmap());
        (*mArrayColorsButtons.get())[i]->setEnabled(false);
    }

    mIconData = IconData(80, 80, mData);
    for (int i = (int)ELightingRoutine::eSingleGlimmer + 1; i < (int)ELightingRoutine::eLightingRoutine_MAX; i++) {
        mIconData.setLightingRoutine((ELightingRoutine)i, EColorGroup::eCustom);
        (*mRoutineButtons.get())[i - (int)ELightingRoutine::eSingleGlimmer - 1]->button->setIcon(mIconData.renderAsQPixmap());
    }
}

void CustomColorsPage::updateColorArray() {
    for (int i = 0; i < mData->groupSize(EColorGroup::eCustom); ++i) {
        (*mArrayColorsButtons.get())[i]->setEnabled(true);
        mIconData.setSolidColor(mData->colorGroup(EColorGroup::eCustom)[i]);
        (*mArrayColorsButtons.get())[i]->setIcon(mIconData.renderAsQPixmap());
    }

    for (int i = mData->groupSize(EColorGroup::eCustom); i < mData->maxColorGroupSize(); ++i) {
        (*mArrayColorsButtons.get())[i]->setIcon(mGreyIcon.renderAsQPixmap());
        (*mArrayColorsButtons.get())[i]->setEnabled(false);
    }
     ui->arraySlider->setSliderColorBackground(mData->colorsAverage(EColorGroup::eCustom));
}
