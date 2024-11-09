#include <gtest/gtest.h>
#include "data/memtable.hpp"

using namespace factdb;

TEST(MemtableColumnTest, DefaultConstructor) {
    MemtableColumn column;
    EXPECT_EQ(column.getcolname_(), "");
    EXPECT_EQ(column.getcoltype_(), ColumnType::UNKNOWN);
    EXPECT_EQ(column.get_serialized_val_(), "");
}

TEST(MemtableColumnTest, ParameterizedConstructor) {
    MemtableColumn column("user_id", ColumnType::STRING, "user123");
    EXPECT_EQ(column.getcolname_(), "user_id");
    EXPECT_EQ(column.getcoltype_(), ColumnType::STRING);
    EXPECT_EQ(column.get_serialized_val_(), "user123");
}

TEST(MemtableColumnTest, SetAndGetColumnName) {
    MemtableColumn column;
    column.setcolname_("activity");
    EXPECT_EQ(column.getcolname_(), "activity");
}

TEST(MemtableColumnTest, SetAndGetColumnType) {
    MemtableColumn column;
    column.setcoltype_(ColumnType::BOOL);
    EXPECT_EQ(column.getcoltype_(), ColumnType::BOOL);
}

TEST(MemtableColumnTest, SetAndGetSerializedValue) {
    MemtableColumn column;
    column.set_serialized_val_("true");
    EXPECT_EQ(column.get_serialized_val_(), "true");
}

TEST(MemtableColumnTest, SerializeInteger) {
    MemtableColumn column;
    column.serialize_col_(42);
    EXPECT_EQ(column.get_serialized_val_(), "42");
}

TEST(MemtableColumnTest, DeserializeInteger) {
    MemtableColumn column;
    column.set_serialized_val_("42");
    EXPECT_EQ(column.deserialize_col_<int>(), 42);
}

TEST(MemtableColumnTest, SerializeFloat) {
    MemtableColumn column;
    column.serialize_col_(3.14f);
    EXPECT_EQ(column.get_serialized_val_(), "3.14");
}

TEST(MemtableColumnTest, DeserializeFloat) {
    MemtableColumn column;
    column.set_serialized_val_("3.14");
    EXPECT_NEAR(column.deserialize_col_<float>(), 3.14f, 0.001);
}

TEST(MemtableColumnTest, SerializeBoolean) {
    MemtableColumn column;
    column.serialize_col_(true);
    EXPECT_EQ(column.get_serialized_val_(), "1");
}

TEST(MemtableColumnTest, DeserializeBoolean) {
    MemtableColumn column;
    column.set_serialized_val_("0");
    EXPECT_EQ(column.deserialize_col_<bool>(), false);
}

TEST(MemtableColumnTest, SerializeString) {
    MemtableColumn column;
    column.serialize_col_(std::string("test"));
    EXPECT_EQ(column.get_serialized_val_(), "test");
}

TEST(MemtableColumnTest, DeserializeString) {
    MemtableColumn column;
    column.set_serialized_val_("hello");
    EXPECT_EQ(column.deserialize_col_<std::string>(), "hello");
}

TEST(MemtableColumnTest, Print) {
    MemtableColumn column("user_id", ColumnType::STRING, "user123");
    testing::internal::CaptureStdout();
    column.print();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Column Name: user_id"), std::string::npos);
    EXPECT_NE(output.find("Column Type: STRING"), std::string::npos);
    EXPECT_NE(output.find("Serialized Value: user123"), std::string::npos);
}

TEST(MemtableColumnTest, EmptySerializedValue) {
    MemtableColumn column;
    column.set_serialized_val_("");
    EXPECT_EQ(column.get_serialized_val_(), "");
    EXPECT_EQ(column.deserialize_col_<std::string>(), "");
}