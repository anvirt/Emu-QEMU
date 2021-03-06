Android Emulator Linux Development
=====================================

This document describes how to get started with emulator development under linux.

# Software Requirements

The instructions here assume you are using a deb based distribution, such as Debian, Ubuntu or Goobuntu.

Install the following dependencies:
```sh
    sudo apt-get install -y git build-essential python qemu-kvm ninja-build python-pip ccache
```
We need python to run the scripts, kvm to be able to run the emulator and git to get the source.

## Obtaining repo

First we need to obtain the repo tool.
```sh
    mkdir $HOME/bin
    curl http://commondatastorage.googleapis.com/git-repo-downloads/repo > $HOME/bin/repo
    chmod 700 $HOME/bin/repo
```
Make sure to add `$HOME/bin` to your path, if it is not already there.

```sh
    export PATH=$PATH:$HOME/bin
```

Do not forget to add this to your `.bashrc` file.


 ### Initialize the repository:

You can initialize repo as follows:

```sh
    mkdir -p emu-master-dev && cd emu-master-dev
    repo init -u https://android.googlesource.com/platform/manifest -b emu-master-dev
```

***Note:*** You could use persistent-https to initialize the repo, this might give you slightly better performance if your have the persistent-https git plugin.

Sync the repo (and get some coffee, or a have a good nap.)

    repo sync -j 8

Congratulations! You have all the sources you need. Now run:

    cd external/qemu && android/rebuild.sh

If all goes well you should have a freshly build emulator in the `objs` directory.

You can pass the flag `--help` to the rebuild script to get an idea of which options you can pass in.

### Incremental builds

The rebuild script does a complete clean build. You can use ninja to partial builds:

    ninja -C objs

### Speeding up builds with 'ccache'

It is highly recommended to install the 'ccache' build tool on your development
machine(s). The Android emulator build scripts will probe for it and use it
if available, which can speed up incremental builds considerably.

    sudo apt-get install ccache

#### Optional ccache step

Configure ccache to use a different cache size with `ccache -M <max size>`. You can see a list of configuration options by calling ccache alone. * The default ccache directory is ~/.ccache. You might want to symlink it to another directory (for example, when using FileVault for your home directory). You should make this large if you are planning to cross compile to many targets (ming/windows).

### Cross compiling to Windows with clang-cl

***It is highly recommended to use a windows machine for windows development, vs cross compilation.***

It is possible to cross compile from linux to windows. This is mainly useful to quickly discover compilation issues, as you will not be able to actually run the code.

The windows target requires you to install the MSVC libraries. These libraries need manual intervention to be installed on your linux machine as you will need to:

#### Enable cross compiling from within Google:
run:

    $AOSP/external/qemu/android/scripts/activate-msvc.sh

This script will guide you through the steps, and configure your machine for cross compiling msvc. Once this is completed you should be able to replicate what is happening on the build bots. **The script will require sudo priveleges**

If all went well you can now target windows as follows:

    cd $AOSP/external/qemu/ && ./android/rebuild.sh --target windows

#### Enable cross compiling from outside Google:

  1. Strongly consider not doing this, and develop on a windows machine.
  2. You will need to have access to a Windows machine.
      - Install [Python](https://www.python.org/downloads/windows/)
      - Install [Chrome Depot Tools](https://dev.chromium.org/developers/how-tos/depottools)
      - Install [Visual Studio](https://visualstudio.microsoft.com/)

  3. Package your Windows SDK installation into a zip file by running the following on a Windows machine:

```sh
    cd path/to/depot_tools/win_toolchain
    # customize the Windows SDK version numbers
    python package_from_installed.py 2017 -w 10.0.17134.0
```

  4. Execute the following on your linux machine:

```sh
    export DISK_LOC=/mnt/jfs_msvc.img
    # Make sure we have JFS
    sudo apt-get install -y jfsutils
    sudo mkdir -p /mnt/msvc

    # Create a OS/2 case insensitive loopback fs.
    echo "Creating empty img in /tmp/jfs.img"
    dd if=/dev/zero of=/tmp/jfs.img bs=1M count=4096
    chmod a+rwx /tmp/jfs.img
    echo "Making filesystem"
    mkfs.jfs -q -O /tmp/jfs.img
    sudo mv /tmp/jfs.img ${DISK_LOC}

    sudo mount ${DISK_LOC} /mnt/msvc -t jfs -o loop
    sudo chmod a+rwx /mnt/msvc
    unzip msvc_zip -D /mnt/msvc
```

If all went well you can now target windows as follows:

    ./android/rebuild.sh --target windows

### Cross compiling to Darwin with clang-cl

***It is higly recommended to use a mac for mac development, vs cross compilation.***

It is possible to cross compile from linux to mac os. This is mainly useful to
quickly discover compilation issues, as you will not be able to actually run the
code.  The cross compiler toolchain can target both arm64 as well as x86_64. The
instructions below assume you are on a Debian/Ubuntu based system.

Make sure you have llvm-11 available on your local system. (See
https://apt.llvm.org/ if the lines below fail)

```sh
   sudo apt-get update
   sudo apt-get install -y llvm-11-dev clang-11 llvm-11
```

If you are withing Google you can get started by using one of the prebuilt
toolchains and install it as follows:

```sh
   gsutil cp gs://emu-dev-development/osxcross-debian-buster-slim-MacOSX11.3.sdk.tar.gz /tmp
   ./android/scripts/unix/activate-darwin-cross.sh --xcode-tar=/tmp/osxcross-debian-buster-slim-MacOSX11.3.sdk.tar.gz --install
```

If you are outside of Google you will have to construct the toolchain yourself.
You will need to obtain a xcode.xip with MacOS-SDK (11.0>) (Xcode 12.3 has been
tested). You can obtain it from [go/xcode](http://go/xcode) if you're within
Google or from [Apple](https://developer.apple.com/download/more.).


Once you have obtained the .xip you can create the toolchain as follows:

```sh
  ./android/scripts/unix/activate-darwin-cross.sh --xcode=/data/.../Xcode_12.3.xip --install
```


After that succeeds you should be able to target darwin:

    ./android/rebuild.sh --target darwin

Or the arm target:

    ./android/rebuild.sh --target darwin_aarch64


### Sending patches

Here you can find details on [submitting patches](
https://gerrit.googlesource.com/git-repo/+/refs/heads/master/SUBMITTING_PATCHES.md). No external configuation should be needed if you are within Google.
