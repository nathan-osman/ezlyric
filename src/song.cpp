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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include "song.h"

static const char *KeyNumber = "number";
static const char *KeyTitle = "title";
static const char *KeyAuthor = "author";
static const char *KeyLyrics = "lyrics";

Song::Song(QObject *parent)
    : QObject(parent),
      mNumber(0)
{
}

bool Song::loadFromFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        mError = file.errorString();
        return false;
    }

    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    if (document.isNull()) {
        mError = error.errorString();
        return false;
    }

    QJsonObject object = document.object();
    mNumber = object[KeyNumber].toInt();
    mTitle = object[KeyTitle].toString();
    mAuthor = object[KeyAuthor].toString();

    QJsonObject lyrics = object[KeyLyrics].toObject();
    for (QJsonObject::const_iterator i = lyrics.constBegin(); i != lyrics.constEnd(); ++i) {
        mLyrics.insert(i.key(), i.value().toString());
    }

    return true;
}

bool Song::saveToFile(const QString &filename)
{
    QJsonObject lyrics;
    for (QStringMap::const_iterator i = mLyrics.constBegin(); i != mLyrics.constEnd(); ++i) {
        lyrics[i.key()] = i.value();
    }

    QJsonObject object{
        { KeyNumber, mNumber },
        { KeyTitle, mTitle },
        { KeyAuthor, mAuthor },
        { KeyLyrics, lyrics }
    };

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        mError = file.errorString();
        return false;
    }

    file.write(QJsonDocument(object).toJson(QJsonDocument::Indented));

    return true;
}
