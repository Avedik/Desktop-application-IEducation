#ifndef DATATYPES_H
#define DATATYPES_H

#include <QDataStream>
#include <QMetaType>

enum class DataTypes : qint32 {
    JSON, IMAGE, PDF_FILE, POINT, BRUSH_COLOR, AUDIO_FILE
};
Q_DECLARE_METATYPE(DataTypes);

QDataStream& operator<<(QDataStream & ds, DataTypes t);

QDataStream& operator>>(QDataStream & ds, DataTypes& t);

#endif // DATATYPES_H
