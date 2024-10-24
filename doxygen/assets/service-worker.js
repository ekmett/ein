const CACHE_NAME = 'hover-prefetch-cache';
const urlsToCache = [
  // Add initial resources to precache if needed
  '/modulemembers_func.html',
  '/anotherpage.html'
];

// On install, precache resources
self.addEventListener('install', event => {
  event.waitUntil(
    caches.open(CACHE_NAME).then(cache => {
      return cache.addAll(urlsToCache);
    })
  );
});

// Fetch event handler to serve resources from cache or fetch them if not cached
self.addEventListener('fetch', event => {
  event.respondWith(
    caches.match(event.request).then(response => {
      return response || fetch(event.request);
    })
  );
});

