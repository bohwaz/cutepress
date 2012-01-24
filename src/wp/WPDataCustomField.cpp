#include "WPDataCustomField.h"

/*
 * used for QList functions, being == does not mean they're the same custom field
 */
bool operator==(const WPDataCustomField& first, const WPDataCustomField& second){
    return first.key == second.key;
}

