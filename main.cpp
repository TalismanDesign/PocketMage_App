// PocketMage app starter.

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "pm_app_api.h"

// ms to wait for a keypress before exiting on its own.
#define K_QUIT_TIMEOUT_MS 20000

static int center_x(int width, const char *text, int style) {
  const int tw = pm_text_width(PM_TARGET_EINK, text, style);
  return (width - tw) / 2;
}

extern "C" int main(int argc, char **argv) {
  const char *app_name = argc > 0 && argv[0] ? argv[0] : "pocketmage app";
  const char *elf_path = argc > 1 ? argv[1] : "";

  printf("pocketmage app starting: %s (%s)\n", app_name, elf_path);

  if (pm_app_abi() != PM_APP_API_ABI) {
    printf("host ABI mismatch: app %u, host %u; this build is stale\n",
           PM_APP_API_ABI, pm_app_abi());
    pm_oled_sysmsg("SDK ABI mismatch", 2000);
    return 1;
  }

  const char *host_version = pm_host_sdk_version();
  printf("built against sdk %s, host reports %s\n", PM_SDK_VERSION,
         host_version ? host_version : "unknown");

  pm_bz_play_jingle(0);  // startup chirp, respects the mute setting

  // ---- e-ink: title, clock line, separator, hint ----
  const int w = pm_eink_width();
  const int h = pm_eink_height();

  char title[25];
  if (app_name) {
    snprintf(title, sizeof(title), "%.24s", app_name);
  } else {
    snprintf(title, sizeof(title), "PocketMage");
  }
  char time_line[20];
  snprintf(time_line, sizeof(time_line), "%s", pm_clock_timestamp());

  const int title_h = pm_font_height(PM_TARGET_EINK, PM_STYLE_HEADING2);
  const int body_h = pm_font_height(PM_TARGET_EINK, PM_STYLE_BODY);
  const int mono_h = pm_font_height(PM_TARGET_EINK, PM_STYLE_MONO);

  pm_eink_clear();
  pm_text_color(PM_TARGET_EINK, 1);
  pm_text(PM_TARGET_EINK, center_x(w, title, PM_STYLE_HEADING2),
          8 + title_h, title, PM_STYLE_HEADING2);
  pm_text(PM_TARGET_EINK, 8, 8 + 2 * title_h, "A PocketMage app",
          PM_STYLE_BODY);
  pm_text(PM_TARGET_EINK, 8, 8 + 2 * title_h + body_h + 4, time_line,
          PM_STYLE_MONO);
  pm_eink_rect(8, h - 30, w - 8, h - 30, true, true);  // 1px separator
  pm_text(PM_TARGET_EINK, center_x(w, "any key to quit", PM_STYLE_CAPTION),
          h - 8, "any key to quit", PM_STYLE_CAPTION);
  pm_eink_refresh();

  // OLED line
  pm_oled_sysmsg("app loaded", 1200);

  int old_state = -1;
  int waited_ms = 0;
  while (waited_ms < K_QUIT_TIMEOUT_MS) {
    const char key = pm_kb_read();
    if (key) {
      printf("exit on key '%c'\n", key);
      pm_oled_sysmsg("app done", 800);
      pm_bz_play_jingle(1);
      return 0;
    }
    const int state = pm_kb_state();  // 0 normal, 1 shift, 2 fn, 3 fn+shift
    if (state != old_state) {
      printf("kb state %d\n", state);
      old_state = state;
    }
    usleep(10000);
    waited_ms += 10;
  }

  printf("timeout, exiting\n");
  pm_bz_play_jingle(1);
  return 0;
}