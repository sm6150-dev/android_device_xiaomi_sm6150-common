package fod

 import (
    "android/soong/android"
    "android/soong/cc"
    "strings"
)

 func deviceFlags(ctx android.BaseContext) []string {
    var cflags []string

    var config = ctx.AConfig().VendorConfig("XIAOMI_SM6150_FOD")
    var posX = strings.TrimSpace(config.String("POS_X"))
    var posY = strings.TrimSpace(config.String("POS_Y"))
    var size = strings.TrimSpace(config.String("SIZE"))

     cflags = append(cflags, "-DFOD_POS_X=" + posX, "-DFOD_POS_Y=" + posY, "-DFOD_SIZE=" + size)
    return cflags
}

 func fodHalBinary(ctx android.LoadHookContext) {
    type props struct {
        Target struct {
            Android struct {
                Cflags []string
            }
        }
    }

     p := &props{}
    p.Target.Android.Cflags = deviceFlags(ctx)
    ctx.AppendProperties(p)
}

 func fodHalBinaryFactory() android.Module {
    module, _ := cc.NewBinary(android.HostAndDeviceSupported)
    newMod := module.Init()
    android.AddLoadHook(newMod, fodHalBinary)
    return newMod
}

 func init() {
    android.RegisterModuleType("xiaomi_sm6150_fod_hal_binary", fodHalBinaryFactory)
}
