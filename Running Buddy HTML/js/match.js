// Match.js - match confirmation page logic

// page init
document.addEventListener('DOMContentLoaded', function() {
    initializeMatchPage();
});

// init match page
function initializeMatchPage() {
    // load match data
    loadMatchData();
    
    // init action buttons
    initializeActionButtons();
    
    // init match details
    initializeMatchDetails();
}

// load match data
function loadMatchData() {
    // try to get matchedRunner first, then fallback to requestedRunner
    let matchedRunnerId = sessionStorage.getItem('matchedRunner');
    if (!matchedRunnerId) {
        matchedRunnerId = sessionStorage.getItem('requestedRunner');
    }
    
    if (!matchedRunnerId) {
        showError(document.querySelector('.main-content'), 'Match data not found');
        return;
    }
    
    const runners = getRunnersData();
    const runner = runners.find(r => r.id === matchedRunnerId);
    
    if (runner) {
        updateMatchInfo(runner);
        // no modal needed - user can click Start Running! button directly
    } else {
        showError(document.querySelector('.main-content'), 'Runner data not found');
    }
}

// update match info
function updateMatchInfo(runner) {
    // update runner avatar
    const avatarImg = document.querySelector('.runner-avatar .avatar-img');
    if (avatarImg) {
        avatarImg.src = runner.avatar;
        avatarImg.alt = runner.name;
    }
    
    // update runner name
    const nameElement = document.querySelector('.runner-name');
    if (nameElement) {
        nameElement.textContent = runner.name;
    }
    
    // update runner level
    const levelElement = document.querySelector('.runner-level');
    if (levelElement) {
        const levelText = getLevelText(runner.level);
        levelElement.textContent = levelText;
    }
    
    // update runner stats
    const statsElements = document.querySelectorAll('.stat');
    if (statsElements.length >= 2) {
        statsElements[0].textContent = `Pace: ${runner.pace}`;
        statsElements[1].textContent = `Frequency: ${runner.runsPerWeek} runs/week`;
    }
}

// get level text
function getLevelText(level) {
    switch (level) {
        case 'Beginner': return 'Beginner Runner';
        case 'Intermediate': return 'Intermediate Runner';
        case 'Advanced': return 'Advanced Runner';
        default: return level;
    }
}

// init action buttons
function initializeActionButtons() {
    // schedule run button
    const scheduleBtn = document.querySelector('.action-btn[onclick*="scheduleRun"]');
    if (scheduleBtn) {
        scheduleBtn.addEventListener('click', function() {
            scheduleRun();
        });
    }
    
}

// init match details
function initializeMatchDetails() {
    // update match time
    const timeElement = document.querySelector('.detail-value');
    if (timeElement) {
        timeElement.textContent = 'Just now';
    }
    
    // update distance
    const distanceElements = document.querySelectorAll('.detail-value');
    if (distanceElements.length >= 2) {
        const matchedRunnerId = sessionStorage.getItem('matchedRunner');
        const runners = getRunnersData();
        const runner = runners.find(r => r.id === matchedRunnerId);
        
        if (runner) {
            distanceElements[1].textContent = runner.distance;
        }
    }
}

// schedule run
function scheduleRun() {
    const matchedRunnerId = sessionStorage.getItem('matchedRunner');
    
    if (!matchedRunnerId) {
        showNotification('Unable to get match info', 'error');
        return;
    }
    
    // save current matched runner ID
    sessionStorage.setItem('selectedRunner', matchedRunnerId);
    
    // navigate to schedule page
    navigateToPage('schedule');
}


// view runner profile
function viewRunnerProfile() {
    const matchedRunnerId = sessionStorage.getItem('matchedRunner');
    
    if (!matchedRunnerId) {
        showNotification('Unable to get match info', 'error');
        return;
    }
    
    // save current runner ID
    sessionStorage.setItem('selectedRunner', matchedRunnerId);
    
    // navigate to profile page
    navigateToPage('profile');
}

// cancel match
function cancelMatch() {
    if (confirm('Are you sure you want to cancel this match?')) {
        // clear match data
        sessionStorage.removeItem('matchedRunner');
        
        // show cancel message
        showNotification('Match cancelled', 'info');
        
        // return to home page
        setTimeout(() => {
            navigateToPage('index');
        }, 1000);
    }
}

// share match
function shareMatch() {
    const matchedRunnerId = sessionStorage.getItem('matchedRunner');
    const runners = getRunnersData();
    const runner = runners.find(r => r.id === matchedRunnerId);
    
    if (!runner) {
        showNotification('Unable to get match info', 'error');
        return;
    }
    
    if (navigator.share) {
        navigator.share({
            title: `Successfully matched with ${runner.name}!`,
            text: `Found the perfect running partner on Running Buddy`,
            url: window.location.href
        });
    } else {
        // copy link to clipboard
        navigator.clipboard.writeText(window.location.href).then(() => {
            showNotification('Link copied to clipboard', 'success');
        });
    }
}

// view match history
function viewMatchHistory() {
    // show match history modal
    showMatchHistoryModal();
}

// show match history modal
function showMatchHistoryModal() {
    const modal = document.createElement('div');
    modal.className = 'modal-overlay';
    modal.innerHTML = `
        <div class="modal-content">
            <div class="modal-header">
                <h3 class="modal-title">Match History</h3>
                <button class="modal-close" onclick="closeModal(this)">
                    <i data-lucide="x" class="icon"></i>
                </button>
            </div>
            <div class="modal-body">
                <div class="match-history">
                    <div class="history-item">
                        <div class="history-date">Today</div>
                        <div class="history-content">
                            <div class="history-title">Matched with Sarah Chen</div>
                            <div class="history-status">Confirmed</div>
                        </div>
                    </div>
                    <div class="history-item">
                        <div class="history-date">Yesterday</div>
                        <div class="history-content">
                            <div class="history-title">Matched with Mike Rodriguez</div>
                            <div class="history-status">Cancelled</div>
                        </div>
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

// show match success modal
function showMatchSuccessModal(runner) {
    const modal = document.createElement('div');
    modal.className = 'modal-overlay';
    modal.innerHTML = `
        <div class="modal-content">
            <div class="modal-header">
                <div class="success-icon">
                    <i data-lucide="check-circle" class="icon"></i>
                </div>
                <h3>Match Successful!</h3>
                <p>You've successfully matched with <strong>${runner.name}</strong>!</p>
            </div>
            <div class="modal-body">
                <div class="confirmation-details">
                    <div class="detail-item">
                        <i data-lucide="calendar" class="icon"></i>
                        <span>Run scheduled and saved to calendar</span>
                    </div>
                    <div class="detail-item">
                        <i data-lucide="bell" class="icon"></i>
                        <span>You'll be notified before the run</span>
                    </div>
                </div>
            </div>
            <div class="modal-footer">
                <button class="btn btn-primary" onclick="startRunningFromMatch()">
                    <i data-lucide="play" class="icon"></i>
                    Start Running!
                </button>
                <button class="btn btn-outline" onclick="goToMapFromMatch()">
                    <i data-lucide="map" class="icon"></i>
                    Go to Map
                </button>
                <button class="btn btn-ghost" onclick="closeModal(this)">
                    <i data-lucide="x" class="icon"></i>
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

// start running from match
function startRunningFromMatch() {
    console.log('Start Running! from match clicked');
    
    // redirect to shared goal page
    navigateToPage('shared-goal');
}

// go to map from match
function goToMapFromMatch() {
    console.log('Go to Map from match clicked');
    
    // close modal
    const modal = document.querySelector('.modal-overlay');
    if (modal) {
        modal.classList.remove('show');
        setTimeout(() => {
            modal.remove();
        }, 300);
    }
    
    // redirect to map (index page)
    setTimeout(() => {
        navigateToPage('index');
    }, 300);
}

// export global functions
window.scheduleRun = scheduleRun;
window.sendMessage = sendMessage;
window.viewRunnerProfile = viewRunnerProfile;
window.cancelMatch = cancelMatch;
window.shareMatch = shareMatch;
window.viewMatchHistory = viewMatchHistory;
window.closeModal = closeModal;
window.startRunningFromMatch = startRunningFromMatch;
window.goToMapFromMatch = goToMapFromMatch;
