#include "dataTypes.h"

QDataStream& operator<<(QDataStream & ds, DataTypes t) {
    return ds << (qint32)t;
}

QDataStream& operator>>(QDataStream & ds, DataTypes& t) {
    qint32 val;
    ds >> val;
    if (ds.status() == QDataStream::Ok)
        t = DataTypes(val);
    return ds;
}
