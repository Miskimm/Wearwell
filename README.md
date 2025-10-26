# Wearwell
# Running Buddy Prototype — Quick Start & Usage Guide

A concise README-style guide for launching and demoing the prototype with our tech flow (wearable → Edge Link → mobile app → matching → shared session).

---

## What’s in this repo

* `index.html` — Home (map) & nearby runners
* `login.html` — Sign in / Sign up
* `profile.html` — Runner profile (pace, weekly frequency, goals…)
* `match.html` — Match flow and invitations
* `schedule.html` — Pick time, place, and distance
* `notifications.html` — Invites & status updates
* `shared-goal.html` — Synchronized session (countdown + calorie target)
* `pwa_files/` — PWA manifest & icons
* `start_pwa.sh` — Helper script to run a local server

---

## Prerequisites

* A laptop/phone with **Chrome or Edge** (Web Bluetooth supported).
* Bluetooth turned **on**.
* Location Services enabled (for the map).
* Optional: two browser windows/devices to show both sides of a match.

> **Tip:** Web Bluetooth works on **https** or **localhost**. Running locally is fine.

---

## 1) Start the platform (PWA)

**Option A (recommended):**

```bash
bash start_pwa.sh
```

This serves the site at `http://localhost:8000` (or the port shown in your terminal).

**Option B (manual):**

```bash
# any simple static server works; for example:
python3 -m http.server 8000
```

Open `http://localhost:8000` in Chrome/Edge.

---

## 2) Start the wearable **simulator**

1. Open `demo.html` in a second tab (or second device).
2. Click **Start Sensors** to simulate:

   * Optical heart rate & SpO₂
   * Accelerometer/steps
   * Clock

---

## 3) Pair over Bluetooth (Edge Link)

1. In the app (main tab), open the **Connect Wearable** control (header or settings).
2. Select the simulated device.
3. Once paired, sensor data aggregates on the device and streams to the phone via **Bluetooth**.

---

## 4) Sign in / Sign up

1. Go to `login.html`.
2. Create an account or log in.
3. The app may ask to **read your location** — choose **Allow** (needed for nearby matches).

---

## 5) Home (Map) & Context

* Open `index.html`.
* The app adds **place** (your location) and **time window** to your context.
* Nearby runners appear as **avatars** on the map.

---

## 6) Find a partner

**Manual pick:**

* Tap a nearby avatar to preview.

**Recommended:**

* Use **Recommended** to auto-filter by your **pace band + distance + free time**.

---

## 7) View runner details

* Open the runner’s **profile** (`profile.html`).
* Check **pace**, **weekly frequency**, **goals**, and other basic stats.
* Decide: **Start Match** or **Go Back** to pick someone else.

---

## 8) Start a match & invite

1. In `match.html`, tap **Invite**.
2. The other runner receives a **push/notification** (`notifications.html`).
3. When they **Accept**, proceed to session setup.

---

## 9) Confirm session details

* In `schedule.html`, both runners set/confirm:

  * **Time & place**
  * **Planned distance**

* After confirming, you join a **waiting list / lobby** until start time.

---

## 10) Run together (synchronized session)

* At start time, both devices switch to the **synchronized session script** (`shared-goal.html`):

  * **Countdown** to begin
  * **Calorie target** (shared goal) shown to both sides
* During the run, each device continues sending sensor updates (HR/SpO₂, steps, time).

---

## 11) Completion & records

* When finished, each device sends a **completion event**.
* The app generates a **shared completion card** and writes the run to your **history** (profile).

---

## Troubleshooting

* **Bluetooth pairing fails:**

  * Refresh both tabs, ensure Chrome/Edge, and try again on **localhost/https**.
  * Turn Bluetooth off/on, close other apps that might hold the adapter.

* **Map doesn’t show your position:**

  * Re-allow **Location** permissions; check system Location Services.

* **No nearby runners:**

  * Open a second browser/device and log in as another user; or use **Recommended** to widen distance/time.

* **PWA install prompt missing:**

  * Keep the site open for a minute; look for “Install app” in the browser menu.

---

## Demo flow (60-second walkthrough)

1. Start server → open `index.html`.
2. Open **Wearable simulator**` → **Start Sensors**.
3. Pair via **Connect Wearable**.
4. **Log in**, allow **Location**.
5. Pick avatar **or** tap **Recommended**.
6. Open **profile**, tap **Start Match** → send invite.
7. Accept on the second device, set **time/place/distance**.
8. Enter **shared session** (countdown + calorie goal).
9. Finish → show **completion card** in history.
