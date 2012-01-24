/**
 * Implementations of Post-related functionality in WPData, both in WPDataPost and outside
 */

#include "WPData.h"
#include "utils.h"

#include <QDebug>

const QString WPDataPost::ITEM_ID_KEY = "postid";

WPDataPost::WPDataPost (WPDataBlog &blog) :
    WPDataItem<WPDataPost>(blog, blog.iPosts)
{

}
    
WPDataPost::~WPDataPost()
{

}

inline WPDataCustomField modifyValueOfCustomField(WPDataCustomField field, const QString& newValue){
    field.value = newValue;
    return field;
}

void WPDataPost::GetContent (WPXmlItem *xml) const
{
    xml->content.insert("title",        iTitle);
    xml->content.insert("description",  iDescription);
    xml->content.insert("post_status",  iStatus);
    xml->content.insert("mt_keywords",  iMtKeywords);
    xml->content.insert("<dateTime.iso8601>date_created_gmt", GetISO8601Time());
    xml->content.insert("wp_password",  iWpPassword);
    
    xml->listContent.insert("categories", iCategories);

#if defined(HAVE_QTM_LOCATION)
    if(locationCustomFieldsExist()){
        if(iLocation.isValid()){
            WPDataCustomField enabledField = modifyValueOfCustomField(getCustomField("geo_enabled"), "1");
            xml->customFields.append(enabledField);
            
            WPDataCustomField latitudeField = 
                    modifyValueOfCustomField(getCustomField("geo_latitude"), QString().setNum(iLocation.coordinate().latitude()));
            xml->customFields.append(latitudeField);
            
            WPDataCustomField longitudeField = 
                    modifyValueOfCustomField(getCustomField("geo_longitude"), QString().setNum(iLocation.coordinate().longitude()));
            xml->customFields.append(longitudeField);
        } else {
            WPDataCustomField enabledField = modifyValueOfCustomField(getCustomField("geo_enabled"), "0");
            xml->customFields.append(enabledField);
        }
    } else if(iLocation.isValid()){
        xml->customFields.append(WPDataCustomField(QString(), "geo_enabled", "1"));
        xml->customFields.append(WPDataCustomField(QString(), "geo_latitude", QString().setNum(iLocation.coordinate().latitude())));
        xml->customFields.append(WPDataCustomField(QString(), "geo_longitude", QString().setNum(iLocation.coordinate().longitude())));
    } 
#endif // defined(HAVE_QTM_LOCATION)
}

void WPDataPost::SetContent (const WPXmlItem &xml)
{
    // typed fields
    SetISO8601Time(xml.content["date_created_gmt"]);

    // generic fields
	iItemId                 = xml.content[ITEM_ID_KEY];
	iStatus                 = xml.content["post_status"];

    // item fields
	iDescription            = xml.content["description"];
	iTitle                  = xml.content["title"];
	iLink                   = xml.content["link"];
	iPermaLink              = xml.content["permaLink"];
	iUserId                 = xml.content["userid"];
	iMtExcerpt              = xml.content["mt_excerpt"];
	iMtTextMore             = xml.content["mt_text_more"];
	iMtAllowComments        = xml.content["mt_allow_comments"];
	iMtAllowPings           = xml.content["mt_allow_pings"];
	iMtKeywords             = xml.content["mt_keywords"];
	iWpSlug                 = xml.content["wp_slug"];
	iWpPassword             = xml.content["wp_password"];
	iWpAuthorId             = xml.content["wp_author_id"];
	iWpAuthorDisplayName    = xml.content["wp_author_display_name"];

    iCategories             = xml.listContent["categories"];

    iCustomFields           = xml.customFields;
    
#if defined(HAVE_QTM_LOCATION)
    iLocation = QtMobility::QGeoPositionInfo();
    
    
    if(locationCustomFieldsExist()){
        WPDataCustomField geoEnabled = getCustomField("geo_enabled");
        if(geoEnabled.value == "1"){
            WPDataCustomField latitudeField = getCustomField("geo_latitude");
            WPDataCustomField longitudeField = getCustomField("geo_longitude");
            
            bool latitudeOk, longitudeOk;
            double latitude = latitudeField.value.toDouble(&latitudeOk);
            double longitude = longitudeField.value.toDouble(&longitudeOk);
            
            if(latitudeOk && longitudeOk){
                iLocation = QtMobility::QGeoPositionInfo(QtMobility::QGeoCoordinate(latitude, longitude), iDateCreated);
                // qDebug(iLocation.coordinate().toString().toAscii().data());    
            } else {
                qDebug("Malformed coordinates.");
            }
        }
    }
#endif // defined(HAVE_QTM_LOCATION)


    // fix data
    if (!iMtTextMore.isEmpty()) {
        // post is split into two parts
        iDescription = (
                iDescription
            +   "\n <!--more--> \n"
            +   iMtTextMore
        );
    }
    
    // updated from network, so we can't be dirty anymore
    iDirty = false;
    iError = false;
}

// XXX: move to WPDataItem?
void WPDataPost::AddSnippet (const QString &html, bool placeOnTop)
{
    if (placeOnTop)
        iDescription = html + iDescription;
    else
        iDescription += html;

    // unsaved changes
    SetDirty();
}
    
void WPDataPost::ItemDeleted ()
{
    // react to remove this post's comments from list
    iBlog.iComments.RefreshAll();
}
    
QString WPDataPost::GetDisplayHTML ()
{
    return Utils::toDisplayHtml(iDescription);
}
    
QList<WPComment> WPDataPost::GetComments ()
{
    QList<WPComment> comments;

    for (QListIterator<WPComment> i = iBlog.GetComments(); i.hasNext(); ) {
        WPComment comment = i.next();

        if (comment->iPostId == this->iItemId)
            comments.append(comment);
    }

    return comments;
}
    
WPComment WPDataPost::NewComment (const QString &body)
{
    // construct new comment and submit it
    WPComment comment = WPComment(new WPDataComment(iBlog, WPPost(this), body));
    emit iBlog._CommentAdded(comment->LocalId().toInt());
    return comment;
}

void WPDataPost::UpdateLocal (
            QString title, QString description, QString tags, QDateTime dateCreated,
            QString password, QStringList categories
#if defined(HAVE_QTM_LOCATION)
            , const QtMobility::QGeoPositionInfo& location
#endif // defined(HAVE_QTM_LOCATION)
) {
    // update fields
    iTitle = title;
    iDescription = description;
    iMtKeywords = tags;
    iDateCreated = dateCreated;
    iWpPassword = password;
    iCategories = categories;
    
#if defined(HAVE_QTM_LOCATION)
    iLocation = QtMobility::QGeoPositionInfo(location);
#endif // defined(HAVE_QTM_LOCATION)

    // super
    WPDataItem<WPDataPost>::UpdateLocal();
}

void WPDataPost::UpdateStatus (
        QString title, QString description, QString tags, QDateTime dateCreated,
        QString password, QStringList categories,
        QString status, QList<WPMediaFile> addedFiles
#if defined(HAVE_QTM_LOCATION)
        , const QtMobility::QGeoPositionInfo& location
#endif // defined(HAVE_QTM_LOCATION)
) {
    // update fields
    iTitle = title;
    iDescription = description;
    iMtKeywords = tags;
    iDateCreated = dateCreated;
    iWpPassword = password;
    iCategories = categories;
#if defined(HAVE_QTM_LOCATION)
    iLocation = QtMobility::QGeoPositionInfo(location);
#endif // defined(HAVE_QTM_LOCATION)  

    // super
    WPDataItem<WPDataPost>::UpdateStatus(status, addedFiles);
}

int WPDataPost::customFieldIndex(const QString& key) const {
   return iCustomFields.indexOf(WPDataCustomField(QString(), key, QString())); 
}

const WPDataCustomField& WPDataPost::getCustomField(const QString& key) const{
    return iCustomFields[customFieldIndex(key)];
}

bool WPDataPost::locationCustomFieldsExist() const {
    return  customFieldIndex("geo_enabled") != -1
         && customFieldIndex("geo_latitude") != -1
         && customFieldIndex("geo_longitude") != -1;
}

QDataStream &operator<< (QDataStream &out, const WPDataPost &post)
{
    out 
        << KDataPostVersionNumber
        << post.GetISO8601Time()
        << post.iDescription
        << post.iTitle
        << post.iLink
        << post.iPermaLink
        << post.iUserId
        << post.iItemId
        << post.iMtExcerpt
        << post.iMtTextMore
        << post.iMtAllowComments
        << post.iMtAllowPings
        << post.iMtKeywords
        << post.iWpSlug
        << post.iWpPassword
        << post.iWpAuthorId
        << post.iWpAuthorDisplayName
        << post.GetISO8601Time() // iDateCreatedGmt
        << post.iStatus
        << post.iCategories
#if defined(HAVE_QTM_LOCATION)
        << true << post.iLocation
#else // defined(HAVE_QTM_LOCATION)
        << false
#endif // defined(HAVE_QTM_LOCATION)
    ;

    return out;
}

QDataStream &operator>> (QDataStream &in, WPDataPost &post)
{
    int version;
    QString strDate;
    QString unused;

    in 
        >> version
        >> strDate
        >> post.iDescription
        >> post.iTitle
        >> post.iLink
        >> post.iPermaLink
        >> post.iUserId
        >> post.iItemId
        >> post.iMtExcerpt
        >> post.iMtTextMore
        >> post.iMtAllowComments
        >> post.iMtAllowPings
        >> post.iMtKeywords
        >> post.iWpSlug
        >> post.iWpPassword
        >> post.iWpAuthorId
        >> post.iWpAuthorDisplayName
        >> unused // iDateCreatedGmt 
        >> post.iStatus
        >> post.iCategories;
 
    if(version == 2) {
#if defined(HAVE_QTM_LOCATION)
        in >> post.iLocation;
#endif // defined(HAVE_QTM_LOCATION)
    } else if(version >=3) {
        bool hasLocation;
        in >> hasLocation;
        if(hasLocation) {
#if defined(HAVE_QTM_LOCATION)
            in >> post.iLocation;
#endif // defined(HAVE_QTM_LOCATION)
        }
    }

    post.SetISO8601Time(strDate);

    return in;
}

