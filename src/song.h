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

#ifndef SONG_H
#define SONG_H

#include <QMap>
#include <QObject>

typedef QMap<QString, QStringList> LyricMap;

/**
 * @brief An individual song with lyrics.
 */
class Song : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int number READ number WRITE setNumber)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString author READ author WRITE setAuthor)
    Q_PROPERTY(LyricMap lyrics READ lyrics WRITE setLyrics)

public:

    explicit Song(QObject *parent = nullptr);

    int number() const { return mNumber; }
    QString title() const { return mTitle; }
    QString author() const { return mAuthor; }
    LyricMap lyrics() const { return mLyrics; }

    void setNumber(int number) { mNumber = number; }
    void setTitle(const QString &title) { mTitle = title; }
    void setAuthor(const QString &author) { mAuthor = author; }
    void setLyrics(const LyricMap &lyrics) { mLyrics = lyrics; }

    bool loadFromFile(const QString &filename);
    bool saveToFile(const QString &filename);

    QString errorString() const { return mError; }

private:

    int mNumber;
    QString mTitle;
    QString mAuthor;
    LyricMap mLyrics;
    QString mError;
};

#endif // SONG_H
