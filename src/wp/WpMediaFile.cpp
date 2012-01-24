#include "WpMediaFile.h"

QString WPMediaFile::getHtmlSnippet() const
    {
    if (type == MEDIA_TYPE_IMAGE)
        {
        QString alignText;
        switch(alignment) {
        case Left:
            {
                alignText = "alignleft";
                break;
            }
        case Right:
            {
                alignText = "alignright";
                break;
            }
        case Bottom:
        case Top:
        default:
            {
                alignText = "alignnone";
                break;
            }
        }

        QString sizeText;
        switch(sizeType) {
        case Thumbnail:
            {
                sizeText = "size-thumbnail";
                break;
            }
        case Medium:
            {
                sizeText = "size-medium";
                break;
            }
        case Large:
            {
                sizeText = "size-large";
                break;
            }
        case Full:
        default:
            {
                sizeText = "size-full";
                break;
            }
        }

        // format HTML
        return  QString(
                "\n<p><a href=\"%1\">"
                "<img class=\"%2 %3 wp-image-6\" src=\"%1\" />"
                "</a></p>\n").arg(resultUrl,alignText,sizeText);
        }
    else if (type == MEDIA_TYPE_VIDEO)
        {
        if(resultVideoPressShortcode.isEmpty())
            {
            QString mimeType = getMimeType();
            if(mimeTypes().contains(resultUrl.right(3)))
                {
                mimeType = mimeTypes()[resultUrl.right(3)]; 
                }
            else if(mimeTypes().contains(resultUrl.right(4)))
                {
                mimeType = mimeTypes()[resultUrl.right(4)];
                }
            
            if(mimeType.isEmpty())
                {
                return QString("<a href=\"%1\">%2</a><br />")
                        .arg(resultUrl).arg(QObject::tr("View Video"));
                }
            else
                {
                return QString("<a type=\"%3\" href=\"%1\">%2</a><br />")
                                .arg(resultUrl).arg(QObject::tr("View Video")).arg(mimeType);
                }
            }
        else
            {
            return resultVideoPressShortcode + "<br />"; 
            }
        }
        return QString();
    }

QString WPMediaFile::getMimeType() const
    {
    QString const & name = resultUrl.isEmpty() ? filename : resultUrl;
    QString mimeType;
    if(mimeTypes().contains(name.right(3)))
        {
        mimeType = mimeTypes()[name.right(3)]; 
        }
    else if(mimeTypes().contains(name.right(4)))
        {
        mimeType = mimeTypes()[name.right(4)];
        }
    return mimeType;
    }

void WPMediaFile::wildcardExtensionFilters(Type type, QStringList &filters)
    {
    QStringList const & list = extensions()[type];
    for(QStringList::const_iterator it = list.begin(); it != list.end(); ++it)
        filters.append(QString("*.") + *it);
    }

WPMediaFile::Type WPMediaFile::typeFromFilename(QString const & filename)
    {
    for(ExtensionMap::const_iterator it = extensions().begin(); it != extensions().end(); ++it)
        {
        if(it.value().contains(filename.right(3), Qt::CaseInsensitive) ||
           it.value().contains(filename.right(4), Qt::CaseInsensitive))
            return it.key();
        }
    return MEDIA_TYPE_NONE;
    }

WPMediaFile::ExtensionMap const & WPMediaFile::extensions()
    {
    static ExtensionMap map;
    if(map.empty())
        {
        map[MEDIA_TYPE_NONE] = QStringList();
        map[MEDIA_TYPE_IMAGE] << "jpg" << "jpeg" << "png" << "bmp";
        map[MEDIA_TYPE_VIDEO] << "mp4" << "mpg";
        }
    
    return map;
    }

WPMediaFile::MimeMap const & WPMediaFile::mimeTypes()
    {
    static MimeMap types;
    
    if(types.empty())
        {
        types["mp4"] = "video/mp4";
        types["mpg"] = "video/mpeg";
        }
    
    return types;
    }

