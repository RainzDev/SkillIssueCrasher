// Include our main header file, containing the logger
#include "main.hpp"

#include <thread>

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-type-check.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

// Include dependency headers
#include "scotland2/shared/modloader.h"

#include "GlobalNamespace/ScoreController.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"


void delayed_task() {
    // This runs in the background

    std::thread([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
    }).detach();
}

// Store the mod ID and version, so it can be sent to the modloader at startup
static modloader::ModInfo modInfo{MOD_ID, VERSION, 0};

MAKE_HOOK_MATCH(ScoreController_HandleNoteWasCut, &GlobalNamespace::ScoreController::HandleNoteWasCut, void,
    GlobalNamespace::ScoreController* self,
    GlobalNamespace::NoteController* noteController,
    ByRef<GlobalNamespace::NoteCutInfo> cutInfo) {
    if (cutInfo->noteData) {
        const auto gameplayType = cutInfo->noteData->get_gameplayType();

        if (gameplayType == GlobalNamespace::NoteData::GameplayType::Bomb) {
            logger.info("Bomb hit!");
            delayed_task();
            std::abort();
        } else if (!cutInfo->get_allIsOK()) {
            logger.info("Bad cut! failReason=%d", static_cast<int>(cutInfo->get_failReason().value__));
            delayed_task();
            std::abort();
        }
    }

    ScoreController_HandleNoteWasCut(self, noteController, cutInfo);
}

MAKE_HOOK_MATCH(ScoreController_HandleNoteWasMissed, &GlobalNamespace::ScoreController::HandleNoteWasMissed, void,
    GlobalNamespace::ScoreController* self,
    GlobalNamespace::NoteController* noteController) {
    logger.info("Missed note!");
    delayed_task();
    std::abort();

    ScoreController_HandleNoteWasMissed(self, noteController);
}

// Called in the early stages of game loading
// (see https://github.com/sc2ad/scotland2?tab=readme-ov-file#installationusage)
// Often used to initialize and load configs, in addition to its contents here
extern "C" __attribute__((visibility("default"))) void setup(CModInfo* info) noexcept {
    *info = modInfo.to_c();

    // Optionally register our logger so that all its messages
    // are stored in a dedicated file
    // Paper::Logger::RegisterFileContextId(MOD_ID);

    logger.info("Completed setup!");
}

// Called later on in the game loading, after all mods have been opened
// Often used to install hooks and use the APIs of other mods or libraries
extern "C" __attribute__((visibility("default"))) void late_load() noexcept {
    INSTALL_HOOK(logger, ScoreController_HandleNoteWasCut);
    INSTALL_HOOK(logger, ScoreController_HandleNoteWasMissed);
    logger.info("Completed load!");
}
