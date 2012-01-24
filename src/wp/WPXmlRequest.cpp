#include "WPXmlRequest.h"


WPXmlRequest::WPXmlRequest (QIODevice *out) :
    QXmlStreamWriter(out), buffer(out)
{
    out->open(QIODevice::WriteOnly);
    
    // start document
    writeStartDocument("1.0");
}
 
void WPXmlRequest::writeRawBase64 (QIODevice &src)
{
    // must be a multiple of 3, so that the intermediate chunks encode cleanly into base64
    // 510 bytes
    static const int CHUNKSIZE = 3 * 170;

    // chunk buffer
    QByteArray chunk (CHUNKSIZE, '\0');

    // read in chunks and encode them to output
    while (!src.atEnd()) {
        qint64 size;
       
        // read chunk
        if ((size = src.read(chunk.data(), chunk.size())) <= 0)
            break;

        // fix size
        chunk.truncate(size);

        // transcode
        QByteArray base64 = chunk.toBase64();

        // write out
        const char *out_buf = base64.data();
        qint64 out_len = base64.size();

        while (out_len > 0) {
            // write to internal buffer
            if ((size = buffer->write(out_buf, out_len)) <= 0)
                break;

            // adjust
            out_buf += size;
            out_len -= size;
        }
    }
}

   
void WPXmlRequest::startMethodCall (QString methodName)
{
    writeStartElement("methodCall");
        writeTextElement("methodName", methodName);
        
        writeStartElement("params");

}
    
void WPXmlRequest::writeScalarParam (QString type, QString value)
{
            writeStartElement("param");
                writeStartElement("value");
                    writeTextElement(type, value);
                writeEndElement();
            writeEndElement();
}
    
void WPXmlRequest::startStructParam ()
{
            writeStartElement("param");
                writeStartElement("value");
                    writeStartElement("struct");
}

void WPXmlRequest::startStructMember(QString name)
{
    writeStartElement("member");
        writeTextElement("name", name);
        writeStartElement("value");
            writeStartElement("struct");
}

void WPXmlRequest::startArrayMember(QString name)
{
    writeStartElement("member");
	writeTextElement("name", name);
	writeStartElement("value");
	    writeStartElement("array");
            	writeStartElement("data");
}
    
void WPXmlRequest::writeScalarMember (QString name, QString type, QString value)
{
                        writeStartElement("member");
                            writeTextElement("name", name);
                            writeStartElement("value");
                                writeTextElement(type, value);
                            writeEndElement();
                        writeEndElement();
}

void WPXmlRequest::writeCustomField(const WPDataCustomField& field)
{
	writeStartElement("value");
	    writeStartElement("struct");
	        if(!field.id.isNull())
    	        writeScalarMember("id", "string", field.id);
	        if(!field.key.isNull())
    		    writeScalarMember("key", "string", field.key);
    		if(!field.value.isNull())
                writeScalarMember("value", "string", field.value);
	    writeEndElement();
	writeEndElement();
}
    
void WPXmlRequest::writeBase64Member (QString name, QIODevice &src)
{
    // preamble
                        writeStartElement("member");
                            writeTextElement("name", name);
                            writeStartElement("value");
                                writeStartElement("base64");

    // we write the base64 data directly to the underlying QBuffer
    // before that, we have to tell QXmlStreamWriter to close the start tag...
                                    writeCharacters("");

    // XXX: this assumes that QXmlStreamWriter doesn't do any buffering of its own...
                                    writeRawBase64(src);

    // postamble                        
                                writeEndElement();
                            writeEndElement();
                        writeEndElement();

}
   
void WPXmlRequest::writeScalarArrayMember (QString name, QString type, QStringList values)
{
                        writeStartElement("member");
                            writeTextElement("name", name);
                            writeStartElement("value");
                                writeStartElement("array");
                                    writeStartElement("data");

                                    foreach (QString value, values) {
                                        writeStartElement("value");
                                            writeTextElement(type, value);
                                        writeEndElement();
                                    }

                                    writeEndElement();
                                writeEndElement();
                            writeEndElement();
                        writeEndElement();
}
    
void WPXmlRequest::writeStringArray(const QStringList& arrayContent)
{
    writeStartElement("array");
        writeStartElement("data");

            foreach (QString value, arrayContent) {
                writeStartElement("value");
                    writeTextElement("string", value);
                writeEndElement();
            }

        writeEndElement();
    writeEndElement();
}

void WPXmlRequest::endStructParam ()
{
                    writeEndElement();
                writeEndElement();
            writeEndElement();
}

void WPXmlRequest::endStructMember()
{
            writeEndElement();
        writeEndElement();
    writeEndElement();
}

void WPXmlRequest::endArrayMember()
{
		writeEndElement();
	    writeEndElement();
	writeEndElement();
    writeEndElement();
}
    
void WPXmlRequest::endMethodCall ()
{
        writeEndElement();
    writeEndElement();
}

