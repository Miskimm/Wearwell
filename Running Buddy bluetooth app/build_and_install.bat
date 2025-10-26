@echo off
rem === 切换到 JDK 17 ===
set "JAVA_HOME=C:\Program Files\Java\jdk-17"
set "PATH=%JAVA_HOME%\bin;%PATH%"

rem === 切换到当前脚本所在目录（项目根目录）===
cd /d %~dp0

rem === 清理并编译 Debug APK ===
call gradlew clean assembleDebug -Dorg.gradle.java.home="%JAVA_HOME%"
if errorlevel 1 (
    echo =============================
    echo ❌ 构建失败，请检查报错信息
    echo =============================
    pause
    exit /b 1
)

rem === 找到生成的 APK 路径 ===
set APK_PATH=app\build\outputs\apk\debug\app-debug.apk

rem === 检查是否生成成功 ===
if not exist "%APK_PATH%" (
    echo =============================
    echo ❌ 没找到 APK: %APK_PATH%
    echo =============================
    pause
    exit /b 1
)

echo =============================
echo ✅ 构建成功，APK 路径: %APK_PATH%
echo =============================

rem === 如果安装了 adb，就直接安装到手机 ===
adb devices 1>nul 2>nul
if errorlevel 1 (
    echo ⚠ 未检测到 adb，请手动把 APK 装到手机
) else (
    echo 📱 正在安装到设备...
    adb install -r "%APK_PATH%"
)

pause
