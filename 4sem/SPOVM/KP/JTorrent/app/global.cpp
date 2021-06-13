#include "global.h"

QString formatSize(qint64 size)
{
	int i;
	qint64 nw = size, mul = 1;
	for (i = 0; nw >= 1024 && i < 7; i++, mul *= 1024, nw /= 1024);
	double nbytes = (double)size/mul;


	QString str = QString::number(nbytes, 'f', 2);
	str += ' ';

	switch (i) {
	case 0:
        return str + "Б";
	case 1:
        return str + "Кб";
	case 2:
        return str + "Мб";
	case 3:
        return str + "Гб";
	case 4:
        return str + "Тб";
	default:
        return str + "0б";
	}
}

QByteArray percentEncode(const QByteArray &data)
{
	QByteArray encoded;
	for (char b : data) {
		encoded += '%';
		encoded += QByteArray::number(b, 16).right(2).rightJustified(2, '0');
	}
	return encoded;
}
