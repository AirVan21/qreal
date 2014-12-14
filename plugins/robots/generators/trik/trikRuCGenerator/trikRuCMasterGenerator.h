#pragma once

#include <trikGeneratorBase/trikMasterGeneratorBase.h>

namespace trik {
namespace ruc {

/// Master generator implementation for generator into QtScript for TRIK platform
class TrikRuCMasterGenerator : public TrikMasterGeneratorBase
{
public:
    TrikRuCMasterGenerator(qrRepo::RepoApi const &repo
            , qReal::ErrorReporterInterface &errorReporter
            , interpreterBase::robotModel::RobotModelManagerInterface const &robotModelManager
            , qrtext::LanguageToolboxInterface &textLanguage
            , qReal::Id const &diagramId
            , QString const &generatorName);

protected:
    QString targetPath() override;
    bool supportsGotoGeneration() const override;
};

}
}
