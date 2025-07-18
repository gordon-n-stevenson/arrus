#ifndef ARRUS_CORE_API_DEVICES_US4R_GPUSETTINGS_H
#define ARRUS_CORE_API_DEVICES_US4R_GPUSETTINGS_H

#include <string>
#include <optional>

namespace arrus::devices {

class GpuSettings {
public:
    explicit GpuSettings(std::optional<std::string> memoryLimit = std::nullopt)
        : memoryLimit(std::move(memoryLimit)) {}

    /**
     * Returns the GPU memory limit as a string (e.g., "6GB", "2GB").
     * 
     * @return Memory limit string, or std::nullopt if no limit is set
     */
    const std::optional<std::string>& getMemoryLimit() const { return memoryLimit; }

    /**
     * Sets the GPU memory limit.
     * 
     * @param memoryLimit Memory limit as a string (e.g., "6GB", "2GB")
     */
    void setMemoryLimit(std::optional<std::string> memoryLimit) {
        this->memoryLimit = std::move(memoryLimit);
    }

    /**
     * Creates default GPU settings (no memory limit).
     * 
     * @return Default GPU settings
     */
    static GpuSettings defaultSettings() {
        return GpuSettings();
    }

private:
    std::optional<std::string> memoryLimit;
};

}

#endif //ARRUS_CORE_API_DEVICES_US4R_GPUSETTINGS_H 