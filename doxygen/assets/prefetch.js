// Wait until the DOM is fully loaded to ensure we can access the meta tag
document.addEventListener('DOMContentLoaded', () => {
    // Look for the meta tag that contains the relative path
    const serviceWorkerPathMeta = document.querySelector('meta[name="service-worker-path"]');
    
    if (serviceWorkerPathMeta) {
        const serviceWorkerPath = serviceWorkerPathMeta.getAttribute('content');
        
        if ('serviceWorker' in navigator) {
            // Dynamically register the service worker with the path from the meta tag
            navigator.serviceWorker.register(serviceWorkerPath)
                .then(reg => console.log('Service worker registered with path:', serviceWorkerPath, reg))
                .catch(err => console.error('Service worker registration failed with path:', serviceWorkerPath, err));
        } else {
            console.warn('Service workers are not supported in this browser.');
        }
    } else {
        console.error('No meta tag found for service worker path.');
    }

    // Hover-based prefetching as before
    const currentHost = window.location.hostname;
    const links = document.querySelectorAll('a[href]');

    links.forEach(link => {
        const href = link.getAttribute('href');
        const linkUrl = new URL(href, window.location.href);

        if (linkUrl.hostname === currentHost) {
            link.addEventListener('mouseenter', () => {
                const url = linkUrl.href;
                // Use the service worker cache to store the hovered page
                caches.open('hover-prefetch-cache').then(cache => {
                    cache.match(url).then(cachedResponse => {
                        if (!cachedResponse) {
                            fetch(url).then(response => {
                                if (response.status === 200) {
                                    cache.put(url, response.clone());
                                    console.log(`Prefetched and cached: ${url}`);
                                }
                            });
                        }
                    });
                });
            });
        }
    });
});

