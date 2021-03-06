#include "sayGenerator.h"

#include <generatorBase/generatorCustomizer.h>

using namespace trik::simple;
using namespace generatorBase::simple;

SayGenerator::SayGenerator(qrRepo::RepoApi const &repo
		, generatorBase::GeneratorCustomizer &customizer
		, qReal::Id const &id
		, QObject *parent)
	: BindingGenerator(repo, customizer, id
			, "say.t"
			, { Binding::createConverting("@@TEXT@@", "Text"
					, customizer.factory()->stringPropertyConverter(id, "Text"))
			} , parent)
{
}
