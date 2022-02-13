/*
 *  Copyright (C) 2021 AnVirt
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 * 
 *  main.cpp
 *  Created by Chen Zhen <cz.worker@gmail.com> on 2021/11/04.
 */

#include <unistd.h>
#include <string.h>
#ifdef __APPLE__
#include <errno.h>
#include <sys/sysctl.h>
#endif
#include <pthread.h>

#define ANV_LOG_TAG "anvirt-emu-main"
#include <anvirt_emu/base/log.h>
#include <anvirt_emu/service/service.h>
#include <anvirt_emu/emu/msg.h>

#define PATH_MAX 256
#define NAME_MAX 64

static int should_exit = 0;

typedef struct anv_emu_arg {
  char anvd_root[PATH_MAX];
  char anvd_name[NAME_MAX];
} anv_emu_arg_t;

static anv_emu_arg_t g_arg;

static int service_on_recv_msg(anv_service_t *s, void *ctx, const anv_ipc_msg_t *msg) {
  if (!msg) {
    // ignore NULL msg
    return 0;
  }
  switch (msg->hdr.t) {
  case AnvEmuMsgRunVM: {
    /* TODO: 接受参数，启动模拟器 */
    break;
  }
  default:
    break;
  }
  return 0;
}

#ifdef __APPLE__
// Use "sysctl.proc_translated" to check if running in Rosetta
// Returns 1 if running in Rosetta
static inline int process_is_translated() {
  int ret = 0;
  size_t size = sizeof(ret);
  // Call the sysctl and if successful return the result
  if (sysctlbyname("sysctl.proc_translated", &ret, &size, NULL, 0) != -1) {
    return ret;
  }
  // If "sysctl.proc_translated" is not present then must be native
  if (errno == ENOENT) {
    return 0;
  }

  // Fall back to native
  return 0;
}
#endif

extern "C" int run_qemu_main(int argc, const char **argv, void (*on_main_loop_done)(void));

static void *qemu_main_thread_func(void *ptr) {
  const anv_emu_arg_t *arg = (const anv_emu_arg_t *) ptr;
  // 构造参数
  int argc = 0;
  const char *argv[] = {
      // clang-format off
      "-dns-server", "10.2.0.59",
      "-mem-path", "/Users/chen/.android/avd/Pixel_XL_API_30.avd/snapshots/default_boot/ram.img",
      "-serial", "stdio",
      "-device", "goldfish_pstore,addr=0xff018000,size=0x10000,file=/Users/chen/.android/avd/"
      "Pixel_XL_API_30.avd/data/misc/pstore/pstore.bin",
      "-cpu", "android64",
      "-enable-hvf",
      "-smp", "cores=4",
      "-m", "2048",
      "-lcd-density", "560",
      "-nodefaults",
      "-kernel", "/Users/chen/Library/Android/sdk/system-images/android-30/default/x86_64//kernel-ranchu",
      "-initrd", "/Users/chen/Library/Android/sdk/system-images/android-30/default/x86_64//ramdisk.img",
      "-drive", "if=none,index=0,id=system,if=none,file=/Users/chen/Library/Android/sdk/system-images/"
                "android-30/default/x86_64//system.img,read-only",
      "-device", "virtio-blk-pci,drive=system,modern-pio-notify",
      "-drive", "if=none,index=1,id=cache,if=none,file=/Users/chen/.android/avd/Pixel_XL_API_30.avd/"
                "cache.img.qcow2,overlap-check=none,cache=unsafe,l2-cache-size=1048576",
      "-device", "virtio-blk-pci,drive=cache,modern-pio-notify",
      "-drive", "if=none,index=2,id=userdata,if=none,file=/Users/chen/.android/avd/Pixel_XL_API_30.avd/"
                "userdata-qemu.img.qcow2,overlap-check=none,cache=unsafe,l2-cache-size=1048576",
      "-device", "virtio-blk-pci,drive=userdata,modern-pio-notify",
      "-drive", "if=none,index=3,id=encrypt,if=none,file=/Users/chen/.android/avd/Pixel_XL_API_30.avd/"
                "encryptionkey.img.qcow2,overlap-check=none,cache=unsafe,l2-cache-size=1048576",
      "-device", "virtio-blk-pci,drive=encrypt,modern-pio-notify",
      "-drive", "if=none,index=4,id=vendor,if=none,file=/Users/chen/Library/Android/sdk/system-images/"
                "android-30/default/x86_64//vendor.img,read-only",
      "-device", "virtio-blk-pci,drive=vendor,modern-pio-notify",
      "-drive", "if=none,index=5,id=sdcard,if=none,file=/Users/chen/.android/avd/Pixel_XL_API_30.avd/"
                "sdcard.img.qcow2,overlap-check=none,cache=unsafe,l2-cache-size=1048576",
      "-device", "virtio-blk-pci,drive=sdcard,modern-pio-notify",
      "-netdev", "user,id=mynet",
      "-device", "virtio-net-pci,netdev=mynet",
      "-device", "virtio-rng-pci",
      "-show-cursor",
      "-device", "virtio_input_multi_touch_pci_1",
      "-device", "virtio_input_multi_touch_pci_2",
      "-device", "virtio_input_multi_touch_pci_3",
      "-device", "virtio_input_multi_touch_pci_4",
      "-device", "virtio_input_multi_touch_pci_5",
      "-device", "virtio_input_multi_touch_pci_6",
      "-device", "virtio_input_multi_touch_pci_7",
      "-device", "virtio_input_multi_touch_pci_8",
      "-device", "virtio_input_multi_touch_pci_9",
      "-device", "virtio_input_multi_touch_pci_10",
      "-device", "virtio_input_multi_touch_pci_11",
      "-device", "virtio-keyboard-pci",
      "-L", "/Volumes/MacEx/android_workspace/sdk/emulator/lib/pc-bios",
      "-soundhw", "hda",
      "-vga", "none",
      "-append",  "no_timer_check clocksource=pit console=0 cma=314M@0-4G mac80211_hwsim.channels=2 "
                  "loop.max_part=7 ramoops.mem_address=0xff018000 ramoops.mem_size=0x10000 "
                  "memmap=0x10000$0xff018000 printk.devkmsg=on qemu=1 androidboot.hardware=ranchu "
                  "androidboot.serialno=EMULATOR30X9X5X0 qemu.gles=1 "
                  "qemu.settings.system.screen_off_timeout=2147483647 qemu.encrypt=1 qemu.vsync=60 "
                  "qemu.gltransport=pipe qemu.gltransport.drawFlushInterval=800 qemu.opengles.version=196608 "
                  "qemu.uirenderer=skiagl qemu.dalvik.vm.heapsize=512m qemu.camera_protocol_ver=1 "
                  "androidboot.vbmeta.size=6144 androidboot.vbmeta.hash_alg=sha256 "
                  "androidboot.vbmeta.digest=d7ade4130cf8cb8b715e6c8a5da1ec2fe32edd7ca3acb609d5c9b4e16f3b68b1 "
                  "androidboot.boot_devices=pci0000:00/0000:00:03.0 qemu.wifi=1 qemu.hwcodec.avcdec=2 "
                  "qemu.hwcodec.vpxdec=2 androidboot.console=ttyS0 android.qemud=1 "
                  "qemu.avd_name=Pixel_XL_API_30",
      "-android-hw", "/Users/chen/.android/avd/Pixel_XL_API_30.avd/hardware-qemu.ini",
      // clang-format on
  };
  argc = sizeof(argv) / sizeof(const char *);
  run_qemu_main(argc, argv, [] {
    // TODO: on main loop done
    ALOGT("QEMU main loop done");
  });
  return NULL;
}

static inline int run_qemu_internel(const anv_emu_arg_t *arg) {
  pthread_t qemu_main_thread;
  pthread_create(&qemu_main_thread, NULL, qemu_main_thread_func, (void *) arg);
  pthread_join(qemu_main_thread, NULL);
  ALOGT("quit");
  return 0;
}

int main(int argc, char *argv[]) {
#ifdef __APPLE__
  if (process_is_translated()) {
    ALOGE("process is translated under Rosetta.");
    return -1;
  }
#endif

  memset(&g_arg, 0x0, sizeof(anv_emu_arg_t));

  anv_service_t *s = anv_service_init("anvs-emu");
  int rc = anv_service_on_recv_msg(s, service_on_recv_msg, &g_arg);
  if (rc) {
    ALOGE("start msg loop failed");
    return -1;
  }

  while (!should_exit) {
    sleep(1);
  }

  return 0;
}