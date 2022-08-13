#include "main.hpp"
#include "GlobalNamespace/PlayerData.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/LevelStatsView.hpp" 

using namespace GlobalNamespace;

DEFINE_CONFIG(ModConfig);

PlayerData *playerData2;

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
// other config tools such as config-utils don't use this config, so it can be removed if those are in use
Configuration &getConfig()
{
    static Configuration config(modInfo);
    return config;
}

// Returns a logger, useful for printing debug messages
Logger &getLogger()
{
    static Logger *logger = new Logger(modInfo);
    return *logger;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo &info)
{
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;

    getConfig().Load();
    getLogger().info("Completed setup!");
}

MAKE_AUTO_HOOK_MATCH(ShowStats, &LevelStatsView::ShowStats, void, LevelStatsView *self, IDifficultyBeatmap *difficultyBeatmap, PlayerData *playerData)
{
    ShowStats(self, difficultyBeatmap, playerData);

    if (getModConfig().Enabled.GetValue())
    playerData->set_lastSelectedBeatmapDifficulty(BeatmapDifficulty(getModConfig().Difficulty.GetValue()));
    playerData2 = playerData;
}   

void DidActivate(HMUI::ViewController *self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    if (firstActivation)
    {
        UnityEngine::GameObject *container = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(self->get_transform());

        std::vector<StringW> Difficulties = { "Easy", "Normal", "Hard", "Expert", "Expert+"};
        StringW Difficulty = Difficulties[2];

        if(getModConfig().Difficulty.GetValue() > 4 || getModConfig().Difficulty.GetValue() < 0)
        getModConfig().Difficulty.SetValue(2);

        Difficulty = Difficulties[getModConfig().Difficulty.GetValue()];

        AddConfigValueToggle(container->get_transform(), getModConfig().Enabled);
        QuestUI::BeatSaberUI::CreateDropdown(container->get_transform(), "Difficulty", Difficulty, Difficulties, [Difficulties](std::string value)
        {
            if(value == Difficulties[0])
            getModConfig().Difficulty.SetValue(0);
            else if(value == Difficulties[1])
            getModConfig().Difficulty.SetValue(1);
            else if(value == Difficulties[2])
            getModConfig().Difficulty.SetValue(2);
            else if(value == Difficulties[3])
            getModConfig().Difficulty.SetValue(3);
            else if(value == Difficulties[4])
            getModConfig().Difficulty.SetValue(4);
            getLogger().info("Default Difficulty was set to %i", getModConfig().Difficulty.GetValue());
            if(playerData2)
            {
            playerData2->set_lastSelectedBeatmapDifficulty(BeatmapDifficulty(getModConfig().Difficulty.GetValue()));
            getLogger().info("PlayerData has been obtained, setting value");
            }
            else
            {
                getLogger().info("Not Setting Value because PlayerData has not been obtained");
            }
        });
    }
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load()
{
    il2cpp_functions::Init();

    getModConfig().Init(modInfo);

    QuestUI::Init();
    QuestUI::Register::RegisterModSettingsViewController(modInfo, DidActivate);
    QuestUI::Register::RegisterMainMenuModSettingsViewController(modInfo, DidActivate);

    getLogger().info("Installing hooks...");
    auto &logger = getLogger();
    Hooks::InstallHooks(logger);
    getLogger().info("Installed all hooks!");
}