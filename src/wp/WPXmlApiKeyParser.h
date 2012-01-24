#ifndef WPXMLAPIKEYPARSER_H
#define WPXMLAPIKEYPARSER_H

#include <QXmlStreamReader>

class WPXmlApiKeyParser : public QXmlStreamReader
{
public:
    explicit WPXmlApiKeyParser(const QByteArray &response);
    virtual ~WPXmlApiKeyParser();

    void ParseResponse();

    QString GetApiKey() const;

private:
    QString apiKey;

};

#endif // WPXMLAPIKEYPARSER_H
