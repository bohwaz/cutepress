/**
 * QXmlStreamReader-based XML parser for incoming HTTP XMLRPC replies.
 */
#ifndef WPXMLRESPONSE_H
#define WPXMLRESPONSE_H

#include <QXmlStreamReader>
#include <QMap>
#include <QStringList>
#include "WPDataCustomField.h"

/**
 * A raw item struct as recieved from XML RPC
 */
struct WPXmlItem
{
    /** Scalar items */
    QMap<QString, QString> content;

    /** Scalar array items */
    QMap<QString, QStringList> listContent;

    /** Custom field items */
    QList<WPDataCustomField> customFields;
};

/**
 * An incoming XML-RPC method response
 */
class WPXmlResponse : public QXmlStreamReader
{
public:
    /**
     * Parse the response from the given QByteArray.
     */
    WPXmlResponse (const QByteArray &data);

    /**
     * Parse the response from the given QIODevice
     */
    WPXmlResponse (QIODevice *device);

private:
    /**
     * Read up to the end tag of the current tag.
     */
    bool readNextEndElement ();
    
    /**
     * Ensure that the current token is the given start tag
     */
    bool checkStartTag (QString tag);

    /**
     * Ensure that the current token is the given end tag
     */
    bool checkEndTag (QString tag);

    /**
     * Read up to the start of the given tag.
     */
    bool startLiteralTag (QString tag);

    /**
     * Read up to the start of the next tag, giving its name.
     */
    bool startTagName (QString *tag);

    /**
     * Read up to the end of the given tag.
     */
    bool endLiteralTag (QString tag);

    /**
     * Read up to the start of the next param's value
     */
    bool startGenericParam ();

    /**
     * Read up to the end of the current param.
     */
    bool endGenericParam ();

    /**
     * Read up to the end of the current tag, giving its contents
     */
    bool readTagContent (QString *content);

    /**
     * Read up to the end of the next tag, giving the tag name and contents.
     */
    bool readTag (QString *tag, QString *content);

    /**
     * Read up to the end of the given next tag, giving its contents.
     */
    bool readLiteralTag (QString tag, QString *content);

    /**
     * Read up to the following <struct> tag
     */
    bool startStruct ();

public:
/* General */
    /**
     * Returns a string describing the condition that caused a method to return false.
     */
    QString getError ();

/* Top-level */
    /**
     * Reads the following <methodResponse> up to the starting <params> tag.
     */
    bool startMethodResponse (bool *fault);

    /**
     * Reads the entire current <fault> tag up to the ending </methodResponse> tag
     */
    bool readFault (int *faultCode, QString *faultString);

    /**
     * Reads the following </params> and </methodResponse> tags
     */
    bool endMethodResponse ();

/* Scalar params */
    /**
     * Read the next <param>, giving the type and value.
     */
    bool readScalarParam (QString *type, QString *value);

    /**
     * Read the next <param>, giving the value as a string, ignoring the type.
     */
    inline bool readRawParam (QString *value)
    {
        return readScalarParam(NULL, value);
    }

    /**
     * Read the next bool <param>, giving its value as a bool.
     */
    bool readBoolParam (bool *value);

    /**
     * Read the next bool <param>, returning its value.
     */
    inline bool readSuccessParam ()
    {
        bool success;

        return readBoolParam(&success) && success;
    }

/* Array params */
    /**
     * Read the entire array param as a QList of WPXmlItems
     */
    bool readStructArrayParam (QList<WPXmlItem> *items);

    /**
     * Read up to the <data> tag of the next param
     */
    bool startArrayParam ();

    /**
     * Read up from the current <array> tag to the </array> tag, returning the scalar values
     */
    bool readScalarArray (QStringList *values);
    
    /**
     * Read up from the current <array> tag to the </array> tag and add all the custom fields found into the item
     */
    bool readCustomFieldsArray (WPXmlItem* item);

    /**
     * Read up to the <value> tag of the next array item, or the </data> tag.
     */
    bool nextArrayItem ();

    /**
     * Read up to the <struct> tag of the next array item.
     */
    bool startStructItem ();

    /**
     * Read to the ending </value> from the </struct> tag of the current struct item
     */
    bool endStructItem ();

    /**
     * Read to the end of the current array param.
     */
    bool endArrayParam ();

/* Structs */
    /**
     * Read the current struct as a WPXmlItem, returning the member names and raw values.
     */
    bool readStruct (WPXmlItem *item);

    /**
     * Read the following struct param as a WPXmlItem.
     */
    bool readStructParam (WPXmlItem *item);

    /**
     * Read the following scalar struct param as a QMap
     */
    bool readScalarStructParam (QMap<QString, QString> *content);

    /**
     * Read scalar scruct of structs.
     */
    bool readStructOfScalarStructParam(QMap< QString, QMap<QString,QString> > *map);

    /**
     * Read a single custom field struct into an item
     */
    bool readCustomFieldsStruct (WPXmlItem* item);

    /**
     * Read up to the <struct> tag of the next param
     */
    bool startStructParam ();

    /**
     * Read up to the <member> tag of the next struct member, or the </struct> tag
     */
    bool nextStructMember ();

    /**
     * Read up to the end of the current member, returning its name, scalar type and value
     */
    bool readScalarMember (QString *name, QString *type, QString *value);

    /**
     * Read up to the <value> tag of the current member, returning its name and type
     */
    bool startMember (QString *name, QString *type);

    /**
     * Read up from the current </value> tag to the end of the current member
     */
    bool endMember ();

    /**
     * Read up to the end of the current member, returning the raw scalar value
     */
    bool finishScalarMember (QString *value);

    /**
     * Read up to the end of the current array member, returning a list of the raw scalar values
     */
    bool finishArrayMember (QStringList *values);
    
    /**
     * Read up to the end of the current custom fields array, adding all the fields found into item given as an argument
     */
     bool finishCustomFieldsArray (WPXmlItem* item);

    /**
     * Read up to the end of the current struct param.
     */
    bool endStructParam ();
};

#endif
