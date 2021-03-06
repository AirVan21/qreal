#pragma once

#include <QtCore/QObject>

#include <thirdparty/qscintilla/Qt4Qt5/Qsci/qscilexerpython.h>
#include <thirdparty/qscintilla/Qt4Qt5/Qsci/qscilexercpp.h>

#include "qrgui/textEditor/textEditorDeclSpec.h"

namespace qReal {
namespace text {

/// Represents the information about the language code on which is will be
/// opened by QReal text editor.
struct LanguageInfo
{
	~LanguageInfo()
	{
	}

	/// The extension of the files containing code on this language.
	QString extension;

	/// The human-readable description of the extension. This will be shown
	/// in the file dialogs filter boxes when working with files with code on this language.
	QString extensionDescription;

	/// True if new lines should be indented with tabs.
	bool tabIndentation;

	/// A size of the tab indent used in editor (in spaces).
	int tabSize;

	/// A pointer to text edit colorer object; nullptr can be passed to disable coloring.
	QSharedPointer<QsciLexer> lexer;

	/// A list of tokens that will be proposed to user in autocompletion window.
	QStringList additionalAutocompletionTokens;

	/// Help method that returns an indent string using the specified indentation info.
	QString indent() const
	{
		return tabIndentation ? "\t" : QString(tabSize, ' ');
	}
};

/// Enumerates the frequently used languages for convenience.
class QRGUI_TEXT_EDITOR_EXPORT Languages
{
public:
	/// Returns a list of all known to system editor settings per language.
	static QList<LanguageInfo> knownLanguages();

	/// Appends the given information about programming language into a list of known.
	/// When in future the system will open text files with the extension specified in
	/// @arg language it will automaticly apply editor settings specified there.
	static void registerLanguage(LanguageInfo const &language);

	/// Tries to find appropriate language by the given file extension;
	/// Returns default textFileInfo(@arg extension) if no such lanugage was found.
	/// The search is performed first in user-defined lannguages, then in other known ones.
	static LanguageInfo pickByExtension(QString const &extension);

	/// Returns editor settings when it edits information on an unknown language.
	static LanguageInfo textFileInfo(QString const &extension, QStringList const &additionalTokens = QStringList())
	{
		return LanguageInfo{extension                                        /* extension */
				, QObject::tr("Text File")                                   /* extension description */
				, true                                                       /* tabs indentation */
				, 8                                                          /* tab size */
				, QSharedPointer<QsciLexer>(nullptr)                         /* lexer */
				, additionalTokens                                           /* additional autocompletion tokens */
		};
	}

	/// Returns the information about the c language.
	static LanguageInfo c(QStringList const &additionalTokens = QStringList())
	{
		return LanguageInfo{"c"                                              /* extension */
				, QObject::tr("C Language Source File")                      /* extension description */
				, true                                                       /* tabs indentation */
				, 8                                                          /* tab size */
				, QSharedPointer<QsciLexer>(new QsciLexerCPP())              /* lexer */
				, additionalTokens                                           /* additional autocompletion tokens */
		};
	}

	/// The information about the russian algorithmic language.
	static LanguageInfo russianC(QStringList const &additionalTokens = QStringList())
	{
		return LanguageInfo{QString::fromUtf8("си")                          /* extension */
				, QObject::tr("Russian Algorithmic Language Source File")    /* extension description */
				, true                                                       /* tabs indentation */
				, 8                                                          /* tab size */
				/// @todo: write own lexer
				, QSharedPointer<QsciLexer>(new QsciLexerCPP())              /* lexer */
				, additionalTokens                                           /* additional autocompletion tokens */
		};
	}

	/// The information about the python language.
	static LanguageInfo python(QStringList const &additionalTokens = QStringList())
	{
		return LanguageInfo{"py"                                             /* extension */
				, QObject::tr("Python Source File")                          /* extension description */
				, false                                                      /* tabs indentation */
				, 2                                                          /* tab size */
				, QSharedPointer<QsciLexer>(new QsciLexerPython())           /* lexer */
				, additionalTokens                                           /* additional autocompletion tokens */
		};
	}

	/// The information about the qt-script language.
	static LanguageInfo qtScript(QStringList const &additionalTokens = QStringList())
	{
		return LanguageInfo{"qts"                                            /* extension */
				, QObject::tr("QtScript Language Source File")               /* extension description */
				, true                                                       /* tabs indentation */
				, 8                                                          /* tab size */
				, QSharedPointer<QsciLexer>(new QsciLexerCPP())              /* lexer */
				, additionalTokens                                           /* additional autocompletion tokens */
		};
	}

	/// The information about the F# language.
	static LanguageInfo fSharp(QStringList const &additionalTokens = QStringList())
	{
		return LanguageInfo{"fs"                                             /* extension */
				, QObject::tr("F# Language Source File")                     /* extension description */
				, false                                                      /* tabs indentation */
				, 4                                                          /* tab size */
				/// @todo: write own lexer
				, QSharedPointer<QsciLexer>(new QsciLexerCPP())              /* lexer */
				, additionalTokens                                           /* additional autocompletion tokens */
		};
	}

private:
	static QList<LanguageInfo> mUserDefinedLanguages;
};

}
}
