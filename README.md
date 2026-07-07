# SHBluetooth

# 编译
```
function Import-VSEnv($arch) {
    if ($arch -eq "x64") {
        $bat = "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat -arch=amd64"
    } elseif ($arch -eq "x86") {
        $bat = "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat -arch=x86"
    } else {
        throw "Unknown arch: $arch"
    }

    cmd /c "`"$bat`" && set" | ForEach-Object {
        if ($_ -match "=") {
            $name, $value = $_ -split "=", 2
            [System.Environment]::SetEnvironmentVariable($name, $value)
        }
    }

    Write-Host ">>> Loaded MSVC environment for $arch"
}

Import-VSEnv x64
cmake -S . -B out\build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=out\install
cmake --build out\build --config Release
cmake --build out\build --target install
```
