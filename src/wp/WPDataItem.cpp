/**
 * WPDataItem-related template method definitions 
 */

#include "WpMediaFile.h"

/* WPDataItem */
template <class DataItemType>
WPDataItem<DataItemType>::WPDataItem (WPDataBlog &blog, WPDataItemCollection<DataItemType> &collection) :
    iBlog(blog),
    iCollection(collection),
    iDateCreated(QDateTime::currentDateTime()),
    iLocalId(0), 
    iDirty(false),
    iDeleted(false),
    iError(false)
{

}

template <class DataItemType>
QString WPDataItem<DataItemType>::GetISO8601Time () const
{
    return iDateCreated.toUTC().toString("yyyyMMddTHH:mm:ss");
}

template <class DataItemType>
void WPDataItem<DataItemType>::SetISO8601Time (const QString &dt)
{
    QDateTime tmp;
   
    // read as UTC
    tmp = QDateTime::fromString(dt, "yyyyMMddTHH:mm:ss");
    tmp.setTimeSpec(Qt::UTC);

    // convert to local time
    iDateCreated = tmp.toLocalTime();
}

template <class DataItemType>
void WPDataItem<DataItemType>::StoreLocal ()
{
    // assign ID
    iLocalId = iBlog.AssignLocalId();

    // add to collection, as superclass type
    iCollection.iItems.append(QExplicitlySharedDataPointer<DataItemType>((DataItemType *) this));
}
    
template <class DataItemType>
void WPDataItem<DataItemType>::MarkError ()
{
    iError = true;
}

template <class DataItemType>
void WPDataItem<DataItemType>::SetDirty ()
{
    iDirty = true;
}

template <class DataItemType>
void WPDataItem<DataItemType>::DropLocal ()
{
    // find and remove from list
    // XXX: very ugly cast
    iCollection.iItems.removeOne(QExplicitlySharedDataPointer<DataItemType>((DataItemType *) this));

    // invalidate
    iLocalId = 0;
}

template <class DataItemType>
void WPDataItem<DataItemType>::UpdateLocal ()
{
    // status
    if (IsOrphaned())
        // register
        StoreLocal();

    // signals, for superclass type
    iCollection.UpdateItem((DataItemType &) (*this));
}   

template <class DataItemType>
void WPDataItem<DataItemType>::UpdateStatus (const QString &status, const QList<WPMediaFile> &addedFiles)
{
    // update
    iStatus = status;

    // status
    if (IsOrphaned())
        // register
        StoreLocal();

    // have local modifications
    SetDirty();

    // signals, for superclass type
    iCollection.UpdateItem((DataItemType &) (*this), addedFiles);
}

template <class DataItemType>
void WPDataItem<DataItemType>::Delete ()
{
    // mark
    iDeleted = true;

    // signals, for superclass type
    iCollection.UpdateItem((DataItemType &) (*this));
}

/* WPDataItemCollection */
template <class DataItemType>
WPDataItemCollection<DataItemType>::WPDataItemCollection (WPDataBlog &blog) :
    iBlog(blog)
{

}

template <class DataItemType>
QListIterator<QExplicitlySharedDataPointer<DataItemType> > WPDataItemCollection<DataItemType>::GetItems () const
{
    return QListIterator<ItemType> (iItems);
}

template <class DataItemType>
QExplicitlySharedDataPointer<DataItemType> WPDataItemCollection<DataItemType>::GetByItemId (const QString &itemId)
{
    if (itemId.isEmpty())
        // never a valid ID
        return ItemType(NULL);

    for (QListIterator<ItemType> i(iItems); i.hasNext(); ) {
        ItemType item = i.next();

        if (item->iItemId == itemId)
            // found
            return item;
    }

    // not found
    return ItemType(NULL);
}

template <class DataItemType>
QExplicitlySharedDataPointer<DataItemType> WPDataItemCollection<DataItemType>::GetByLocalId (int localId)
{
    if (localId == 0)
        // never a valid ID
        return ItemType(NULL);

    for (QListIterator<ItemType> i(iItems); i.hasNext(); ) {
        ItemType item = i.next();

        if (item->iLocalId == localId)
            // found
            return item;
    }

    // not found
    return ItemType(NULL);
}

template <class DataItemType>
QExplicitlySharedDataPointer<DataItemType> WPDataItemCollection<DataItemType>::GetByLocalId (const QString &localId)
{
    return GetByLocalId(localId.toInt());
}
    
template <class DataItemType>
void WPDataItemCollection<DataItemType>::RefreshAll ()
{
    // signal network engine
    emit FetchItems();
}

template <class DataItemType>
void WPDataItemCollection<DataItemType>::UpdateItem (DataItemType &item, QList<WPMediaFile> addedFiles)
{
    if (item.iDeleted) {
        // page should be deleted
        if (item.IsLocal()) {
            // item updated
            emit _ItemRemoved(item.iLocalId);

            // drop item from list
            item.DropLocal();
            
        } else {
            // remove on server
            emit DeleteItem(item.iLocalId, item.iItemId);

            // XXX: item busy
            emit _ItemChanged(item.iLocalId);
        }

    } else if (!addedFiles.isEmpty()) {

        // upload images for file, will re-run UpdateItem from FilesUploaded once done
        emit UploadFiles(item.iLocalId, addedFiles);

        // XXX: item busy
        emit _ItemChanged(item.iLocalId);

    } else if (item.IsDirty()) {
        // local changes, send to server
        WPXmlItem xml;

        // item data
        item.GetContent(&xml);

        if (item.IsLocal()) {
            // new item not yet on server
            emit NewItem(item.iLocalId, xml);

            // item listed, busy
            emit _ItemChanged(item.iLocalId);

        } else {
            // existing item
            emit EditItem(item.iLocalId, item.iItemId, xml);

            // item changed, busy
            emit _ItemChanged(item.iLocalId);
        }

    } else if (item.IsLocal()) {
        // item updated
        emit _ItemChanged(item.iLocalId);

    } else {
        // nothing... ?
        return;
    }

    // list changed
    emit UpdateItemData(iBlog);
}

template <class DataItemType>
void WPDataItemCollection<DataItemType>::ItemsListed (QList<WPXmlItem> itemList)
{
    // add new items, edit existing
    for (QListIterator<WPXmlItem> i(itemList); i.hasNext(); ) {
        const WPXmlItem &xml = i.next();
        ItemType item;

        const QString &itemId = xml.content[DataItemType::ITEM_ID_KEY];
        
        // find existing
        if (!(item = GetByItemId(itemId))) {
            // register new
            item = new DataItemType(iBlog);
            item->StoreLocal(); // XXX: should be protected

            // update contents
            item->SetContent(xml);

            // new item; listed
            // emit _ItemAdded(item->iLocalId);

        } else {
             // update contents
            item->SetContent(xml);

            // item changed
            // emit _ItemChanged(item->iLocalId);
        }
    }

    // remove missing items
    for (QMutableListIterator<ItemType> i(iItems); i.hasNext(); ) {
        ItemType item = i.next();

        // ignore local-only items
        if (item->IsLocal())
            continue;

        bool found = false;

        // find corresonding content entry
        for (QListIterator<WPXmlItem> j(itemList); j.hasNext(); ) {
            const WPXmlItem &xml = j.next();
            
            if (xml.content[DataItemType::ITEM_ID_KEY] != item->iItemId)
                continue;
            
            // match
            found = true;

            // XXX: j.remove() ?

            break;
        }

        if (found)
            // keep
            continue;
       
        // item about to be removed 
        // emit _ItemRemoved(item->iLocalId);
        
        // XXX: just drop these silently in these cases
        // item->ItemDeleted();

        // item isn't wasn't in list, so remove it
        // could we just use DropLocal instead?
        item->iLocalId = 0;
        i.remove();
    }
    
    // list changed
    emit UpdateItemDataFull(iBlog);
}

template <class DataItemType>
void WPDataItemCollection<DataItemType>::ItemGotten (int localId, WPXmlItem xml)
{
    ItemType item = GetByLocalId(localId);

    if (!item)
        return;

    // update, unset dirty
    item->SetContent(xml);

    emit _ItemChanged(localId);
    emit UpdateItemData(iBlog);
}

template <class DataItemType>
void WPDataItemCollection<DataItemType>::ItemCreated (int localId, QString itemId)
{
    ItemType item = GetByLocalId(localId);

    if (!item)
        return;

    // assign ID
    item->iItemId = itemId;

    // leave dirty, and update additional data before marking clean
    emit GetItem(localId, itemId);
   
    // item was actually added by UpdateItem's ItemChanged, so don't add twice!
    // XXX: wait for ItemGotten...
//    emit _ItemChanged(localId);
//    emit UpdateItemData(iBlog);
}

template <class DataItemType>
void WPDataItemCollection<DataItemType>::ItemEdited (int localId)
{
    ItemType item = GetByLocalId(localId);

    if (!item)
        return;

    // update changes to make sure
    emit GetItem(localId, item->iItemId);
    
    // XXX: wait for ItemGotten...
//    emit _ItemChanged(localId);
//    emit UpdateItemData(iBlog);
}

template <class DataItemType>
void WPDataItemCollection<DataItemType>::ItemDeleted (int localId)
{
    ItemType item = GetByLocalId(localId);

    if (!item)
        return;

    // item about to be removed from list
    emit _ItemRemoved(localId);
   
    // react 
    item->ItemDeleted();

    // remove
    item->DropLocal();

    // list changed
    emit UpdateItemData(iBlog);
}
    
template <class DataItemType>
void WPDataItemCollection<DataItemType>::ItemFailed (int localId, TWPNetworkEngineCommand command)
{
    // XXX: do we care?
    (void) command;

    if (!localId)
        // probably a DeleteItem...
        return;

    ItemType item = GetByLocalId(localId);

    if (!item)
        // XXX: warn
        return;

    // mark
    item->MarkError();
    
    // item failed
    emit _ItemChanged(localId);
    emit UpdateItemData(iBlog);
}

template <class DataItemType>
void WPDataItemCollection<DataItemType>::FilesUploaded (int localId, QList<WPMediaFile> files)
{
    ItemType item = GetByLocalId(localId);

    if (!item)
        // not our upload
        return;
   
    foreach (const WPMediaFile &file, files) {
        // add in link
        item->AddSnippet(file.getHtmlSnippet(), file.alignment == WPMediaFile::Bottom ? false : true);
    }

    // update for modified content
    // XXX: as reference
    UpdateItem(*item);
}

template <class DataItemType>
QDataStream & operator<< (QDataStream &out, const WPDataItemCollection<DataItemType> &collection)
{
    // XXX: version number...

    out << (quint32) collection.iItems.size();

    for (QListIterator<QExplicitlySharedDataPointer<DataItemType> > i(collection.iItems); i.hasNext(); ) {
        QExplicitlySharedDataPointer<DataItemType> item = i.next();

        // no localId involved here
        out << *item;
    }

    return out;
}

template <class DataItemType>
QDataStream & operator>> (QDataStream &in, WPDataItemCollection<DataItemType> &collection)
{
    quint32 count;

    // XXX: version number...

    in >> count;

    while (count--) {
        QExplicitlySharedDataPointer<DataItemType> item(new DataItemType(collection.iBlog));

        // read in content
        in >> *item;
    
        // store in collection
        item->StoreLocal();
    }

    return in;
}


