/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <QDialogButtonBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QInputDialog>
#include <QLabel>
#include <QList>
#include <QListWidgetItem>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>

#include "sizehintwidget.h"
#include "songeditor.h"

SongEditor::SongEditor(Song *song, QWidget *parent)
    : QDialog(parent),
      mSong(song),
      mPartMenu(new QMenu(this)),
      mNumber(new QSpinBox),
      mTitle(new QLineEdit(song->title())),
      mAuthor(new QLineEdit(song->author())),
      mPartList(new QListWidget),
      mPartEditor(new QTextEdit)
{
    // Add menu items for common parts
    foreach (const QString &part, QStringList({"V1", "V2", "V3", "V4", "V5", "V6", "C", "B"})) {
        mPartMenu->addAction(part, [this, part]() {
            QListWidgetItem *item = new QListWidgetItem(part);
            mPartList->addItem(item);
            mPartList->setCurrentItem(item);
            mPartEditor->setFocus();
        });
    }

    // Add a menu item for custom parts
    mPartMenu->addSeparator();
    mPartMenu->addAction(tr("&Custom..."), [this]() {
        QString text = QInputDialog::getText(this, tr("Custom Part"), tr("Enter name of custom part:"));
        if (!text.isNull()) {
            QListWidgetItem *item = new QListWidgetItem(text);
            mPartList->addItem(item);
            mPartList->setCurrentItem(item);
            mPartEditor->setFocus();
        }
    });

    mNumber->setMaximum(999);
    mNumber->setValue(song->number());

    // Initialize the part list
    mPartList->setSortingEnabled(true);
    connect(mPartList, &QListWidget::currentItemChanged, [this](QListWidgetItem *current, QListWidgetItem *previous) {
        if (previous) {
            previous->setData(Qt::UserRole, mPartEditor->toPlainText());
        }
        mPartEditor->setPlainText(current->data(Qt::UserRole).toString());
    });

    mPartEditor->setAcceptRichText(false);

    // Add existing parts
    for (QStringMap::const_iterator i = song->lyrics().constBegin(); i != song->lyrics().constEnd(); ++i) {
        QListWidgetItem *item = new QListWidgetItem(i.key());
        item->setData(Qt::UserRole, i.value());
        mPartList->addItem(item);
    }

    mPartList->setCurrentRow(0);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SongEditor::onAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SongEditor::close);

    QGridLayout *propLayout = new QGridLayout;
    propLayout->addWidget(new QLabel(tr("Number:")), 0, 0);
    propLayout->addWidget(mNumber, 0, 1);
    propLayout->addWidget(new QLabel(tr("Title:")), 1, 0);
    propLayout->addWidget(mTitle, 1, 1);
    propLayout->addWidget(new QLabel(tr("Author:")), 2, 0);
    propLayout->addWidget(mAuthor, 2, 1);

    QPushButton *partAdd = new QPushButton;
    partAdd->setIcon(QIcon(":/img/add.png"));
    partAdd->setMenu(mPartMenu);

    QPushButton *partRemove = new QPushButton;
    partRemove->setIcon(QIcon(":/img/remove.png"));
    connect(partRemove, &QPushButton::clicked, [this]() {
        int row = mPartList->currentRow();
        if (row != -1) {
            delete mPartList->takeItem(row);
        }
    });

    QHBoxLayout *partListButtonLayout = new QHBoxLayout;
    partListButtonLayout->addWidget(partAdd);
    partListButtonLayout->addWidget(partRemove);

    QVBoxLayout *partListLayout = new QVBoxLayout;
    partListLayout->setMargin(0);
    partListLayout->addWidget(mPartList, 1);
    partListLayout->addLayout(partListButtonLayout);

    SizeHintWidget *splitterWidget = new SizeHintWidget(100, 100);
    splitterWidget->setLayout(partListLayout);

    QSplitter *partSplitter = new QSplitter;
    partSplitter->setChildrenCollapsible(false);
    partSplitter->addWidget(splitterWidget);
    partSplitter->setStretchFactor(0, 0);
    partSplitter->addWidget(mPartEditor);
    partSplitter->setStretchFactor(1, 1);

    QFrame *frame = new QFrame();
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);

    QVBoxLayout *dialogLayout = new QVBoxLayout();
    dialogLayout->addLayout(propLayout);
    dialogLayout->addWidget(new QLabel(tr("Lyrics:")));
    dialogLayout->addWidget(partSplitter, 1);
    dialogLayout->addWidget(frame);
    dialogLayout->addWidget(buttonBox);
    setLayout(dialogLayout);

    resize(600, 800);
    setWindowTitle(tr("Add / Edit Song"));
}

void SongEditor::onAccepted()
{
    mSong->setNumber(mNumber->value());
    mSong->setTitle(mTitle->text());
    mSong->setAuthor(mAuthor->text());

    QStringMap lyrics;
    for (int i = 0; i < mPartList->count(); ++i) {
        auto item = mPartList->item(i);
        lyrics.insert(item->text(), item->data(Qt::UserRole).toString());
    }

    mSong->setLyrics(lyrics);

    close();
}
