# 📱 Running Buddy PWA 文件

这个文件夹包含了所有PWA（Progressive Web App）相关的配置和工具文件。

## 📂 文件说明

### 核心配置文件
- **`manifest.json`** - PWA应用配置文件（图标、名称、主题色等）
- **`sw.js`** - Service Worker文件（处理离线缓存和资源管理）

### 服务器脚本
- **`start_server.sh`** - 快速启动脚本（推荐使用）
- **`server.js`** - Node.js HTTP服务器

### 工具文件
- **`generate_icons.html`** - 浏览器端图标生成工具
- **`generate_icons.py`** - Python图标生成脚本
- **`PWA_SETUP.md`** - 详细的PWA设置指南

## 🚀 快速开始

### 方法1：使用启动脚本（推荐）

```bash
cd pwa_files
./start_server.sh
```

### 方法2：使用Python服务器

```bash
cd ..  # 回到项目根目录
python3 -m http.server 8080
```

### 方法3：使用Node.js服务器

```bash
cd pwa_files
node server.js
```

## 📱 在手机上访问

1. **启动服务器**（使用上面任一方法）

2. **查看本地IP地址**
   - Mac: 系统设置 > 网络
   - 或者运行: `ipconfig getifaddr en0`

3. **在手机浏览器打开**
   ```
   http://YOUR_IP:8080
   ```

4. **安装到主屏幕**
   - iOS: 分享按钮 → 添加到主屏幕
   - Android: 菜单 → 安装应用

## 🔧 修改配置

### 更改应用名称
编辑 `manifest.json`:
```json
{
  "name": "你的应用名称",
  "short_name": "短名称"
}
```

### 更改主题色
编辑 `manifest.json`:
```json
{
  "theme_color": "#你的颜色代码",
  "background_color": "#你的背景色"
}
```

### 更改缓存策略
编辑 `sw.js` 中的 `urlsToCache` 数组

## 📝 注意事项

1. PWA必须通过HTTP/HTTPS访问，不能直接打开HTML文件
2. Service Worker需要服务器支持
3. iOS Safari对PWA支持有限制
4. 首次访问需要网络，之后可离线使用

## 🐛 故障排除

### 图标不显示
确保在 `../assets/` 文件夹中有 `icon-192.png` 和 `icon-512.png`

### Service Worker未注册
1. 检查浏览器控制台
2. 确保通过HTTP服务器访问
3. 清除浏览器缓存

### 手机无法访问
1. 确保手机和Mac在同一WiFi
2. 检查防火墙设置
3. 尝试使用不同端口

## 📖 详细文档

查看 `PWA_SETUP.md` 获取更详细的设置指南和说明。

