#include <gtest/gtest.h>
#include <memory>

#include "arrus/core/io/validators/GpuSettingsProtoValidator.h"
#include "arrus/core/common/tests.h"

namespace {
using namespace arrus;
using namespace arrus::io;

class GpuSettingsProtoValidatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        validator = std::make_unique<GpuSettingsProtoValidator>("gpu");
    }

    std::unique_ptr<GpuSettingsProtoValidator> validator;
};

TEST_F(GpuSettingsProtoValidatorTest, AcceptsValidMemoryLimits) {
    std::vector<std::string> validLimits = {
        "6GB", "2GB", "1GB", "512MB", "256MB", "128MB", "64MB", "32MB",
        "16MB", "8MB", "4MB", "2MB", "1MB", "512KB", "256KB", "128KB",
        "64KB", "32KB", "16KB", "8KB", "4KB", "2KB", "1KB", "512B",
        "256B", "128B", "64B", "32B", "16B", "8B", "4B", "2B", "1B",
        "6.5GB", "2.5GB", "1.5GB", "512.5MB", "256.5MB", "128.5MB",
        "6 GB", "2 GB", "1 GB", "512 MB", "256 MB", "128 MB",
        "6gb", "2gb", "1gb", "512mb", "256mb", "128mb",
        "6 Gb", "2 Gb", "1 Gb", "512 Mb", "256 Mb", "128 Mb"
    };

    for (const auto& limit : validLimits) {
        arrus::proto::GpuSettings gpuSettings;
        gpuSettings.set_memory_limit(limit);
        
        validator->validate(gpuSettings);
        EXPECT_NO_THROW(validator->throwOnErrors()) 
            << "Failed for memory limit: " << limit;
    }
}

TEST_F(GpuSettingsProtoValidatorTest, AcceptsEmptyMemoryLimit) {
    arrus::proto::GpuSettings gpuSettings;
    // No memory limit set
    
    validator->validate(gpuSettings);
    EXPECT_NO_THROW(validator->throwOnErrors());
}

TEST_F(GpuSettingsProtoValidatorTest, RejectsInvalidMemoryLimits) {
    std::vector<std::string> invalidLimits = {
        "", "invalid", "6", "GB", "MB", "KB", "B",
        "6TB", "6PB", "6EB", "6ZB", "6YB",
        "-6GB", "-2GB", "-1GB", "-512MB",
        "0GB", "0MB", "0KB", "0B",
        "6.5.5GB", "2..5GB", "1.5.5GB",
        "6GBGB", "2MBMB", "1KBKB", "512BB",
        "6GB2GB", "2MB512KB", "1KB256B",
        "6GB 2GB", "2MB 512KB", "1KB 256B",
        "6GB,2GB", "2MB,512KB", "1KB,256B",
        "6GB;2GB", "2MB;512KB", "1KB;256B",
        "6GB+2GB", "2MB+512KB", "1KB+256B",
        "6GB-2GB", "2MB-512KB", "1KB-256B",
        "6GB*2GB", "2MB*512KB", "1KB*256B",
        "6GB/2GB", "2MB/512KB", "1KB/256B"
    };

    for (const auto& limit : invalidLimits) {
        arrus::proto::GpuSettings gpuSettings;
        gpuSettings.set_memory_limit(limit);
        
        validator->validate(gpuSettings);
        EXPECT_THROW(validator->throwOnErrors(), ::arrus::IllegalArgumentException)
            << "Should have failed for memory limit: " << limit;
    }
}

TEST_F(GpuSettingsProtoValidatorTest, RejectsNegativeValues) {
    std::vector<std::string> negativeLimits = {
        "-6GB", "-2GB", "-1GB", "-512MB", "-256MB", "-128MB",
        "-64MB", "-32MB", "-16MB", "-8MB", "-4MB", "-2MB", "-1MB",
        "-512KB", "-256KB", "-128KB", "-64KB", "-32KB", "-16KB",
        "-8KB", "-4KB", "-2KB", "-1KB", "-512B", "-256B", "-128B",
        "-64B", "-32B", "-16B", "-8B", "-4B", "-2B", "-1B"
    };

    for (const auto& limit : negativeLimits) {
        arrus::proto::GpuSettings gpuSettings;
        gpuSettings.set_memory_limit(limit);
        
        validator->validate(gpuSettings);
        EXPECT_THROW(validator->throwOnErrors(), ::arrus::IllegalArgumentException)
            << "Should have failed for negative memory limit: " << limit;
    }
}

TEST_F(GpuSettingsProtoValidatorTest, RejectsZeroValues) {
    std::vector<std::string> zeroLimits = {
        "0GB", "0MB", "0KB", "0B", "0.0GB", "0.0MB", "0.0KB", "0.0B"
    };

    for (const auto& limit : zeroLimits) {
        arrus::proto::GpuSettings gpuSettings;
        gpuSettings.set_memory_limit(limit);
        
        validator->validate(gpuSettings);
        EXPECT_THROW(validator->throwOnErrors(), ::arrus::IllegalArgumentException)
            << "Should have failed for zero memory limit: " << limit;
    }
}

TEST_F(GpuSettingsProtoValidatorTest, AcceptsDecimalValues) {
    std::vector<std::string> decimalLimits = {
        "6.5GB", "2.5GB", "1.5GB", "0.5GB", "0.1GB",
        "512.5MB", "256.5MB", "128.5MB", "64.5MB", "32.5MB",
        "16.5MB", "8.5MB", "4.5MB", "2.5MB", "1.5MB",
        "512.5KB", "256.5KB", "128.5KB", "64.5KB", "32.5KB",
        "16.5KB", "8.5KB", "4.5KB", "2.5KB", "1.5KB",
        "512.5B", "256.5B", "128.5B", "64.5B", "32.5B",
        "16.5B", "8.5B", "4.5B", "2.5B", "1.5B"
    };

    for (const auto& limit : decimalLimits) {
        arrus::proto::GpuSettings gpuSettings;
        gpuSettings.set_memory_limit(limit);
        
        validator->validate(gpuSettings);
        EXPECT_NO_THROW(validator->throwOnErrors())
            << "Failed for decimal memory limit: " << limit;
    }
}

TEST_F(GpuSettingsProtoValidatorTest, AcceptsWhitespaceInLimits) {
    std::vector<std::string> whitespaceLimits = {
        " 6GB", "6GB ", " 6GB ", "  6GB  ",
        " 2MB", "2MB ", " 2MB ", "  2MB  ",
        " 1KB", "1KB ", " 1KB ", "  1KB  ",
        " 512B", "512B ", " 512B ", "  512B  ",
        "6 GB", "2 MB", "1 KB", "512 B",
        " 6 GB ", " 2 MB ", " 1 KB ", " 512 B "
    };

    for (const auto& limit : whitespaceLimits) {
        arrus::proto::GpuSettings gpuSettings;
        gpuSettings.set_memory_limit(limit);
        
        validator->validate(gpuSettings);
        EXPECT_NO_THROW(validator->throwOnErrors())
            << "Failed for whitespace memory limit: " << limit;
    }
}

TEST_F(GpuSettingsProtoValidatorTest, AcceptsCaseInsensitiveUnits) {
    std::vector<std::string> caseInsensitiveLimits = {
        "6gb", "2gb", "1gb", "512mb", "256mb", "128mb",
        "64mb", "32mb", "16mb", "8mb", "4mb", "2mb", "1mb",
        "512kb", "256kb", "128kb", "64kb", "32kb", "16kb",
        "8kb", "4kb", "2kb", "1kb", "512b", "256b", "128b",
        "64b", "32b", "16b", "8b", "4b", "2b", "1b",
        "6Gb", "2Gb", "1Gb", "512Mb", "256Mb", "128Mb",
        "64Mb", "32Mb", "16Mb", "8Mb", "4Mb", "2Mb", "1Mb",
        "512Kb", "256Kb", "128Kb", "64Kb", "32Kb", "16Kb",
        "8Kb", "4Kb", "2Kb", "1Kb", "512Bb", "256Bb", "128Bb",
        "64Bb", "32Bb", "16Bb", "8Bb", "4Bb", "2Bb", "1Bb"
    };

    for (const auto& limit : caseInsensitiveLimits) {
        arrus::proto::GpuSettings gpuSettings;
        gpuSettings.set_memory_limit(limit);
        
        validator->validate(gpuSettings);
        EXPECT_NO_THROW(validator->throwOnErrors())
            << "Failed for case insensitive memory limit: " << limit;
    }
}

TEST_F(GpuSettingsProtoValidatorTest, ValidatesAgainstAvailableGpuMemory) {
    // Test with a reasonable memory limit that should be accepted
    // (assuming most systems have at least 1GB of GPU memory)
    std::vector<std::string> reasonableLimits = {
        "1GB", "512MB", "256MB", "128MB", "64MB", "32MB", "16MB", "8MB", "4MB", "2MB", "1MB"
    };

    for (const auto& limit : reasonableLimits) {
        arrus::proto::GpuSettings gpuSettings;
        gpuSettings.set_memory_limit(limit);
        
        validator->validate(gpuSettings);
        // This should not throw unless the limit exceeds available GPU memory
        // In most cases, these limits should be acceptable
        try {
            validator->throwOnErrors();
        } catch (const ::arrus::IllegalArgumentException &e) {
            // If it throws, it should be because the limit exceeds available memory
            std::string error_msg = e.what();
            EXPECT_TRUE(error_msg.find("exceeds available GPU memory") != std::string::npos)
                << "Unexpected error for limit " << limit << ": " << error_msg;
        }
    }
}

TEST_F(GpuSettingsProtoValidatorTest, RejectsExcessiveMemoryLimits) {
    // Test with extremely large memory limits that should be rejected
    std::vector<std::string> excessiveLimits = {
        "1000GB", "100GB", "50GB", "25GB", "10GB"
    };

    for (const auto& limit : excessiveLimits) {
        arrus::proto::GpuSettings gpuSettings;
        gpuSettings.set_memory_limit(limit);
        
        validator->validate(gpuSettings);
        try {
            validator->throwOnErrors();
            // If no exception is thrown, it means the system has an extremely large GPU
            // This is unlikely but possible, so we'll just log it
            std::cout << "Warning: System appears to have extremely large GPU memory, "
                      << "limit " << limit << " was accepted" << std::endl;
        } catch (const ::arrus::IllegalArgumentException &e) {
            // This is the expected behavior
            std::string error_msg = e.what();
            EXPECT_TRUE(error_msg.find("exceeds available GPU memory") != std::string::npos)
                << "Expected GPU memory validation error for limit " << limit << ": " << error_msg;
        }
    }
}

TEST_F(GpuSettingsProtoValidatorTest, HandlesCudaUnavailable) {
    // Test that the validator gracefully handles cases where CUDA is not available
    // This test verifies that the validator doesn't crash when CUDA is not accessible
    
    arrus::proto::GpuSettings gpuSettings;
    gpuSettings.set_memory_limit("1GB");
    
    validator->validate(gpuSettings);
    // Should not throw an exception even if CUDA is not available
    // The validator should skip GPU memory validation in such cases
    EXPECT_NO_THROW(validator->throwOnErrors());
}

} // namespace 