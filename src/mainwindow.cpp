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

#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QVBoxLayout>

#include "mainwindow.h"

const QString SettingGeometry("geometry");
const QString SettingWindowState("windowState");

const QString LargeButtonStylesheet("QPushButton{padding: 16px 0;}");
const QString LargeLabelStylesheet("QLabel{font-size: 12pt; font-weight: bold;}");

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      mFileContent(nullptr),
      mOutputFile(new QLabel(tr("[empty]"))),
      mShowText(nullptr),
      mClearLine(nullptr),
      mNextLine(nullptr)
{
    mFileContent = new QListWidget();

    auto loadFile = new QPushButton(tr("Load..."));
    loadFile->setStyleSheet(LargeButtonStylesheet);
    connect(loadFile, &QPushButton::clicked, this, &MainWindow::onLoadFileClicked);

    // Output selection
    auto setOutput = new QPushButton(tr("Set output file..."));
    connect(setOutput, &QPushButton::clicked, this, &MainWindow::onSetOutputClicked);

    QHBoxLayout *outputLayout = new QHBoxLayout();
    outputLayout->addWidget(mOutputFile, 1);
    outputLayout->addWidget(setOutput, 0);

    // Action buttons
    mShowText = new QPushButton(tr("Show Text..."));
    mShowText->setEnabled(false);
    mShowText->setStyleSheet(LargeButtonStylesheet);
    connect(mShowText, &QPushButton::clicked, this, &MainWindow::onShowTextClicked);

    mClearLine = new QPushButton(tr("Clear"));
    mClearLine->setEnabled(false);
    mClearLine->setStyleSheet(LargeButtonStylesheet);
    connect(mClearLine, &QPushButton::clicked, this, &MainWindow::onClearLineClicked);

    mNextLine = new QPushButton(tr("Next"));
    mNextLine->setEnabled(false);
    mNextLine->setStyleSheet(LargeButtonStylesheet);
    connect(mNextLine, &QPushButton::clicked, this, &MainWindow::onNextLineClicked);

    auto actionLayout = new QHBoxLayout();
    actionLayout->addWidget(mShowText);
    actionLayout->addWidget(mClearLine);
    actionLayout->addWidget(mNextLine);

    auto contentLabel = new QLabel(tr("Lyric Content"));
    contentLabel->setStyleSheet(LargeLabelStylesheet);

    auto outputLabel = new QLabel(tr("Output File"));
    outputLabel->setStyleSheet(LargeLabelStylesheet);

    // Main layout for the application
    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addWidget(contentLabel);
    vboxLayout->addWidget(mFileContent);
    vboxLayout->addWidget(loadFile);
    vboxLayout->addWidget(outputLabel);
    vboxLayout->addLayout(outputLayout);
    vboxLayout->addLayout(actionLayout);

    QWidget *widget = new QWidget;
    widget->setLayout(vboxLayout);
    setCentralWidget(widget);

    // Load the previous window state
    QSettings settings;
    restoreGeometry(settings.value(SettingGeometry).toByteArray());
    restoreState(settings.value(SettingWindowState).toByteArray());

    setWindowIcon(QIcon(":/logo.png"));
    setWindowTitle(tr("EZLyric"));
}

void MainWindow::onLoadFileClicked()
{
    auto filename = QFileDialog::getOpenFileName(this, tr("Load File"));
    if (!filename.isNull()) {
        QFile file(filename);
        if (file.open(QIODevice::ReadOnly)) {
            mFileContent->clear();
            foreach(auto line, QString::fromUtf8(file.readAll()).split("\n")) {
                line = line.trimmed();
                if (!line.isEmpty()) {
                    mFileContent->addItem(line);
                }
            }
        } else {
            QMessageBox::critical(this, tr("Error"), file.errorString());
        }
    }
}

void MainWindow::onSetOutputClicked()
{
    auto filename = QFileDialog::getSaveFileName(this, tr("Set Output File"));
    if (!filename.isNull()) {
        mOutputFile->setText(filename);
        mOutputFileName = filename;
        mShowText->setEnabled(true);
        mClearLine->setEnabled(true);
        mNextLine->setEnabled(true);
    }
}

void MainWindow::onShowTextClicked()
{
    auto text = QInputDialog::getText(this, tr("Input"), tr("Enter text to display below:"));
    if (!text.isNull()) {
        outputLine(text);
    }
}

void MainWindow::onClearLineClicked()
{
    outputLine("");
}

void MainWindow::onNextLineClicked()
{
    int row = mFileContent->currentRow();
    if (row == -1) {
        if (mFileContent->count()) {
            mFileContent->setCurrentRow(row = 0);
        } else {
            return;
        }
    }

    outputLine(mFileContent->currentItem()->text());
    mFileContent->setCurrentRow(row + 1);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue(SettingGeometry, saveGeometry());
    settings.setValue(SettingWindowState, saveState());
    QMainWindow::closeEvent(event);
}

void MainWindow::outputLine(const QString &line)
{
    QFile file(mOutputFileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(line.toUtf8());
        file.close();
    } else {
        QMessageBox::critical(this, tr("Error"), file.errorString());
    }
}
