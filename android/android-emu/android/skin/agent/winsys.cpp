/* Copyright (C) AnVirt Emu
**
** This software is licensed under the terms of the GNU General Public
** License version 2, as published by the Free Software Foundation, and
** may be copied, distributed, and modified under those terms.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
*/

#include <stdio.h>  // for fprintf, stderr

#include <atomic>
#include <cstdlib>
#include <memory>  // for static_pointer...

#include "android/cmdline-option.h"
#include "android/globals.h"                           // for android_hw
#include "android/skin/rect.h"                         // for SkinRect, SkinPos
#include "android/skin/winsys.h"                       // for WinsysPreferre...
#include "android/ui-emu-agent.h"                      // for UiEmuAgent
#include "android/base/threads/Thread.h"

#include "agent.h"

#ifdef _WIN32
#include <shellapi.h>
#include <windows.h>
#include "android/base/system/Win32UnicodeString.h"
using android::base::Win32UnicodeString;
#else
#include <pthread.h>
#include <signal.h>  // for kill, pthread_...
#include <unistd.h>  // for getpid
#endif

#ifdef _WIN32
#include <shellapi.h>
#include <windows.h>
#include "android/base/system/Win32UnicodeString.h"
#endif

#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

extern double devicePixelRatio(void *win);

#define DEBUG 1

#if DEBUG
#include "android/utils/debug.h"  // for VERBOSE_PRINT

#define D(...) VERBOSE_PRINT(surface, __VA_ARGS__)
#else
#define D(...) ((void)0)
#endif

struct GlobalState {
    int argc;
    char** argv;
    bool geo_saved;
    int window_geo_x;
    int window_geo_y;
    int window_geo_w;
    int window_geo_h;
    int frame_geo_x;
    int frame_geo_y;
    int frame_geo_w;
    int frame_geo_h;
};

static GlobalState* globalState() {
    static GlobalState sGlobalState = {
            .argc = 0,
            .argv = NULL,
            .geo_saved = false,
            .window_geo_x = 0,
            .window_geo_y = 0,
            .window_geo_w = 0,
            .window_geo_h = 0,
            .frame_geo_x = 0,
            .frame_geo_y = 0,
            .frame_geo_w = 0,
            .frame_geo_h = 0,
    };
    return &sGlobalState;
}

static std::atomic<bool> sMainLoopShouldExit(false);

#ifdef _WIN32
static HANDLE sWakeEvent;
#endif

static void enableSigChild() {
    // The issue only occurs on Darwin so to be safe just do this on Darwin
    // to prevent potential issues. The function exists on all platforms to
    // make the calling code look cleaner. In addition the issue only occurs
    // when the extended window has been created. We do not currently know
    // why this only happens on Darwin and why it only happens once the
    // extended window is created. The sigmask is not changed after the
    // extended window has been created.
#ifdef __APPLE__
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    // We only need to enable SIGCHLD for the Qt main thread since that's where
    // all the Qt stuff runs. The main loop should eventually make syscalls that
    // trigger signals.
    int result = pthread_sigmask(SIG_UNBLOCK, &set, nullptr);
    if (result != 0) {
        D("Could not set thread sigmask: %d", result);
    }
#endif
}

static bool onMainQtThread() {
    return true;
}

std::shared_ptr<void> skin_winsys_get_shared_ptr() {
    return std::shared_ptr<void>(nullptr);
}

extern void skin_winsys_enter_main_loop(bool no_window) {
    (void)no_window;
    anv_emu_agent_run_main_loop(anv_emu_agent_get());
    D("Finished QEMU main loop\n");
}

extern void skin_winsys_get_monitor_rect(SkinRect* rect) {
    if (!rect)
        return;

    D("skin_winsys_get_monitor_rect: begin\n");
    rect->pos.x = 0;
    rect->pos.y = 0;
    D("skin_winsys_get_monitor_rect: Begin calling platform specific display queries.\n");
#if defined(_WIN32)
    D("skin_winsys_get_monitor_rect: Windows: GetSystemMetrics(SM_CXSCREEN)\n");
    rect->size.w = (int)GetSystemMetrics(SM_CXSCREEN);
    D("skin_winsys_get_monitor_rect: Windows: GetSystemMetrics(SM_CYSCREEN)\n");
    rect->size.h = (int)GetSystemMetrics(SM_CYSCREEN);
#elif defined(__APPLE__)
    D("skin_winsys_get_monitor_rect: macOS: CGMainDisplayID()\n");
    int displayId = CGMainDisplayID();
    D("skin_winsys_get_monitor_rect: macOS: CGDisplayPixelsWide()\n");
    rect->size.w = CGDisplayPixelsWide(displayId);
    D("skin_winsys_get_monitor_rect: macOS: CGDisplayPixelsHigh()\n");
    rect->size.h = CGDisplayPixelsHigh(displayId);
#else
#error "TODO: impl skin_winsys_get_monitor_rect()"
#endif
    D("%s: (%d,%d) %dx%d", __FUNCTION__, rect->pos.x, rect->pos.y, rect->size.w,
      rect->size.h);
}

extern int skin_winsys_get_device_pixel_ratio(double* dpr) {
    D("skin_winsys_get_device_pixel_ratio");
    *dpr = devicePixelRatio(skin_winsys_get_window_handle());
    return 0;
}

extern void* skin_winsys_get_window_handle() {
    void *hnd = NULL;
    anv_emu_agent_win_id(anv_emu_agent_get(), &hnd);
    return hnd;
}

extern void skin_winsys_get_window_pos(int* x, int* y) {
    D("skin_winsys_get_window_pos (noqt)");
    *x = 0;
    *y = 0;
}

extern void skin_winsys_get_window_size(int* w, int* h) {
    D("skin_winsys_get_window_size (noqt)");
    *w = android_hw->hw_lcd_width;
    *h = android_hw->hw_lcd_height;
    D("%s: size: %d x %d", __FUNCTION__, *w, *h);
}

extern void skin_winsys_get_frame_pos(int* x, int* y) {
    D("skin_winsys_get_frame_pos (noqt)");
    *x = 0;
    *y = 0;
    D("%s: x=%d y=%d", __FUNCTION__, *x, *y);
}

extern void skin_winsys_get_frame_size(int* w, int* h) {
    D("skin_winsys_get_frame_size (noqt)");
    *w = android_hw->hw_lcd_width;
    *h = android_hw->hw_lcd_height;
    D("%s: size: %d x %d", __FUNCTION__, *w, *h);
}

extern bool skin_winsys_window_has_frame() {
    return false;
}

extern void skin_winsys_set_device_geometry(const SkinRect* rect) {
    (void)rect;
}

extern void skin_winsys_save_window_geo() {
    int x = 0, y = 0, w = 0, h = 0;
    int frameX = 0, frameY = 0, frameW = 0, frameH = 0;
    skin_winsys_get_window_pos(&x, &y);
    skin_winsys_get_window_size(&w, &h);
    skin_winsys_get_frame_pos(&frameX, &frameY);
    skin_winsys_get_frame_size(&frameW, &frameH);
    GlobalState* g = globalState();
    g->geo_saved = true;
    g->window_geo_x = x;
    g->window_geo_y = y;
    g->window_geo_w = w;
    g->window_geo_h = h;
    g->frame_geo_x = frameX;
    g->frame_geo_y = frameY;
    g->frame_geo_w = frameW;
    g->frame_geo_h = frameH;
}

extern bool skin_winsys_is_window_fully_visible() {
    return true;
}

extern bool skin_winsys_is_window_off_screen() {
    return false;
}

WinsysPreferredGlesBackend skin_winsys_override_glesbackend_if_auto(
        WinsysPreferredGlesBackend backend) {
    WinsysPreferredGlesBackend currentPreferred =
            skin_winsys_get_preferred_gles_backend();
    if (currentPreferred == WINSYS_GLESBACKEND_PREFERENCE_AUTO) {
        return backend;
    }
    return currentPreferred;
}

extern WinsysPreferredGlesBackend skin_winsys_get_preferred_gles_backend() {
    D("skin_winsys_get_preferred_gles_backend");
    return WINSYS_GLESBACKEND_PREFERENCE_AUTO;
}

void skin_winsys_set_preferred_gles_backend(
        WinsysPreferredGlesBackend backend) {
    D("skin_winsys_set_preferred_gles_backend");
    (void)backend;
}

extern WinsysPreferredGlesApiLevel skin_winsys_get_preferred_gles_apilevel() {
    D("skin_winsys_get_preferred_gles_apilevel");
    return WINSYS_GLESAPILEVEL_PREFERENCE_AUTO;
}

extern void skin_winsys_quit_request() {
    D(__FUNCTION__);

    // Someone else already requested close.
    bool expectExit = false;
    if (!sMainLoopShouldExit.compare_exchange_strong(expectExit, true)) {
        // We did not exchange the value, i.e. sMainLoopShouldExit != false.
        return;
    }

    anv_emu_agent_request_close(anv_emu_agent_get());
}

void skin_winsys_destroy() {
    D(__FUNCTION__);
}

extern void skin_winsys_set_window_icon(const unsigned char* data,
                                        size_t size) {
    D("skin_winsys_set_window_icon");
    (void)data;
    (void)size;
}

extern void skin_winsys_set_window_pos(int x, int y) {
    D("skin_winsys_set_window_pos %d, %d", x, y);
    anv_emu_agent_set_window_pos(anv_emu_agent_get(), x, y);
}

extern void skin_winsys_set_window_size(int w, int h) {
    anv_emu_agent_set_window_pos(anv_emu_agent_get(), w, h);
}

extern void skin_winsys_set_window_cursor_resize(int which_corner) {
    (void)which_corner;
}

extern void skin_winsys_paint_overlay_for_resize(int mouse_x, int mouse_y) {
    (void)mouse_x;
    (void)mouse_y;
}

extern void skin_winsys_set_window_overlay_for_resize(int which_corner) {
    (void)which_corner;
}

extern void skin_winsys_clear_window_overlay() {}

extern void skin_winsys_set_window_cursor_normal() {}

extern void skin_winsys_set_window_title(const char* title) {}

extern void skin_winsys_update_rotation(SkinRotation rotation) {
    (void)rotation;
}

extern void skin_winsys_show_virtual_scene_controls(bool show) {
    (void)show;
}

extern void skin_winsys_spawn_thread(bool no_window,
                                     StartFunction f,
                                     int argc,
                                     char** argv) {
    D("skin_spawn_thread");
    (void)no_window;
    anv_emu_agent_spawn_thread(anv_emu_agent_get(), f, argc, argv);
}

void skin_winsys_setup_library_paths() {}

extern void skin_winsys_init_args(int argc, char** argv) {
    GlobalState* g = globalState();
    g->argc = argc;
    g->argv = argv;
}

extern int skin_winsys_snapshot_control_start() {
    derror("%s: Snapshot UI not available yet in headless mode", __func__);
    return 0;
}

extern void skin_winsys_start(bool no_window) {
    (void)no_window;
    GlobalState* g = globalState();
    skin_winsys_setup_library_paths();
    anv_emu_agent_start(anv_emu_agent_get());
}

typedef struct run_on_ui_thread_context {
    SkinGenericFunction f;
    void* data;
} run_on_ui_thread_context_t;

static void run_on_ui_thread_wrapper_func(void *data) {
    android::base::setUiThreadId(android::base::getCurrentThreadId());
    run_on_ui_thread_context_t *ctx = (run_on_ui_thread_context_t *) data;
    ctx->f(ctx->data);
    delete ctx;

    android::base::setUiThreadId(0);
}

void skin_winsys_run_ui_update(SkinGenericFunction f, void* data, bool wait) {
    run_on_ui_thread_context_t *ctx = new run_on_ui_thread_context_t;
    ctx->f = f;
    ctx->data = data;
    anv_emu_agent_run_on_ui_thread(anv_emu_agent_get(), run_on_ui_thread_wrapper_func, ctx, wait);
}

extern void skin_winsys_error_dialog(const char* message, const char* title) {
    derror("%s: error dialog: title: [%s] msg: [%s]\n", __func__, title,
           message);
    anv_emu_agent_show_error(anv_emu_agent_get(), title, message);
}

void skin_winsys_set_ui_agent(const UiEmuAgent* agent) {
    // TODO: check this ui agent
}

void skin_winsys_report_entering_main_loop(void) {}

extern bool skin_winsys_is_folded() {
    return false;
}

extern void skin_winsys_touch_qt_extended_virtual_sensors(void) {}

// Other skin functions (Just fix link errors for now)

extern "C" int sim_is_present() {
    return 1;
}
