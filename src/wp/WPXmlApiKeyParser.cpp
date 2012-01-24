#include "WPXmlApiKeyParser.h"

WPXmlApiKeyParser::WPXmlApiKeyParser(const QByteArray &response) :
    QXmlStreamReader(response)
{
}

WPXmlApiKeyParser::~WPXmlApiKeyParser() {}

void WPXmlApiKeyParser::ParseResponse()
{
    if (readNextStartElement()) {
        if (name() == "userinfo") {
            if (readNextStartElement()) {
                if (name() == "apikey") {
                    qDebug("Api key read successfully");
                    apiKey = readElementText();
                }
            }
        }
    }
}

QString WPXmlApiKeyParser::GetApiKey() const
{
    return apiKey;
}
