#include "WPData.h"

const QString WPDataComment::ITEM_ID_KEY = "comment_id";

WPDataComment::WPDataComment (WPDataBlog &blog) :
    WPDataItem<WPDataComment>(blog, blog.iComments)
{

}
    
WPDataComment::WPDataComment (WPDataBlog &blog, WPPost post, const QString &body) :
    WPDataItem<WPDataComment>(blog, blog.iComments),
    iContent(body), iPostId(post->iItemId) 
{
    // save
    // XXX: default status for new comment is ...?
    UpdateStatus("approve");
}

void WPDataComment::GetContent (WPXmlItem *xml) const
{
    // XXX: doesn't /exactly/ fit...

    // values for NewComment
    xml->content.insert("post_id", iPostId);            // comment on given post
    xml->content.insert("comment_parent", iParent);     // reply on given comment
    xml->content.insert("content", iContent);           // comment data
    
    // values for EditComment
    xml->content.insert("status", iStatus);             // comment status
}

void WPDataComment::SetContent (const WPXmlItem &xml)
{
    // typed fields
    SetISO8601Time(xml.content["date_created_gmt"]);

    // generic fields
	iItemId                 = xml.content[ITEM_ID_KEY];
	iStatus                 = xml.content["status"];

    // item fields
	iUserId                 = xml.content["user_id"];
	iParent                 = xml.content["parent"];
	iContent                = xml.content["content"];
	iLink                   = xml.content["link"];
	iPostId                 = xml.content["post_id"];
	iPostTitle              = xml.content["post_title"];
	iAuthor                 = xml.content["author"];
	iAuthorUrl              = xml.content["author_url"];
	iAuthorEmail            = xml.content["author_email"];
	iAuthorIp               = xml.content["author_ip"];
	iType                   = xml.content["type"];
    
    // updated from network, so we can't be dirty anymore
    iDirty = false;
    iError = false;
}
    
void WPDataComment::AddSnippet (const QString &html, bool placeOnTop)
{
    (void) html;

    // XXX: not implemented
    Q_ASSERT(false);
}

bool WPDataComment::IsUnapproved() const
{
    return iStatus == "hold";
}

bool WPDataComment::operator== (const WPDataComment& other) const
{
    return
            iItemId == other.iItemId 
        &&  iStatus == other.iStatus 
        &&  iContent == other.iContent
        &&  iDateCreated == other.iDateCreated
        &&  iUserId == other.iUserId
        &&  iParent == other.iParent
        &&  iLink == other.iLink
        &&  iPostId == other.iPostId
        &&  iPostTitle == other.iPostTitle
        &&  iAuthor == other.iAuthor
        &&  iAuthorUrl == other.iAuthorUrl
        &&  iAuthorEmail == other.iAuthorEmail
        &&  iAuthorIp == other.iAuthorIp
        &&  iType == other.iType
    ;
}

bool WPDataComment::operator< (const WPDataComment& other) const
{
    // unapproved before approved
    if (IsUnapproved() && !other.IsUnapproved())
        return true;
    
    // approved before unapproved
    if (!IsUnapproved() && other.IsUnapproved())
        return false;

    // sort by date order
    return iDateCreated > other.iDateCreated;
}

QDataStream &operator<< (QDataStream &out, const WPDataComment &comment)
{
    out 
        << KDataCommentVersionNumber
        << comment.GetISO8601Time()
        << comment.iUserId
        << comment.iItemId
        << comment.iParent
        << comment.iStatus
        << comment.iContent
        << comment.iLink
        << comment.iPostId
        << comment.iPostTitle
        << comment.iAuthor
        << comment.iAuthorUrl
        << comment.iAuthorEmail
        << comment.iAuthorIp
        << comment.iType
    ;

    return out;
}

QDataStream &operator>> (QDataStream &in, WPDataComment &comment)
{
    int version;
    QString strDate;

    in 
        >> version
        >> strDate
        >> comment.iUserId
        >> comment.iItemId
        >> comment.iParent
        >> comment.iStatus
        >> comment.iContent
        >> comment.iLink
        >> comment.iPostId
        >> comment.iPostTitle
        >> comment.iAuthor
        >> comment.iAuthorUrl
        >> comment.iAuthorEmail
        >> comment.iAuthorIp
        >> comment.iType
    ;
    
    comment.SetISO8601Time(strDate);

    return in;
}

