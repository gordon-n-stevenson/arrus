#include <gtest/gtest.h>
#include <memory>

#include "arrus/core/api/devices/us4r/GpuSettings.h"
#include "arrus/core/common/tests.h"

namespace {
using namespace arrus;
using namespace arrus::devices;

class GpuSettingsTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(GpuSettingsTest, DefaultConstructor) {
    GpuSettings settings;
    EXPECT_FALSE(settings.getMemoryLimit().has_value());
}

TEST_F(GpuSettingsTest, ConstructorWithMemoryLimit) {
    GpuSettings settings("6GB");
    EXPECT_TRUE(settings.getMemoryLimit().has_value());
    EXPECT_EQ(settings.getMemoryLimit().value(), "6GB");
}

TEST_F(GpuSettingsTest, ConstructorWithEmptyMemoryLimit) {
    GpuSettings settings("");
    EXPECT_FALSE(settings.getMemoryLimit().has_value());
}

TEST_F(GpuSettingsTest, ConstructorWithNullopt) {
    GpuSettings settings(std::nullopt);
    EXPECT_FALSE(settings.getMemoryLimit().has_value());
}

TEST_F(GpuSettingsTest, SetMemoryLimit) {
    GpuSettings settings;
    EXPECT_FALSE(settings.getMemoryLimit().has_value());
    
    settings.setMemoryLimit("4GB");
    EXPECT_TRUE(settings.getMemoryLimit().has_value());
    EXPECT_EQ(settings.getMemoryLimit().value(), "4GB");
    
    settings.setMemoryLimit("2GB");
    EXPECT_TRUE(settings.getMemoryLimit().has_value());
    EXPECT_EQ(settings.getMemoryLimit().value(), "2GB");
    
    settings.setMemoryLimit(std::nullopt);
    EXPECT_FALSE(settings.getMemoryLimit().has_value());
}

TEST_F(GpuSettingsTest, DefaultSettings) {
    GpuSettings settings = GpuSettings::defaultSettings();
    EXPECT_FALSE(settings.getMemoryLimit().has_value());
}

TEST_F(GpuSettingsTest, CopyConstructor) {
    GpuSettings original("8GB");
    GpuSettings copy(original);
    
    EXPECT_TRUE(copy.getMemoryLimit().has_value());
    EXPECT_EQ(copy.getMemoryLimit().value(), "8GB");
}

TEST_F(GpuSettingsTest, AssignmentOperator) {
    GpuSettings original("8GB");
    GpuSettings assigned;
    
    assigned = original;
    EXPECT_TRUE(assigned.getMemoryLimit().has_value());
    EXPECT_EQ(assigned.getMemoryLimit().value(), "8GB");
}

TEST_F(GpuSettingsTest, MoveConstructor) {
    GpuSettings original("8GB");
    GpuSettings moved(std::move(original));
    
    EXPECT_TRUE(moved.getMemoryLimit().has_value());
    EXPECT_EQ(moved.getMemoryLimit().value(), "8GB");
    // Original should be in a valid but unspecified state
}

TEST_F(GpuSettingsTest, MoveAssignmentOperator) {
    GpuSettings original("8GB");
    GpuSettings moved;
    
    moved = std::move(original);
    EXPECT_TRUE(moved.getMemoryLimit().has_value());
    EXPECT_EQ(moved.getMemoryLimit().value(), "8GB");
    // Original should be in a valid but unspecified state
}

TEST_F(GpuSettingsTest, VariousMemoryLimitFormats) {
    std::vector<std::string> testLimits = {
        "6GB", "2GB", "1GB", "512MB", "256MB", "128MB",
        "64MB", "32MB", "16MB", "8MB", "4MB", "2MB", "1MB",
        "512KB", "256KB", "128KB", "64KB", "32KB", "16KB",
        "8KB", "4KB", "2KB", "1KB", "512B", "256B", "128B",
        "64B", "32B", "16B", "8B", "4B", "2B", "1B",
        "6.5GB", "2.5GB", "1.5GB", "512.5MB", "256.5MB", "128.5MB"
    };
    
    for (const auto& limit : testLimits) {
        GpuSettings settings(limit);
        EXPECT_TRUE(settings.getMemoryLimit().has_value());
        EXPECT_EQ(settings.getMemoryLimit().value(), limit);
    }
}

TEST_F(GpuSettingsTest, EmptyAndNullMemoryLimits) {
    // Empty string should result in no memory limit
    GpuSettings emptySettings("");
    EXPECT_FALSE(emptySettings.getMemoryLimit().has_value());
    
    // Nullopt should result in no memory limit
    GpuSettings nullSettings(std::nullopt);
    EXPECT_FALSE(nullSettings.getMemoryLimit().has_value());
}

} // namespace 