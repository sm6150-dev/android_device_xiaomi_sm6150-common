# Building and running Cuttlestone

## Overview

The Cuttlestone target is derived from the Cuttlefish Android build
(cf_x86_phone). It is a virtual Android device, hosted in a VM which runs on the
[Google Compute Engine](https://console.cloud.google.com/compute). The normal
workflow for building and running Cuttlestone is:

source build/envsetup.sh
lunch cuttlestone_x86_phone-userdebug
make -j dist

Key build artifacts:

* `out/dist/cuttlestone_x86_phone-img-eng.android-build.zip` - *.img files for
   the cuttlestone_x86_phone target (boot, cache, system, userdata, and vendor
   images)
