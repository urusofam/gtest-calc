#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <limits>
#include <stdexcept>

#include "IHistory.h"
#include "SimpleCalculator.h"
#include "InMemoryHistory.h"

using namespace calc;

// Мока истории
class MockHistory : public IHistory {
public:
    MOCK_METHOD(void, AddEntry, (const std::string&), (override));
    MOCK_METHOD(std::vector<std::string>, GetLastOperations, (size_t), (const, override));
};

// Тесты взаимодействия калькулятора с историей
class CalculatorHistoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockHistory = std::make_unique<MockHistory>();
        calculator = std::make_unique<SimpleCalculator>(*mockHistory);
    }

    std::unique_ptr<MockHistory> mockHistory;
    std::unique_ptr<SimpleCalculator> calculator;
};

TEST_F(CalculatorHistoryTest, AddCallsHistory) {
    EXPECT_CALL(*mockHistory, AddEntry("2 + 3 = 5"));
    int result = calculator->Add(2, 3);
    EXPECT_EQ(result, 5);
}

TEST_F(CalculatorHistoryTest, SubtractCallsHistory) {
    EXPECT_CALL(*mockHistory, AddEntry("10 - 4 = 6"));
    int result = calculator->Subtract(10, 4);
    EXPECT_EQ(result, 6);
}

TEST_F(CalculatorHistoryTest, MultiplyCallsHistory) {
    EXPECT_CALL(*mockHistory, AddEntry("7 * 8 = 56"));
    int result = calculator->Multiply(7, 8);
    EXPECT_EQ(result, 56);
}

TEST_F(CalculatorHistoryTest, DivideCallsHistory) {
    EXPECT_CALL(*mockHistory, AddEntry("15 / 3 = 5"));
    int result = calculator->Divide(15, 3);
    EXPECT_EQ(result, 5);
}

TEST_F(CalculatorHistoryTest, NegativeNumbersFormattedCorrectly) {
    EXPECT_CALL(*mockHistory, AddEntry("-5 + 3 = -2"));
    EXPECT_CALL(*mockHistory, AddEntry("10 - -3 = 13"));

    calculator->Add(-5, 3);
    calculator->Subtract(10, -3);
}

TEST_F(CalculatorHistoryTest, MultipleOperationsCalledInOrder) {
    testing::InSequence seq;
    EXPECT_CALL(*mockHistory, AddEntry("1 + 2 = 3"));
    EXPECT_CALL(*mockHistory, AddEntry("3 * 4 = 12"));
    EXPECT_CALL(*mockHistory, AddEntry("12 / 2 = 6"));

    calculator->Add(1, 2);
    calculator->Multiply(3, 4);
    calculator->Divide(12, 2);
}

// Тест смены истории (работает неправильно сейчас)
TEST_F(CalculatorHistoryTest, SetHistoryChangesHistory) {
    // SetHistory сейчас не работает, потому что в калькуляторе мы храним
    // ссылку на историю, а ссылку нельзя переназначить после инициализации
    auto mewMockHistory = std::make_unique<MockHistory>();

    // Первая операция идет в первую историю
    EXPECT_CALL(*mockHistory, AddEntry("1 + 1 = 2"));
    calculator->Add(1, 1);

    // Вызываем SetHistory
    calculator->SetHistory(*mewMockHistory);

    // Вторая операция все еще идет в первую историю
    EXPECT_CALL(*mockHistory, AddEntry("2 + 2 = 4"));
    calculator->Add(2, 2);
}

// Тесты базовых операций
class BasicOperationsTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockHistory = std::make_unique<MockHistory>();
        calculator = std::make_unique<SimpleCalculator>(*mockHistory);
    }

    std::unique_ptr<MockHistory> mockHistory;
    std::unique_ptr<SimpleCalculator> calculator;
};

TEST_F(BasicOperationsTest, AdditionBasic) {
    EXPECT_CALL(*mockHistory, AddEntry("2 + 3 = 5"));
    EXPECT_EQ(calculator->Add(2, 3), 5);

    EXPECT_CALL(*mockHistory, AddEntry("0 + 0 = 0"));
    EXPECT_EQ(calculator->Add(0, 0), 0);

    EXPECT_CALL(*mockHistory, AddEntry("-5 + 3 = -2"));
    EXPECT_EQ(calculator->Add(-5, 3), -2);

    EXPECT_CALL(*mockHistory, AddEntry("-5 + -3 = -8"));
    EXPECT_EQ(calculator->Add(-5, -3), -8);
}

TEST_F(BasicOperationsTest, SubtractionBasic) {
    EXPECT_CALL(*mockHistory, AddEntry("10 - 4 = 6"));
    EXPECT_EQ(calculator->Subtract(10, 4), 6);

    EXPECT_CALL(*mockHistory, AddEntry("0 - 0 = 0"));
    EXPECT_EQ(calculator->Subtract(0, 0), 0);

    EXPECT_CALL(*mockHistory, AddEntry("-5 - 3 = -8"));
    EXPECT_EQ(calculator->Subtract(-5, 3), -8);

    EXPECT_CALL(*mockHistory, AddEntry("-5 - -3 = -2"));
    EXPECT_EQ(calculator->Subtract(-5, -3), -2);
}

TEST_F(BasicOperationsTest, MultiplicationBasic) {
    EXPECT_CALL(*mockHistory, AddEntry("3 * 4 = 12"));
    EXPECT_EQ(calculator->Multiply(3, 4), 12);

    EXPECT_CALL(*mockHistory, AddEntry("0 * 5 = 0"));
    EXPECT_EQ(calculator->Multiply(0, 5), 0);

    EXPECT_CALL(*mockHistory, AddEntry("-3 * 4 = -12"));
    EXPECT_EQ(calculator->Multiply(-3, 4), -12);

    EXPECT_CALL(*mockHistory, AddEntry("-3 * -4 = 12"));
    EXPECT_EQ(calculator->Multiply(-3, -4), 12);
}

TEST_F(BasicOperationsTest, DivisionBasic) {
    EXPECT_CALL(*mockHistory, AddEntry("12 / 3 = 4"));
    EXPECT_EQ(calculator->Divide(12, 3), 4);

    EXPECT_CALL(*mockHistory, AddEntry("0 / 5 = 0"));
    EXPECT_EQ(calculator->Divide(0, 5), 0);

    EXPECT_CALL(*mockHistory, AddEntry("-12 / 3 = -4"));
    EXPECT_EQ(calculator->Divide(-12, 3), -4);

    EXPECT_CALL(*mockHistory, AddEntry("-12 / -3 = 4"));
    EXPECT_EQ(calculator->Divide(-12, -3), 4);
}

// Тесты дробного деления
class FractionalDivisionTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockHistory = std::make_unique<MockHistory>();
        calculator = std::make_unique<SimpleCalculator>(*mockHistory);
    }

    std::unique_ptr<MockHistory> mockHistory;
    std::unique_ptr<SimpleCalculator> calculator;
};

TEST_F(FractionalDivisionTest, IntegerFractionalDivision) {
    // Целочисленное деление отбрасывает дробную часть
    EXPECT_CALL(*mockHistory, AddEntry("7 / 2 = 3"));
    EXPECT_EQ(calculator->Divide(7, 2), 3);  // 7/2 = 3.5 -> 3

    EXPECT_CALL(*mockHistory, AddEntry("8 / 3 = 2"));
    EXPECT_EQ(calculator->Divide(8, 3), 2);  // 8/3 = 2.666... -> 2

    EXPECT_CALL(*mockHistory, AddEntry("1 / 2 = 0"));
    EXPECT_EQ(calculator->Divide(1, 2), 0); // 1/2 = 0.5 -> 0
}

TEST_F(FractionalDivisionTest, NegativeIntegerFractionalDivision) {
    EXPECT_CALL(*mockHistory, AddEntry("-7 / 2 = -3"));
    EXPECT_EQ(calculator->Divide(-7, 2), -3);  // -7/2 = -3.5 -> -3

    EXPECT_CALL(*mockHistory, AddEntry("7 / -2 = -3"));
    EXPECT_EQ(calculator->Divide(7, -2), -3);  // 7/-2 = -3.5 -> -3

    EXPECT_CALL(*mockHistory, AddEntry("-7 / -2 = 3"));
    EXPECT_EQ(calculator->Divide(-7, -2), 3);  // -7/-2 = 3.5 -> 3
}

// Тесты истории
class InMemoryHistoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        history = std::make_unique<InMemoryHistory>();
    }

    std::unique_ptr<InMemoryHistory> history;
};

TEST_F(InMemoryHistoryTest, AddAndRetrieveOperations) {
    history->AddEntry("1 + 1 = 2");
    history->AddEntry("2 * 3 = 6");

    auto ops = history->GetLastOperations(2);
    EXPECT_EQ(ops.size(), 2);
    EXPECT_EQ(ops[0], "1 + 1 = 2");
    EXPECT_EQ(ops[1], "2 * 3 = 6");
}

TEST_F(InMemoryHistoryTest, GetLastOperationsLimitedCount) {
    history->AddEntry("op1");
    history->AddEntry("op2");
    history->AddEntry("op3");

    auto ops = history->GetLastOperations(2);
    EXPECT_EQ(ops.size(), 2);
    EXPECT_EQ(ops[0], "op2");
    EXPECT_EQ(ops[1], "op3");
}

TEST_F(InMemoryHistoryTest, GetLastOperationsMoreThanAvailable) {
    history->AddEntry("op1");
    history->AddEntry("op2");

    auto ops = history->GetLastOperations(5);
    EXPECT_EQ(ops.size(), 2);
    EXPECT_EQ(ops[0], "op1");
    EXPECT_EQ(ops[1], "op2");
}

TEST_F(InMemoryHistoryTest, GetLastOperationsZeroCount) {
    history->AddEntry("op1");

    auto ops = history->GetLastOperations(0);
    EXPECT_EQ(ops.size(), 0);
}

TEST_F(InMemoryHistoryTest, EmptyHistoryReturnsEmpty) {
    auto ops = history->GetLastOperations(5);
    EXPECT_EQ(ops.size(), 0);
}

// Тесты неопределнного поведения
class UndefinedBehaviorTest : public ::testing::Test {
protected:
    void SetUp() override {
        history = std::make_unique<InMemoryHistory>();
        calculator = std::make_unique<SimpleCalculator>(*history);
    }

    std::unique_ptr<InMemoryHistory> history;
    std::unique_ptr<SimpleCalculator> calculator;
};

TEST_F(UndefinedBehaviorTest, DISABLED_DivisionByZero) {
    // Деление на ноль не обрабатывается
    EXPECT_DEATH(calculator->Divide(10, 0), ".*") << "Division by zero should be handled";
}

TEST_F(UndefinedBehaviorTest, DISABLED_IntegerOverflowAddition) {
    // Переполнение при сложении не обрабатывается
    int maxInt = INT_MAX;

    int result = calculator->Add(maxInt, 1);

    // Результат непредсказуем
    EXPECT_TRUE(result < 0 || result == maxInt + 1) << "Integer overflow should be handled";
}

TEST_F(UndefinedBehaviorTest, DISABLED_IntegerOverflowMultiplication) {
    // Переполнение при умножении также не обрабатывается
    int largeNum = INT_MAX / 2;

    calculator->Multiply(largeNum, 3);

    // Результат также непредсказуем
    SUCCEED() << "Multiplication overflow should be handled";
}

TEST_F(UndefinedBehaviorTest, DISABLED_HistoryMemoryGrowthUnlimited) {
    // История может расти безгранично, а это потенциальная утечка памяти
    for (int i = 0; i < 100000; ++i) {
        history->AddEntry("operation " + std::to_string(i));
    }

    // Проверяем, что все записи сохранились (может привести к нехватке памяти)
    auto ops = history->GetLastOperations(100000);
    EXPECT_EQ(ops.size(), 100000) << "History has no size limit (potential memory issue)";
}

TEST_F(UndefinedBehaviorTest, DISABLED_VeryLongHistoryEntry) {
    // Нет ограничения на длину записи, а это тоже потенциальная утечка памяти
    std::string longEntry(10000000, 'x');
    history->AddEntry(longEntry);

    // Проверяем, что запись сохранилась (может привести к нехватке памяти)
    auto ops = history->GetLastOperations(1);
    EXPECT_EQ(ops[0].length(), 10000000) << "No limit on history entry length (potential memory issue)";
}

TEST_F(UndefinedBehaviorTest, DISABLED_SetHistoryDoesNotWork) {
    // SetHistory не работает
    auto newHistory = std::make_unique<InMemoryHistory>();

    calculator->Add(1, 1);  // Идет в первую историю

    calculator->SetHistory(*newHistory);  // Попытка сменить историю

    calculator->Add(2, 2);  // Должно идти во вторую историю

    // Но это не работает
    auto ops1 = history->GetLastOperations(1);
    auto ops2 = newHistory->GetLastOperations(1);

    EXPECT_EQ(ops1.size(), 1) << "Original history should have 1 operation";
    EXPECT_EQ(ops2.size(), 1) << "New history should have 1 operation - but this fails";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}