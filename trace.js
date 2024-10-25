// SPDX-FileType: Source
// SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

// change all .trace and .json links to launch perfetto on load
window.addEventListener("DOMContentLoaded", () => {
  document.querySelectorAll('a[href$=".trace"], a[href$=".json"]').forEach(anchor => {
    const trace_url = anchor.getAttribute("href");
    anchor.removeAttribute("href");
    anchor.addEventListener("click", (event) => { event.preventDefault(); perfetto(trace_url); });
  });
});

// launch perfetto for a given trace url
async function perfetto(trace_url) {
  const title = trace_url.split('/').pop().replace(/\.(trace|json)$/, "");
  const response = await fetch(trace_url);
  if (!response.ok) { console.error("Failed to fetch trace:", response.status); return; }
  const trace_blob = await response.blob();
  const trace_buffer = await trace_blob.arrayBuffer();
  const ui = 'https://ui.perfetto.dev';
  const w = window.open(ui);
  if (!w) { console.error("Failed to open new window to origin:", ui); return; }
  const timer = setInterval(() => { w.postMessage('PING', ui); }, 50);
  const onMessageHandler = (e) => {
    if (e.data !== 'PONG') { console.error("Received unexpected message:", e.data); return; }
    window.clearInterval(timer);
    window.removeEventListener('message', onMessageHandler);
    const reopen_url = new URL(location.href);
    reopen_url.hash = `#reopen=${trace_url}`;
    w.postMessage({ perfetto: { buffer: trace_buffer, title: title, url: reopen_url.toString(), } }, ui);
  };
  window.addEventListener('message', onMessageHandler);
}

// handle reopening
if (location.hash.startsWith('#reopen=')) {
  perfetto(location.hash.substr(8))
}
