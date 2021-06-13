#ifndef BENCODEVALUE_H
#define BENCODEVALUE_H

#include <QByteArray>
#include <QTextStream>
#include <QString>
#include <QList>
#include <QMap>

class BencodeParser;
class BencodeInteger;
class BencodeString;
class BencodeList;
class BencodeDictionary;

class BencodeException
{
    QString _errorString;

public:
    BencodeException(const QString &errorString) : _errorString(errorString) {}
	BencodeException() {}

	template<typename T>
	BencodeException &operator<<(const T &toAppend)
	{
        QTextStream stream(&_errorString);
		stream << toAppend;
		return *this;
	}

	const QString &what() const
	{
        return _errorString;
	}
};

class BencodeValue
{
public:
	enum class Type
	{
		Integer, String, List, Dictionary
	};

protected:
    Type _type;

    int _dataPosBegin;
    int _dataPosEnd;
    const QByteArray *_bencodeData;

	virtual void loadFromByteArray(const QByteArray &data, int &position) = 0;

public:
	BencodeValue(Type type);
	virtual ~BencodeValue();

	Type type() const;

	bool isInteger() const;
	bool isString() const;
	bool isList() const;
	bool isDictionary() const;

	BencodeInteger *toBencodeInteger();
	BencodeString *toBencodeString();
	BencodeList *toBencodeList();
	BencodeDictionary *toBencodeDictionary();

	virtual qint64 toInt();
	virtual QByteArray toByteArray();
	virtual QList<BencodeValue *> toList();

	virtual QByteArray bencode(bool includeMetadata = true) const = 0;

	QByteArray getRawBencodeData(bool includeMetadata = true);

	static BencodeValue *createFromByteArray(const QByteArray &data, int &position);

	virtual void print(QTextStream &out) const = 0;
	virtual bool equalTo(BencodeValue *other) const = 0;
};

class BencodeInteger : public BencodeValue
{
protected:
    qint64 _value;

	void loadFromByteArray(const QByteArray &data, int &position);

public:
	BencodeInteger();
	BencodeInteger(qint64 value);
	~BencodeInteger();

	qint64 toInt();
	void setValue(qint64 value);
	QByteArray bencode(bool includeMetadata = true) const;
	void print(QTextStream &out) const;
	bool equalTo(BencodeValue *other) const;
};

class BencodeString : public BencodeValue
{
protected:
    QByteArray _value;

	void loadFromByteArray(const QByteArray &data, int &position);

public:
	BencodeString();
	BencodeString(const QByteArray &value);
	~BencodeString();

	QByteArray toByteArray();
	void setValue(const QByteArray &value);
	QByteArray bencode(bool includeMetadata = true) const;
	void print(QTextStream &out) const;
	bool equalTo(BencodeValue *other) const;
};

class BencodeList : public BencodeValue
{
protected:
    QList<BencodeValue *> _values;

	void loadFromByteArray(const QByteArray &data, int &position);

public:
	BencodeList();
	~BencodeList();

	QList<BencodeValue *> toList();
	void setValues(const QList<BencodeValue *> &values);
	void add(BencodeValue *value);
	QByteArray bencode(bool includeMetadata = true) const;
	void print(QTextStream &out) const;
	bool equalTo(BencodeValue *other) const;
};

class BencodeDictionary : public BencodeValue
{
protected:
    QMap<QByteArray, BencodeValue *> _values;

	void loadFromByteArray(const QByteArray &data, int &position);

public:
	BencodeDictionary();
	~BencodeDictionary();

	void print(QTextStream &out) const;
	bool equalTo(BencodeValue *other) const;

	QList<QByteArray> keys() const;
	QList<BencodeValue *> values() const;
	bool keyExists(const QByteArray &key) const;
	BencodeValue *value(const QByteArray &key) const;
	void add(const QByteArray &key, BencodeValue *value);
	QByteArray bencode(bool includeMetadata = true) const;
};

#endif
