#include "bencodeparser.h"
#include "bencodevalue.h"
#include <QFile>

void BencodeParser::setError(const QString &errorString)
{
    _errorString = errorString;
}

void BencodeParser::clearError()
{
    _errorString.clear();
}


BencodeParser::BencodeParser()
{

}

BencodeParser::~BencodeParser()
{
    for (BencodeValue *value : _mainList) {
		delete value;
	}
}


QString BencodeParser::errorString() const
{
    return _errorString;
}


void BencodeParser::setData(const QByteArray &data)
{
    _bencodeData = data;
}

bool BencodeParser::readFile(const QString &fileName)
{
	clearError();

	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly)) {
		setError(file.errorString());
		return false;
	}
    _bencodeData = file.readAll();
	file.close();
	return true;
}

bool BencodeParser::parse(const QByteArray &data)
{
	setData(data);
	return parse();
}

bool BencodeParser::parse()
{
	clearError();

    for (BencodeValue *value : _mainList) {
		delete value;
	}
    _mainList.clear();

	int i = 0;
    while (i < _bencodeData.size()) {
		BencodeValue *value;
		try {
            value = BencodeValue::createFromByteArray(_bencodeData, i);
		} catch (BencodeException &ex) {
			setError(ex.what());
			return false;
		}

        _mainList.push_back(value);
	}

	return true;
}


const QByteArray &BencodeParser::rawBencodeData() const
{
    return _bencodeData;
}


QList<BencodeValue *> BencodeParser::list() const
{
    return _mainList;
}
