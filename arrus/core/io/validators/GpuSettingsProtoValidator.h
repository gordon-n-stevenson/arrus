#ifndef ARRUS_CORE_IO_VALIDATORS_GPUSETTINGSPROTOVALIDATOR_H
#define ARRUS_CORE_IO_VALIDATORS_GPUSETTINGSPROTOVALIDATOR_H

#include "arrus/common/compiler.h"
#include "arrus/core/common/validation.h"

COMPILER_PUSH_DIAGNOSTIC_STATE
COMPILER_DISABLE_MSVC_WARNINGS(4127)

#include "io/proto/devices/us4r/GpuSettings.pb.h"

COMPILER_POP_DIAGNOSTIC_STATE

#include <regex>
#include <algorithm>
#include <cuda_runtime.h>

namespace arrus::io {

class GpuSettingsProtoValidator : public Validator<arrus::proto::GpuSettings> {
public:
    explicit GpuSettingsProtoValidator(const std::string &name) : Validator(name) {}

    void validate(const arrus::proto::GpuSettings &obj) override {
        // Memory limit is optional, but if provided, it should be a valid format
        if (!obj.memory_limit().empty()) {
            validateMemoryLimitFormat(obj.memory_limit());
        }
    }

private:
    void validateMemoryLimitFormat(const std::string &memoryLimit) {
        // Check if the memory limit follows the expected format (e.g., "6GB", "2GB", "512MB")
        std::string upperLimit = memoryLimit;
        std::transform(upperLimit.begin(), upperLimit.end(), upperLimit.begin(), ::toupper);
        
        // Remove whitespace
        upperLimit.erase(std::remove_if(upperLimit.begin(), upperLimit.end(), ::isspace), upperLimit.end());
        
        // Check format: number followed by optional unit (B, KB, MB, GB)
        std::regex pattern(R"(^(\d+(?:\.\d+)?)\s*(GB|MB|KB|B)?$)");
        std::smatch match;
        
        if (!std::regex_match(upperLimit, match, pattern)) {
            expectTrue("memory_limit", false, 
                      "Memory limit format is invalid. Expected format like '6GB', '2GB', '512MB'. Got: " + memoryLimit);
            return;
        }
        
        // Validate the number is positive
        double number;
        try {
            number = std::stod(match[1].str());
            if (number <= 0) {
                expectTrue("memory_limit", false, 
                          "Memory limit value must be positive. Got: " + memoryLimit);
                return;
            }
        } catch (const std::exception &) {
            expectTrue("memory_limit", false, 
                      "Memory limit value is not a valid number. Got: " + memoryLimit);
            return;
        }
        
        // Validate the unit if present
        std::string unit = "B"; // Default unit
        if (match[2].matched) {
            unit = match[2].str();
            if (unit != "B" && unit != "KB" && unit != "MB" && unit != "GB") {
                expectTrue("memory_limit", false, 
                          "Memory limit unit is invalid. Supported units: B, KB, MB, GB. Got: " + memoryLimit);
                return;
            }
        }
        
        // Convert to bytes and validate against available GPU memory
        size_t requestedBytes = convertToBytes(number, unit);
        validateAgainstAvailableMemory(requestedBytes, memoryLimit);
    }
    
    size_t convertToBytes(double number, const std::string &unit) {
        size_t bytes = static_cast<size_t>(number);
        
        if (unit == "KB") {
            bytes *= 1024;
        } else if (unit == "MB") {
            bytes *= 1024 * 1024;
        } else if (unit == "GB") {
            bytes *= 1024 * 1024 * 1024;
        }
        // For "B" unit, bytes is already correct
        
        return bytes;
    }
    
    void validateAgainstAvailableMemory(size_t requestedBytes, const std::string &memoryLimit) {
        // Get available GPU memory
        size_t availableBytes = getAvailableGpuMemory();
        
        if (availableBytes == 0) {
            // If we can't get GPU memory info, skip this validation
            // This could happen if CUDA is not available or GPU is not accessible
            return;
        }
        
        if (requestedBytes > availableBytes) {
            std::string availableStr = formatBytes(availableBytes);
            expectTrue("memory_limit", false, 
                      "Memory limit (" + memoryLimit + ") exceeds available GPU memory (" + availableStr + "). "
                      "Requested: " + std::to_string(requestedBytes) + " bytes, "
                      "Available: " + std::to_string(availableBytes) + " bytes");
        }
    }
    
    size_t getAvailableGpuMemory() {
        size_t freeMemory = 0;
        size_t totalMemory = 0;
        
        cudaError_t result = cudaMemGetInfo(&freeMemory, &totalMemory);
        
        if (result != cudaSuccess) {
            // If CUDA is not available or there's an error, return 0 to skip validation
            return 0;
        }
        
        // Return free memory (available for allocation)
        return freeMemory;
    }
    
    std::string formatBytes(size_t bytes) {
        const std::vector<std::string> units = {"B", "KB", "MB", "GB"};
        size_t unitIndex = 0;
        double value = static_cast<double>(bytes);
        
        while (value >= 1024.0 && unitIndex < units.size() - 1) {
            value /= 1024.0;
            unitIndex++;
        }
        
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.1f%s", value, units[unitIndex].c_str());
        return std::string(buffer);
    }
};

}

#endif //ARRUS_CORE_IO_VALIDATORS_GPUSETTINGSPROTOVALIDATOR_H

#endif //ARRUS_CORE_IO_VALIDATORS_GPUSETTINGSPROTOVALIDATOR_H 