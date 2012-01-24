/*
 * UiData.cpp
 *
 *  Created on: 8.9.2010
 *      Author: sber
 */

#include "UiData.h"

#include <QFile>

#include "utils.h"

const int UiData::version = 1;
const QString UiData::filename("ui_data.txt");

UiData::UiData()
  : videoNoteShown(false)
    {
    QFile file(filename);
    if (!Utils::openLocalDataFile(file, QIODevice::ReadOnly))
        return;

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_6);
    in >> *this;
    }

UiData::~UiData()
    {
    }

void UiData::Save()
    {
    QFile file(filename);
    if (!Utils::openLocalDataFile(file, QIODevice::WriteOnly))
        return;

    // serialize our data
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_6);

    out << *this;
    }

QDataStream & operator<< (QDataStream &out, const UiData & data)
    {
    out << data.version;
    out << data.videoNoteShown;
    
    return out;
    }

QDataStream & operator>> (QDataStream &in, UiData & data)
    {
    int version;
    
    in >> version;
    in >> data.videoNoteShown;
    
    return in;
    }
