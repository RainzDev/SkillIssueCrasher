#include "main.hpp"

std::string runCommand(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    
    // "2>&1" redirects stderr to stdout so you can see error messages in logs
    std::string fullCmd = std::string(cmd) + " 2>&1";
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(fullCmd.c_str(), "r"), pclose);
    
    if (!pipe) return "Failed to run popen";
    
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void executeAsync() {
    std::thread([]() {
        std::string output = runCommand("su -c \"echo 1 > /proc/sys/kernel/sysrq; echo 5 > /proc/sys/kernel/panic; sync; echo u > /proc/sysrq-trigger; sleep 1; echo c > /proc/sysrq-trigger\"");
        logger.info("Shell Output: {}", output.c_str());
    }).detach();
}