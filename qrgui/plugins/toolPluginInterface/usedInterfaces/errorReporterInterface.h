#pragma once

#include <QtCore/QString>
#include <QtCore/QList>

#include <qrkernel/ids.h>
#include <qrkernel/definitions.h>

namespace qReal {

class ErrorReporterInterface
{
public:
	virtual void addInformation(QString const &message, Id const &position = Id::rootId()) = 0;
	virtual void addWarning(QString const &message, Id const &position = Id::rootId()) = 0;
	virtual void addError(QString const &message, Id const &position = Id::rootId()) = 0;
	virtual void addCritical(QString const &message, Id const &position = Id::rootId()) = 0;

	virtual void clear() = 0;
	virtual void clearErrors() = 0;

	/// Are there errors reported by now or not. Counted after latest clearErrors() call
	/// @returns True, when there are messages with severity "Critical" and "Error" registered
	virtual bool wereErrors() = 0;
};

}
