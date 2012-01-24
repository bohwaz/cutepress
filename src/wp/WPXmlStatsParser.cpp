/*
 * WPXmlStatsParser.cpp
 *
 *  Created on: 15.3.2010
 *      Author: aahv
 */

#include "WPXmlStatsParser.h"

#include <QDate>

WPXmlStatsParser::WPXmlStatsParser(const QByteArray &response, WPDataStats& aStats) : QXmlStreamReader(response), iStats(aStats), iResponse(response)
{
}

WPXmlStatsParser::~WPXmlStatsParser() {

}

void WPXmlStatsParser::ParseResponse() {
    if (readNextStartElement()) {
        if (name() == QLatin1String("views") || name() == QLatin1String("postviews"))
            HandleViewsTag();
        else if (name() == QLatin1String("searchterms"))
            HandleSearchTermsTag();
        else if (name() == QLatin1String("referrers"))
            HandleReferrersTag();
    }

    if (hasError()) {
        iStats.iLastUpdated = QDate();
        raiseError(iResponse.section('\n', 0, 1));
    }
}

void WPXmlStatsParser::HandleViewsTag() {
    while (readNextStartElement()) {
        if (name() == QLatin1String("day")) {
            QDate date = QDate::fromString(attributes().value("date").toString(), Qt::ISODate);
            int visits = readElementText().toInt();
            iStats.iVisits.insert(date, visits);
        } else if (name() == QLatin1String("total")) {
            int total = 0;
            QMap<QDate, int>::iterator it = iStats.iVisits.begin();
            while (it != iStats.iVisits.end()) {
                if (it.key().daysTo(QDate::currentDate()) > 365)
                    it = iStats.iVisits.erase(it);
                else {
                    total += it.value();
                }

                ++it;
            }

            iStats.iTotalVisits = total;
            iStats.iLastUpdated = QDate::currentDate();
        }
    }
}

void WPXmlStatsParser::HandleSearchTermsTag() {
    iStats.iSearchTerms.clear();
    QDate date;
    int depth = 1;
    readNext();
    while (!atEnd() && depth > 0) {
        if (isStartElement()) {
            if (depth == 1 && name() == QLatin1String("day")) {
                date = QDate::fromString(attributes().value("date").toString(), Qt::ISODate);
                depth++;
            } else if (depth == 2 && name() == QLatin1String("searchterm")) {
                QString value = attributes().value("value").toString();
                int hits = readElementText().toInt();
                iStats.iSearchTerms[date].push_back(QPair<QString, int>(value, hits));
            }
        } else if (isEndElement()) {
            depth--;
        }

        readNext();
    }
}

void WPXmlStatsParser::HandleReferrersTag() {
    iStats.iReferrers.clear();
    QDate date;
    int depth = 1;
    readNext();
    while (!atEnd() && depth > 0) {
        if (isStartElement()) {
            if (depth == 1 && name() == QLatin1String("day")) {
                date = QDate::fromString(attributes().value("date").toString(), Qt::ISODate);
                depth++;
            } else if (depth == 2 && name() == QLatin1String("referrer")) {
                QString value = attributes().value("value").toString();
                int index = value.indexOf("://");
                if (index != -1)
                    value = value.mid(index + 3);
                int hits = readElementText().toInt();
                iStats.iReferrers[date].push_back(QPair<QString, int>(value, hits));
            }
        } else if (isEndElement()) {
            depth--;
        }

        readNext();
    }
}
