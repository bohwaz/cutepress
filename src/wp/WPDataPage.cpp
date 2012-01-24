/**
 * Implementations of Page-related functionality in WPData, both in WPDataPage and outside
 */

#include "WPData.h"

#include <QDebug>

const QString WPDataPage::ITEM_ID_KEY = "page_id";

WPDataPage::WPDataPage (WPDataBlog &blog) :
    WPDataItem<WPDataPage>(blog, blog.iPages)
{

}
    
WPDataPage::~WPDataPage()
{

}

void WPDataPage::GetContent (WPXmlItem *xml) const
{
    xml->content.insert("title",        iTitle);
    xml->content.insert("description",  iDescription);
    xml->content.insert("wp_password",  iWpPassword);
    xml->content.insert("page_status",  iStatus); 
    xml->content.insert("<dateTime.iso8601>date_created_gmt", GetISO8601Time());
}

void WPDataPage::SetContent (const WPXmlItem &xml)
{
    // typed fields
    SetISO8601Time(xml.content["date_created_gmt"]);

    // generic fields
    iItemId                 = xml.content[ITEM_ID_KEY];
    iStatus                 = xml.content["page_status"];
    
    // item fields
    iDescription            = xml.content["description"];
    iTitle                  = xml.content["title"];
    iLink                   = xml.content["link"];
    iPermaLink              = xml.content["permaLink"];
    iExcerpt                = xml.content["excerpt"];
    iTextMore               = xml.content["text_more"];
    iWpSlug                 = xml.content["wp_slug"];
    iWpPassword             = xml.content["wp_password"];
    iWpAuthor               = xml.content["wp_author"];
    iWpPageParentTitle      = xml.content["wp_page_parent_title"];
    iWpAuthorDisplayName    = xml.content["wp_author_display_name"];
    iWpPageTemplate         = xml.content["wp_page_template"];
    iUserId                 = xml.content["userid"];
    iAllowComments          = xml.content["mt_allow_comments"];
    iAllowPings             = xml.content["mt_allow_pings"];
    iPageParentId           = xml.content["wp_page_parent_id"];
    iWpPageOrder            = xml.content["wp_page_order"];
    iWpAuthorId             = xml.content["wp_author_id"];

    // fix
    if (!iTextMore.isEmpty()) {
        // post is split into two parts
        iDescription = (
                iDescription
            +   "\n <!--more--> \n"
            +   iTextMore
        );
    }

    // updated from network, so we can't be dirty anymore
    iDirty = false;
    iError = false;
}

// XXX: move to WPDataItem?
void WPDataPage::AddSnippet (const QString &html, bool placeOnTop)
{
    if (placeOnTop)
        iDescription = html + iDescription;
    else
        iDescription += html;
    
    // unsaved changes
    SetDirty();
}

void WPDataPage::UpdateLocal (QString title, QString description, QString password, QDateTime dateCreated)
{
    // update fields
    iTitle = title;
    iDescription = description;
    iWpPassword = password;
    qDebug()<<"DataPage"<<iWpPassword;
    iDateCreated = dateCreated;

    // super
    WPDataItem<WPDataPage>::UpdateLocal();
}
    
void WPDataPage::UpdateStatus (QString title, QString description, QString password, QDateTime dateCreated, QString status, QList<WPMediaFile> addedFiles)
{
    // update fields
    iTitle = title;
    iDescription = description;
    iWpPassword = password;
    qDebug()<<"DataPage"<<iWpPassword;
    iDateCreated = dateCreated;

    // super
    WPDataItem<WPDataPage>::UpdateStatus(status, addedFiles);
}
    

QDataStream &operator<< (QDataStream &out, const WPDataPage &page)
{
    out
        << KDataPageVersionNumber 
        << page.iDateCreated
        << page.iStatus
        << page.iDescription
        << page.iTitle
        << page.iLink
        << page.iPermaLink
        << page.iExcerpt
        << page.iTextMore
        << page.iWpSlug
        << page.iWpPassword
        << page.iWpAuthor
        << page.iWpPageParentTitle
        << page.iWpAuthorDisplayName
        << page.GetISO8601Time() // iDateCreatedGmt
        << page.iWpPageTemplate
        << page.iUserId
        << page.iItemId
        << page.iAllowComments
        << page.iAllowPings
        << page.iPageParentId
        << page.iWpPageOrder
        << page.iWpAuthorId
    ;

    return out;
}

QDataStream &operator>> (QDataStream &in, WPDataPage &page)
{
    int version;
    QString unused;

    in >> version;

    // dateCreated
    if (version >= 2) {
        // directly as a QDateTime object
        in >> page.iDateCreated;

    } else {
        QString strDate;    // this is NOT hungarian notation!

        // indirectly as a QString
        in >> strDate;

        // set
        page.SetISO8601Time(strDate);
    }
   
    in 
        >> page.iStatus 
        >> page.iDescription 
        >> page.iTitle
        >> page.iLink 
        >> page.iPermaLink 
        >> page.iExcerpt 
        >> page.iTextMore 
        >> page.iWpSlug
        >> page.iWpPassword 
        >> page.iWpAuthor 
        >> page.iWpPageParentTitle 
        >> page.iWpAuthorDisplayName
        >> unused // iDateCreatedGmt 
        >> page.iWpPageTemplate 
        >> page.iUserId 
        >> page.iItemId
        >> page.iAllowComments 
        >> page.iAllowPings 
        >> page.iPageParentId 
        >> page.iWpPageOrder 
        >> page.iWpAuthorId
    ;

    return in;
}

