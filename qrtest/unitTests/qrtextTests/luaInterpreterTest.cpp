#include "luaInterpreterTest.h"

#include "gtest/gtest.h"

#include "qrtext/lua/types/float.h"
#include "qrtext/lua/types/integer.h"

using namespace qrTest;
using namespace qrtext;
using namespace qrtext::lua;
using namespace qrtext::lua::details;

void LuaInterpreterTest::SetUp()
{
	mAnalyzer.reset(new LuaSemanticAnalyzer(mErrors));
	mParser.reset(new LuaParser(mErrors));
	mLexer.reset(new LuaLexer(mErrors));
	mInterpreter.reset(new LuaInterpreter(mErrors));
}

QSharedPointer<qrtext::core::ast::Node> LuaInterpreterTest::parseAndAnalyze(QString const &code)
{
	return mAnalyzer->analyze(mParser->parse(mLexer->tokenize(code), mLexer->userFriendlyTokenNames()));
}

TEST_F(LuaInterpreterTest, sanityCheck)
{
	int result = interpret<int>("1");
	ASSERT_EQ(1, result);
}

TEST_F(LuaInterpreterTest, assignment)
{
	interpret<int>("a = 1");
	int result = interpret<int>("a");
	ASSERT_EQ(1, result);
}

TEST_F(LuaInterpreterTest, intrinsicFunction)
{
	mAnalyzer->addIntrinsicFunction("f", QSharedPointer<types::Function>(new types::Function(
			QSharedPointer<core::types::TypeExpression>(new types::Float()),
			{QSharedPointer<core::types::TypeExpression>(new types::Integer())}
			)));

	mInterpreter->addIntrinsicFunction("f", [](QList<QVariant> params) {
			return params[0].toInt() * params[0].toInt();
			});

	int result = interpret<int>("f(5)");

	ASSERT_TRUE(mErrors.isEmpty());
	ASSERT_EQ(25, result);
}

TEST_F(LuaInterpreterTest, tables)
{
	int result = interpret<int>("a = {239}; a[0]");

	ASSERT_TRUE(mErrors.isEmpty());
	ASSERT_EQ(239, result);
}

TEST_F(LuaInterpreterTest, implicitTable)
{
	int result = interpret<int>("a[1] = 239; a[2] = 30; a[0]");

	ASSERT_TRUE(mErrors.isEmpty());
	ASSERT_EQ(0, result);

	result = interpret<int>("a[1]");
	ASSERT_TRUE(mErrors.isEmpty());
	ASSERT_EQ(239, result);

	result = interpret<int>("a[2]");
	ASSERT_TRUE(mErrors.isEmpty());
	ASSERT_EQ(30, result);
}

TEST_F(LuaInterpreterTest, tableExpressions)
{
	int result = interpret<int>("a[1 - 1] = 239; a[3 * 0]");

	ASSERT_TRUE(mErrors.isEmpty());
	ASSERT_EQ(239, result);
}

TEST_F(LuaInterpreterTest, moreTables)
{
	interpret<int>("a = {10; 15}");

	int result = interpret<int>("a[0]");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ(10, result);

	result = interpret<int>("a[1]");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ(15, result);
}

TEST_F(LuaInterpreterTest, tableExplicitIndexerSyntax)
{
	interpret<int>("a = {[1] = 10; [3] = 15}");

	int result = interpret<int>("a[0]");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ(0, result);

	result = interpret<int>("a[1]");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ(10, result);

	result = interpret<int>("a[2]");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ(0, result);

	result = interpret<int>("a[3]");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ(15, result);
}

TEST_F(LuaInterpreterTest, tableAsReturnValue)
{
	auto const result = interpret<QStringList>("a = 1; b = 2; a, b");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ("1", result[0]);
	EXPECT_EQ("2", result[1]);
}

TEST_F(LuaInterpreterTest, inequality)
{
	bool result = interpret<bool>("1 != 2");

	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(result);

	result = interpret<bool>("1 != 1");

	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FALSE(result);

	result = interpret<bool>("1 ~= 2");

	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(result);

	result = interpret<bool>("1 ~= 1");

	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FALSE(result);
}

TEST_F(LuaInterpreterTest, concatenation)
{
	auto result = interpret<QString>("'ab' .. 'cd'");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ("abcd", result);
}

TEST_F(LuaInterpreterTest, operatorsTest)
{
	auto doubleResult = interpret<double>("5.2 + 2.4");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FLOAT_EQ(7.6, doubleResult);

	doubleResult = interpret<double>("5.2 - 2.4");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FLOAT_EQ(2.8, doubleResult);

	doubleResult = interpret<double>("6 * 6");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FLOAT_EQ(36, doubleResult);

	doubleResult = interpret<double>("5 / 2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FLOAT_EQ(2.5, doubleResult);

	doubleResult = interpret<double>("5 / 0");
	ASSERT_TRUE(!mErrors.isEmpty());
	mErrors.clear();

	auto intResult = interpret<int>("5 // 2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ(2, intResult);

	intResult = interpret<int>("5 // 0");
	ASSERT_TRUE(!mErrors.isEmpty());
	mErrors.clear();

	doubleResult = interpret<double>("2 ^ 3");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FLOAT_EQ(8, doubleResult);

	doubleResult = interpret<double>("2.5 ^ 3.2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FLOAT_EQ(18.767569281, doubleResult);

	doubleResult = interpret<double>("0 ^ -1");
	ASSERT_TRUE(mErrors.isEmpty());
	mErrors.clear();

	intResult = interpret<int>("5 % 2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ(1, intResult);

	intResult = interpret<int>("5 % 0");
	ASSERT_TRUE(!mErrors.isEmpty());
	mErrors.clear();

	intResult = interpret<int>("2 & 4");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ(0, intResult);

	intResult = interpret<int>("2 & 3");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ(2, intResult);

	intResult = interpret<int>("~~2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ(2, intResult);

	intResult = interpret<int>("~2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ(-3, intResult);

	intResult = interpret<int>("6 | 3");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ(7, intResult);

	intResult = interpret<int>("1 << 3");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ(8, intResult);

	intResult = interpret<int>("6 >> 1");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ(3, intResult);

	auto stringResult = interpret<QString>("'ab' .. 'cd'");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ("abcd", stringResult);

	auto boolResult = interpret<bool>("1 < 2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	boolResult = interpret<bool>("1 > 2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FALSE(boolResult);

	boolResult = interpret<bool>("1 <= 2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	boolResult = interpret<bool>("1 >= 2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FALSE(boolResult);

	boolResult = interpret<bool>("1 == 2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FALSE(boolResult);

	boolResult = interpret<bool>("2 == 2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	boolResult = interpret<bool>("'asd' == 'asd'");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	boolResult = interpret<bool>("'asd' == 'fgh'");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FALSE(boolResult);

	boolResult = interpret<bool>("{1, 2} == {1, 2}");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	boolResult = interpret<bool>("{1, 2} == {1, 3}");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FALSE(boolResult);

	boolResult = interpret<bool>("1 != 2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	boolResult = interpret<bool>("2 != 2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FALSE(boolResult);

	boolResult = interpret<bool>("'asd' != 'asd'");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FALSE(boolResult);

	boolResult = interpret<bool>("'asd' != 'fgh'");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	boolResult = interpret<bool>("{1, 2} != {1, 2}");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FALSE(boolResult);

	boolResult = interpret<bool>("{1, 2} != {1, 3}");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	boolResult = interpret<bool>("1 ~= 2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	boolResult = interpret<bool>("2 ~= 2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FALSE(boolResult);

	boolResult = interpret<bool>("'asd' ~= 'asd'");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FALSE(boolResult);

	boolResult = interpret<bool>("'asd' ~= 'fgh'");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	boolResult = interpret<bool>("{1, 2} ~= {1, 2}");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FALSE(boolResult);

	boolResult = interpret<bool>("{1, 2} ~= {1, 3}");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	boolResult = interpret<bool>("true and false");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FALSE(boolResult);

	boolResult = interpret<bool>("1 and 2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	boolResult = interpret<bool>("true && false");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FALSE(boolResult);

	boolResult = interpret<bool>("1 && 2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	boolResult = interpret<bool>("true or false");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	boolResult = interpret<bool>("1 or 2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	boolResult = interpret<bool>("true || false");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	boolResult = interpret<bool>("1 || 2");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	intResult = interpret<int>("-(1)");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ(-1, intResult);

	doubleResult = interpret<double>("-(1.5)");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FLOAT_EQ(-1.5, doubleResult);

	intResult = interpret<int>("-1");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ(-1, intResult);

	doubleResult = interpret<double>("-1.5");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FLOAT_EQ(-1.5, doubleResult);

	boolResult = interpret<bool>("not false");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	boolResult = interpret<bool>("not 'abcd'");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FALSE(boolResult);


	boolResult = interpret<bool>("not ''");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	boolResult = interpret<bool>("not 1");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_FALSE(boolResult);

	boolResult = interpret<bool>("not 0");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_TRUE(boolResult);

	intResult = interpret<int>("#'asdf'");
	ASSERT_TRUE(mErrors.isEmpty());
	EXPECT_EQ(4, intResult);
}
