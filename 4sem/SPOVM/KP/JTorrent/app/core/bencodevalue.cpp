#include "bencodevalue.h"
#include <QDataStream>
#include <QDebug>

BencodeValue::BencodeValue(Type type)
    : _type(type)
    , _dataPosBegin(0)
    , _dataPosEnd(0)
    , _bencodeData(nullptr)
{
}

BencodeValue::~BencodeValue()
{
}


BencodeValue::Type BencodeValue::type() const
{
    return _type;
}


bool BencodeValue::isInteger() const
{
    return _type == Type::Integer;
}

bool BencodeValue::isString() const
{
    return _type == Type::String;
}

bool BencodeValue::isList() const
{
    return _type == Type::List;
}

bool BencodeValue::isDictionary() const
{
    return _type == Type::Dictionary;
}


BencodeInteger *BencodeValue::toBencodeInteger()
{
	if (!isInteger()) {
		QString errorString;
		QTextStream err(&errorString);
		err << "BencodeValue::toBencodeInteger(): Value is not an integer: ";
		print(err);
		throw BencodeException(errorString);
	}
	return static_cast<BencodeInteger *>(this);
}

BencodeString *BencodeValue::toBencodeString()
{
	if (!isString()) {
		QString errorString;
		QTextStream err(&errorString);
		err << "bencodeValue::toBencodeString(): Value is not an string: ";
		print(err);
		throw BencodeException(errorString);
	}
	return static_cast<BencodeString *>(this);
}

BencodeList* BencodeValue::toBencodeList()
{
	if (!isList()) {
		QString errorString;
		QTextStream err(&errorString);
		err << "BencodeValue::toBencodeList(): Value is not an list: ";
		print(err);
		throw BencodeException(errorString);
	}
	return static_cast<BencodeList *>(this);
}

BencodeDictionary *BencodeValue::toBencodeDictionary()
{
	if (!isDictionary()) {
		QString errorString;
		QTextStream err(&errorString);
		err << "BencodeValue::toBencodeDictionary(): Value is not an dictionary";
		print(err);
		throw BencodeException(errorString);
	}
	return static_cast<BencodeDictionary *>(this);
}


qint64 BencodeValue::toInt()
{
	return toBencodeInteger()->toInt();
}

QByteArray BencodeValue::toByteArray()
{
	return toBencodeString()->toByteArray();
}

QList<BencodeValue *> BencodeValue::toList()
{
	return toBencodeList()->toList();
}


QByteArray BencodeValue::getRawBencodeData(bool includeMetadata)
{
	QByteArray returnData;
    int begin = _dataPosBegin;
    int end = _dataPosEnd;

	if (!includeMetadata) {
		begin++;
		end--;
	}

	for (int i = begin; i < end; i++) {
        returnData.push_back(_bencodeData->at(i));
	}

	return returnData;
}


BencodeValue *BencodeValue::createFromByteArray(const QByteArray &data, int &position)
{
	BencodeException ex("BencodeValue::createFromByteArray(): ");

	if (position >= data.size()) {
		throw ex << "Unexpectedly reached end of the data stream";
	}

	BencodeValue *value;
	char firstByte = data[position];
	if (firstByte == 'i') {
		value = new BencodeInteger;
	} else if (firstByte >= '0' && firstByte <= '9') {
		value = new BencodeString;
	} else if (firstByte == 'l') {
		value = new BencodeList;
	} else if (firstByte == 'd') {
		value = new BencodeDictionary;
	} else {
		throw ex << "Invalid begining character for bencode value: "
				 << "'" << firstByte << "'."
				 << "Expected 'i', 'l', 'd' or a digit.";
	}
	try {
		value->loadFromByteArray(data, position);
	} catch (BencodeException &ex2) {
		delete value;
		throw ex << "Failed to load value" << endl
				 << ex2.what();
	}
	return value;
}



BencodeInteger::BencodeInteger() : BencodeValue(Type::Integer)
{
}

BencodeInteger::BencodeInteger(qint64 value)
	: BencodeValue(Type::Integer)
    , _value(value)
{
}

BencodeInteger::~BencodeInteger()
{
}

qint64 BencodeInteger::toInt()
{
    return _value;
}

void BencodeInteger::loadFromByteArray(const QByteArray &data, int &position)
{
	BencodeException ex("BencodeInteger::loadFromByteArray(): ");

    _bencodeData = &data;
    _dataPosBegin = position;
	int &i = position;
	if(i >= data.size()) {
		throw ex << "Unexpectedly reached end of the data stream";
	}

	char firstByte = data[i++];
	if(firstByte != 'i') {
		throw ex << "First byte of Integer must be 'i', insted got '" << firstByte << "'";
	}

	QString valueString;
	for(;;) {
		if(i == data.size()) {
			throw ex << "Unexpectedly reached end of the data stream";
		}
		char byte = data[i++];
		if(byte == 'e') {
			break;
		}
		if((byte < '0' || byte > '9') && byte != '-') {
			throw ex << "Illegal character: '" << byte << "'";
		}
		valueString += byte;
	}
	bool ok;
    _value = valueString.toLongLong(&ok);
    _dataPosEnd = i;
	if(!ok) {
		throw ex << "Value not an integer: '" << valueString << "'";
	}
}



BencodeString::BencodeString() : BencodeValue(Type::String)
{
}

BencodeString::BencodeString(const QByteArray& value)
	: BencodeValue(Type::String)
    , _value(value)
{
}

BencodeString::~BencodeString()
{
}

QByteArray BencodeString::toByteArray()
{
    return _value;
}

void BencodeString::loadFromByteArray(const QByteArray &data, int &position)
{
	BencodeException ex("BencodeString::loadFromByteArray(): ");

    _bencodeData = &data;
    _dataPosBegin = position;
	int& i = position;
	if(i >= data.size()) {
		throw ex << "Unexpectedly reached end of the data stream";
	}

	char firstByte = data[i];
	if(firstByte < '0' || firstByte > '9') {
		throw ex << "First byte must be a digit, but got '" << firstByte << "'";
	}

	QString lengthString;
	for(;;) {
		if(i == data.size()) {
			throw ex << "Unexpectedly reached end of the data stream";
		}
		char byte = data[i++];
		if(byte == ':') {
			break;
		}
		if((byte < '0' || byte > '9') && byte != '-') {
			throw ex << "Illegal character: '" << byte << "'";
		}
		lengthString += byte;
	}
	bool ok;
	int length = lengthString.toInt(&ok);
	if(!ok) {
		throw ex << "Length not an integer: '" << lengthString << "'";
	}

	for(int j = 0; j < length; j++) {
		if(i == data.size()) {
			throw ex << "Unexpectedly reached end of the data stream";
		}
		char byte = data[i++];
        _value += byte;
	}
    _dataPosEnd = i;
}



BencodeList::BencodeList() : BencodeValue(Type::List)
{
}

BencodeList::~BencodeList()
{
    for(auto value : _values) {
		delete value;
	}
}

QList<BencodeValue*> BencodeList::toList()
{
    return _values;
}

void BencodeList::loadFromByteArray(const QByteArray &data, int &position)
{
	BencodeException ex("BencodeList::loadFromByteArray(): ");

    _bencodeData = &data;
    _dataPosBegin = position;
	int& i = position;
	if(i >= data.size()) {
		throw ex << "Unexpectedly reached end of the data stream";
	}

	char firstByte = data[i++];
	if(firstByte != 'l') {
		throw ex << "First byte of list must be 'l', instead got '" << firstByte << "'";
	}

	for(;;) {
		if(i >= data.size()) {
			throw ex << "Unexpectedly reached end of the data stream";
		}
		if(data[i] == 'e') {
			i++;
			break;
		}

		BencodeValue* element;
		try {
			element = BencodeValue::createFromByteArray(data, i);
		} catch(BencodeException& ex2) {
			throw ex << "Failed to create element" << endl << ex2.what();
		}

        _values.push_back(element);
	}
    _dataPosEnd = i;
}



BencodeDictionary::BencodeDictionary() : BencodeValue(Type::Dictionary)
{
}

BencodeDictionary::~BencodeDictionary()
{
    for(BencodeValue* value : _values.values()) {
		delete value;
	}
}

QList<QByteArray> BencodeDictionary::keys() const
{
    return _values.keys();
}

QList<BencodeValue*> BencodeDictionary::values() const
{
    return _values.values();
}

bool BencodeDictionary::keyExists(const QByteArray& key) const
{
    return _values.keys().contains(key);
}

BencodeValue* BencodeDictionary::value(const QByteArray& key) const
{
	if(keyExists(key)) {
        return _values.value(key);
	}
	throw BencodeException("BencodeDictionary::value(): No such key: '" + key + "'");
}

void BencodeDictionary::loadFromByteArray(const QByteArray &data, int &position)
{
	BencodeException ex("BencodeDictionary::loadFromByteArray(): ");

    _bencodeData = &data;
    _dataPosBegin = position;
	int& i = position;
	if(i == data.size()) {
		throw ex << "Unexpectedly reached end of the data stream";
	}
	char firstByte = data[i++];
	if(firstByte != 'd') {
		throw ex << "First byte of a dictionary must be 'd', instead got '" << firstByte << "'";
	}
	for(;;) {
		if(i >= data.size()) {
			throw ex << "Unexpectedly reached end of the data stream";
		}
		if(data[i] == 'e') {
			i++;
			break;
		}
		BencodeValue* key = nullptr;
		QByteArray keyString;
		BencodeValue* value;
		try {
			key = BencodeValue::createFromByteArray(data, i);
			keyString = key->toByteArray();
			delete key;
			key = nullptr;
		} catch(BencodeException& ex2) {
			if(key) {
				delete key;
			}
			throw ex << "Failed to load key" << endl << ex2.what();
		}

		try {
			value = BencodeValue::createFromByteArray(data, i);
		} catch(BencodeException& ex2) {
			throw ex << "Failed to load value" << endl << ex2.what();
		}

        _values[keyString] = value;
	}
    _dataPosEnd = i;
}


void BencodeInteger::setValue(qint64 value)
{
    _value = value;
}

void BencodeString::setValue(const QByteArray &value)
{
    _value = value;
}

void BencodeList::setValues(const QList<BencodeValue *> &values)
{
    _values = values;
}

void BencodeList::add(BencodeValue *value)
{
    _values.push_back(value);
}

void BencodeDictionary::add(const QByteArray& key, BencodeValue* value)
{
    _values[key] = value;
}


QByteArray BencodeInteger::bencode(bool includeMetadata) const
{
	QByteArray data;
	if(includeMetadata) {
        data.append('i').append(QString::number(_value)).append('e');
	} else {
        data.append(QString::number(_value));
	}
	return data;
}

QByteArray BencodeString::bencode(bool includeMetadata) const
{
	QByteArray data;
	if(includeMetadata) {
        data.append(QString::number(_value.size())).append(':').append(_value);
	} else {
        data.append(_value);
	}
	return data;
}

QByteArray BencodeList::bencode(bool includeMetadata) const
{
	QByteArray data;
	if(includeMetadata) {
		data.append('l');
	}
    for(BencodeValue* value : _values) {
		data.append(value->bencode());
	}
	if(includeMetadata) {
		data.append('e');
	}
	return data;
}

QByteArray BencodeDictionary::bencode(bool includeMetadata) const
{
	QByteArray data;
	if(includeMetadata) {
		data.append('d');
	}
    for(const QByteArray& key : _values.keys()) {
		BencodeString bkey(key);
        data.append(bkey.bencode()).append(_values.value(key)->bencode());
	}
	if(includeMetadata) {
		data.append('e');
	}
	return data;
}


void BencodeInteger::print(QTextStream& out) const
{
    out << _value;
}

void BencodeString::print(QTextStream& out) const
{
    out << _value;
}

void BencodeList::print(QTextStream& out) const
{
	out << "List {" << endl;
    for(auto v : _values) {
		QString s;
		QTextStream stream(&s);
		v -> print(stream);
		while(!stream.atEnd()) {
			QString line = stream.readLine();
			out << '\t' << line << endl;
		}
	}
	out << "}";
}

void BencodeDictionary::print(QTextStream& out) const
{
	out << "Dictionary {" << endl;
    for(const QByteArray& key : _values.keys()) {
		out << key;
		out << " : ";
		QString s;
		QTextStream stream(&s);
        _values[key] -> print(stream);
		out << stream.readLine() << endl;
		while(!stream.atEnd()) {
			out << '\t' << stream.readLine() << endl;
		}
	}
	out << "}";
}


bool BencodeInteger::equalTo(BencodeValue *other) const
{
	try {
        return other->toInt() == _value;
	} catch(BencodeException& ex) {
		return false;
	}
}

bool BencodeString::equalTo(BencodeValue *other) const
{
	try {
        return other->toByteArray() == _value;
	} catch(BencodeException& ex) {
		return false;
	}
}

bool BencodeList::equalTo(BencodeValue *other) const
{
	try {
		auto list = other->toList();
        if(list.size() != _values.size()) {
			return false;
		}
		for(int i = 0; i < list.size(); i++) {
            if(!list[i]->equalTo(_values[i])) {
				return false;
			}
		}
		return true;
	} catch(BencodeException& ex) {
		return false;
	}
}

bool BencodeDictionary::equalTo(BencodeValue *other) const
{
	try {
		BencodeDictionary* otherDict = other->toBencodeDictionary();
		if(keys() != otherDict->keys()) {
			return false;
		}
		if(values() != otherDict->values()) {
			return false;
		}
		return true;
	} catch(BencodeException& ex) {
		return false;
	}
}
