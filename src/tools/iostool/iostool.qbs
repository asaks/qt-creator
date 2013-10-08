import qbs.base 1.0
import "../QtcTool.qbs" as QtcTool

QtcTool {
    name: "iostool"
    condition: qbs.targetOS.contains("osx")

    Depends { name: "Qt.widgets" }
    Depends { name: "Qt.xml" }
    Depends { name: "app_version_header" }

    files: [
        "main.cpp",
        "iosdevicemanager.cpp",
        "iosdevicemanager.h"
    ]
    cpp.linkerFlags: base.concat(["-sectcreate", "__TEXT", "__info_plist", path + "/Info.plist"])
    cpp.frameworks: base.concat(["CoreFoundation", "CoreServices", "IOKit", "Security",
                                 "SystemConfiguration"])
    cpp.libraries: base.concat(["ssl", "bz2"])

    toolInstallDir: project.ide_libexec_path + "/ios"
}
