# Add project specific ProGuard rules here.
# You can control the set of applied configuration files using the
# proguardFiles setting in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}

# Uncomment this to preserve the line number information for
# debugging stack traces.
#-keepattributes SourceFile,LineNumberTable

# If you keep the line number information, uncomment this to
# hide the original source file name.
#-renamesourcefileattribute SourceFile
# 保留 Application、Activity、BroadcastReceiver 及其子类
-keep public class * extends android.app.Application
-keep public class * extends android.app.Activity
-keep public class * extends android.content.BroadcastReceiver

# 保留 ViewModel 及其子类
-keep class * extends androidx.lifecycle.ViewModel { *; }

# 保留 LiveData 相关
-keep class androidx.lifecycle.LiveData { *; }
-keep class androidx.lifecycle.MutableLiveData { *; }

# 保留 DataBinding 相关
-keep class **.databinding.*Binding { *; }
-keep class androidx.databinding.** { *; }

# 保留 Timber 日志库
-keep class timber.log.** { *; }

# 保留 EventBus（LiveEventBus）相关
-keep class com.jeremyliao.liveeventbus.** { *; }

# 保留 DialogX 相关
-keep class com.kongzue.dialogx.** { *; }

# 保留序列化相关
-keepnames class * implements java.io.Serializable
-keepclassmembers class * implements java.io.Serializable {
    static final long serialVersionUID;
    private static final java.io.ObjectStreamField[] serialPersistentFields;
    private void writeObject(java.io.ObjectOutputStream);
    private void readObject(java.io.ObjectInputStream);
    java.lang.Object writeReplace();
    java.lang.Object readResolve();
}

# 保留所有匿名内部类（如回调常用匿名类）
-keepclassmembers class * {
    <init>(...);
}
-keepclassmembers class * {
    *;
}

# 保留 native 方法相关类和方法名
-keepclasseswithmembernames class com.example.bluetooth_869_10N_EN.** {
    native <methods>;
}

-keepclasseswithmembernames class * {
    native <methods>;
}

# 保留安全相关类及其所有成员，防止混淆导致 native 找不到方法
-keep class com.example.core.SecurityUtils { *; }
-keep class com.example.core.SecurityChecker { *; }
-keep class com.example.bluetooth_869_10N_EN.business.BtController { *; }
-keep class com.example.bluetooth_869_10N_EN.base.Constant { *; }

# 保留经典蓝牙相关核心类
-keep class android.bluetooth.BluetoothAdapter { *; }
-keep class android.bluetooth.BluetoothDevice { *; }
-keep class android.bluetooth.BluetoothSocket { *; }

# 保留自定义注解
-keep @interface *