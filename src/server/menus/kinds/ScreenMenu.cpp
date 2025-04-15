#include "ScreenMenu.h"

#include <utils/utils.h>
#include <server/configuration/configuration.h>

#include <regex>

ScreenMenu::ScreenMenu(std::string id, std::string title, std::string color, std::vector<std::pair<std::string, std::string>> options, bool tmp)
{
    int r = 255, g = 255, b = 255, a = 255;
    try {
        r = std::stoi(color.substr(0, 2), nullptr, 16);
    }
    catch (std::exception& e) {}

    try {
        g = std::stoi(color.substr(2, 2), nullptr, 16);
    }
    catch (std::exception& e) {}

    try {
        b = std::stoi(color.substr(4, 2), nullptr, 16);
    }
    catch (std::exception& e) {}

    if (color.length() >= 8) {
        try {
            a = std::stoi(color.substr(6, 2), nullptr, 16);
        }
        catch (std::exception& e) {}
    }

    this->id = id;
    this->color.SetColor(r, g, b, a);
    this->options = options;
    this->title = title;
    this->temporary = tmp;

    ProcessOptions();
}

ScreenMenu::~ScreenMenu()
{
    this->id.clear();
    this->title.clear();
    this->options.clear();
    this->processedOptions.clear();
}

std::string ScreenMenu::GetID()
{
    return this->id;
}

std::string ScreenMenu::GetCommandFromOption(int page, int selected)
{
    if (page < 1)
        return "";

    return processedOptions[page - 1][selected].second;
}

int ScreenMenu::GetItemsOnPage(int page)
{
    if (page < 1)
        return 0;

    return processedOptions[page - 1].size();
}

std::string stringWithSplit(std::string text, int maxCharsPerRow) {
    std::vector<std::string> result;

    auto splitted = explode(text, " ");
    std::string tmp = "";
    for (std::string& str : splitted) {
        if (tmp.size() + str.size() > maxCharsPerRow) {
            result.push_back(tmp);
            tmp = "";
        }
        tmp += (str + " ");
    }
    if (tmp != "") result.push_back(tmp);

    return implode(result, "\n ");
}

std::string RemoveHtmlTags(std::string input) {
    std::regex pattern("<(/?)(div|font)[^>]*>");

    return std::regex_replace(input, pattern, "");
}

void ScreenMenu::ProcessOptions()
{
    int pages = 0;
    int processedItems = 0;
    int totalProcessedItems = 0;
    std::vector<std::pair<std::string, std::string>> tempmap;

    int reservedItems = g_Config.FetchValue<bool>("core.menu.buttons.exit.option") ? 3 : 2;
    int allowedItems = g_Config.FetchValue<int>("core.menu.kind_settings.screen.itemsPerPage") - reservedItems;

    for (const std::pair<std::string, std::string> entry : this->options)
    {
        ++processedItems;
        ++totalProcessedItems;

        tempmap.push_back({ stringWithSplit(RemoveHtmlTags(entry.first), 35), entry.second });

        if (processedItems - (pages == 0 ? 1 : 0) == allowedItems)
        {
            tempmap[tempmap.size() - 1].first += "\n";
            if (options.size() - totalProcessedItems > 0) tempmap.push_back({ stringWithSplit(RemoveHtmlTags(g_translations.FetchTranslation("core.menu.next")), 35), "menunext" });
            if (pages != 0) tempmap.push_back({ stringWithSplit(RemoveHtmlTags(g_translations.FetchTranslation("core.menu.back")), 35), "menuback" });
            if (reservedItems == 3) tempmap.push_back({ stringWithSplit(RemoveHtmlTags(g_translations.FetchTranslation("core.menu.exit")), 35), "menuexit" });

            processedItems = 0;
            pages++;
            this->processedOptions.push_back(tempmap);
            tempmap.clear();
        }
    }

    if (tempmap.size() > 0)
    {
        std::string padding = "\n";
        for (int i = 0; i < allowedItems - tempmap.size() + 2; i++) padding += "\n";
        tempmap[tempmap.size() - 1].first += padding;

        if (this->processedOptions.size() != 0) tempmap.push_back({ stringWithSplit(RemoveHtmlTags(g_translations.FetchTranslation("core.menu.back")), 35), "menuback" });
        if (reservedItems == 3) tempmap.push_back({ stringWithSplit(RemoveHtmlTags(g_translations.FetchTranslation("core.menu.exit")), 35), "menuexit" });

        processedItems = 0;
        this->processedOptions.push_back(tempmap);
        tempmap.clear();
    }
}

std::string ScreenMenu::GeneratedItems(int playerid, int page)
{
    return this->generatedPages[playerid][page - 1];
}

void ScreenMenu::RegeneratePage(int playerid, int page, int selected)
{
    if (this->generatedPages.find(playerid) == this->generatedPages.end())
        this->generatedPages.insert({ playerid, {} });

    while (this->generatedPages[playerid].size() < page)
    {
        this->generatedPages[playerid].push_back("");
    }

    std::string stringPage = string_format(" %s\n", stringWithSplit(RemoveHtmlTags(this->title), 35).c_str());
    for (int i = 0; i < processedOptions[page - 1].size(); i++) {
        stringPage += string_format("\n%s%d. %s", g_Config.FetchValue<std::string>("core.menu.inputMode") == "chat" ? " " : (i == selected ? (g_Config.FetchValue<std::string>("core.menu.navigation_prefix")).c_str() : " "), i + 1, processedOptions[page - 1][i].first.c_str());
    }

    stringPage += string_format("\n\n %s", GenerateFooter(page).c_str());

    this->generatedPages[playerid][page - 1] = stringPage;
}

bool ScreenMenu::IsTemporary()
{
    return this->temporary;
}

Color ScreenMenu::GetColor()
{
    return this->color;
}

std::string ScreenMenu::GenerateFooter(int page)
{
    if (g_Config.FetchValue<std::string>("core.menu.inputMode") == "chat") {
        std::string footer = replace(g_translations.FetchTranslation("core.menu.input.chat"), "{PAGE}", std::to_string(page));
        footer = replace(footer, "{MAXPAGES}", std::to_string(processedOptions.size()));
        footer = replace(footer, "{PREFIX}", GenerateCommandDefaultPrefix());
        return footer;
    }
    else {
        std::string footer = replace(g_translations.FetchTranslation(g_Config.FetchValue<bool>("core.menu.buttons.exit.option") ? "core.menu.screen.footer" : "core.menu.screen.footer.nooption"), "{PAGE}", std::to_string(page));
        footer = replace(footer, "{MAXPAGES}", std::to_string(processedOptions.size()));
        footer = replace(footer, "{CYCLE_BUTTON}", str_toupper(g_Config.FetchValue<std::string>("core.menu.buttons.scroll")));
        footer = replace(footer, "{USE_BUTTON}", str_toupper(g_Config.FetchValue<std::string>("core.menu.buttons.use")));
        footer = replace(footer, "{EXIT_BUTTON}", str_toupper(g_Config.FetchValue<std::string>("core.menu.buttons.exit.button")));
        return footer;
    }
}

bool ScreenMenu::RenderEachTick()
{
    return false;
}

std::string ScreenMenu::GetKind()
{
    return "screen";
}