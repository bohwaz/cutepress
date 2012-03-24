#include "WPXmlResponse.h"
#include <QDebug>

WPXmlResponse::WPXmlResponse (const QByteArray &data) :
    QXmlStreamReader(data)
{

}
    
WPXmlResponse::WPXmlResponse (QIODevice *device) :
    QXmlStreamReader(device)
{

}

/* Internal API */
bool WPXmlResponse::readNextEndElement ()
{
    skipCurrentElement();

    return true;
}

bool WPXmlResponse::checkStartTag (QString tag)
{
    return isStartElement() && (name() == tag);
}

bool WPXmlResponse::checkEndTag (QString tag)
{
    return isEndElement() && (name() == tag);
}

bool WPXmlResponse::startLiteralTag (QString tag)
{
    return (
            // start tag
            readNextStartElement()

            // with given name
        &&  (name() == tag)
    );
}

bool WPXmlResponse::startTagName (QString *tag)
{
    if (!readNextStartElement())
        return false;

    if (tag)
        *tag = name().toString();

    return true;
}


bool WPXmlResponse::endLiteralTag (QString tag)
{
    return (
            // end tag
            readNextEndElement()

            // with given name
        &&  (name() == tag)
    );
}

bool WPXmlResponse::startGenericParam ()
{
    return (
            startLiteralTag("param")
       &&   startLiteralTag("value")
    );
}

bool WPXmlResponse::endGenericParam ()
{
    return (
            endLiteralTag("value")
        &&  endLiteralTag("param")
    );
}

bool WPXmlResponse::readTagContent (QString *content)
{
    if (content)
        // just include full text in case of child elements
        *content = readElementText(QXmlStreamReader::IncludeChildElements);
    else
        skipCurrentElement();
 
    return true;  
}

bool WPXmlResponse::readTag (QString *tag, QString *content)
{
    return (
            startTagName(tag)
        &&  readTagContent(content)
        &&  (!tag || checkEndTag(*tag))
    );
}

bool WPXmlResponse::readLiteralTag (QString tag, QString *content)
{
    return (
            startLiteralTag(tag)
        &&  readTagContent(content)
    );
}

bool WPXmlResponse::startStruct ()
{
    return (
            startLiteralTag("struct")
    );
}


/* Public API */
QString WPXmlResponse::getError ()
{
    if (hasError())
        return errorString();
    else
        return QObject::tr("Unexpected token: %1:%2").arg(tokenString()).arg(name().toString());
}

bool WPXmlResponse::startMethodResponse (bool *fault)
{
    QString tag; 

    // read <methodResponse> and first tag
    if (
            !startLiteralTag("methodResponse")
        ||  !startTagName(&tag)
    )
        return false;

    // parse tag
    if (tag == "params")
        *fault = false;
    
    else if (tag == "fault")
        *fault = true;

    else
        return false;

    // ok
    return true;
}

bool WPXmlResponse::readFault (int *faultCode, QString *faultString)
{
    // read up to the start of the struct
    if (
            !checkStartTag("fault")
        ||  !startLiteralTag("value")
        ||  !startStruct()
    )
        return false;

    // read each member
    while (nextStructMember()) {
        QString name, value;

        // read member, ignore type
        if (!readScalarMember(&name, NULL, &value))
            return false;

        // store
        if (name == "faultCode")
            *faultCode = value.toInt();

        else if (name == "faultString")
            *faultString = value;

        else
            // just ignore
            continue;
    }

    // finish off the struct and stuff
    if (
            !checkEndTag("struct")
        ||  !endLiteralTag("value")
        ||  !endLiteralTag("fault")
        ||  !endLiteralTag("methodResponse")
    )
        return false;

    // ok
    return true;
}
    
bool WPXmlResponse::endMethodResponse ()
{
    return (
            endLiteralTag("params")
        &&  endLiteralTag("methodResponse")
    );
}

bool WPXmlResponse::readScalarParam (QString *type, QString *value)
{
    return (
            startGenericParam()
        &&  readTag(type, value)
        &&  endGenericParam()
    );
}

bool WPXmlResponse::readBoolParam (bool *value)
{
    QString type;
    QString raw;
    bool ok;

    // check type
    if (!readScalarParam(&type, &raw) || type != "boolean")
        return false;

    // parse
    *value = bool(raw.toInt(&ok, 10));

    return ok;
}

bool WPXmlResponse::readStructArrayParam (QList<WPXmlItem> *items)
{
    if (!startArrayParam())
        return false;

    // each array item
    while (nextArrayItem()) {
        WPXmlItem item;
        
        // read item
        if (
                !startStructItem()
            ||  !readStruct(&item)
            ||  !endStructItem()
        )
            return false;
        
        // store
        items->append(item);
    }

    if (!endArrayParam())
        return false;

    // ok
    return true;
}

bool WPXmlResponse::startArrayParam ()
{
    return (
            startGenericParam()
        &&  startLiteralTag("array")
        &&  startLiteralTag("data")
    );
}
    
bool WPXmlResponse::readScalarArray (QStringList *values)
{
    if (
            !checkStartTag("array")
        ||  !startLiteralTag("data")
    )
        return false;

    // ...<value> or ...</data>
    while (nextArrayItem()) {
        QString value;
    
        if (
                !readTag(NULL, &value)
            ||  !endLiteralTag("value")
        )
            return false;

        values->append(value);
    }

    if (
            !checkEndTag("data")
        ||  !endLiteralTag("array")
    )
        return false;

    // ok
    return true;
}

bool WPXmlResponse::readCustomFieldsArray (WPXmlItem* item)
{
    if(!checkStartTag("array") || !startLiteralTag("data"))
        return false;
        
    while(nextArrayItem()){        
        if(!readCustomFieldsStruct(item))
            return false;
        
        if(!endLiteralTag("value"))
            return false;
    }
    
    if (!checkEndTag("data") || !endLiteralTag("array"))
        return false;
        
    return true;
}

bool WPXmlResponse::nextArrayItem ()
{
    if (startLiteralTag("value"))
        // <value> tag
        return true;
    else    
        // nope, something else, probably </data>
        return false;
}

bool WPXmlResponse::startStructItem ()
{
    return (
            startStruct()
    );
}

bool WPXmlResponse::endStructItem ()
{
    return (
            checkEndTag("struct")
        &&  endLiteralTag("value")
    );
}

bool WPXmlResponse::endArrayParam ()
{
    return (
            checkEndTag("data")
        &&  endLiteralTag("array")
        &&  endGenericParam()
    );
}
    
bool WPXmlResponse::readStruct (WPXmlItem *item)
{
    // each struct member
    while (nextStructMember()) {
        QString name, type;

        // struct item header
        if (!startMember(&name, &type))
            return false;
        
        if (type == "array") {
            if(name == "custom_fields"){
                if(!finishCustomFieldsArray(item))
                    return false;
            } else {
                QStringList values;

                // list of category strings
                if (!finishArrayMember(&values))
                    return false;

                item->listContent.insert(name, values);
            }

        } else {
            QString value;

            // raw value
            if (!finishScalarMember(&value))
                return false;

            item->content.insert(name, value);
        }
    }

    // ok
    return true;
}

bool WPXmlResponse::readCustomFieldsStruct(WPXmlItem* item)
{
    if(!startStruct())
        return false;
    
    QString name, type, memberValue;
    QString id, key, value;
    
    while(nextStructMember()){        
        if(!startMember(&name, &type) || !finishScalarMember(&memberValue))
            return false;
        
        if(name == "id"){
            id = memberValue;
        } else if(name == "key"){
            key = memberValue;
        } else if(name == "value"){
            value = memberValue;
        }        
    }
    
    if(!id.isEmpty() && !key.isEmpty() && !value.isEmpty()){
        item->customFields.append(WPDataCustomField(id, key, value));
    }
    
    if(!checkEndTag("struct"))
            return false;
            
    return true;
}
    
bool WPXmlResponse::readStructParam (WPXmlItem *item)
{
    return (
            startStructParam()
        &&  readStruct(item)
        &&  endStructParam()
    );
}
    
bool WPXmlResponse::readScalarStructParam (QMap<QString, QString> *content)
{
    if (!startStructParam())
        return false;
            
    // each struct member
    while (nextStructMember()) {
        QString name, value;
        
        // value as raw string
        if (!readScalarMember(&name, NULL, &value))
            return false;

        // store
        content->insert(name, value);
    }

    if (!endStructParam())
        return false;

    // ok
    return true;
}
    
bool WPXmlResponse::readStructOfScalarStructParam(QMap< QString, QMap<QString,QString> > *map)
{
    if (!startStructParam())
        return false;

    QString name;
    QString type;

    while (nextStructMember()) {
        WPXmlItem item;
        // struct item header
        if (!startMember(&name, &type))
            return false;

        if (type != "struct")
            return false;

        if (readStruct(&item)) {
                map->insert(name, item.content);
            }

        if (!finishScalarMember(&type))
            return false;
    }

    if (!endStructParam())
        return false;

    return true;
}

bool WPXmlResponse::startStructParam ()
{
    return (
            startGenericParam()
        &&  startLiteralTag("struct")
    );
}

bool WPXmlResponse::nextStructMember ()
{
    if (startLiteralTag("member"))
        // <member> tag
        return true;
    else
        // nope, probably </struct>
        return false;
}

bool WPXmlResponse::readScalarMember (QString *name, QString *type, QString *value)
{
    return (
            checkStartTag("member")
        &&  readLiteralTag("name", name)
        &&  startLiteralTag("value")
        &&  readTag(type, value)
        &&  endLiteralTag("value")
        &&  endLiteralTag("member")
    );
}

bool WPXmlResponse::startMember (QString *name, QString *type)
{
    return (
            checkStartTag("member")
        &&  readLiteralTag("name", name)
        &&  startLiteralTag("value")
        &&  startTagName(type)
    );
}
    
bool WPXmlResponse::endMember ()
{
    return (
            endLiteralTag("value")
        &&  endLiteralTag("member")
    );
}

bool WPXmlResponse::finishScalarMember (QString *value)
{
    return (
            readTagContent(value)
        &&  endMember()
    );
}

bool WPXmlResponse::finishArrayMember (QStringList *values)
{
    return (
            checkStartTag("array")
        &&  readScalarArray(values)
        &&  endMember()
    );
}

bool WPXmlResponse::finishCustomFieldsArray (WPXmlItem* item)
{
    return (
            checkStartTag("array")
        &&  readCustomFieldsArray(item)
        &&  endMember()
    );
}

bool WPXmlResponse::endStructParam ()
{
    return (
            checkEndTag("struct")
        &&  endGenericParam()
    );
}

