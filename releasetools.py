# Copyright (C) 2009 The Android Open Source Project
# Copyright (c) 2011, The Linux Foundation. All rights reserved.
# Copyright (C) 2017-2020 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import common
import re

def FullOTA_Assertions(info):
  AddBasebandAssertion(info, info.input_zip)
  return

def IncrementalOTA_Assertions(info):
  AddBasebandAssertion(info, info.target_zip)
  return

def FullOTA_InstallEnd(info):
  OTA_InstallEnd(info)
  return

def IncrementalOTA_InstallEnd(info):
  OTA_InstallEnd(info)
  return

def AddBasebandAssertion(info, input_zip):
  android_info = input_zip.read("OTA/android-info.txt")
  variants = []
  for variant in ('in', 'cn', 'eea'):
    variants.append(re.search(r'require\s+version-{}\s*=\s*(\S+)'.format(variant), android_info).group(1).split(','))
  cmd = 'assert(getprop("ro.boot.hwc") == "{0}" && (xiaomi.verify_baseband("{2}", "{1}") == "1" || abort("ERROR: This package requires baseband from atleast {2}. Please upgrade firmware and retry!");) || true);'
  for variant in variants:
    info.script.AppendExtra(cmd.format(*variant))

def AddImage(info, basename, dest):
  name = basename
  data = info.input_zip.read("IMAGES/" + basename)
  common.ZipWriteStr(info.output_zip, name, data)
  info.script.Print("Patching {} image unconditionally...".format(dest.split('/')[-1]))
  info.script.AppendExtra('package_extract_file("%s", "%s");' % (name, dest))

def OTA_InstallEnd(info):
  AddImage(info, "vbmeta.img", "/dev/block/bootdevice/by-name/vbmeta")
  AddImage(info, "dtbo.img", "/dev/block/bootdevice/by-name/dtbo")
  return
