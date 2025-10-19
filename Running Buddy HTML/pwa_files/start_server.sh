#!/bin/bash

# Running Buddy PWA Server Startup Script

PORT=8080

# detect if running from pwa_files folder or project root
if [ -f "manifest.json" ]; then
    # running from pwa_files folder, go up one level
    cd ..
fi

echo ""
echo "🚀 Starting Running Buddy PWA Server..."
echo ""
echo "📱 Local: http://localhost:$PORT"
echo "📱 Network: http://$(ipconfig getifaddr en0):$PORT"
echo ""
echo "To access from your phone:"
echo "1. Make sure your phone is on the same WiFi network"
echo "2. Open http://$(ipconfig getifaddr en0):$PORT on your phone"
echo "3. On iOS: Tap Share button → Add to Home Screen"
echo "4. On Android: Tap menu (⋮) → Add to Home screen"
echo ""
echo "Press Ctrl+C to stop the server"
echo ""

python3 -m http.server $PORT

