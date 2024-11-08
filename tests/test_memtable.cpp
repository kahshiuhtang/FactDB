#include <gtest/gtest.h>
#include "data/memtable.hpp"

using namespace factdb;

TEST(MemtableColumnTest, DefaultConstructor) {
    MemtableColumn column;
    EXPECT_EQ(column.getcolname_(), "");
    EXPECT_EQ(column.getcoltype_(), ColumnType::UNKNOWN);
    EXPECT_EQ(column.get_serialize(), "");
}

TEST(MemtableColumnTest, ParameterizedConstructor) {
    MemtableColumn column("user_id", ColumnType::STRING, "user123");
    EXPECT_EQ(column.getcolname_(), "user_id");
    EXPECT_EQ(column.getcoltype_(), ColumnType::STRING);
    EXPECT_EQ(column.get_serialize(), "user123");
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
    column.set_serialize_("true");
    EXPECT_EQ(column.get_serialize(), "true");
}

TEST(MemtableColumnTest, SerializeInteger) {
    MemtableColumn column;
    column.serialize_(42);
    EXPECT_EQ(column.get_serialize(), "42");
}

TEST(MemtableColumnTest, DeserializeInteger) {
    MemtableColumn column;
    column.set_serialize_("42");
    EXPECT_EQ(column.deserialize_<int>(), 42);
}

TEST(MemtableColumnTest, SerializeFloat) {
    MemtableColumn column;
    column.serialize_(3.14f);
    EXPECT_EQ(column.get_serialize(), "3.14");
}

TEST(MemtableColumnTest, DeserializeFloat) {
    MemtableColumn column;
    column.set_serialize_("3.14");
    EXPECT_NEAR(column.deserialize_<float>(), 3.14f, 0.001);
}

TEST(MemtableColumnTest, SerializeBoolean) {
    MemtableColumn column;
    column.serialize_(true);
    EXPECT_EQ(column.get_serialize(), "1");
}

TEST(MemtableColumnTest, DeserializeBoolean) {
    MemtableColumn column;
    column.set_serialize_("0");
    EXPECT_EQ(column.deserialize_<bool>(), false);
}

TEST(MemtableColumnTest, SerializeString) {
    MemtableColumn column;
    column.serialize_(std::string("test"));
    EXPECT_EQ(column.get_serialize(), "test");
}

TEST(MemtableColumnTest, DeserializeString) {
    MemtableColumn column;
    column.set_serialize_("hello");
    EXPECT_EQ(column.deserialize_<std::string>(), "hello");
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
    column.set_serialize_("");
    EXPECT_EQ(column.get_serialize(), "");
    EXPECT_EQ(column.deserialize_<std::string>(), "");
}