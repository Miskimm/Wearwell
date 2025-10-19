# Running Buddy PWA 设置指南

## 📱 如何在手机上使用

### 方法1：使用内置服务器（推荐）

1. **启动服务器**
   ```bash
   cd "/Users/yip/Comp7110/quant/plug in/Running Buddy HTML"
   ./start_server.sh
   ```

2. **获取本地IP地址**
   - 服务器启动后会自动显示你的本地IP地址
   - 或者在"系统设置 > 网络"中查看

3. **在手机上访问**
   - 确保手机和Mac在同一WiFi网络
   - 在手机浏览器打开：`http://YOUR_IP:8080`
   - 例如：`http://192.168.1.100:8080`

### 方法2：使用Python简单服务器

```bash
cd "/Users/yip/Comp7110/quant/plug in/Running Buddy HTML"
python3 -m http.server 8080
```

然后在手机浏览器访问：`http://YOUR_MAC_IP:8080`

### 方法3：使用Go Live（VS Code插件）

1. 右键点击 `index.html`
2. 选择 "Open with Live Server"
3. 查看Live Server的端口号（通常是5500）
4. 在手机浏览器访问：`http://YOUR_MAC_IP:5500`

## 📲 安装PWA到手机主屏幕

### iOS (iPhone/iPad)

1. 在Safari浏览器打开应用
2. 点击底部的"分享"按钮 (⬆️)
3. 滚动找到"添加到主屏幕"
4. 点击"添加"
5. 应用图标会出现在主屏幕上

### Android

1. 在Chrome浏览器打开应用
2. 点击右上角菜单 (⋮)
3. 选择"添加到主屏幕"或"安装应用"
4. 确认添加
5. 应用图标会出现在主屏幕上

## 🎨 生成PWA图标

1. 在浏览器打开 `generate_icons.html`
2. 点击下载按钮获取图标
3. 将图标保存到 `assets` 文件夹：
   - `icon-192.png` (192x192)
   - `icon-512.png` (512x512)

## 🔧 技术细节

### PWA功能特性

- ✅ 离线访问支持
- ✅ 可安装到主屏幕
- ✅ 全屏显示（无浏览器导航栏）
- ✅ 自动缓存资源
- ✅ 快速加载
- ✅ 原生应用体验

### 文件说明

- `manifest.json` - PWA配置文件
- `sw.js` - Service Worker（缓存管理）
- `start_server.sh` - 快速启动服务器脚本

## 🐛 故障排除

### 图标不显示
- 确保在 `assets` 文件夹中有 `icon-192.png` 和 `icon-512.png`
- 使用 `generate_icons.html` 生成默认图标

### 手机无法访问
- 确保Mac和手机在同一WiFi网络
- 检查Mac防火墙设置
- 尝试使用不同的端口号

### Service Worker未注册
- 确保使用HTTP服务器（不能直接打开文件）
- 查看浏览器控制台的错误信息
- 清除浏览器缓存后重试

## 📝 注意事项

1. PWA必须通过HTTP/HTTPS协议访问（不能使用 `file://`）
2. 首次访问需要网络，之后可以离线使用
3. 更新应用时需要清除Service Worker缓存
4. iOS Safari对PWA的支持有限制

## 🚀 快速开始

```bash
# 1. 进入项目目录
cd "/Users/yip/Comp7110/quant/plug in/Running Buddy HTML"

# 2. 生成图标（可选）
# 在浏览器打开 generate_icons.html 并下载图标

# 3. 启动服务器
./start_server.sh

# 4. 在手机浏览器访问显示的地址

# 5. 安装到主屏幕
# iOS: 分享 → 添加到主屏幕
# Android: 菜单 → 安装应用
```

完成！现在您可以像使用原生应用一样使用Running Buddy了！🎉

