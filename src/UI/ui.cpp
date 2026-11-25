#include "main.hpp"

#include "../config.hpp"

#include "metacore/shared/unity.hpp"
#include "bsml/shared/BSML.hpp"
#include "../utils/CommandExecute/cmd.hpp"

void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    // Only create the elements once per menu
    if (!firstActivation)
        return;

    auto container = BSML::Lite::CreateScrollableSettingsContainer(self);

    std::thread([container]() {
        std::string output = runCommand("su -c id");
        logger.info("Shell Output: {}", output.c_str());

        if (output.starts_with("uid=0(")) {
            MetaCore::Engine::ScheduleMainThread([container]() {
                AddConfigValueToggle(container, getConfig().SystemCrash);

                auto warningText = BSML::Lite::CreateText(container, "WARNING: Your headset can potentially cause issues if the system crash was to happen. By enabling the toggle, you agree to let the mod run a command with administrator privilege to perform the system crash during gameplay if any sort of misses, bad cut, or bomb hits happen.");
                warningText->set_color(UnityEngine::Color(244, 255, 0, 0.8f));
            });
        }
    }).detach();
}