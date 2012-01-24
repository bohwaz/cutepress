/**
 * QXmlStreamWriter-based XML generators for the outgoing HTTP XMLRPC requests
 */

#include <QXmlStreamWriter>
#include <QBuffer>
#include <QStringList>
#include "WPDataCustomField.h"

/**
 * An outgoing XML-RPC method call
 */
class WPXmlRequest : public QXmlStreamWriter
{
private:
    /** Internal buffer for outgoing data */
    QIODevice * buffer;

public:
    /**
     * Start writing the XML-RPC methodCall document into the given QIODevice.
     */
    WPXmlRequest (QIODevice *out);

private:
    /** Write raw base64 data to buffer */
    void writeRawBase64 (QIODevice &src);

public:    
    /**
     * Start <methodCall> with params
     */
    void startMethodCall (QString methodName);

    /**
     * Write out a parameter
     */
    void writeScalarParam (QString type, QString value);

    /** Write out a <string> parameter */
    inline void writeStringParam (QString value)
    { 
        writeScalarParam("string", value);
    }

    /**
     * Start <struct> parameter with members
     */
    void startStructParam ();

    /**
     * Start <struct> member
     */
    void startStructMember (QString name);

    /**
     * Start <array> member
     */
    void startArrayMember (QString name);

    /**
     * Write out a scalar struct <member>
     */
    void writeScalarMember (QString name, QString type, QString value);

    /**
     * Write out a custom field struct member
     */
    void writeCustomField (const WPDataCustomField& field);

    /** Write out a <string> member */
    inline void writeStringMember (QString name, QString value)
    { 
        writeScalarMember(name, "string", value); 
    }

    /** Write out a <base64> member from the given QIODevice */
    void writeBase64Member (QString name, QIODevice &src);

    /** Write out an <array> member with the given raw item data and type */
    void writeScalarArrayMember (QString name, QString type, QStringList values);

	/** Write out an <array> with given string items. */
    void writeStringArray(const QStringList& arrayContent);

    /**
     * Finish the <struct> parameter
     */
    void endStructParam ();

    /**
     * Finish the <struct> member
     */
    void endStructMember ();
    
    /** 
     * End the <array> member
     */
    void endArrayMember ();

    /**
     * Finish the <methodCall>
     */
    void endMethodCall ();
};
