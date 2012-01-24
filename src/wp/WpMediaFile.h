#ifndef WPMEDIAFILE_H_
#define WPMEDIAFILE_H_

#include <QString>
#include <QStringList>
#include <QMap>
#include <QSize>

class WPMediaFile
{
public:
    /** Type */
    enum Type {
        /** Not a known media file */
        MEDIA_TYPE_NONE = 0,
    
        /** JPEG image */
        MEDIA_TYPE_IMAGE,

        /** ??? video clip */
        MEDIA_TYPE_VIDEO,

    };

    enum Align {
        Bottom = 0,
        Top,
        Center,
        Left,
        Right
    };

    enum SizeType {
        Full = 0,
        Thumbnail,
        Medium,
        Large
    };
    
    WPMediaFile (const QString &filename)
        : filename(filename)
        , type(typeFromFilename(filename))
        , rotationAngle(0)
        , size(QSize(0,0))
        , alignment(Bottom)
    {}
    
    WPMediaFile (const QString &filename, Type type)
        : filename(filename)
        , type(type)
        , rotationAngle(0)
        , size(QSize(0,0))
        , alignment(Bottom)
        {}
    
    QString getHtmlSnippet() const;
    
    QString getMimeType() const;
    
    /** Fill filters with wildcarded extensions e.g. "*.jpg" */
    static void wildcardExtensionFilters(Type type, QStringList &filters);
    
    static Type typeFromFilename(QString const & filename);

public: // public data

    /** Path to local file */
    QString filename;

    /** File type */
    Type type;

    /** Resulting ulr */
    QString resultUrl;

    /** VideoPress code... */
    QString resultVideoPressShortcode;

    /** Rotation angle */
    int   rotationAngle;

    /** target resize size, (0,0) if no resize */
    QSize size;

    /** defines how to align the img. */
    Align alignment;

    /** defines how the picture is scaled in the blog page/post. */
    SizeType sizeType;

    /** name of the file on server */
    QString serverFilename;

private:
    
    typedef QMap<Type, QStringList> ExtensionMap;
    static ExtensionMap const & extensions();
    
    typedef QMap<QString, QString> MimeMap;
    static MimeMap const & mimeTypes();

};

#endif /* WPMEDIAFILE_H_ */
