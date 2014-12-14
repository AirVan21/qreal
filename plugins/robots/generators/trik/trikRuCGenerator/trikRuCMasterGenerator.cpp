#include "trikRuCMasterGenerator.h"

using namespace trik::ruc;

TrikRuCMasterGenerator::TrikRuCMasterGenerator(qrRepo::RepoApi const &repo
        , qReal::ErrorReporterInterface &errorReporter
        , interpreterBase::robotModel::RobotModelManagerInterface const &robotModelManager
        , qrtext::LanguageToolboxInterface &textLanguage
        , qReal::Id const &diagramId
        , QString const &generatorName)
    : TrikMasterGeneratorBase(repo, errorReporter, robotModelManager, textLanguage, diagramId, generatorName)
{
}

QString TrikRuCMasterGenerator::targetPath()
{
    return QString("%1/%2.c").arg(mProjectDir, mProjectName);
}

bool TrikRuCMasterGenerator::supportsGotoGeneration() const
{
    return false;
}
