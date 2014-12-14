#include "trikRuCGeneratorPlugin.h"

#include <QtWidgets/QApplication>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>

#include <trikGeneratorBase/trikGeneratorPluginBase.h>
#include <utils/tcpRobotCommunicator.h>

#include "trikRuCMasterGenerator.h"

using namespace trik::ruc;
using namespace qReal;

TrikRuCGeneratorPlugin::TrikRuCGeneratorPlugin()
    : mGenerateCodeAction(new QAction(nullptr))
    , mUploadProgramAction(new QAction(nullptr))
    , mRunProgramAction(new QAction(nullptr))
    , mStopRobotAction(new QAction(nullptr))
    , mCommunicator(nullptr)
{
}

TrikRuCGeneratorPlugin::~TrikRuCGeneratorPlugin()
{
    delete mCommunicator;
}

void TrikRuCGeneratorPlugin::init(qReal::PluginConfigurator const &configurator
        , interpreterBase::robotModel::RobotModelManagerInterface const &robotModelManager
        , qrtext::LanguageToolboxInterface &textLanguage)
{
    RobotsGeneratorPluginBase::init(configurator, robotModelManager, textLanguage);
    mCommunicator = new utils::TcpRobotCommunicator("TrikTcpServer");
    mCommunicator->setErrorReporter(configurator.mainWindowInterpretersInterface().errorReporter());
}

QList<ActionInfo> TrikRuCGeneratorPlugin::actions()
{
    QAction *separator = new QAction(this);
    separator->setSeparator(true);
    qReal::ActionInfo separatorInfo(separator, "generators", "tools");

    mGenerateCodeAction->setText(tr("Generate RuC TRIK code"));
    mGenerateCodeAction->setIcon(QIcon(":/images/generateQtsCode.svg"));
    ActionInfo generateCodeActionInfo(mGenerateCodeAction, "generators", "tools");
    connect(mGenerateCodeAction, SIGNAL(triggered()), this, SLOT(generateCode()), Qt::UniqueConnection);

    mUploadProgramAction->setText(tr("Upload RuC program"));
    mUploadProgramAction->setIcon(QIcon(":/images/uploadProgram.svg"));
    ActionInfo uploadProgramActionInfo(mUploadProgramAction, "generators", "tools");
    connect(mUploadProgramAction, SIGNAL(triggered()), this, SLOT(uploadProgram()), Qt::UniqueConnection);

    mRunProgramAction->setText(tr("Run RuC program"));
    mRunProgramAction->setIcon(QIcon(":/images/uploadAndExecuteProgram.svg"));
    ActionInfo runProgramActionInfo(mRunProgramAction, "generators", "tools");
    connect(mRunProgramAction, SIGNAL(triggered()), this, SLOT(runProgram()), Qt::UniqueConnection);

    /* Exclude */
    mStopRobotAction->setText(tr("Stop robot"));
    mStopRobotAction->setIcon(QIcon(":/images/stopRobot.svg"));
    ActionInfo stopRobotActionInfo(mStopRobotAction, "generators", "tools");
    connect(mStopRobotAction, SIGNAL(triggered()), this, SLOT(stopRobot()), Qt::UniqueConnection);


    return {generateCodeActionInfo, uploadProgramActionInfo, runProgramActionInfo, stopRobotActionInfo, separatorInfo};
}

QList<HotKeyActionInfo> TrikRuCGeneratorPlugin::hotKeyActions()
{
    mGenerateCodeAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
    mUploadProgramAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
    mRunProgramAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F5));
    mStopRobotAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_F5));

    HotKeyActionInfo generateCodeInfo("Generator.GenerateTrik", tr("Generate TRIK Code"), mGenerateCodeAction);
    HotKeyActionInfo uploadProgramInfo("Generator.UploadTrik", tr("Upload TRIK Program"), mUploadProgramAction);
    HotKeyActionInfo runProgramInfo("Generator.RunTrik", tr("Run TRIK Program"), mRunProgramAction);
    HotKeyActionInfo stopRobotInfo("Generator.StopTrik", tr("Stop TRIK Robot"), mStopRobotAction);

    return { generateCodeInfo, uploadProgramInfo, runProgramInfo, stopRobotInfo };
}

generatorBase::MasterGeneratorBase *TrikRuCGeneratorPlugin::masterGenerator()
{
    return new TrikRuCMasterGenerator(*mRepo
            , *mMainWindowInterface->errorReporter()
            , *mRobotModelManager
            , *mTextLanguage
            , mMainWindowInterface->activeDiagram()
            , generatorName());
}

QString TrikRuCGeneratorPlugin::defaultFilePath(QString const &projectName) const
{
    return QString("trik/%1/%1.c").arg(projectName);
}

text::LanguageInfo TrikRuCGeneratorPlugin::language() const
{
    return qReal::text::Languages::c();
}

QString TrikRuCGeneratorPlugin::generatorName() const
{
   return "trikRuC";
}

bool TrikRuCGeneratorPlugin::uploadProgram()
{
    QFileInfo const fileInfo = generateCodeForProcessing();

    if (fileInfo != QFileInfo() && !fileInfo.absoluteFilePath().isEmpty()) {
        bool const result = mCommunicator->uploadProgram(fileInfo.absoluteFilePath());
        if (!result) {
            mMainWindowInterface->errorReporter()->addError(tr("No connection to robot"));
        }

        return result;
    } else {
        qDebug() << "Code generation failed, aborting";
        return false;
    }
}

void TrikRuCGeneratorPlugin::runProgram()
{
    if (uploadProgram()) {
        QFileInfo const fileInfo = generateCodeForProcessing();
        mCommunicator->runProgram(fileInfo.fileName());
    } else {
        qDebug() << "Program upload failed, aborting";
    }
}

void TrikRuCGeneratorPlugin::stopRobot()
{
    if (!mCommunicator->stopRobot()) {
        mMainWindowInterface->errorReporter()->addError(tr("No connection to robot"));
    }

    mCommunicator->runDirectCommand(
            "brick.system(\"killall aplay\"); \n"
            "brick.system(\"killall vlc\"); \n"
            "brick.system(\"killall rover-cv\");"
            , true
            );
}
