#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>

QString formatSize(qint64 size);

QByteArray percentEncode(const QByteArray &data);

#endif
