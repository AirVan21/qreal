#include "nxtEnginesBackwardBlock.h"

#include "src/robotModel/parts/nxtMotor.h"

using namespace nxtKitInterpreter::blocks::details;
using namespace nxtKitInterpreter::robotModel::parts;

NxtEnginesBackwardBlock::NxtEnginesBackwardBlock(interpreterBase::robotModel::RobotModelInterface &robotModel)
	: EngineCommandBlock(robotModel)
{
}

void NxtEnginesBackwardBlock::run()
{
	int const power = -evaluate("Power").toInt();

	/// @todo Makes sense only for NXT.
	bool const breakMode = stringProperty("Mode") != QString::fromUtf8("скользить");

	for (NxtMotor * const motor : parsePorts<NxtMotor>()) {
		motor->on(power, breakMode);
	}

	emit done(mNextBlockId);
}
