// Service Worker for Running Buddy PWA

const CACHE_NAME = 'running-buddy-v1';
const urlsToCache = [
  '/',
  '/index.html',
  '/login.html',
  '/runners.html',
  '/profile.html',
  '/match.html',
  '/schedule.html',
  '/demo.html',
  '/css/reset.css',
  '/css/base.css',
  '/css/layout.css',
  '/css/components.css',
  '/css/index.css',
  '/css/login.css',
  '/css/runners.css',
  '/css/profile.css',
  '/css/match.css',
  '/css/schedule.css',
  '/js/app.js',
  '/js/index.js',
  '/js/login.js',
  '/js/runners.js',
  '/js/profile.js',
  '/js/match.js',
  '/js/schedule.js',
  '/assets/sarah.png',
  '/assets/profile.png',
  '/assets/map.png'
];

// install event - cache files
self.addEventListener('install', event => {
  console.log('[Service Worker] Installing...');
  event.waitUntil(
    caches.open(CACHE_NAME)
      .then(cache => {
        console.log('[Service Worker] Caching files');
        return cache.addAll(urlsToCache);
      })
      .catch(err => {
        console.log('[Service Worker] Cache failed:', err);
      })
  );
  self.skipWaiting();
});

// activate event - clean up old caches
self.addEventListener('activate', event => {
  console.log('[Service Worker] Activating...');
  event.waitUntil(
    caches.keys().then(cacheNames => {
      return Promise.all(
        cacheNames.map(cacheName => {
          if (cacheName !== CACHE_NAME) {
            console.log('[Service Worker] Deleting old cache:', cacheName);
            return caches.delete(cacheName);
          }
        })
      );
    })
  );
  return self.clients.claim();
});

// fetch event - serve from cache, fallback to network
self.addEventListener('fetch', event => {
  event.respondWith(
    caches.match(event.request)
      .then(response => {
        // return cached version or fetch from network
        return response || fetch(event.request).then(fetchResponse => {
          // cache new resources
          return caches.open(CACHE_NAME).then(cache => {
            cache.put(event.request, fetchResponse.clone());
            return fetchResponse;
          });
        });
      })
      .catch(() => {
        // offline fallback
        if (event.request.mode === 'navigate') {
          return caches.match('/index.html');
        }
      })
  );
});

