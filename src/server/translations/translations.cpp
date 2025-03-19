#include "translations.h"

#include <filesystem/files/files.h>
#include <utils/common.h>
#include <utils/utils.h>
#include <core/entrypoint.h>

#include <memory/encoders/json.h>

#include <server/configuration/configuration.h>

#include <nlohmann/json.hpp>

#define HAS_MEMBER(DOCUMENT, MEMBER_NAME, MEMBER_PATH)                                     \
    if (!DOCUMENT.contains(MEMBER_NAME))                                                   \
    {                                                                                      \
        TranslationsError(string_format("The field \"%s\" doesn't exists.", MEMBER_PATH)); \
        continue;                                                                          \
    }

#define IS_STRING(DOCUMENT, MEMBER_NAME, MEMBER_PATH)                                       \
    if (!DOCUMENT[MEMBER_NAME].is_string())                                                 \
    {                                                                                       \
        TranslationsError(string_format("The field \"%s\" is not a string.", MEMBER_PATH)); \
        continue;                                                                           \
    }

#define IS_OBJECT(DOCUMENT, MEMBER_NAME, MEMBER_PATH)                                        \
    if (!DOCUMENT[MEMBER_NAME].is_object())                                                  \
    {                                                                                        \
        TranslationsError(string_format("The field \"%s\" is not an object.", MEMBER_PATH)); \
        continue;                                                                            \
    }

void TranslationsError(std::string text)
{
    if (!g_SMAPI)
        return;

    PLUGIN_PRINTF("Translations", "%s\n", text.c_str());
}

void Translations::LoadTranslations()
{
    m_translations.clear();

    if (!Files::ExistsPath("addons/swiftly/translations"))
        Files::CreateDirectory("addons/swiftly/translations");

    std::vector<std::string> translationFiles = Files::FetchFileNames("addons/swiftly/translations");

    for (const std::string &translationFilePath : translationFiles)
    {
        if (!ends_with(translationFilePath, ".json"))
            continue;
        if (!starts_with(translationFilePath, string_format("addons/swiftly/translations%stranslation.", WIN_LINUX("\\", "/"))))
            continue;

        std::string translationFileName = explode(translationFilePath, string_format("addons/swiftly/translations%s", WIN_LINUX("\\", "/")))[1];
        std::string mainTranslationKey = explode(explode(translationFileName, ".json")[0], "translation.")[1];

        auto transFile = encoders::json::FromString(Files::Read(translationFilePath), translationFilePath);
        if (!transFile.is_object())
        {
            TranslationsError(string_format("Translation file \"%s\" needs to be an object.", translationFileName.c_str()));
            continue;
        }

        for (auto it = transFile.begin(); it != transFile.end(); ++it)
        {
            std::string key = it.key();

            IS_OBJECT(transFile, key.c_str(), string_format("%s.%s", mainTranslationKey.c_str(), key.c_str()).c_str())
            HAS_MEMBER(it.value(), "en", string_format("%s.%s.en", mainTranslationKey.c_str(), key.c_str()).c_str())

            Translation translation;
            for (auto it2 = it.value().begin(); it2 != it.value().end(); ++it2)
            {
                std::string transKey = it2.key();

                IS_STRING(it.value(), transKey.c_str(), string_format("%s.%s.%s", mainTranslationKey.c_str(), key.c_str(), transKey.c_str()).c_str())

                std::string transVal = it2.value().get<std::string>();
                translation.RegisterLanguage(transKey, transVal);
            }
            m_translations.insert({mainTranslationKey + "." + key, translation});
        }
    }
}

std::string Translations::FetchTranslation(std::string key, int playerid)
{
    if(g_Config.FetchValue<bool>("core.use_player_language")) {
        // @todo After adding player object
    } else {
        std::string language = g_Config.FetchValue<std::string>("core.language");    

        if (m_translations.find(key) == m_translations.end())
            return key + "." + language;

        std::string translation = m_translations[key].FetchLanguage(language);
        if (translation == "NO_TRANSLATION")
            return key + "." + language;
        else
            return translation;
    }
}