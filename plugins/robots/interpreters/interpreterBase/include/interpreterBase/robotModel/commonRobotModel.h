#pragma once

#include "interpreterBase/robotModel/robotModelInterface.h"
#include "interpreterBase/interpreterBaseDeclSpec.h"

namespace interpreterBase {
namespace robotModel {

class ROBOTS_INTERPRETER_BASE_EXPORT CommonRobotModel : public RobotModelInterface
{
	Q_OBJECT

public:
	CommonRobotModel();
	~CommonRobotModel() override;

	QString name() const override;

	void init() override;
	void stopRobot() override;
	void disconnectFromRobot() override;

	bool needsConnection() const override;

	ConfigurationInterface &mutableConfiguration() override;  // TODO: Don't like this, breaks incapsulation.
	ConfigurationInterface const &configuration() const override;  // TODO: Don't like this, breaks incapsulation.

	robotParts::Brick &brick() override;
	robotParts::Display &display() override;

	virtual QList<PortInfo> availablePorts() const override;
	virtual QList<PluggableDeviceInfo> supportedSensors() const override;
};

}
}
