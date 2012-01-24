/*
 * WPNetworkEngine.cpp
 *
 *  Created on: 4.11.2009
 *      Author: jkal
 */

#include <QtGlobal>
#include <QAuthenticator>
#include <QBuffer>
#include <QDebug>
#include <QNetworkReply>
#include <QStringList>
#include <QTreeWidget>
#include <QListWidgetItem>
#include <QDesktopServices>
#include <QTime>
#include <QByteArray>
#include <QSettings>

#ifdef HAVE_QTMOBILITY
#include <qsysteminfo.h>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#endif

#include "WPNetworkEngine.h"
#include "WPXmlRsdParser.h"
#include "WPXmlStatsParser.h"
#include "WPXmlApiKeyParser.h"
#include "WPXmlRequest.h"
#include "WPXmlResponse.h"
#include "WPData.h"
#include "version.h"
#include "utils.h"

#include "ScopedTimer.h"

WPNetworkQueueItem::WPNetworkQueueItem()
    : iNetworkReply(NULL), iShowWaitNote(false), iShowErrorNote(false), iNetworkReqId(0), iBlogData(NULL)
{
}

WPNetworkQueueItem::WPNetworkQueueItem(QNetworkReply* aNetworkReply, bool aShowWaitNote, bool aShowErrorNote, int aClientSideItemId, WPDataBlog* aBlogData)
    : iNetworkReply(aNetworkReply), iShowWaitNote(aShowWaitNote), iShowErrorNote(aShowErrorNote), iNetworkReqId(aClientSideItemId), iBlogData(aBlogData)
{
}

WPNetworkEngine::WPNetworkEngine() 
    : iUniqReqId(1)
#ifdef HAVE_QTMOBILITY
    , iNetworkSession(0)
#endif    
{
    iNetworkAccessManager = new QNetworkAccessManager(this);
    connect(
            iNetworkAccessManager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(HandleNetworkReply(QNetworkReply*))
            );

    connect(
            iNetworkAccessManager, SIGNAL(sslErrors(QNetworkReply *, QList<QSslError>)),
            this, SLOT(HandleSslErrors(QNetworkReply *, QList<QSslError>))
            );

    connect(
            iNetworkAccessManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            this, SLOT(HandleAuthentication(QNetworkReply*,QAuthenticator*))
            );
    
    OpenNetworkSession();
}

WPNetworkEngine::~WPNetworkEngine()
{
    //ClearRequestQueue();

#ifdef HAVE_QTMOBILITY
    if(iNetworkSession) {
        disconnect(iNetworkSession, 0, this, 0);
        iNetworkSession->close();
    }
#endif
}

void WPNetworkEngine::SetEndpoint (WPNetworkEndpoint endpoint)
{
    Q_ASSERT(!endpoint.iRpcUrl.isNull());
    Q_ASSERT(!endpoint.iBlogId.isNull());

    // flush requests for old endpoint
    ClearRequestQueue();

    // store, replacing old endpoint
    // this endpoint should contain all required info (iBlogId, iUsername, iPassword)
    iEndpoint = endpoint;

    if (endpoint.isWordpressHosted() && endpoint.iApiKey.isEmpty())
        GetApiKey();
}

void WPNetworkEngine::DiscoverEndpoint (WPNetworkEndpoint endpoint)
{
    // flush requests for old endpoint
    ClearRequestQueue();

    // store, replacing old endpoint
    // XXX: this endpoint will NOT contain a valid iBlogId or iRpcUrl, nor can we set them.
    iEndpoint = endpoint;

    // url
    // XXX: test url for validity
    // XXX: don't parse it here!
    QUrl url = QUrl::fromUserInput(iEndpoint.iBlogUrl);

    // request blog main page with GET
    ExecuteRequest(url, EGetMainPage, 0);
}

/* Methods for outgoing requests */
int WPNetworkEngine::ExecuteRequest (const QUrl &url, TWPNetworkEngineCommand cmd, int id, QSharedPointer<QIODevice> body, const QString &contentType)
{
    // qDebug("WPNetworkEngine::ExecuteRequest[%d] url=%s, cmd=%d", id, url.toString().toAscii().data(), cmd);

    if(QueueRequestIfNecessary(url, cmd, id, body, contentType)) { return -1; }
    
    // build NetworkRequest with header info
    QNetworkRequest request(url);

    request.setAttribute(QNetworkRequest::Attribute(QNetworkRequest::User + 1), QVariant((int) cmd));
    request.setRawHeader("User-Agent", WP_USERAGENT_STR.toAscii());

    // Content-Type
    if (!contentType.isNull())
        request.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
    
    QNetworkReply *reply;

    // post req+data via Manager to get Reply
    if (!body)
        reply = iNetworkAccessManager->get(request);
    else
    {
        body->open(QIODevice::ReadOnly);
        body->seek(0);
        reply = iNetworkAccessManager->post(request, body.data());
    }

    // track req/reply state with ID
    WPNetworkQueueItem queueItem(reply, true, true, id);
    queueItem.iPOSTData = body;
    iCurrentRequests.append(queueItem);

    // UI busy/status
    emit TaskStarted(cmd);

    // ok, return ID for tracking
    return id;
}

int WPNetworkEngine::ExecuteRequest (const WPNetworkEndpoint &endpoint, TWPNetworkEngineCommand cmd, int id, QSharedPointer<QIODevice> body, const QString &contentType)
{
    // XXX: don't parse URL every time
    QUrl url(endpoint.iRpcUrl);

    return ExecuteRequest(url, cmd, id, body, contentType);
}

int WPNetworkEngine::ExecuteCustomRequest (TWPNetworkEngineCommand cmd, int id,
                                           const QString &method,
                                           const QStringList *params,
                                           const QMap<QString, QString> *content,
                                           const QMap<QString, QStringList> *listContent,
                                           const QList<WPDataCustomField> *customFields,
                                           const QString *suffix,
                                           const QList<QString> *arrayContent = NULL)
{
    QSharedPointer<QBuffer> body(new QBuffer());
    WPXmlRequest req(body.data());
    
    // build XML
    req.startMethodCall(method);

    // optional parameters
    if (params) {
        foreach (QString value, *params)
            req.writeStringParam(value);
    }

    // optional object
    if (content || listContent) {
        req.startStructParam();

        if (content) {
            for (QMapIterator<QString, QString> i(*content); i.hasNext(); ) {
                i.next();

                QString name = i.key();
                QString type = "string";
                QString value = i.value();

                // type tag?
                // XXX: is this a hack? Meh, QMap<QString, QString> isn't that great...
                if (name.startsWith('<')) {
                    int start = 0;                  // first <
                    int end = name.indexOf('>');    // first >

                    // extract <...>...
                    type = name.mid(start + 1, end - start - 1);    // non-inclusive on both sides
                    name = name.mid(end + 1);                       // the rest of the string
                }

                // member
                req.writeScalarMember(name, type, value);
            }

            if(customFields){
                req.startArrayMember("custom_fields");

                foreach (WPDataCustomField field, *customFields){
                    qDebug("Writing custom field %s, %s, %s", field.id.toAscii().data(), field.key.toAscii().data(), field.value.toAscii().data());
                    req.writeCustomField(field);
                }

                req.endArrayMember();
            }
        }

        if (listContent) {
            for (QMapIterator<QString, QStringList> i(*listContent); i.hasNext(); ) {
                i.next();

                QString name = i.key();
                QStringList values = i.value();

                // array member
                req.writeScalarArrayMember(name, "string", values);
            }
        }

        req.endStructParam();
    }

    if (arrayContent && arrayContent->size() > 0) {
        req.writeStringArray(*arrayContent);
    }

    // optional suffix
    if (suffix) {
        req.writeStringParam(*suffix);
    }

    req.endMethodCall();

    // send
    int ret = ExecuteRequest(iEndpoint, cmd, id, body, "text/xml");
    
    qDebug("WPNetworkEngine::ExecuteRequest[%d] url=%s method=%s", ret, iEndpoint.iRpcUrl.toAscii().data(), method.toAscii().data());

    return ret;
}


int WPNetworkEngine::ExecuteGenericRequest (TWPNetworkEngineCommand cmd, int id,
                                            const QString &method,
                                            const QString *objectId,
                                            const QMap<QString, QString> *content,
                                            const QMap<QString, QStringList> *listContent,
                                            const QList<WPDataCustomField> *customFields,
                                            const QString *suffix )
{

    QStringList params;

    // usual params
    params << iEndpoint.iBlogId << iEndpoint.iUsername << iEndpoint.iPassword;

    // optional object ID
    if (objectId)
        params << *objectId;

    return ExecuteCustomRequest(cmd, id, method, &params, content, listContent, customFields, suffix, 0);
}

/* Stubs */
int WPNetworkEngine::ExecuteSimpleRequest (TWPNetworkEngineCommand cmd,
                                           const QString &method )
{
    return ExecuteGenericRequest(cmd, 0, method, NULL, NULL, NULL);
}

int WPNetworkEngine::ExecuteSimpleRequest (TWPNetworkEngineCommand cmd,
                                           const QString &method,
                                           const QString &objectId )
{
    return ExecuteGenericRequest(cmd, 0, method, &objectId, NULL, NULL);
}

int WPNetworkEngine::ExecuteSimpleRequest (TWPNetworkEngineCommand cmd,
                                           const QString &method,
                                           const QMap<QString, QString> &object )
{
    return ExecuteGenericRequest(cmd, 0, method, NULL, &object, NULL);
}

int WPNetworkEngine::ExecuteSimpleRequest (TWPNetworkEngineCommand cmd,
                                           const QString &method,
                                           const QString &objectId,
                                           const QMap<QString, QString> &object )
{
    return ExecuteGenericRequest(cmd, 0, method, &objectId, &object, NULL);
}

/* Update status from server */
void WPNetworkEngine::GetUsersBlogs(const QUrl& aUrl)
{
    QSharedPointer<QBuffer> body(new QBuffer());
    WPXmlRequest req(body.data());

    req.startMethodCall("wp.getUsersBlogs");

    req.writeStringParam(iEndpoint.iUsername);
    req.writeStringParam(iEndpoint.iPassword);

    req.endMethodCall();
    
    // XXX: wait, error, modal
    ExecuteRequest(aUrl, EGetUsersBlogs, 0, body, "text/xml");
}

void WPNetworkEngine::GetPages()
{
    // XXX: wait, noerror, nomodal
    ExecuteSimpleRequest(EGetPages, "wp.getPages", "-1");
}

void WPNetworkEngine::GetPosts()
{
    // XXX: wait, noerror, nomodal
    ExecuteSimpleRequest(EGetPosts, "metaWeblog.getRecentPosts", iEndpoint.iNumberOfPosts);
}

void WPNetworkEngine::GetCategories()
{
    // XXX: wait, noerror, nomodal
    ExecuteSimpleRequest(EGetCategories, "wp.getCategories");
}

void WPNetworkEngine::GetTags()
{
    // XXX: nowait, noerror, noemit
    ExecuteSimpleRequest(EGetTags, "wp.getTags");
}

void WPNetworkEngine::GetMedia()
{
    // XXX: wait, noerror, nomodal
    //ExecuteSimpleRequest(EGetMediaLibrary, "wp.getMediaLibrary ", "-1");
    QStringList params;
    params << iEndpoint.iBlogId << iEndpoint.iUsername << iEndpoint.iPassword;

    ExecuteCustomRequest(EGetMediaLibrary, 0, "wp.getMediaLibrary", &params, NULL, NULL, NULL, NULL, NULL);
}

void WPNetworkEngine::GetPageStatuses()
{
    // XXX: nowait, noerror, noemit
    ExecuteSimpleRequest(EGetPageStatuses, "wp.getPageStatusList");
}

void WPNetworkEngine::GetPostStatuses()
{
    // XXX: nowait, noerror, noemit
    ExecuteSimpleRequest(EGetPostStatuses, "wp.getPostStatusList");
}

void WPNetworkEngine::GetCommentStatuses()
{
    // XXX: nowait, noerror, noemit
    ExecuteSimpleRequest(EGetCommentStatuses, "wp.getCommentStatusList");
}

void WPNetworkEngine::GetComments(TGetCommentsStatus aStatus, const QString& aPostId, int aNumberOfComments, int aOffset)
{
    QMap<QString, QString> map;

    if (aStatus  == EPendingComments)
        map.insert("status", "hold");

    if (!aPostId.isEmpty())
        map.insert("post_id", aPostId);

    if (aNumberOfComments != 10)
        map.insert("number", QString::number(aNumberOfComments));

    if (aOffset != 0)
        map.insert("offset", QString::number(aOffset));

    // XXX: wait, error, nomodal
    ExecuteSimpleRequest(EGetComments, "wp.getComments", map);
}

void WPNetworkEngine::GetStats (QString table, int days) {
    if (iEndpoint.iApiKey.isEmpty())
        // nothing to do
        return;

    // build request URL
    QUrl url = QUrl("http://stats.wordpress.com/csv.php");
    url.addQueryItem("api_key", iEndpoint.iApiKey);
    url.addQueryItem("blog_uri", iEndpoint.iBlogUrl);
    url.addQueryItem("format", "xml");

    if (!table.isEmpty())
        url.addQueryItem("table", table);

    url.addQueryItem("days", QString::number(days)); // last n days
    url.addQueryItem("limit", "-1"); // -1 means unlimited records

    // do GET
    ExecuteRequest(url, EGetStats, 0);
}

void WPNetworkEngine::GetApiKey()
{
    ExecuteRequest(QUrl("https://public-api.wordpress.com/getuserblogs.php"), EFetchingApiKey, 0);
}

void WPNetworkEngine::GetMediaSettings()
{
    QStringList params;
    params << iEndpoint.iBlogId << iEndpoint.iUsername << iEndpoint.iPassword;

    QStringList optionsList;
    optionsList.append("thumbnail_size_w");
    optionsList.append("thumbnail_size_h");
    optionsList.append("thumbnail_crop");
    optionsList.append("medium_size_w");
    optionsList.append("medium_size_h");
    optionsList.append("large_size_w");
    optionsList.append("large_size_h");

    ExecuteCustomRequest(EGetMediaSettings, 0, "wp.getOptions", &params, 0, 0, 0, 0, &optionsList);
}

void WPNetworkEngine::GetLocationAddress(qreal latitude, qreal longitude)
{
    QUrl url("http://maps.googleapis.com/maps/api/geocode/xml");
    url.addQueryItem("latlng", QString("%1,%2").arg(latitude).arg(longitude));
    url.addQueryItem("sensor", "false");
    ExecuteRequest(url, EFetchingLocation, 0);
}

void WPNetworkEngine::HandleSslErrors (QNetworkReply *reply, const QList<QSslError> &errors)
{
    qWarning() << "WPNetworkEngine::HandleSslErrors - " << reply->url().toString();

    foreach (const QSslError &err, errors)
        qWarning() << "\t" << err.errorString();

    // XXX: insecure. Very insecure. User not happy. Bad code monkey. No banana.
    reply->ignoreSslErrors();
}

QString WPNetworkEngine::FaultToError (int faultCode, QString faultString)
{
    switch (faultCode) {
    case 403:   return tr("Username and/or password do not match: ") + faultString;

        // XXX: faultString: XML-RPC services are disabled on this blog.  An admin user can enable them at http://samarium.futurice.com/wp-admin/options-writing.php
    case 405:   return tr("XML RPC interface not enabled. Please enable it from your blog's admin interface and retry: ");
    default:    return tr("XML RPC request failed. Please check the blog settings: ") + faultString;
    }
}

QString WPNetworkEngine::ParseResponseToError (WPXmlResponse &response)
{
    return tr("Failed to parse XMLRPC response: ") + response.getError();
}

void WPNetworkEngine::HandleNetworkReply (QNetworkReply *networkReply)
{
    TIME_FUNCTION

            WPNetworkQueueItem item;
    bool found = false;

    // find matching request by reply object pointer
    for (int i = 0; i < iCurrentRequests.size(); i++) {
        if (iCurrentRequests[i].iNetworkReply == networkReply) {
            item = iCurrentRequests.takeAt(i);
            found = true;

            // found
            break;
        }
    }

    if (!found) {
        // already handled?
        qWarning("WPNetworkEngine::HandleNetworkReply - no item for reply");

        return;
    }

    // Lookup command type
    TWPNetworkEngineCommand command;

    bool success;
    QVariant typeVariant = networkReply->request().attribute(QNetworkRequest::Attribute(QNetworkRequest::User + 1));
    
    command = (TWPNetworkEngineCommand) typeVariant.toInt(&success);

    if (!success) {
        // missing cmd type attribute in reply
        emit TaskFailed(command, "Invalid QNetworkReply");

        networkReply->deleteLater();
        return;
    }

    // log
    qDebug("WPNetworkEngine::HandleNetworkReply[%d] cmd=%d, host=%s", item.iNetworkReqId, command, networkReply->url().host().toAscii().constData());

    // handle TCP/HTTP-level errors
    if (networkReply->error()) {
        // generic networkReply-level error
        HandleNetworkError(networkReply, item, command, NetworkReplyToError(networkReply));

        networkReply->deleteLater();
        return;
    }

    // determine content type
    switch (command) {

        // list of response handlers that use WPXmlResponse
    case EGetUsersBlogs:
    case EGetPageStatuses:
    case EGetPostStatuses:
    case EGetCommentStatuses:
    case EGetPosts:
    case EGetPages:
    case EGetComments:
    case EGetCategories:
    case EGetTags:
    case EGetPage:
    case ENewPage:
    case EEditPage:
    case EDeletePage: 
    case EGetPost:
    case ENewPost:
    case EEditPost:
    case EDeletePost:
    case EGetComment:
    case ENewComment:
    case EEditComment:
    case EDeleteComment:
    case ENewCategory:
    case EUploadFile:
    case EUploadMultipleFiles:
    case EGetMediaSettings:
    case EGetMediaLibrary:
        {
            QByteArray rawResponse = networkReply->readAll();

            qDebug() << rawResponse;

            // common case for WPXmlResponse
            bool fault;
            WPXmlResponse response (rawResponse);

            // check for <fault>
            if (!response.startMethodResponse(&fault)) {
                // XMLRPC parse error
                HandleNetworkError(networkReply, item, command, ParseResponseToError(response));

            } else if (fault) {
                // XMLRPC fault
                int faultCode;
                QString faultString;

                // read in fault info
                if (!response.readFault(&faultCode, &faultString)) {
                    // XMLRPC parse error
                    HandleNetworkError(networkReply, item, command, ParseResponseToError(response));

                } else {
                    // XMLRPC fault
                    HandleNetworkError(networkReply, item, command, FaultToError(faultCode, faultString));
                }

            } else {
                // handle response
                if (!HandleXmlResponse(response, item, command)) {
                    // XMLRPC parse error
                    HandleNetworkError(networkReply, item, command, ParseResponseToError(response));

                } else {
                    // success
                    emit TaskFinished(command);
                }
            }

        } break;

    case EGetGravatar: {
            QByteArray hash = ParseHashFromGravatarUrl(networkReply->url());

            QString path;

            // XXX: code duplication
#ifdef Q_OS_SYMBIAN
            // current dir
            path = "";
#else
            // app data dir
            path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
            path += ".wordpress/";
#endif

            // filename
            path += hash + ".jpg";

            // open local
            QFile file(path);

            if (!file.open(QIODevice::WriteOnly)) {
                // failed
                HandleNetworkError(networkReply, item, command, "Saving gravatar file failed");

                break;
            }

            // write out
            while (!networkReply->atEnd()) {
                char buf[4096];
                qint64 len, ret;

                // read chunk
                if ((len = networkReply->read(buf, sizeof(buf))) <= 0)
                    // stop
                    break;

                // write chunk
                while (len > 0 && (ret = file.write(buf, len)) > 0)
                    len -= ret;
            }

            // done, flush
            file.close();

            // notify
            emit GravatarDownloaded(hash);

            // ok
            emit TaskFinished(command);

        } break;

    case EGetStats: {
            WPXmlStatsParser parser(networkReply->readAll(), iBlogData->iStats);
            parser.ParseResponse();

            if (parser.hasError()) {
                // failure
                HandleNetworkError(networkReply, item, command, parser.errorString());

                break;

            } else {
                // XXX: set iLastUpdated?

                // done
                emit TaskFinished(command);

                // API key is valid
                emit GetStatsSucceeded(iEndpoint.iBlogUrl, iEndpoint.iApiKey);

                // update UI
                emit UpdateStats(iBlogData->iStats);
            }
        } break;

    case EFetchingApiKey: {
            WPXmlApiKeyParser parser(networkReply->readAll());

            parser.ParseResponse();
            if (parser.hasError()) {
                HandleNetworkError(networkReply, item, command, tr("Couldn't parse api key"));
            } else {
                iEndpoint.iApiKey = parser.GetApiKey();
                emit ApiKeyReceived(iEndpoint.iApiKey);
                emit TaskFinished(command);
            }
        } break;

    case EFetchingLocation: {
            QRegExp pattern("<formatted_address>([^<]+)</formatted_address>");
            if (pattern.indexIn(QString::fromUtf8(networkReply->readAll())) > -1) {
                QString latlng = networkReply->request().url().queryItemValue("latlng");
                int pos = latlng.indexOf(",");
                QString lat = latlng.mid(0, pos);
                QString lng = latlng.mid(pos + 1);
                QString formattedAddress = pattern.cap(1).trimmed();
                emit FormattedAddressGotten(lat.toDouble(), lng.toDouble(), formattedAddress);
                emit TaskFinished(EFetchingLocation);
            } else {
                HandleNetworkError(networkReply, item, command, tr("Couldn't parse formatted address from location response."));
            }
        } break;

    case EGetRSD:
        {
        QList<WPDataRsdApi> rsdEndpoints;
        WPXmlRsdParser parser(networkReply->readAll(), rsdEndpoints);

        if (!rsdEndpoints.isEmpty()) {
            // follow first one
            // XXX: bad, the RSD also has e.g. the Atom endpoint
            QUrl apiUrl = QUrl::fromUserInput(rsdEndpoints[0].iApiLink);
            GetUsersBlogs(apiUrl);

            emit TaskFinished(command);

        } else {
            // failure
            HandleNetworkError(networkReply, item, command, "Invalid RSD. Check blog's XML-RPC API");
        }
    }
        break;

    case EReport:
        {
            qDebug() << "EReport:" << networkReply->readAll();

            QDate curDate = QDate::currentDate();

            // mark
            QSettings settings("WordPress", "MC Stats");
            settings.setValue("statsDate", curDate);
            
            emit TaskFinished(command);

        } break;

        // XXX: fix this up
    case EGetMainPage:
        {
    	QByteArray rawResponse = networkReply->readAll();
        QString response = QString::fromUtf8(rawResponse.constData(), rawResponse.length());
        QString rsdUrl;

        int rsdIndex = response.indexOf("rel=\"EditURI\"");

        if (rsdIndex > 0)
        {
            // Assuming that the HTML is valid at this point. If full certainty is wanted,
            // we should make a new function that parses the string and has checks for all possible
            // errors.
            rsdIndex = response.indexOf("href", rsdIndex);
            const int start = response.indexOf('\"', rsdIndex);
            const int end = response.indexOf('\"', start + 1);
            rsdUrl = response.mid(start + 1, end - start - 1); // stripping the '\"' characters

            // GET request
            ExecuteRequest(QUrl::fromUserInput(rsdUrl), EGetRSD, 0);
        }
        else if (networkReply->hasRawHeader("X-Pingback")) // If no rsd url found, using http header
        {
            QString pingbackUrl = QString::fromAscii(networkReply->rawHeader("X-Pingback"));
            GetUsersBlogs(pingbackUrl);
        }
        else
        {
            QUrl possibleRedirectUrl(networkReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl());

            if (!possibleRedirectUrl.isEmpty() && possibleRedirectUrl != networkReply->url())
            {
                // Unqueued network get is ok here, because we are guaranteed to have a connection
                iCurrentRequests.append(WPNetworkQueueItem(iNetworkAccessManager->get(CreateNetworkRequest(possibleRedirectUrl, EGetMainPage)), true, true));
                emit TaskStarted(EGetMainPage);
            } 
            else 
            {
                // no response
                HandleNetworkError(networkReply, item, command, "Could not find blog at address: " + networkReply->url().toString());

            }
        }

        emit TaskFinished(command);
    } break;

    default: {
            // XXX: unrecognized command
            qWarning("Unknown command response: %d", command);

        } break;

    }   // end switch

    // finally, cleanup   
    networkReply->deleteLater();
}

void WPNetworkEngine::HandleAuthentication(QNetworkReply *networkReply, QAuthenticator *authenticator)
{
    qDebug("HandleAuthentication");

    // Lookup command type
    TWPNetworkEngineCommand command;

    bool success;
    QVariant typeVariant = networkReply->request().attribute(QNetworkRequest::Attribute(QNetworkRequest::User + 1));

    command = (TWPNetworkEngineCommand) typeVariant.toInt(&success);

    if (!success) {
        qWarning("Unable to authenticate request because network engine command argument is missing");
        return;
    }

    if (command == EFetchingApiKey) {
        qDebug("Authenticating api key request");
        authenticator->setUser(iEndpoint.iUsername);
        authenticator->setPassword(iEndpoint.iPassword);
    } else {
        QUrl url = networkReply->request().url();
        QString user = url.userName();
        QString password = url.password();
        if (!user.isEmpty() && !password.isEmpty()) {
            authenticator->setUser(user);
            authenticator->setPassword(password);
        }
    }
}

QString WPNetworkEngine::NetworkReplyToError (QNetworkReply *networkReply)
{
    switch (networkReply->error()) {
    case QNetworkReply::NoError:
        return tr("Success.");  // I hope this never comes up...

    case QNetworkReply::TimeoutError:
        return tr("Connection timed out.");

    case QNetworkReply::ConnectionRefusedError:
        return tr("Connection refused.");

    case QNetworkReply::ContentNotFoundError:   // 404
        return tr("404 Not found.");

    case QNetworkReply::RemoteHostClosedError:
        return tr("Server closed the connection.");

    case QNetworkReply::SslHandshakeFailedError:
        return tr("Secure connection could not be established: ") + networkReply->errorString();

    case QNetworkReply::ContentAccessDenied:    // 401
        return tr("Access denied.");

    case QNetworkReply::HostNotFoundError:
        return tr("Host not found. Please check the blog URL.");

    case QNetworkReply::ProxyConnectionClosedError:
    case QNetworkReply::ProxyConnectionRefusedError:
    case QNetworkReply::ProxyNotFoundError:
    case QNetworkReply::ProxyTimeoutError:
    case QNetworkReply::ProxyAuthenticationRequiredError:
    case QNetworkReply::UnknownProxyError:
        return tr("Proxy server returned an error: ") + networkReply->errorString();

    case QNetworkReply::AuthenticationRequiredError:
        return tr("Authentication not accepted.");

    case QNetworkReply::ContentReSendError:
    case QNetworkReply::ContentOperationNotPermittedError:
    case QNetworkReply::ProtocolUnknownError:
    case QNetworkReply::ProtocolInvalidOperationError:
    case QNetworkReply::UnknownNetworkError:
    case QNetworkReply::UnknownContentError:
    case QNetworkReply::ProtocolFailure:
    default:
        qDebug() << "Error code:" << networkReply->error();
        return tr("Connection error: ") + networkReply->errorString();
    }
}

void WPNetworkEngine::HandleNetworkError (QNetworkReply *networkReply, WPNetworkQueueItem &item, TWPNetworkEngineCommand command, const QString &message)
{
    qDebug()<<"NETWORK Error"<<message;
    // XXX: already read, so this won't really work?
    QByteArray rawResponse = networkReply->readAll();

    qDebug("WPNetworkEngine::HandleNetworkError[%d] failed: %s", item.iNetworkReqId, message.toAscii().constData());
    qDebug("\tNetworkReply: %s", rawResponse.data());

    // command-specific handling
    switch (command) {
    case EUploadFile: {
        // file upload failed
        emit FileUploadFailed(*iFileUploadItem);

    } break;
    case EUploadMultipleFiles: {
        // file upload failed, skip it and continue to next item, or finish
        // XXX: mark errors? Or is the TaskFailed enough?
        // XXX: This assumes a bit too much about iFileUploadQueue and
        //      the file list of the first item 
        iFileUploadQueue.first().files.removeFirst();
        NextFileUpload();

    } break;

    case EGetGravatar: {
            // get hash
            QByteArray hash = ParseHashFromGravatarUrl(networkReply->url());

            if (networkReply->error() == QNetworkReply::ContentNotFoundError) {
                // handle these specially
                emit GravatarNotFound(hash);

            } else {
                // failure
                // XXX: better signal?
                emit GravatarFailed(hash);
            }

            // suppress error signal
            emit TaskFinished(command);

            return;

        } break;

    case EGetPage:
    case ENewPage:
    case EEditPage:
    case EDeletePage:
        // page failed
        emit PageFailed(item.iNetworkReqId, command);

        break;

    case EGetPost:
    case ENewPost:
    case EEditPost:
    case EDeletePost:
        // post failed
        emit PostFailed(item.iNetworkReqId, command);

        break;

    case EGetComment:
    case ENewComment:
    case EEditComment:
    case EDeleteComment:
        // comment failed
        emit CommentFailed(item.iNetworkReqId, command);

        break;

    default:
        // no special handling, just the usual TaskFailed
        break;
    }

    // report error
    if (item.iShowErrorNote)
        emit TaskFailed(command, message);
}

bool WPNetworkEngine::HandleXmlResponse (WPXmlResponse &response, WPNetworkQueueItem &item, TWPNetworkEngineCommand command)
{
    switch (command) {
    case EGetUsersBlogs: {
            QList<WPXmlItem> blogList;
            
            // parse
            if (!response.readStructArrayParam(&blogList))
                return false;

            // this list includes the actual XML-RPC URLs
            emit BlogsListed(iEndpoint, blogList);

        } break;

    case EGetPageStatuses: {
            iBlogData->iPageStatuses.clear();

            // read struct members directly into the statuses map
            if (!response.readScalarStructParam(&iBlogData->iPageStatuses))
                return false;

        } break;

    case EGetPostStatuses: {
            iBlogData->iPostStatuses.clear();

            // read struct members directly into the statuses map
            if (!response.readScalarStructParam(&iBlogData->iPostStatuses))
                return false;

        } break;

    case EGetCommentStatuses: {
            iBlogData->iCommentStatuses.clear();

            // read struct members directly into the statuses map
            if (!response.readScalarStructParam(&iBlogData->iCommentStatuses))
                return false;

        } break;

    case EGetPosts: {
            QList<WPXmlItem> postList;

            // array of structs 
            if (!response.readStructArrayParam(&postList))
                return false;

            // emit
            emit PostsListed(postList);

        } break;

    case EGetPages: {
            QList<WPXmlItem> pageList;

            // array of structs 
            if (!response.readStructArrayParam(&pageList))
                return false;

            // emit
            emit PagesListed(pageList);

        } break;

    case EGetComments: {
            QList<WPXmlItem> commentList;

            // array of structs
            if (!response.readStructArrayParam(&commentList))
                return false;

            emit CommentsListed(commentList);

        } break;

    case EGetCategories: {
            // raw list of categories
            QList<WPXmlItem> categoryList;

            // array of structs
            if (!response.readStructArrayParam(&categoryList))
                return false;

            // ok
            emit CategoriesListed(categoryList);

        } break;

    case EGetTags: {
            QList<WPXmlItem> tagList;

            // array of structs
            if (!response.readStructArrayParam(&tagList))
                return false;

            // apply new list
            iBlogData->iTags.clear();

            for (QListIterator<WPXmlItem> i(tagList); i.hasNext(); ) {
                const WPXmlItem &xml = i.next();

                // new WPDataCategoryItem
                iBlogData->iTags.append(WPDataTag(&xml));
            }

            // XXX: UpdateTagData?

        } break;

    case EGetPage: {
            WPXmlItem pageItem;

            // struct param
            if (!response.readStructParam(&pageItem))
                return false;

            // ok
            emit PageGotten(item.iNetworkReqId, pageItem);

        } break;

    case ENewPage: {
            QString pageId;
            
            // new item ID
            if (!response.readRawParam(&pageId))
                return false;
            
            // ok
            emit PageCreated(item.iNetworkReqId, pageId);

        } break;

    case EEditPage: {

            if (!response.readSuccessParam())
                return false;

            // ok
            emit PageEdited(item.iNetworkReqId);

        } break;

    case EDeletePage: {

            if (!response.readSuccessParam())
                return false;

            // ok
            emit PageDeleted(item.iNetworkReqId);

        } break;

    case EGetPost: {
            WPXmlItem postItem;

            // struct param
            if (!response.readStructParam(&postItem))
                return false;

            // ok
            emit PostGotten(item.iNetworkReqId, postItem);

        } break;

    case ENewPost: {
            QString postId;
            
            // new item ID
            if (!response.readRawParam(&postId))
                return false;
            
            // ok
            emit PostCreated(item.iNetworkReqId, postId);

        } break;

    case EEditPost: {

            if (!response.readSuccessParam())
                return false;

            // ok
            emit PostEdited(item.iNetworkReqId);

        } break;

    case EDeletePost: {

            if (!response.readSuccessParam())
                return false;

            // ok
            emit PostDeleted(item.iNetworkReqId);

        } break;

    case EGetComment: {
            WPXmlItem commentItem;

            // struct param
            if (!response.readStructParam(&commentItem))
                return false;

            // ok
            emit CommentGotten(item.iNetworkReqId, commentItem);

        } break;

    case ENewComment: {
            QString commentId;
            
            // new item ID
            if (!response.readRawParam(&commentId))
                return false;
            
            // ok
            emit CommentCreated(item.iNetworkReqId, commentId);

        } break;

    case EEditComment: {

            if (!response.readSuccessParam())
                return false;

            // ok
            emit CommentEdited(item.iNetworkReqId);

        } break;

    case EDeleteComment: {

            if (!response.readSuccessParam())
                return false;

            // ok
            emit CommentDeleted(item.iNetworkReqId);

        } break;

    case ENewCategory: {
            QString categoryId;

            // new item ID
            if (!response.readRawParam(&categoryId))
                return false;

            // ok
            emit CategoryCreated(item.iNetworkReqId, categoryId);

        } break;

    case EUploadFile: {
            QMap<QString, QString> info;

            // response struct param
            if (!response.readScalarStructParam(&info))
                return false;

            // new URL to file
            iFileUploadItem->resultUrl = info["url"];
            iFileUploadItem->serverFilename = info["file"];
            if (iFileUploadItem->type == WPMediaFile::MEDIA_TYPE_VIDEO)
                iFileUploadItem->resultVideoPressShortcode = info["videopress_shortcode"];

            emit FileUploaded(*iFileUploadItem);

        } break;

    case EUploadMultipleFiles: {
            QMap<QString, QString> info;

            // response struct param
            if (!response.readScalarStructParam(&info))
                return false;

            // matching FileUploadItem
            FileUploadItem &file_item = iFileUploadQueue.first();

            // pop uploaded file
            WPMediaFile file = file_item.files.takeFirst();

            // new URL to file
            file.resultUrl = info["url"];

            if (file.type == WPMediaFile::MEDIA_TYPE_VIDEO)
                file.resultVideoPressShortcode = info["videopress_shortcode"];

            // accumulate list of uploaded files
            file_item.results.append(file);

            // keep going, or finish
            NextFileUpload();

        } break;

    case EGetMediaSettings: {
            QMap< QString,QMap<QString,QString> > map;

            if (!response.readStructOfScalarStructParam(&map))
                break; // not fatal, may not be supported by server.

            int value = map.value("large_size_w").value("value").toInt();
            iBlogData->iLargeSize.setWidth(value == 0 ? 1024 : value);
            value = map.value("large_size_h").value("value").toInt();
            iBlogData->iLargeSize.setHeight(value == 0 ? 768 : value);
            value = map.value("medium_size_w").value("value").toInt();
            iBlogData->iMediumSize.setWidth(value == 0 ? 640 : value);
            value = map.value("medium_size_h").value("value").toInt();
            iBlogData->iMediumSize.setHeight(value == 0 ? 480 : value);
            value = map.value("thumbnail_size_w").value("value").toInt();
            iBlogData->iThumbnailSize.setWidth(value == 0 ? 150 : value);
            value = map.value("thumbnail_size_h").value("value").toInt();
            iBlogData->iThumbnailSize.setHeight(value == 0 ? 150 : value);

        } break;

    case EGetMediaLibrary: {
        // raw list of media items
        QList<WPXmlItem> mediaList;

        // array of structs
        if (!response.readStructArrayParam(&mediaList))
            return false;

        // ok
        emit MediaListed(mediaList);

        } break;

    default: {
            // oops, shouldn't be here...
            qFatal("WPNetworkEngine::HandleXmlResponse - Unknown command: %d", command);

        } break;
    }
    
    // finish off off the request
    if (!response.endMethodResponse())
        // XXX: ignore extraneous params?
        qWarning("WPNetworkEngine::HandleXmlResponse - Extra params in method response");

    // great
    return true;
}

/* Pages */
void WPNetworkEngine::GetPage (int localId, QString itemId)
{
    // non-standard parameter order
    QStringList params;
    params << iEndpoint.iBlogId << itemId << iEndpoint.iUsername << iEndpoint.iPassword;

    ExecuteCustomRequest(EGetPage, localId, "wp.getPage", &params, NULL, NULL, NULL, NULL, NULL);
}

void WPNetworkEngine::NewPage (int localId, WPXmlItem item)
{
    // XXX: wut?
    QString publish = "1";

    ExecuteGenericRequest(ENewPage, localId, "wp.newPage", NULL, &item.content, &item.listContent,
                          &item.customFields, &publish);
}

void WPNetworkEngine::EditPage (int localId, QString itemId, WPXmlItem item)
{
    // XXX: wut?
    QString publish = "0";

    // non-standard parameter order
    QStringList params;
    params << iEndpoint.iBlogId << itemId << iEndpoint.iUsername << iEndpoint.iPassword;

    ExecuteCustomRequest(EEditPage, localId, "wp.editPage", &params, &item.content, &item.listContent, &item.customFields, &publish, NULL);
}

void WPNetworkEngine::DeletePage (int localId, QString itemId)
{
    ExecuteGenericRequest(EDeletePage, localId, "wp.deletePage", &itemId, NULL, NULL, NULL);
}

/* Posts */
void WPNetworkEngine::GetPost (int localId, QString itemId)
{
    // non-standard parameters
    QStringList params;
    params << itemId << iEndpoint.iUsername << iEndpoint.iPassword;

    ExecuteCustomRequest(EGetPost, localId, "metaWeblog.getPost", &params, NULL, NULL, NULL, NULL); 
}

void WPNetworkEngine::NewPost (int localId, WPXmlItem item)
{
    // XXX: wut?
    QString publish = "1";

    ExecuteGenericRequest(ENewPost, localId, "metaWeblog.newPost", NULL, &item.content,
                          &item.listContent, &item.customFields, &publish);
}

void WPNetworkEngine::EditPost (int localId, QString itemId, WPXmlItem item)
{
    // XXX: wut?
    QString publish = "0";

    // non-standard parameter order
    QStringList params;
    params << itemId << iEndpoint.iUsername << iEndpoint.iPassword;

    ExecuteCustomRequest(EEditPost, localId, "metaWeblog.editPost", &params, &item.content,
                         &item.listContent, &item.customFields, &publish);
}

void WPNetworkEngine::DeletePost (int localId, QString itemId)
{
    // XXX: wut?
    QString publish = "1";

    // non-standard parameter order
    QStringList params;

    // XXX: app key = 123
    params << "123" << itemId << iEndpoint.iUsername << iEndpoint.iPassword;

    ExecuteCustomRequest(EDeletePost, localId, "metaWeblog.deletePost", &params, NULL, NULL, NULL, &publish); 
}

/* Comments */
void WPNetworkEngine::GetComment (int localId, QString itemId)
{
    ExecuteGenericRequest(EGetComment, localId, "wp.getComment", &itemId, NULL, NULL, NULL);
}

void WPNetworkEngine::NewComment (int localId, WPXmlItem item)
{
    QString parentId;
    QMap <QString, QString> content;

    // XXX: decode item
    parentId = item.content["post_id"];
    content["comment_parent"] = item.content["comment_parent"];
    content["content"] = item.content["content"];

    ExecuteGenericRequest(ENewComment, localId, "wp.newComment", &parentId, &content, NULL, NULL);
}

void WPNetworkEngine::EditComment (int localId, QString itemId, WPXmlItem item)
{
    QMap <QString, QString> content;

    // XXX: decode item
    content["status"] = item.content["status"];

    ExecuteGenericRequest(EEditComment, localId, "wp.editComment", &itemId, &content, NULL, NULL);
}

void WPNetworkEngine::DeleteComment (int localId, QString itemId)
{
    ExecuteGenericRequest(EDeleteComment, localId, "wp.deleteComment", &itemId, NULL, NULL, NULL);
}

/* Files */
void WPNetworkEngine::UploadFiles (int localId, QList<WPMediaFile> files)
{
    // ensure we have some files to upload...
    if (files.isEmpty())
        // XXX: notify failure...
        return;

    // add item
    FileUploadItem item;
    item.files = files;
    item.localId = localId;

    // enqueue, and possibly start
    EnqueueFileUploads(item);
}

/* File */
void WPNetworkEngine::UploadFile (WPMediaFile *mfile)
{
    qDebug()<<"Network"<<mfile->filename;
    int localId = -22;
    // ensure we have some files to upload...
    if (mfile->filename.isEmpty())
        // XXX: notify failure...
        return;

    // add item
    iFileUploadItem = mfile;

    QFile file (mfile->filename);

    // selected data source
    QIODevice *rawDevice = NULL;

    // temporary buffer for possible resize
    QBuffer tmp_buffer;
    if (mfile->type == WPMediaFile::MEDIA_TYPE_IMAGE &&
            (!mfile->size.isNull() || mfile->rotationAngle != 0)) {
        if (Utils::resizeImage(mfile->filename, tmp_buffer, mfile->size, mfile->rotationAngle)) {
            rawDevice = &tmp_buffer;
        }
    }

    // No manipulation on file needed (or manipulation failed)
    if(!rawDevice) {
        rawDevice = &file;
    }

    // re-open for read only
    if (!rawDevice->open(QIODevice::ReadOnly))
        return;// failure

    // ok, send
    // 2KiB + approx. base64 length
    // the XML is perhaps 500 bytes or so, so 2KiB should be enough
    // but if this fills up, the QByteArray will *double* in size, afaik
    // this assumes that all of the QIODevices above will report their size(), or just 0
    int const approxRequestSize = (2 * 1024) + (rawDevice->size() * 4 / 3);

    // buffer for request XML
    QSharedPointer<QIODevice> body;
    if(approxRequestSize < 1 * 1024 * 1024) {
        QSharedPointer<QBuffer> buffer(new QBuffer());
        buffer->buffer().reserve(approxRequestSize);
        body = buffer;
    } else {
        body = Utils::getTempFileWithMinimumSize(approxRequestSize);
    }

    if(!body) {
        // XXX Show error!
        //ErrorDialog dlg;
        //dlg.ShowError(tr("Upload failed"), tr("Please free some disk space and try again"));
        return;
    }

    // outgoing document
    WPXmlRequest req(body.data());

    // wp.uploadFile
    req.startMethodCall("wp.uploadFile");

    // params
    req.writeStringParam(iEndpoint.iBlogId);    // blog_id
    req.writeStringParam(iEndpoint.iUsername);  // username
    req.writeStringParam(iEndpoint.iPassword);  // password
    req.startStructParam();                 // data

    req.writeStringMember("name", mfile->filename.section('/', -1));
    req.writeStringMember("type", mfile->getMimeType());
    req.writeBase64Member("bits", *rawDevice);

    req.endStructParam();

    req.endMethodCall();

    // send request, emitting TaskStarted
    ExecuteRequest(iEndpoint, EUploadFile, localId, body, "text/xml");
}

void WPNetworkEngine::GetGravatar (QByteArray hash, int size)
{
    // request URL
    QUrl url("http://www.gravatar.com/avatar/" + hash + "?d=404&s=" + QString::number(size));

    // execute and track
    ExecuteRequest(url, EGetGravatar, 0);
}

/* Categories */
void WPNetworkEngine::NewCategory (int localId, WPXmlItem item)
{
    ExecuteGenericRequest(ENewCategory, localId, "wp.newCategory", NULL, &item.content);
}

/* Legacy requests */
QNetworkRequest WPNetworkEngine::CreateNetworkRequest(const QUrl& aUrl, TWPNetworkEngineCommand aType)
{
    QNetworkRequest request(aUrl);
    request.setAttribute(QNetworkRequest::Attribute(QNetworkRequest::User + 1), QVariant((int)aType));
    request.setRawHeader("User-Agent", WP_USERAGENT_STR.toAscii());
    return request;
}

/* Gravatars */
QByteArray WPNetworkEngine::ParseHashFromGravatarUrl (const QUrl &url)
{
    QString path = url.path();

    // from '/' ... END
    int start = path.lastIndexOf('/');

    return path.mid(start + 1, -1).toAscii();
}

QString WPNetworkEngine::LocalizedTime(const QDateTime& aDateTime)
{
    QLocale locale;
    QString localStringFormat = locale.dateTimeFormat(QLocale::ShortFormat);
    return aDateTime.toString(localStringFormat);
}

QString WPNetworkEngine::LocalizedTime(const QString& aTimeString)
{
    return LocalizedTime(QDateTime::fromString(aTimeString, "yyyyMMddTHH:mm:ss"));
}

QString WPNetworkEngine::ShortLocalizedTime(const QString& aTimeString)
{
    QDateTime time(QDateTime::fromString(aTimeString, "yyyyMMddTHH:mm:ss"));
    return time.toString("MM/dd HH:mm");
}

QString WPNetworkEngine::ISO8601Time(const QDateTime& aDateTime)
{
    return aDateTime.toString("yyyyMMddTHH:mm:ss");
}

void WPNetworkEngine::ClearRequestQueue()
{
    // We copy the request list and call abort to each request on the copied array.
    // This assures that all requests will be aborted, no matter whether
    // abort() function is synchronous or asynchronous.
    QList<WPNetworkQueueItem> requests;
    const int requestCount = iCurrentRequests.size();
    for (int i = 0; i < requestCount; i++)
        requests.append(iCurrentRequests[i]);
    for (int i = 0; i < requestCount; i++)
        requests[i].iNetworkReply->abort();
    // TODO if this happens, make sure that clientonly articles are
    // added to local drafts.
}

/* File uploads */
void WPNetworkEngine::EnqueueFileUploads (FileUploadItem &item)
{
    // enqueue copy
    iFileUploadQueue.append(item);

    // list was previously empty
    if (iFileUploadQueue.size() == 1)
        // start uploading
        NextFileUpload();
}

void WPNetworkEngine::NextFileUpload ()
{
    Q_ASSERT(!iFileUploadQueue.isEmpty());

    do {
        FileUploadItem &item = iFileUploadQueue.first();

        // remaining files for this item?
        while (!item.files.isEmpty()) {
            // examine next file
            WPMediaFile &file = item.files.first();

            // and upload it
            if (ExecuteFileUpload(item.localId, file)) {
                // ok, success, file is uploading
                return;
            } else {
                // item failed to start upload, skip it and try the next one
                // XXX: notify error?
                item.files.removeFirst();
                continue;
            }
        }
        
        // file upload is done
        // results list from EUploadMultipleFiles handler
        emit FilesUploaded(item.localId, item.results);

        // remove it, invalidating item
        iFileUploadQueue.removeFirst();

        // continue with the next item if we have one
    } while (!iFileUploadQueue.isEmpty());
}

bool WPNetworkEngine::ExecuteFileUpload (int localId, const WPMediaFile &fileinfo)
{
    QFile file (fileinfo.filename);

    // selected data source
    QIODevice *rawDevice = NULL;

    // temporary buffer for possible resize
    QBuffer tmp_buffer;
    if (fileinfo.type == WPMediaFile::MEDIA_TYPE_IMAGE &&
            (!fileinfo.size.isNull() || fileinfo.rotationAngle != 0)) {
        if (Utils::resizeImage(fileinfo.filename, tmp_buffer, fileinfo.size, fileinfo.rotationAngle)) {
            rawDevice = &tmp_buffer;
        }
    }

    // No manipulation on file needed (or manipulation failed)
    if(!rawDevice) {
        rawDevice = &file;
    }
    
    // re-open for read only
    if (!rawDevice->open(QIODevice::ReadOnly))
        // failure
        return false;

    // ok, send
    ExecuteFileUploadRequest(localId, fileinfo, rawDevice);

    // ok
    return true;
}

void WPNetworkEngine::ExecuteFileUploadRequest (int localId, const WPMediaFile &file, QIODevice *src)
{
    // 2KiB + approx. base64 length
    // the XML is perhaps 500 bytes or so, so 2KiB should be enough
    // but if this fills up, the QByteArray will *double* in size, afaik
    // this assumes that all of the QIODevices above will report their size(), or just 0
    int const approxRequestSize = (2 * 1024) + (src->size() * 4 / 3); 
    
    // buffer for request XML
    QSharedPointer<QIODevice> body;
    if(approxRequestSize < 1 * 1024 * 1024) {
        QSharedPointer<QBuffer> buffer(new QBuffer());
        buffer->buffer().reserve(approxRequestSize);
        body = buffer;
    } else {
        body = Utils::getTempFileWithMinimumSize(approxRequestSize);
    }

    if(!body) {
        // XXX Show error!
        //ErrorDialog dlg;
        //dlg.ShowError(tr("Upload failed"), tr("Please free some disk space and try again"));
        return;
    }
    
    // outgoing document
    WPXmlRequest req(body.data());

    // wp.uploadFile
    req.startMethodCall("wp.uploadFile");

    // params
    req.writeStringParam(iEndpoint.iBlogId);    // blog_id
    req.writeStringParam(iEndpoint.iUsername);  // username
    req.writeStringParam(iEndpoint.iPassword);  // password
    req.startStructParam();                 // data

    req.writeStringMember("name", file.filename.section('/', -1));
    req.writeStringMember("type", file.getMimeType());
    req.writeBase64Member("bits", *src);

    req.endStructParam();
    
    req.endMethodCall();

    // send request, emitting TaskStarted
    ExecuteRequest(iEndpoint, EUploadMultipleFiles, localId, body, "text/xml");
}

int WPNetworkEngine::NextUniqId()
{
    return iUniqReqId++;
}

void WPNetworkEngine::ReportStatsIfNeeded (int blogCount)
{
#ifdef __WINS__
    return;
#endif
    
    // XXX: use normal WPData here instead?
    QSettings settings("WordPress", "MC Stats");
    QDate statsDate = settings.value("statsDate", 0).toDate();
    QDate curDate = QDate::currentDate();

    // report stats initially, or every 7 days
    if (!statsDate.isValid() || statsDate.daysTo(curDate) > 7) {    
        QUrl url("http://api.wordpress.org/nokiaapp/update-check/1.0/");

        // POST params
        QUrl params = QUrl();
        
#ifdef HAVE_QTMOBILITY
        QtMobility::QSystemInfo sysInfo;
        QtMobility::QSystemDeviceInfo devInfo;

        // sysinfo
        params.addQueryItem("device_version",sysInfo.version(QtMobility::QSystemInfo::Firmware));
        params.addQueryItem("device_language",sysInfo.currentLanguage());
        params.addQueryItem("mobile_country_code",sysInfo.currentCountryCode());
        params.addQueryItem("device_uuid",devInfo.imei());
        // params.addQueryItem("mobile_network_number", /* network operator (MCC+MNC) */);
#endif

        // generic
        params.addQueryItem("app_version", WP_VERSION_STR);
        params.addQueryItem("num_blogs", QString::number(blogCount));

        // os
#if defined(Q_OS_SYMBIAN)
        params.addQueryItem("device_os", "Symbian");
#elif defined(Q_WS_MAEMO_5)
        params.addQueryItem("device_os", "Maemo");
#else
        params.addQueryItem("device_os", "Other");
#endif
        
        // x-www-form-urlencoded POST Data
        QSharedPointer<QBuffer> body(new QBuffer());
        body->buffer() = params.encodedQuery();

        // POST
        ExecuteRequest(url, EReport, 0, body, "application/x-www-form-urlencoded");
    }
}

void WPNetworkEngine::OpenNetworkSession()
{
#ifdef HAVE_QTMOBILITY
    using namespace QtMobility;

    if(iNetworkSession) {
        iNetworkSession->deleteLater();
        iNetworkSession = 0;
    }
    
    // Set Internet Access Point
    QNetworkConfigurationManager manager;
    const bool canStartIAP = (manager.capabilities()
                              & QNetworkConfigurationManager::CanStartAndStopInterfaces);
    // Is there default access point, use it
    QNetworkConfiguration cfg = manager.defaultConfiguration();
    if (!cfg.isValid() || (!canStartIAP && cfg.state() != QNetworkConfiguration::Active)) {
        emit NoAccessPoints();
        return;
    }

    iNetworkSession = new QNetworkSession(cfg, this);
    
    connect(iNetworkSession, SIGNAL(stateChanged(QNetworkSession::State)),
            this, SLOT(SessionStateChanged(QNetworkSession::State)));
    
    qDebug("Opening network session...");
    emit TaskStarted(EOpenNetworkSession);
    iNetworkSession->open();
#endif // HAVE_QTMOBILITY
}

bool WPNetworkEngine::QueueRequestIfNecessary(const QUrl &url, TWPNetworkEngineCommand cmd, int id, QSharedPointer<QIODevice> body, const QString &contentType)
{
    // Don't queue on scratchbox i386 targets
#ifdef SBOX_DPKG_INST_ARCH
    if(#SBOX_DPKG_INST_ARCH == "i386") { return false; }
#endif // SBOX_DPKG_INST_ARCH

#ifdef HAVE_QTMOBILITY
    using namespace QtMobility;
    
    if(!iNetworkSession) {
        OpenNetworkSession();
    } else if (iNetworkSession->state() == QNetworkSession::Connected) {
        return false;
    }
    
    iQueuedRequests.append(QueuedNetworkRequest(url, cmd, id, body, contentType));
    return true;
    
#else // HAVE_QTMOBILITY
    Q_UNUSED(url); Q_UNUSED(cmd); Q_UNUSED(id); Q_UNUSED(body); Q_UNUSED(contentType);
    return false; // Never queue
#endif // HAVE_QTMOBILITY
}

void WPNetworkEngine::ExecuteQueuedRequests()
{
    // Try to execute the requests currently in queue,
    // don't care what happens in the end of the queue
    int size = iQueuedRequests.size();
    for(int i = 0; i < size; ++i) {
        QueuedNetworkRequest const & r(iQueuedRequests.first());
        ExecuteRequest(r.url, r.cmd, r.id, r.body, r.contentType);
        iQueuedRequests.removeFirst();
    }
}

#ifdef HAVE_QTMOBILITY
void WPNetworkEngine::SessionStateChanged(QtMobility::QNetworkSession::State state)
{
    using namespace QtMobility;

    switch(state) {
    case QNetworkSession::Invalid:
        emit TaskFailed(EOpenNetworkSession, tr("Network configuration invalid"));
        break;
    case QNetworkSession::NotAvailable:
        emit TaskFailed(EOpenNetworkSession, tr("Network unavailable"));
        break;
    case QNetworkSession::Disconnected:
        emit TaskFailed(EOpenNetworkSession, tr("Disconnected from network"));
        break;
    case QNetworkSession::Connected:
        emit TaskFinished(EOpenNetworkSession);
        break;
    default:
        break;
    }

    switch(state) {
    case QNetworkSession::Invalid:
    case QNetworkSession::NotAvailable:
    case QNetworkSession::Disconnected:
        qWarning() << "Network session error:" << iNetworkSession->errorString();
        iNetworkSession->deleteLater();
        iNetworkSession = 0;
        break;
    case QNetworkSession::Connected:
        qDebug("Connected");
        ExecuteQueuedRequests();
        break;
    default:
        // Do nothing
        break;
    }
}
#endif // HAVE_QTMOBILITY

// End of file
