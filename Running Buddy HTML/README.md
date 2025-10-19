# 🏃 Running Buddy - Running Buddy Matching App

A mobile progressive web app that helps runners find their perfect running partner.

## ✨ Features

- 🗺️ **Map View** - See the real-time location of nearby runners
- 📋 **List View** - Browse detailed information for all runners
- 👤 **Profile** - View runner statistics and activity logs
- 💬 **Message** - Communicate with running partners
- 📅 **Schedule** - Schedule runs
- 📱 **PWA Support** - Install on your phone's home screen for offline use
- 🎨 **Responsive Design** - Perfect for mobile devices

## 🚀 Quick Start

### Method 1: One-Click Start (Recommended)

```bash
./start_pwa.sh
```

### Method 2: Start from the pwa_files folder

```bash
cd pwa_files
./start_server.sh
```

### Method 3: Using Python

```bash
python3 -m http.server 8080
```

## 📱 Using on a Mobile Phone

1. **Start the server** (use any of the methods above)

2. **Check the access address**
```
📱 Network: http://YOUR_IP:8080
```

3. **Open this address in your mobile browser**

4. **Install to Home Screen**
- **iOS**: Tap Share → Add to Home Screen
- **Android**: Tap Menu → Install App

## 📂 Project Structure

```
Running Buddy HTML/
├── pwa_files/ # PWA configuration and tools
├── css/ # Stylesheet files
├── js/ # JavaScript files
├── assets/ # Images and icons
├── *.html # HTML pages
└── *.md # Documentation Files
```

For detailed structure, see [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md)

## 📚 Documentation

- [PWA Quick Start](PWA_QuickStart.md) - Quick Start Guide
- [Project Structure](PROJECT_STRUCTURE.md) - Detailed project structure
- [PWA Setup Guide](pwa_files/PWA_SETUP.md) - Complete PWA configuration guide
- [PWA File Description](pwa_files/README.md) - Description of PWA-related files

## 🎯 Page Descriptions

| Page | File | Description |
|------|------|------|
| Home | `index.html` | View nearby runners in dual map and list view |
| Login | `login.html` | User login and registration |
| Profile | `profile.html` | View runner details |
| Match Confirmation | `match.html` | Confirm your matching running partner |
| Schedule | `schedule.html` | Schedule your run |

## 🔧 Technology Stack

- HTML5, CSS3, JavaScript (Vanilla)
- PWA (Service Worker + Manifest)
- Lucide Icons
- Python HTTP Server
- Mobile-First Responsive Design

## 🎨 Design Features

- **Mobile-First** - UI designed specifically for mobile
- **Modular CSS** - Easy-to-maintain style architecture
- **Native Experience** - PWA provides a native-like experience
- **Offline Support** - Service Worker for offline access
- **Beautiful Interface** - Modern gradients and animations

## 💡 Development Instructions

### Local Development

1. Clone the project
2. Run `./start_pwa.sh` to start the server
3. Open the app in your browser `http://localhost:8080`

### Generate PWA Icons

```bash
cd pwa_files
python3 generate_icons.py
```

Or open `pwa_files/generate_icons.html` in a browser

### Modify Configuration

Edit `pwa_files/manifest.json` to modify the PWA configuration:
- App Name
- Theme Color
- Icon Path
- Launch URL

## 🐛 Troubleshooting

### Unable to access from phone
- ✓ Ensure phone and Mac are on the same WiFi network
- ✓ Check Mac firewall settings
- ✓ Try using a different port number

### Service Worker Not Registered
- ✓ Must access via HTTP/HTTPS (cannot open HTML file directly)
- ✓ Clear browser cache
- ✓ Check browser console for error messages

### Icon Not Displaying
- ✓ Ensure icon file exists in `assets/` folder
- ✓ Create an icon using an icon generator.
- ✓ Check the path in manifest.json

## 📝 Notes

1. PWAs must be accessed through a server; you cannot directly open the HTML file.
2. An internet connection is required for the initial access; it can be used offline thereafter.
3. iOS Safari has some limitations on PWA support.
4. Chrome or Safari is recommended.

## 📄 License

This project is for learning and demonstration purposes only.

## 🙋 Support

Having problems? Check the documentation:
- [PWA Quick Start](PWA_QuickStart.md)
- [PWA Setup Guide](pwa_files/PWA_SETUP.md)
- [Project Structure](PROJECT_STRUCTURE.md)

---

**Operating Environment**: macOS / Linux / Windows
**Browser**: Chrome, Safari, Firefox, Edge
**Version**: 1.0
**Last Update**: 2024

