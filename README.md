# Wearwell
🏃‍♀️ WearWell — Running Buddy App
📘 Project Overview

WearWell is a web-based prototype that helps users find running partners, track shared goals, and stay motivated through social support and wearable integration.
This project was developed as part of the DECO3500 – Social & Mobile Computing course at The University of Queensland.

Key Functions:

- Find Nearby Runners — Find running partners near you based on location, pace, and fitness level.

- Shared Goals — Set and track mutual goals with your partner, viewing both individual and shared progress in real time.

- Real-Time Tracking — Monitor workout performance through synced wearable data for instant feedback and motivation.

This prototype illustrates how social and mobile technologies can foster motivation, accountability, and community engagement in everyday fitness experiences.

```
Wearwell-release/
│
├── Information sheet.pdf         # Participant info sheet for testing
├── Interview/
│   └── interview.docx            # Interview questions & data
│
├── Poster.pdf                    # Final conference/tradeshow poster
├── README.md                     # Documentation file
│
├── Research paper/               # Reference research articles
│
├── Running Buddy HTML/           # Main prototype files
│   ├── assets/                   # Images, icons, and assets
│   ├── css/                      # Styling files (base.css, layout.css, etc.)
│   ├── js/                       # JavaScript interactions
│   ├── index.html                # Entry page
│   ├── login.html                # Login interface
│   ├── match.html                # Pair matching page
│   ├── notifications.html        # Notification center
│   ├── profile.html              # User profile page
│   ├── schedule.html             # Training schedule page
│   ├── shared-goal.html          # Shared goal dashboard
│   ├── demo.html                 # App demo page
│   ├── start_pwa.sh              # Launch script (Mac)
│   └── pwa_files/                # Progressive Web App setup files
│
└── README.md                     # This file
```

🧩 How to Run the Prototype
Option 1 — Run with Live Server (Recommended for Beginners)

Open the folder Running Buddy HTML in VS Code.

Install the Live Server extension.

Open index.html and click “Go Live” at the bottom right.

Visit:
👉 http://localhost:5500
 (or the port shown in VS Code).

Option 2 — Run via Command Line (Fixed Port 8080)
cd "Running Buddy HTML"
python -m http.server 8080
Then visit
👉 http://localhost:8080

or on your phone (same WiFi): http://[your-IP-address]:8080.

### Testing and Improvement
Through usability testing of the Running Buddy prototype, we discovered that users struggled with setting their running level, understanding the matching logic, and confirming run appointments. To address these issues, the team optimized the level description cards, added location coordinates, and implemented a quick run appointment feature within the chat interface. These improvements significantly improved operational clarity and matching efficiency, bringing the app closer to real-world user needs.