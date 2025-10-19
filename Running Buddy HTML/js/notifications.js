// notifications.js - inline request banner logic

document.addEventListener('DOMContentLoaded', () => {
  // 保持与首页切换一致
  if (typeof initializeViewToggle === 'function') {
    initializeViewToggle();
  }

  // 首次展示（1秒后）
  setTimeout(() => {
    const nextId = getNextDemoRunnerId();
    showInlineRequest(nextId);
  }, 1000);

  // 每20秒重复一次（仅当当前没有显示时）
  startDemoRequestLoop(20000);
});

const DEMO_RUNNER_IDS = ['1', '2', '3'];

function getNextDemoRunnerId() {
  const idx = parseInt(sessionStorage.getItem('demoRequestIdx') || '0', 10) || 0;
  const id = DEMO_RUNNER_IDS[idx % DEMO_RUNNER_IDS.length];
  sessionStorage.setItem('demoRequestIdx', String((idx + 1) % DEMO_RUNNER_IDS.length));
  return id;
}

function startDemoRequestLoop(intervalMs) {
  setInterval(() => {
    if (!isBannerVisible()) {
      const nextId = getNextDemoRunnerId();
      showInlineRequest(nextId);
    }
  }, intervalMs);
}

function isBannerVisible() {
  const banner = document.getElementById('inlineRequestNotice');
  return !!(banner && banner.classList.contains('show'));
}

function showInlineRequest(runnerId) {
  const runners = (typeof getRunnersData === 'function') ? getRunnersData() : [];
  const runner = runners.find(r => r.id === runnerId) || { name: 'Runner' };
  const banner = document.getElementById('inlineRequestNotice');
  if (!banner) return;

  // 填充详情
  const nameEl = document.getElementById('reqName');
  const levelEl = document.getElementById('reqLevel');
  const paceEl = document.getElementById('reqPace');
  const distanceEl = document.getElementById('reqDistance');
  const timeEl = document.getElementById('reqTime');
  const avatarEl = document.getElementById('reqAvatar');
  const msgEl = document.getElementById('reqMsg');
  if (nameEl) nameEl.textContent = runner.name || 'Runner';
  if (avatarEl) { avatarEl.src = runner.avatar || 'assets/profile.png'; avatarEl.alt = runner.name || 'Runner'; }
  if (levelEl) {
    levelEl.textContent = runner.level ? (runner.level) : 'Runner';
    levelEl.className = `badge ${runner.level === 'Advanced' ? 'badge-warning' : runner.level === 'Intermediate' ? 'badge-success' : 'badge-primary'} req-level`;
  }
  if (paceEl) paceEl.textContent = `
    ${runner.pace || '—'}
  `.trim();
  if (distanceEl) distanceEl.textContent = runner.distance || 'nearby';
  if (timeEl) timeEl.textContent = 'Just now';
  if (msgEl) msgEl.textContent = 'Would like to run together tomorrow morning!';

  banner.classList.add('show');
  if (typeof lucide !== 'undefined') { lucide.createIcons(); }

  // 存起来以便 accept 使用
  sessionStorage.setItem('incomingRequestRunner', runnerId);
}

function acceptRequest() {
  const runnerId = sessionStorage.getItem('incomingRequestRunner');
  if (!runnerId) return;

  // 记录为已匹配，但尚未开始
  sessionStorage.setItem('matchedRunner', runnerId);
  sessionStorage.setItem('selectedRunner', 'lanbell'); // 回到当前用户视角
  sessionStorage.removeItem('incomingRequestRunner');

  // 轻提示
  if (typeof showNotification === 'function' && typeof getRunnersData === 'function') {
    const runners = getRunnersData();
    const r = runners.find(x => x.id === runnerId);
    showNotification(`Matched with ${r?.name || 'runner'}`, 'success');
  }

  // 隐藏当前横幅
  const banner = document.getElementById('inlineRequestNotice');
  if (banner) banner.classList.remove('show');

  // 跳到用户 Profile，展示 Matched Partner 卡片
  navigateToPage('profile');
}

function dismissRequest() {
  const banner = document.getElementById('inlineRequestNotice');
  if (banner) banner.classList.remove('show');
  sessionStorage.removeItem('incomingRequestRunner');
}

// export for buttons
window.acceptRequest = acceptRequest;
window.dismissRequest = dismissRequest;
// 暴露给 demo.html 的手动触发
window.showInlineRequest = showInlineRequest;
