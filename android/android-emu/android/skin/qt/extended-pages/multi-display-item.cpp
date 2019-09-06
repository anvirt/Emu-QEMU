// Copyright (C) 2019 The Android Open Source Project
//
// This software is licensed under the terms of the GNU General Public
// License version 2, as published by the Free Software Foundation, and
// may be copied, distributed, and modified under those terms.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#include "multi-display-item.h"
#include "multi-display-page.h"
#include "android/skin/qt/emulator-qt-window.h"
#include "android/skin/qt/extended-pages/common.h"

std::vector<MultiDisplayItem::displayType> MultiDisplayItem::sDisplayTypes =
    {{"480p(720x480)", "480p", 720, 480, 142},
     {"720p(1280x720)", "720p", 1280, 720, 213},
     {"1080p(1920x1080)", "1080p", 1920, 1080, 320},
     {"4K(3840x2160)", "4K", 3840, 2160, 320},
     {"4K upscaled(3840x2160)", "4K upscaled", 3840, 2160, 640},
     {"custom", "custom", 720, 480, 142},
    };

MultiDisplayItem::MultiDisplayItem(int id, MultiDisplayPage* page, QWidget* parent)
    : QWidget(parent), mMultiDisplayPage(page),
      mUi(new Ui::MultiDisplayItem()), mId(id) {

    mUi->setupUi(this);

    // Initialize the drop-down menu
    for (auto const& type : sDisplayTypes) {
        mUi->selectDisplayType->addItem(type.name.c_str());
    }
    mUi->width->setMinValue(0);
    mUi->width->setMaxValue(2160);
    mUi->height->setMinValue(0);
    mUi->height->setMaxValue(4096);
    mUi->dpi->setMinValue(120);
    mUi->dpi->setMaxValue(640);

    mCurrentIndex = 1; /* 720p as default */
    mUi->selectDisplayType->setCurrentIndex(mCurrentIndex);
    setValues(mCurrentIndex);
    connect(mUi->selectDisplayType, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onDisplayTypeChanged(int)));
    connect(this, SIGNAL(changeDisplay(int)), mMultiDisplayPage,
            SLOT(changeSecondaryDisplay(int)));
    connect(this, SIGNAL(deleteDisplay(int)), mMultiDisplayPage,
            SLOT(deleteSecondaryDisplay(int)));
    connect(mUi->height, SIGNAL(enterPressed()), this, SLOT(onCustomParaChanged()));
    connect(mUi->width, SIGNAL(enterPressed()), this, SLOT(onCustomParaChanged()));
    connect(mUi->dpi, SIGNAL(enterPressed()), this, SLOT(onCustomParaChanged()));
    mUi->deleteDisplay->setIcon(getIconForCurrentTheme("close"));
    std::string s = "Display " + std::to_string(mId);
    mUi->display_title->setText(s.c_str());

    // hide the configuration box for resolution and dpi
    hideWidthHeightDpiBox(true);
}

MultiDisplayItem::MultiDisplayItem(int id, uint32_t width, uint32_t height,
                                   uint32_t dpi, MultiDisplayPage* page,
                                   QWidget* parent)
    : MultiDisplayItem(id, page, parent) {
    // check with displayType fits the width, height and dpi, then set to it.
    // If not found, set to "custom" type.
    int i = 0;
    for ( ; i < sDisplayTypes.size() - 1 ; i++) {
        if (sDisplayTypes[i].width == width && sDisplayTypes[i].height == height &&
            sDisplayTypes[i].dpi == dpi) {
            break;
        }
    }
    mCurrentIndex =  i;
    if (mUi->selectDisplayType->currentIndex() != mCurrentIndex) {
        mUi->selectDisplayType->setCurrentIndex(mCurrentIndex);
        onDisplayTypeChanged(mCurrentIndex);
    }
}

void MultiDisplayItem::onDisplayTypeChanged(int index) {
    if (index == sDisplayTypes.size() - 1) {
        uint32_t w, h, dpi;
        EmulatorQtWindow::getInstance()->getMultiDisplay(mId,
                                                         NULL, NULL,
                                                         &w, &h, &dpi,
                                                         NULL, NULL);
        mUi->height->setValue(h);
        mUi->width->setValue(w);
        mUi->dpi->setValue(dpi);
        hideWidthHeightDpiBox(false);
    } else {
        hideWidthHeightDpiBox(true);
        setValues(index);
    }
    mCurrentIndex = index;
    emit changeDisplay(mId);
}

void MultiDisplayItem::onCustomParaChanged() {
    uint32_t w, h, d;
    getValues(&w, &h, &d);
    if (EmulatorQtWindow::getInstance()->multiDisplayParamValidate(mId, w, h, d, 0)) {
        emit changeDisplay(mId);
    }
}

void MultiDisplayItem::on_deleteDisplay_clicked() {
    emit deleteDisplay(mId);
}

void MultiDisplayItem::setValues(int index) {
    if (index >= sDisplayTypes.size()) {
        return;
    }
    mUi->width->setValue(sDisplayTypes[index].width);
    mUi->height->setValue(sDisplayTypes[index].height);
    mUi->dpi->setValue(sDisplayTypes[index].dpi);
}

void MultiDisplayItem::getValues(uint32_t* width, uint32_t* height, uint32_t* dpi) {
    if (width) {
        *width = (uint32_t)mUi->width->value();
    }
    if (height) {
        *height = (uint32_t)mUi->height->value();
    }
    if (dpi) {
        *dpi = (uint32_t)mUi->dpi->value();
    }
}

std::string& MultiDisplayItem::getName() {
    return sDisplayTypes[mCurrentIndex].shortName;
}

void MultiDisplayItem::hideWidthHeightDpiBox(bool hide) {
    mUi->height->setHidden(hide);
    mUi->heightTitle->setHidden(hide);
    mUi->width->setHidden(hide);
    mUi->widthTitle->setHidden(hide);
    mUi->dpi->setHidden(hide);
    mUi->dpiTitle->setHidden(hide);
}

void MultiDisplayItem::focusInEvent(QFocusEvent* event) {
    mUi->selectDisplayType->setStyleSheet("border: 1px solid blue;");
    mMultiDisplayPage->setArrangementHighLightDisplay(mId);
}

void MultiDisplayItem::focusOutEvent(QFocusEvent* event) {
    mUi->selectDisplayType->setStyleSheet("border: none;");
    mMultiDisplayPage->setArrangementHighLightDisplay(-1);
}