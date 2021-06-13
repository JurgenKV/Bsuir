#ifndef BENCODE_H
#define BENCODE_H

#include "bencodevalue.h"
#include <QByteArray>
#include <QString>
#include <QList>
#include <QTextStream>

class BencodeParser
{

    QString _errorString;
	void setError(const QString &errorString);
	void clearError();

    QByteArray _bencodeData;

    QList<BencodeValue *> _mainList;

public:
	BencodeParser();
	~BencodeParser();

	QString errorString() const;

	void setData(const QByteArray &data);

	bool readFile(const QString &fileName);

	bool parse(const QByteArray &data);

	bool parse();

	const QByteArray& rawBencodeData() const;

	QList<BencodeValue *> list() const;
};

#endif
