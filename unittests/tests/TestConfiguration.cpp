#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include <nlohmann/json.hpp>

#include "Configuration.hpp"
#include "mocks/FileMock.hpp"
#include "mocks/FilesystemMock.hpp"

// clang-format off
TEST_GROUP(TestConfiguration)  // NOLINT
{
    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    FilesystemMock m_filesystemMock;
    FileMock m_fileMock;
};
// clang-format on

TEST(TestConfiguration, ShouldReturnDefaultValues)  // NOLINT
{
    Configuration<FilesystemMock> configuration{m_filesystemMock};

    CHECK_EQUAL(configuration.isMqttEnabled(), false);
    CHECK_EQUAL(configuration.getMqttName(), "Humidifier");
    CHECK_EQUAL(configuration.getMqttUser(), "defaultUser");
    CHECK_EQUAL(configuration.getMqttPasswd(), "defaultPasswd");
    CHECK_EQUAL(configuration.getMqttIP(), "192.168.1.1");
    CHECK_EQUAL(configuration.getMqttPort(), 1883);
    CHECK_EQUAL(configuration.isOtaEnabled(), false);
}

TEST(TestConfiguration, ShouldDumpDataWithoutPassword)  // NOLINT
{
    Configuration<FilesystemMock> configuration{m_filesystemMock};

    auto dump = configuration.dumpWithoutPasswd();
    auto data = nlohmann::json::parse(dump, nullptr, false);

    CHECK_TRUE(data.contains("mqttName"));
    CHECK_FALSE(data.contains("mqttPasswd"));
}

TEST(TestConfiguration, ShouldSaveDataToFilesystemIfCanOpenFile)  // NOLINT
{
    Configuration<FilesystemMock> configuration{m_filesystemMock};

    configuration.setMqttName("SomeTestName");
    auto dumpedAsStr = configuration.dumpAll();

    mock("FilesystemMock")
        .expectOneCall("open")
        .ignoreOtherParameters()
        .andReturnValue(&m_fileMock);
    mock("FileMock").expectOneCall("isFileOk").andReturnValue(true);
    mock("FileMock").expectOneCall("print").withParameter("str", dumpedAsStr.c_str());
    mock("FileMock").expectOneCall("close");

    CHECK_TRUE(configuration.save() == Configuration<FilesystemMock>::Status::OK);
}

TEST(TestConfiguration, ShouldNotSaveIfCantOpenFile)  // NOLINT
{
    Configuration<FilesystemMock> configuration{m_filesystemMock};

    mock("FilesystemMock")
        .expectOneCall("open")
        .ignoreOtherParameters()
        .andReturnValue(&m_fileMock);
    mock("FileMock").expectOneCall("isFileOk").andReturnValue(false);
    mock("FileMock").expectOneCall("close");

    mock("FileMock").expectNoCall("print");
    CHECK_TRUE(configuration.save() == Configuration<FilesystemMock>::Status::OPENING_FILE_ERROR);
}

TEST(TestConfiguration, ShouldLoadDataFromFilesystemIfCanOpenFile)  // NOLINT
{
    Configuration<FilesystemMock> configuration{m_filesystemMock};

    configuration.setMqttName("SomeTestName");

    mock("FilesystemMock")
        .expectOneCall("open")
        .ignoreOtherParameters()
        .andReturnValue(&m_fileMock);
    mock("FileMock").expectOneCall("isFileOk").andReturnValue(true);
    mock("FileMock").expectOneCall("readAsString").andReturnValue(R"({"mqttName": "loadedName"})");
    mock("FileMock").expectOneCall("close");

    CHECK_TRUE(configuration.load() == Configuration<FilesystemMock>::Status::OK);
    CHECK_EQUAL(configuration.getMqttName(), "loadedName");
    CHECK_EQUAL(configuration.getMqttUser(), "defaultUser");  // Not loaded from file, so default value
}

TEST(TestConfiguration, ShouldSetAndReadProperties)  // NOLINT
{
    Configuration<FilesystemMock> configuration{m_filesystemMock};

    configuration.setMqttEnabled(true);
    CHECK_EQUAL(true, configuration.isMqttEnabled());
    configuration.setMqttEnabled(false);
    CHECK_EQUAL(false, configuration.isMqttEnabled());

    configuration.setMqttName("SomeTestName");
    CHECK_EQUAL("SomeTestName", configuration.getMqttName());

    configuration.setMqttUser("SomeTestUser");
    CHECK_EQUAL("SomeTestUser", configuration.getMqttUser());

    configuration.setMqttPasswd("SomeTestPasswd");
    CHECK_EQUAL("SomeTestPasswd", configuration.getMqttPasswd());

    configuration.setMqttIP("192.168.1.2");
    CHECK_EQUAL("192.168.1.2", configuration.getMqttIP());

    configuration.setMqttPort(1883);
    CHECK_EQUAL(1883, configuration.getMqttPort());

    configuration.setOtaEnabled(true);
    CHECK_EQUAL(true, configuration.isOtaEnabled());
    configuration.setOtaEnabled(false);
    CHECK_EQUAL(false, configuration.isOtaEnabled());
}

