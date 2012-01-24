/*
 * WPXmlStatsParser.h
 *
 *  Created on: 15.3.2010
 *      Author: aahv
 */

#ifndef WPXMLSTATSPARSER_H
#define WPXMLSTATSPARSER_H

#include <QXmlStreamReader>

#include "WPData.h"

class WPXmlStatsParser : public QXmlStreamReader
{
public:
    WPXmlStatsParser(const QByteArray &response, WPDataStats& aStats);
    virtual ~WPXmlStatsParser();

    void ParseResponse();

protected:
    void HandleViewsTag();
    void HandleSearchTermsTag();
    void HandleReferrersTag();

private:
    WPDataStats& iStats;
    QString iResponse;

};

#endif // WPXMLSTATSPARSER_H
