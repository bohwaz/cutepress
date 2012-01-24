/*
 * UiData.h
 *
 *  Created on: 8.9.2010
 *      Author: sber
 */

#ifndef UIDATA_H_
#define UIDATA_H_

#include <QDataStream>

class UiData
    {
public:
    UiData();
    virtual ~UiData();
    
    bool videoNoteHasBeenShown() { return videoNoteShown; }
    void setVideoNoteShown() { videoNoteShown = true; Save(); }
    
private:
    
    Q_DISABLE_COPY(UiData)
    
    void Save();
    
    bool videoNoteShown;
    
    static const int version;
    static const QString filename;
    
    friend QDataStream & operator<< (QDataStream &out, const UiData & data);
    friend QDataStream &  operator>> (QDataStream &in, UiData & data);
    };

QDataStream & operator<< (QDataStream &out, const UiData & data);
QDataStream & operator>> (QDataStream &in, UiData & data);

#endif /* UIDATA_H_ */
