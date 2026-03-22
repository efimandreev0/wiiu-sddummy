#include "modpackSelector.h"
#include "globals.h"
#include "version.h"
#include <content_redirection/redirection.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string>
#include <utils/logger.h>

// Убрали весь мусор с отрисовкой экранов и кнопок!

bool ReplaceContentInternal(const std::string &basePath, const std::string &subdir, CRLayerHandle *layerHandle) {
    std::string layerName = "SDInstalled /vol/" + subdir;
    std::string fullPath  = basePath + "/" + subdir;
    struct stat st {};
    
    // Проверяем, есть ли папка
    if (stat(fullPath.c_str(), &st) < 0) {
        DEBUG_FUNCTION_LINE_WARN("Skip /vol/%s to %s redirection. Dir does not exist", subdir.c_str(), fullPath.c_str());
        return false;
    }

    // Делаем мгновенный хук файловой системы (Aroma Magic)
    auto res = ContentRedirection_AddFSLayer(layerHandle,
                                             layerName.c_str(),
                                             fullPath.c_str(),
                                             subdir == "aoc" ? FS_LAYER_TYPE_AOC_MERGE : FS_LAYER_TYPE_CONTENT_MERGE);
    if (res == CONTENT_REDIRECTION_RESULT_SUCCESS) {
        DEBUG_FUNCTION_LINE("Redirect /vol/%s to %s", subdir.c_str(), fullPath.c_str());
        return true;
    } else {
        DEBUG_FUNCTION_LINE_ERR("Failed to redirect /vol/%s to %s", subdir.c_str(), fullPath.c_str());
        return false;
    }
}

// Главная функция вызывается при старте игры
void HandleMultiModPacks(uint64_t titleID) {
    char TitleIDString[17];
    snprintf(TitleIDString, 17, "%016llX", titleID);

    // НОВЫЙ ПУТЬ: sd:/wiiu/sdinstalled/[TitleID]
    const std::string basePath = std::string("fs:/vol/external01/wiiu/sdinstalled/").append(TitleIDString);

    struct stat st {};
    
    // Если папки с игрой на SD нет - просто выходим, пусть грузит из NAND (и крашится об пустышку)
    if (stat(basePath.c_str(), &st) < 0) {
        DEBUG_FUNCTION_LINE_WARN("No SD install found at %s", basePath.c_str());
        return;
    }

    // МГНОВЕННО применяем подмену content и aoc (DLC), без меню, без ожидания кадров!
    ReplaceContentInternal(basePath, "content", &gContentLayerHandle);
    ReplaceContentInternal(basePath, "aoc", &gAocLayerHandle);
}

// Оставили пустышку для совместимости с другими файлами (если она вызывается где-то еще в исходниках)
void console_print_pos(int x, int y, const char *format, ...) {
    // Ничего не делаем, экран больше не рисуем
}