# 🏃 Running Buddy - 跑步伙伴匹配应用

一个帮助跑步爱好者找到完美跑步伙伴的移动端PWA应用。

## ✨ 功能特性

- 🗺️ **地图视图** - 查看附近跑步者的实时位置
- 📋 **列表视图** - 浏览所有跑步者的详细信息
- 👤 **个人资料** - 查看跑步者的统计数据和活动记录
- 💬 **消息功能** - 与跑步伙伴交流
- 📅 **日程安排** - 预约跑步时间
- 📱 **PWA支持** - 可安装到手机主屏幕，离线使用
- 🎨 **响应式设计** - 完美适配移动设备

## 🚀 快速开始

### 方法1：一键启动（推荐）

```bash
./start_pwa.sh
```

### 方法2：从pwa_files文件夹启动

```bash
cd pwa_files
./start_server.sh
```

### 方法3：使用Python

```bash
python3 -m http.server 8080
```

## 📱 在手机上使用

1. **启动服务器**（使用上述任一方法）

2. **查看访问地址**
   ```
   📱 Network: http://YOUR_IP:8080
   ```

3. **在手机浏览器打开该地址**

4. **安装到主屏幕**
   - **iOS**: 点击分享 → 添加到主屏幕
   - **Android**: 点击菜单 → 安装应用

## 📂 项目结构

```
Running Buddy HTML/
├── pwa_files/          # PWA配置和工具
├── css/                # 样式文件
├── js/                 # JavaScript文件
├── assets/             # 图片和图标
├── *.html              # HTML页面
└── *.md                # 文档文件
```

详细结构请查看 [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md)

## 📚 文档

- [PWA快速开始](PWA_快速开始.md) - 快速开始指南
- [项目结构](PROJECT_STRUCTURE.md) - 详细的项目结构说明
- [PWA设置指南](pwa_files/PWA_SETUP.md) - 完整的PWA配置指南
- [PWA文件说明](pwa_files/README.md) - PWA相关文件的说明

## 🎯 页面说明

| 页面 | 文件 | 说明 |
|------|------|------|
| 首页 | `index.html` | 地图/列表双视图，查看附近跑步者 |
| 登录 | `login.html` | 用户登录和注册 |
| 个人资料 | `profile.html` | 查看跑步者详细信息 |
| 匹配确认 | `match.html` | 确认匹配的跑步伙伴 |
| 日程安排 | `schedule.html` | 安排跑步时间 |

## 🔧 技术栈

- HTML5, CSS3, JavaScript (Vanilla)
- PWA (Service Worker + Manifest)
- Lucide Icons
- Python HTTP Server
- Mobile-first Responsive Design

## 🎨 设计特点

- **移动优先** - 专为手机设计的界面
- **模块化CSS** - 易于维护的样式架构
- **原生体验** - PWA提供类似原生应用的体验
- **离线支持** - Service Worker实现离线访问
- **美观界面** - 现代化的渐变色和动画效果

## 💡 开发说明

### 本地开发

1. 克隆项目
2. 运行 `./start_pwa.sh` 启动服务器
3. 在浏览器打开 `http://localhost:8080`

### 生成PWA图标

```bash
cd pwa_files
python3 generate_icons.py
```

或在浏览器打开 `pwa_files/generate_icons.html`

### 修改配置

编辑 `pwa_files/manifest.json` 修改PWA配置：
- 应用名称
- 主题颜色
- 图标路径
- 启动URL

## 🐛 故障排除

### 手机无法访问
- ✓ 确保手机和Mac在同一WiFi网络
- ✓ 检查Mac防火墙设置
- ✓ 尝试使用不同的端口号

### Service Worker未注册
- ✓ 必须通过HTTP/HTTPS访问（不能直接打开HTML文件）
- ✓ 清除浏览器缓存
- ✓ 检查浏览器控制台错误信息

### 图标不显示
- ✓ 确保 `assets/` 文件夹中有图标文件
- ✓ 使用图标生成工具创建图标
- ✓ 检查manifest.json中的路径

## 📝 注意事项

1. PWA必须通过服务器访问，不能直接打开HTML文件
2. 首次访问需要网络，之后可离线使用
3. iOS Safari对PWA的支持有一些限制
4. 建议使用Chrome或Safari浏览器

## 📄 许可证

本项目仅用于学习和演示目的。

## 🙋 支持

遇到问题？查看文档：
- [PWA快速开始](PWA_快速开始.md)
- [PWA设置指南](pwa_files/PWA_SETUP.md)
- [项目结构说明](PROJECT_STRUCTURE.md)

---

**运行环境**: macOS / Linux / Windows  
**浏览器**: Chrome, Safari, Firefox, Edge  
**版本**: 1.0  
**最后更新**: 2024

