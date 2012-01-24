/*
 * WPXmlRsdParser.cpp
 *
 *  Created on: 13.11.2009
 *      Author: jkal
 */

#include <QDomDocument>
#include <QString>

#include "WPXmlRsdParser.h"
#include "WPData.h"

WPXmlRsdParser::WPXmlRsdParser(const QByteArray& aContent, QList<WPDataRsdApi>& aList)
    : iList(aList)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument doc;
    const int error = doc.setContent(aContent, &errorStr, &errorLine, &errorColumn);
    if (!error)
    {
        aList.clear();
        return;
    }

    QDomElement root = doc.documentElement();
    if (root.tagName() != QLatin1String("rsd"))
    {
        aList.clear();
        return;
    }

    ParseRsdElement(root);
}

WPXmlRsdParser::~WPXmlRsdParser()
{
}

void WPXmlRsdParser::ParseRsdElement(const QDomElement& aElement)
{
    QDomNode child = aElement.firstChild();
    while (!child.isNull())
    {
        if (child.toElement().tagName() == QLatin1String("service"))
            ParseServiceElement(child.toElement());
        child = child.nextSibling();
    }
}

void WPXmlRsdParser::ParseServiceElement(const QDomElement& aElement)
{
    QDomNode child = aElement.firstChild();
    while (!child.isNull())
    {
        if (child.toElement().tagName() == QLatin1String("apis"))
            ParseApisElement(child.toElement());
        child = child.nextSibling();
    }
}

void WPXmlRsdParser::ParseApisElement(const QDomElement& aElement)
{
    QDomNode child = aElement.firstChild();
    while (!child.isNull())
    {
        if (child.toElement().tagName() == QLatin1String("api"))
            ParseApiElement(child.toElement());
        child = child.nextSibling();
    }
}

void WPXmlRsdParser::ParseApiElement(const QDomElement& aElement)
{
    WPDataRsdApi item;
    if (aElement.hasAttribute("apiLink"))
        item.iApiLink = aElement.attribute("apiLink");
    if (aElement.hasAttribute("blogID"))
        item.iBlogId = aElement.attribute("blogID");
    if (aElement.hasAttribute("preferred"))
        item.iPreferred = aElement.attribute("preferred");
    if (aElement.hasAttribute("name"))
        item.iName = aElement.attribute("name");
    iList.append(item);
}
