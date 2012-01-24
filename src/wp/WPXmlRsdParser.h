/*
 * WPXmlRsdParser.h
 *
 *  Created on: 13.11.2009
 *      Author: jkal
 */

#ifndef WPXMLRSDPARSER_H_
#define WPXMLRSDPARSER_H_

#include <QList>

class WPDataRsdApi;
class QString;
class QDomElement;

class WPXmlRsdParser
{
public:
    WPXmlRsdParser(const QByteArray& aContent, QList<WPDataRsdApi>& aList);
    virtual ~WPXmlRsdParser();

    void ParseRsdElement(const QDomElement& aElement);
    void ParseServiceElement(const QDomElement& aElement);
    void ParseApisElement(const QDomElement& aElement);
    void ParseApiElement(const QDomElement& aElement);

private: // Data
    QList<WPDataRsdApi>& iList;
};

#endif /* WPXMLRSDPARSER_H_ */
