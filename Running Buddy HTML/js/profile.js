// Profile.js - runner profile page logic

// page init
document.addEventListener('DOMContentLoaded', function() {
    initializeProfilePage();
});

// init profile page
function initializeProfilePage() {
    // load runner data
    loadRunnerData();
    
    // init action buttons
    initializeActionButtons();
    
    // init charts
    initializeCharts();
    
    // init activity timeline
    initializeActivityTimeline();

    // render pending match for current user
    renderPendingMatchIfAny();

    // hide actions for current user (no request/send buttons)
    hideActionsForCurrentUser();
}

// load runner data
function loadRunnerData() {
    // get selected runner ID from sessionStorage
    const selectedRunnerId = sessionStorage.getItem('selectedRunner');
    
    // 如果是点击头像进入的，显示当前用户（LAN BELL）
    const isCurrentUser = !selectedRunnerId || selectedRunnerId === 'lanbell';
    
    if (isCurrentUser) {
        // 显示当前用户 LAN BELL 的资料
        const currentUserProfile = {
            id: 'lanbell',
            name: 'LAN BELL',
            avatar: 'assets/profile.png',
            level: 'Intermediate Runner',
            pace: '5:30',
            runsPerWeek: '4',
            lastActivity: 'Ran yesterday',
            totalRuns: 156,
            totalDistance: 780,
            bestPace: '5:15 min/km',
            recentActivities: [
                { date: 'Yesterday', distance: '5.2km', time: '28:45', pace: '5:32 min/km' },
                { date: '2 days ago', distance: '3.8km', time: '21:30', pace: '5:39 min/km' },
                { date: '4 days ago', distance: '7.1km', time: '39:15', pace: '5:31 min/km' }
            ]
        };
        updateRunnerProfile(currentUserProfile);
        return;
    }
    
    // get all runners data
    const runners = getRunnersData();
    
    // find the selected runner or default to first runner
    let selectedRunner = runners.find(r => r.id === selectedRunnerId);
    if (!selectedRunner) {
        selectedRunner = runners[0]; // default to first runner
    }
    
    // load the selected runner's profile data
    const runnerProfile = {
        id: selectedRunner.id,
        name: selectedRunner.name,
        avatar: selectedRunner.avatar,
        level: selectedRunner.level + ' Runner',
        pace: selectedRunner.pace.split(' ')[0], // extract just the time part
        runsPerWeek: selectedRunner.runsPerWeek.toString(),
        lastActivity: selectedRunner.lastActivity,
        totalRuns: getTotalRuns(selectedRunner.id),
        totalDistance: getTotalDistance(selectedRunner.id),
        bestPace: getBestPace(selectedRunner.id),
        recentActivities: getRecentActivities(selectedRunner.id)
    };
    
    updateRunnerProfile(runnerProfile);
}

// update runner profile
function updateRunnerProfile(runner) {
    // update avatar
    const avatarImg = document.getElementById('profile-avatar');
    if (avatarImg) {
        avatarImg.src = runner.avatar;
        avatarImg.alt = runner.name;
    }
    
    // update name
    const nameElement = document.getElementById('profile-name');
    if (nameElement) {
        nameElement.textContent = runner.name;
    }
    
    // update level with appropriate badge color
    const levelElement = document.getElementById('profile-level');
    if (levelElement) {
        const badgeClass = getLevelClass(runner.level.split(' ')[0]);
        levelElement.innerHTML = `<span class="level-badge badge ${badgeClass}">${runner.level}</span>`;
    }
    
    // update distance (get from original runner data)
    const distanceElement = document.getElementById('profile-distance');
    if (distanceElement) {
        const selectedRunnerId = sessionStorage.getItem('selectedRunner');
        const runners = getRunnersData();
        const selectedRunner = runners.find(r => r.id === selectedRunnerId) || runners[0];
        distanceElement.textContent = selectedRunner.distance;
    }
    
    // update pace
    const paceElement = document.getElementById('profile-pace');
    if (paceElement) {
        paceElement.textContent = runner.pace;
    }
    
    // update runs count
    const runsElement = document.getElementById('profile-runs');
    if (runsElement) {
        runsElement.textContent = runner.runsPerWeek;
    }
    
    // update last activity
    const activityElement = document.getElementById('profile-activity');
    if (activityElement) {
        activityElement.textContent = runner.lastActivity;
    }
    
    // update activity stats
    const totalRunsElement = document.querySelector('.activity-stat-card .activity-stat-value');
    if (totalRunsElement && runner.totalRuns) {
        totalRunsElement.textContent = runner.totalRuns;
    }
    
    const totalDistanceElement = document.querySelectorAll('.activity-stat-card .activity-stat-value')[1];
    if (totalDistanceElement && runner.totalDistance) {
        totalDistanceElement.textContent = runner.totalDistance;
    }
    
    const bestPaceElement = document.querySelectorAll('.activity-stat-card .activity-stat-value')[2];
    if (bestPaceElement && runner.bestPace) {
        bestPaceElement.textContent = runner.bestPace;
    }
    
    // update recent activities if available
    if (runner.recentActivities) {
        updateRecentActivities(runner.recentActivities);
    }
}

// update recent activities
function updateRecentActivities(activities) {
    const activityList = document.querySelector('.activity-list');
    if (activityList && activities) {
        activityList.innerHTML = '';
        activities.forEach(activity => {
            const activityItem = document.createElement('div');
            activityItem.className = 'activity-item';
            activityItem.innerHTML = `
                <div class="activity-time">${activity.time}</div>
                <div class="activity-detail">${activity.detail}</div>
            `;
            activityList.appendChild(activityItem);
        });
    }
}

// get level text
function getLevelText(level) {
    switch (level) {
        case 'Beginner': return 'Beginner';
        case 'Intermediate': return 'Intermediate';
        case 'Advanced': return 'Advanced';
        default: return level;
    }
}

// get level class
function getLevelClass(level) {
    switch (level) {
        case 'Beginner': return 'badge-primary';
        case 'Intermediate': return 'badge-success';
        case 'Advanced': return 'badge-warning';
        default: return 'badge-default';
    }
}

// init action buttons
function initializeActionButtons() {
    // action buttons removed as requested
}

// init charts
function initializeCharts() {
    // can add chart init logic here
    // like using Chart.js or other chart libraries
    console.log('charts init done');
}

// init activity timeline
function initializeActivityTimeline() {
    const activityList = document.querySelector('.activity-list');
    if (activityList) {
        // add animation effects
        const activityItems = activityList.querySelectorAll('.activity-item');
        activityItems.forEach((item, index) => {
            item.style.animationDelay = `${index * 0.1}s`;
            item.classList.add('fade-in');
        });
    }
}

// view running stats
function viewRunningStats() {
    const runnerId = sessionStorage.getItem('selectedRunner');
    
    if (!runnerId) {
        showNotification('Unable to get runner info', 'error');
        return;
    }
    
    // show stats modal
    showStatsModal(runnerId);
}

// show stats modal
function showStatsModal(runnerId) {
    const modal = document.createElement('div');
    modal.className = 'modal-overlay';
    modal.innerHTML = `
        <div class="modal-content">
            <div class="modal-header">
                <h3 class="modal-title">Running Statistics</h3>
                <button class="modal-close" onclick="closeModal(this)">
                    <i data-lucide="x" class="icon"></i>
                </button>
            </div>
            <div class="modal-body">
                <div class="stats-grid">
                    <div class="stat-item">
                        <div class="stat-value">156</div>
                        <div class="stat-label">Total Runs</div>
                    </div>
                    <div class="stat-item">
                        <div class="stat-value">1,234</div>
                        <div class="stat-label">Total Distance (km)</div>
                    </div>
                    <div class="stat-item">
                        <div class="stat-value">98:45</div>
                        <div class="stat-label">Total Time (hours)</div>
                    </div>
                    <div class="stat-item">
                        <div class="stat-value">5:30</div>
                        <div class="stat-label">Average Pace</div>
                    </div>
                </div>
            </div>
            <div class="modal-footer">
                <button class="btn btn-primary" onclick="closeModal(this)">
                    Close
                </button>
            </div>
        </div>
    `;
    
    // add to page
    document.body.appendChild(modal);
    
    // init icons
    if (typeof lucide !== 'undefined') {
        lucide.createIcons();
    }
    
    // show modal
    setTimeout(() => {
        modal.classList.add('show');
    }, 100);
}

// share runner profile
function shareProfile() {
    const runnerId = sessionStorage.getItem('selectedRunner');
    const runnerName = document.getElementById('profile-name')?.textContent || 'Runner';
    
    if (navigator.share) {
        navigator.share({
            title: `${runnerName} - Running Buddy`,
            text: `View ${runnerName}'s running profile`,
            url: window.location.href
        });
    } else {
        // copy link to clipboard
        navigator.clipboard.writeText(window.location.href).then(() => {
            showNotification('Link copied to clipboard', 'success');
        });
    }
}

// report runner
function reportRunner() {
    const runnerId = sessionStorage.getItem('selectedRunner');
    
    if (!runnerId) {
        showNotification('Unable to get runner info', 'error');
        return;
    }
    
    // show report confirmation
    if (confirm('Are you sure you want to report this runner?')) {
        showNotification('Report submitted, we will handle it soon', 'success');
    }
}

// add to favorites
function addToFavorites() {
    const runnerId = sessionStorage.getItem('selectedRunner');
    
    if (!runnerId) {
        showNotification('Unable to get runner info', 'error');
        return;
    }
    
    // get favorites list
    let favorites = JSON.parse(localStorage.getItem('favorites') || '[]');
    
    if (favorites.includes(runnerId)) {
        showNotification('Already in favorites', 'info');
        return;
    }
    
    // add to favorites
    favorites.push(runnerId);
    localStorage.setItem('favorites', JSON.stringify(favorites));
    
    showNotification('Added to favorites', 'success');
}

// remove from favorites
function removeFromFavorites() {
    const runnerId = sessionStorage.getItem('selectedRunner');
    
    if (!runnerId) {
        showNotification('Unable to get runner info', 'error');
        return;
    }
    
    // get favorites list
    let favorites = JSON.parse(localStorage.getItem('favorites') || '[]');
    
    if (!favorites.includes(runnerId)) {
        showNotification('Not in favorites list', 'info');
        return;
    }
    
    // remove from favorites
    favorites = favorites.filter(id => id !== runnerId);
    localStorage.setItem('favorites', JSON.stringify(favorites));
    
    showNotification('Removed from favorites', 'success');
}

// check if in favorites
function isInFavorites(runnerId) {
    const favorites = JSON.parse(localStorage.getItem('favorites') || '[]');
    return favorites.includes(runnerId);
}

// helper functions for runner data
function getTotalRuns(runnerId) {
    const runsData = {
        '1': '156', '2': '234', '3': '89', '4': '178', 
        '5': '312', '6': '67', '7': '145'
    };
    return runsData[runnerId] || '100';
}

function getTotalDistance(runnerId) {
    const distanceData = {
        '1': '780km', '2': '1170km', '3': '445km', '4': '890km',
        '5': '1560km', '6': '335km', '7': '725km'
    };
    return distanceData[runnerId] || '500km';
}

function getBestPace(runnerId) {
    const paceData = {
        '1': '4:45 min/km', '2': '4:15 min/km', '3': '6:20 min/km', '4': '4:55 min/km',
        '5': '4:05 min/km', '6': '6:45 min/km', '7': '5:20 min/km'
    };
    return paceData[runnerId] || '5:30 min/km';
}

function getRecentActivities(runnerId) {
    const activitiesData = {
        '1': [
            { time: 'Ran 2 days ago', detail: '5km Run' },
            { time: '5 days ago', detail: '3km Run' },
            { time: '1 week ago', detail: '10km Run' }
        ],
        '2': [
            { time: 'Ran 2 hours ago', detail: '8km Run' },
            { time: 'Yesterday', detail: '12km Run' },
            { time: '3 days ago', detail: '6km Run' }
        ],
        '3': [
            { time: 'Ran yesterday', detail: '3km Run' },
            { time: '3 days ago', detail: '2km Run' },
            { time: '1 week ago', detail: '5km Run' }
        ],
        '4': [
            { time: 'Ran this morning', detail: '7km Run' },
            { time: '2 days ago', detail: '4km Run' },
            { time: '4 days ago', detail: '9km Run' }
        ],
        '5': [
            { time: 'Ran this morning', detail: '10km Run' },
            { time: 'Yesterday', detail: '15km Run' },
            { time: '2 days ago', detail: '8km Run' }
        ],
        '6': [
            { time: 'Ran 2 days ago', detail: '2km Run' },
            { time: '4 days ago', detail: '3km Run' },
            { time: '1 week ago', detail: '4km Run' }
        ],
        '7': [
            { time: 'Ran yesterday', detail: '6km Run' },
            { time: '3 days ago', detail: '4km Run' },
            { time: '5 days ago', detail: '8km Run' }
        ]
    };
    return activitiesData[runnerId] || [
        { time: 'Ran recently', detail: '5km Run' },
        { time: 'Last week', detail: '3km Run' }
    ];
}

// request run together - direct to waiting animation
function requestRunTogether() {
    const runnerId = sessionStorage.getItem('selectedRunner');
    const runnerName = document.getElementById('profile-name')?.textContent || 'Runner';
    
    if (!runnerId) {
        showNotification('Unable to get runner info', 'error');
        return;
    }
    
    // show sending animation immediately
    showSendingAnimation(runnerName);
}

// show sending animation
function showSendingAnimation(runnerName) {
    // create sending animation modal
    const modal = document.createElement('div');
    modal.className = 'modal-overlay sending-animation';
    modal.innerHTML = `
        <div class="modal-content sending-content">
            <div class="sending-animation-container">
                <div class="sending-icon">
                    <i data-lucide="send" class="icon"></i>
                </div>
                <div class="sending-text">
                    <h3>Sending Request...</h3>
                    <p>Sending run request to <strong>${runnerName}</strong></p>
                </div>
                <div class="loading-spinner">
                    <div class="spinner"></div>
                </div>
            </div>
        </div>
    `;
    
    // add to page
    document.body.appendChild(modal);
    
    // init icons
    if (typeof lucide !== 'undefined') {
        lucide.createIcons();
    }
    
    // show modal
    setTimeout(() => {
        modal.classList.add('show');
    }, 100);
    
    // simulate sending time (0.5 seconds)
    setTimeout(() => {
        // close sending animation
        modal.classList.remove('show');
        setTimeout(() => {
            modal.remove();
        }, 200);
        
        // show success notification
        showNotification(`Run request sent to ${runnerName}!`, 'success');
        
        // save request info
        const selectedRunnerId = sessionStorage.getItem('selectedRunner');
        sessionStorage.setItem('requestedRunner', selectedRunnerId);
        sessionStorage.setItem('requestedRunnerName', runnerName);
        sessionStorage.setItem('requestStatus', 'waiting');
        
        // show waiting page immediately
        showWaitingPage(runnerName);
        
    }, 500);
}

// ---- 快捷消息清单（可按需改文案）----
const QUICK_MESSAGES = [
  "Hi! Want to run together tomorrow morning?",
  "Nice pace! Join me for 5–8km this weekend?",
  "I usually run around UQ at 6:30am — keen?",
  "Great stats! Fancy an evening 5k today?",
  "Free for a recovery run (5:45–6:00) this week?"
];

// 覆盖：点击底部按钮时，弹出“快捷消息”对话框
function sendMessage() {
  const runnerId = sessionStorage.getItem('selectedRunner');
  const runnerName = document.getElementById('profile-name')?.textContent || 'Runner';
  if (!runnerId) {
    showNotification('Unable to get runner info', 'error');
    return;
  }
  showQuickMessageModal(runnerName);
}

// 新增：快捷消息弹窗
function showQuickMessageModal(runnerName) {
  const modal = document.createElement('div');
  modal.className = 'modal-overlay';
  modal.setAttribute('role', 'dialog');
  modal.setAttribute('aria-labelledby', 'quick-msg-title');

  const quickList = QUICK_MESSAGES.map((m, idx) => `
    <button class="quick-reply" data-msg="${m.replace(/"/g, '&quot;')}">
      <i data-lucide="message-circle" class="icon"></i>
      <span>${m}</span>
    </button>
  `).join('');

  modal.innerHTML = `
    <div class="modal-content">
      <div class="modal-header">
        <h3 id="quick-msg-title" class="modal-title">Quick message to ${runnerName}</h3>
        <button class="modal-close" onclick="closeModal(this)">
          <i data-lucide="x" class="icon"></i>
        </button>
      </div>

      <div class="modal-body">
        <div class="quick-replies">
          ${quickList}
        </div>

        <div class="divider"><span>or</span></div>

        <div class="message-form">
          <textarea class="message-input" placeholder="Write a custom message..." rows="3"></textarea>
          <button class="btn btn-subtle send-custom-btn">Send custom message</button>
        </div>
      </div>

      <div class="modal-footer">
        <button class="btn btn-outline" onclick="closeModal(this)">Cancel</button>
      </div>
    </div>
  `;

  document.body.appendChild(modal);

  // 图标
  if (typeof lucide !== 'undefined') {
    lucide.createIcons();
  }

  // 显示动效
  setTimeout(() => modal.classList.add('show'), 50);

  // 绑定：点快捷消息直接发送并自动关闭
  modal.querySelectorAll('.quick-reply').forEach(btn => {
    btn.addEventListener('click', () => {
      const msg = btn.getAttribute('data-msg') || '';
      sendQuickMessage(msg, btn);
    });
  });

  // 绑定：发送自定义消息
  const customBtn = modal.querySelector('.send-custom-btn');
  const textarea = modal.querySelector('.message-input');
  customBtn.addEventListener('click', () => {
    const msg = (textarea.value || '').trim();
    if (!msg) {
      showNotification('Please enter a message', 'error');
      return;
    }
    sendQuickMessage(msg, customBtn);
  });
}

// 新增：统一的发送逻辑（含“发送中”状态与自动关闭）
function sendQuickMessage(message, triggerEl) {
  // 防重复点击
  const modal = triggerEl.closest('.modal-overlay');
  modal.querySelectorAll('button').forEach(b => b.disabled = true);

  // 简单“发送中”提示
  triggerEl.classList.add('sending');
  triggerEl.innerHTML = `<span>Sending...</span>`;

  // 模拟发送
  setTimeout(() => {
    showNotification('Message sent successfully!', 'success');
    // 关闭弹窗
    modal.classList.remove('show');
    setTimeout(() => modal.remove(), 200);
  }, 500);
}

// send message to runner
function sendMessageToRunner(button) {
    const modal = button.closest('.modal-overlay');
    const messageInput = modal.querySelector('.message-input');
    const message = messageInput.value.trim();
    
    if (!message) {
        showNotification('Please enter a message', 'error');
        return;
    }
    
    // simulate sending message
    showNotification('Message sent successfully!', 'success');
    
    // close modal
    closeModal(button);
}

// show run request confirmation modal
function showRunRequestModal(runnerName) {
    console.log('showRunRequestModal called with:', runnerName);
    const modal = document.createElement('div');
    modal.className = 'modal-overlay';
    modal.innerHTML = `
        <div class="modal-content">
            <div class="modal-header">
                <h3 class="modal-title">Request Run Together</h3>
                <button class="modal-close" onclick="closeModal(this)">
                    <i data-lucide="x" class="icon"></i>
                </button>
            </div>
            <div class="modal-body">
                <div class="request-info">
                    <div class="request-icon">
                        <i data-lucide="users" class="icon"></i>
                    </div>
                    <h4 class="request-title">Send Run Request</h4>
                    <p class="request-description">Send a run request to <strong>${runnerName}</strong>?</p>
                    <div class="request-details">
                        <div class="detail-item">
                            <i data-lucide="bell" class="icon"></i>
                            <span>They'll get notified</span>
                        </div>
                        <div class="detail-item">
                            <i data-lucide="calendar" class="icon"></i>
                            <span>Schedule after acceptance</span>
                        </div>
                    </div>
                </div>
            </div>
            <div class="modal-footer">
                <button class="btn btn-primary" onclick="confirmRunRequest(this)" data-runner-name="${runnerName}">
                    <i data-lucide="send" class="icon"></i>
                    Send Request
                </button>
                <button class="btn btn-outline" onclick="closeModal(this)">
                    Cancel
                </button>
            </div>
        </div>
    `;
    
    // add to page
    document.body.appendChild(modal);
    
    // init icons
    if (typeof lucide !== 'undefined') {
        lucide.createIcons();
    }
    
    // show modal
    setTimeout(() => {
        modal.classList.add('show');
    }, 100);
}

// confirm run request (simplified - no modal needed)
function confirmRunRequest(button) {
    const runnerName = button.getAttribute('data-runner-name');
    console.log('Runner name:', runnerName);
    
    if (!runnerName) {
        console.error('No runner name found');
        return;
    }
    
    // simulate sending request
    showNotification(`Run request sent to ${runnerName}!`, 'success');
    
    // close modal
    closeModal(button);
    
    // redirect to waiting page after a short delay
    setTimeout(() => {
        // save request info for waiting page
        const selectedRunnerId = sessionStorage.getItem('selectedRunner');
        sessionStorage.setItem('requestedRunner', selectedRunnerId);
        sessionStorage.setItem('requestedRunnerName', runnerName);
        sessionStorage.setItem('requestStatus', 'waiting');
        
        // create and show waiting page
        showWaitingPage(runnerName);
    }, 300);
}

// show waiting page
function showWaitingPage(runnerName) {
    // create waiting page modal
    const modal = document.createElement('div');
    modal.className = 'modal-overlay waiting-page';
    modal.innerHTML = `
        <div class="modal-content waiting-content">
            <div class="waiting-header">
                <div class="waiting-icon">
                    <i data-lucide="clock" class="icon"></i>
                </div>
                <h2 class="waiting-title">Waiting for Response</h2>
                <p class="waiting-description">Your run request has been sent to <strong>${runnerName}</strong></p>
            </div>
            
            <div class="waiting-body">
                <div class="waiting-status">
                    <div class="status-item">
                        <i data-lucide="send" class="icon sent"></i>
                        <span>Request Sent</span>
                    </div>
                    <div class="status-item pending">
                        <i data-lucide="clock" class="icon"></i>
                        <span>Waiting for Response</span>
                    </div>
                    <div class="status-item pending">
                        <i data-lucide="calendar" class="icon"></i>
                        <span>Schedule Run</span>
                    </div>
                </div>
                
                <div class="waiting-actions">
                    <button class="btn btn-outline" onclick="cancelRequest()">
                        <i data-lucide="x" class="icon"></i>
                        Cancel
                    </button>
                    <button class="btn btn-primary" onclick="goToMap()">
                        <i data-lucide="map" class="icon"></i>
                        Go to Map
                    </button>
                </div>
            </div>
        </div>
    `;
    
    // add to page
    document.body.appendChild(modal);
    
    // init icons
    if (typeof lucide !== 'undefined') {
        lucide.createIcons();
    }
    
    // show modal
    setTimeout(() => {
        modal.classList.add('show');
    }, 100);
    
    // auto redirect to schedule after 2 seconds (give user time to read)
    setTimeout(() => {
        // close waiting modal
        modal.classList.remove('show');
        setTimeout(() => {
            modal.remove();
        }, 300);
        
        // set matched runner data for match page
        const requestedRunnerId = sessionStorage.getItem('requestedRunner');
        if (requestedRunnerId) {
            sessionStorage.setItem('matchedRunner', requestedRunnerId);
        }
        
        // redirect to schedule page
        navigateToPage('schedule');
    }, 2000);
}

// go to map
function goToMap() {
    // close waiting modal
    const waitingModal = document.querySelector('.waiting-page');
    if (waitingModal) {
        waitingModal.classList.remove('show');
        setTimeout(() => {
            waitingModal.remove();
        }, 300);
    }
    
    // redirect to map (index page)
    setTimeout(() => {
        navigateToPage('index');
    }, 300);
}

// cancel request
function cancelRequest() {
    // clear request data
    sessionStorage.removeItem('requestedRunner');
    sessionStorage.removeItem('requestedRunnerName');
    sessionStorage.removeItem('requestStatus');
    
    // show cancellation notification
    showNotification('Run request cancelled', 'info');
    
    // close waiting modal
    const waitingModal = document.querySelector('.waiting-page');
    if (waitingModal) {
        waitingModal.classList.remove('show');
        setTimeout(() => {
            waitingModal.remove();
        }, 300);
    }
}

// 关闭模态框（profile 专用）
function closeModal(button) {
    const modal = button.closest('.modal-overlay');
    if (modal) {
        modal.classList.remove('show');
        setTimeout(() => {
            modal.remove();
        }, 300);
    }
}

// 渲染“待开始的匹配伙伴”
function renderPendingMatchIfAny() {
    const selectedRunnerId = sessionStorage.getItem('selectedRunner');
    const isCurrentUser = !selectedRunnerId || selectedRunnerId === 'lanbell';
    if (!isCurrentUser) return;

    const matchedRunnerId = sessionStorage.getItem('matchedRunner');
    const requestedRunnerId = sessionStorage.getItem('requestedRunner');
    const partnerId = matchedRunnerId || requestedRunnerId;
    if (!partnerId) return;

    const runners = getRunnersData();
    const partner = runners.find(r => r.id === partnerId);
    if (!partner) return;

    const statusText = matchedRunnerId ? 'Matched · Not started' : 'Request sent · Waiting';

    const section = document.createElement('section');
    section.className = 'pending-match-section';
    section.innerHTML = `
        <h3 class="section-title">Matched Partner</h3>
        <div class="profile-card">
            <div class="profile-header">
                <div class="profile-avatar"><img src="${partner.avatar}" alt="${partner.name}" class="avatar-img"/></div>
                <div class="profile-info">
                    <h2 class="profile-name">${partner.name}</h2>
                    <div class="profile-level"><span class="level-badge badge ${getLevelClass(partner.level)}">${partner.level} Runner</span></div>
                    <div class="profile-location"><i data-lucide="clock" class="icon"></i><span>${statusText}</span></div>
                </div>
            </div>
            <div class="action-section">
                <button class="action-btn btn btn-primary" onclick="startRunFromPending()">
                    <i data-lucide="play" class="icon"></i>
                    Start Run
                </button>
                <button class="action-btn btn btn-outline" onclick="cancelPendingMatch()">
                    <i data-lucide="x" class="icon"></i>
                    Cancel Match
                </button>
            </div>
        </div>
    `;

    // 插入到用户简介（.profile-card）之后
    const anchor = document.querySelector('.main-content .profile-card');
    if (anchor && anchor.parentElement) {
        anchor.parentElement.insertBefore(section, anchor.nextSibling);
        if (typeof lucide !== 'undefined') { lucide.createIcons(); }
    }
}

function startRunFromPending() {
    const runnerId = sessionStorage.getItem('matchedRunner') || sessionStorage.getItem('requestedRunner');
    if (!runnerId) { showNotification('No pending partner found', 'error'); return; }
    sessionStorage.setItem('selectedRunner', runnerId);
    // 确保 shared-goal 能读取 matchedRunner
    sessionStorage.setItem('matchedRunner', runnerId);
    navigateToPage('shared-goal');
}

function cancelPendingMatch() {
    const partnerId = sessionStorage.getItem('matchedRunner') || sessionStorage.getItem('requestedRunner');
    const runners = getRunnersData();
    const partner = runners.find(r => r.id === partnerId);
    const name = partner?.name || 'this partner';

    const modal = document.createElement('div');
    modal.className = 'modal-overlay';
    modal.innerHTML = `
        <div class="modal-content">
            <div class="modal-header">
                <h3 class="modal-title">Cancel Match?</h3>
                <button class="modal-close" onclick="closeModal(this)"><i data-lucide="x" class="icon"></i></button>
            </div>
            <div class="modal-body">
                <p>Are you sure you want to cancel the match with <strong>${name}</strong>?</p>
            </div>
            <div class="modal-footer">
                <button class="btn btn-outline" onclick="closeModal(this)">Keep</button>
                <button class="btn btn-primary" onclick="confirmCancelPending(this)">Cancel Match</button>
            </div>
        </div>
    `;
    document.body.appendChild(modal);
    if (typeof lucide !== 'undefined') { lucide.createIcons(); }
    setTimeout(() => modal.classList.add('show'), 50);
}

function confirmCancelPending(button) {
    closeModal(button);
    sessionStorage.removeItem('matchedRunner');
    sessionStorage.removeItem('requestedRunner');
    sessionStorage.removeItem('requestedRunnerName');
    const section = document.querySelector('.pending-match-section');
    if (section && section.parentElement) section.parentElement.removeChild(section);
    showNotification('Match cancelled', 'info');
}

function hideActionsForCurrentUser() {
    const selectedRunnerId = sessionStorage.getItem('selectedRunner');
    const isCurrentUser = !selectedRunnerId || selectedRunnerId === 'lanbell';
    if (!isCurrentUser) return;
    document.querySelectorAll('.main-content > .action-section').forEach(el => el.remove());
}

// export global functions
window.sendMessage = sendMessage;
window.requestRunTogether = requestRunTogether;
window.viewRunningStats = viewRunningStats;
window.shareProfile = shareProfile;
window.reportRunner = reportRunner;
window.addToFavorites = addToFavorites;
window.removeFromFavorites = removeFromFavorites;
window.closeModal = closeModal;
window.sendMessageToRunner = sendMessageToRunner;
window.confirmRunRequest = confirmRunRequest;
window.goToMap = goToMap;
window.cancelRequest = cancelRequest;
window.startRunFromPending = startRunFromPending;
window.cancelPendingMatch = cancelPendingMatch;
window.confirmCancelPending = confirmCancelPending;
window.hideActionsForCurrentUser = hideActionsForCurrentUser;