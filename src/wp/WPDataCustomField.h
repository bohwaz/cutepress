#ifndef WPDATACUSTOMFIELD_H
#define WPDATACUSTOMFIELD_H

#include <QString>

/*
 * A simple struct for storing post (and perhaps comment?) custom fields for WordPress
 */
struct WPDataCustomField {
    WPDataCustomField(QString id, QString key, QString value) : id(id), key(key), value(value) {}
    
    QString id;
    QString key;
    QString value;
};

bool operator==(const WPDataCustomField& first, const WPDataCustomField& second);

#endif
