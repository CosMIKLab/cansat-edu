#include "storage.hpp"
#include "config.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_vfs_fat.h"
#include "driver/spi_common.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>

static const char* TAG = "Storage";

static sdmmc_card_t* _card        = nullptr;
static FILE*         _telem       = nullptr;
static FILE*         _events      = nullptr;
static uint32_t      _writeCount  = 0;
static bool          _ok          = false;

static int nextSession() {
    DIR* dir = opendir("/sdcard");
    if (!dir) return 1;
    int highest = 0;
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        const char* name = entry->d_name;
        if (entry->d_type == DT_DIR &&
            name[0] == 'S' &&
            isdigit((uint8_t)name[1]) &&
            isdigit((uint8_t)name[2]) &&
            isdigit((uint8_t)name[3]) &&
            name[4] == '\0') {
            int n = atoi(name + 1);
            if (n > highest) highest = n;
        }
    }
    closedir(dir);
    return highest + 1;
}

namespace Storage {

bool init() {
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();

    spi_bus_config_t bus_cfg = {
        .mosi_io_num     = PIN_SD_MOSI,
        .miso_io_num     = PIN_SD_MISO,
        .sclk_io_num     = PIN_SD_CLK,
        .quadwp_io_num   = -1,
        .quadhd_io_num   = -1,
        .max_transfer_sz = 4096,
    };
    spi_bus_initialize((spi_host_device_t)host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);

    sdspi_device_config_t slot_cfg = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_cfg.gpio_cs  = (gpio_num_t)PIN_SD_CS;
    slot_cfg.host_id  = (spi_host_device_t)host.slot;

    esp_vfs_fat_sdmmc_mount_config_t mnt_cfg = {
        .format_if_mount_failed = false,
        .max_files              = 5,
        .allocation_unit_size   = 16 * 1024,
    };

    esp_err_t ret = esp_vfs_fat_sdspi_mount("/sdcard", &host, &slot_cfg, &mnt_cfg, &_card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SD mount failed: %s", esp_err_to_name(ret));
        return false;
    }

    int n = nextSession();
    if (n > 999) { ESP_LOGE(TAG, "Too many sessions"); return false; }

    char session[6];
    snprintf(session, sizeof(session), "S%03d", n);

    char dir[16];
    snprintf(dir, sizeof(dir), "/sdcard/%s", session);
    mkdir(dir, 0777);

    char path[32];
    snprintf(path, sizeof(path), "/sdcard/%s/telem.csv", session);
    _telem = fopen(path, "w");
    if (!_telem) { ESP_LOGE(TAG, "Cannot open telem.csv"); return false; }
    fprintf(_telem, "time_ms,temp_c,press_hpa,hum_pct,ax_g,ay_g,az_g,gx_dps,gy_dps,gz_dps\n");
    fflush(_telem);

    snprintf(path, sizeof(path), "/sdcard/%s/events.txt", session);
    _events = fopen(path, "w");
    if (!_events) { fclose(_telem); ESP_LOGE(TAG, "Cannot open events.txt"); return false; }

    _ok = true;
    _writeCount = 0;

    char msg[32];
    snprintf(msg, sizeof(msg), "BOOT session=%s", session);
    event(msg);

    ESP_LOGI(TAG, "Session %s opened", session);
    return true;
}

bool log(const TelemetryRecord& r) {
    if (!_ok || !_telem) return false;
    fprintf(_telem, "%lu,%.2f,%.2f,%.2f,%.3f,%.3f,%.3f,%.2f,%.2f,%.2f\n",
            (unsigned long)r.time_ms,
            r.temp, r.pressure, r.humidity,
            r.ax, r.ay, r.az,
            r.gx, r.gy, r.gz);
    if (++_writeCount % SD_FLUSH_EVERY == 0) fflush(_telem);
    return true;
}

void event(const char* msg) {
    if (!_events) return;
    int64_t now_ms = esp_timer_get_time() / 1000;
    fprintf(_events, "[%8lld] %s\n", (long long)now_ms, msg);
    fflush(_events);
}

void flush() {
    if (!_ok) return;
    if (_telem)  fflush(_telem);
    if (_events) fflush(_events);
}

void close() {
    if (!_ok) return;
    flush();
    fclose(_telem);  _telem  = nullptr;
    fclose(_events); _events = nullptr;
    esp_vfs_fat_sdcard_unmount("/sdcard", _card);
    _card = nullptr;
    _ok   = false;
}

} // namespace Storage
