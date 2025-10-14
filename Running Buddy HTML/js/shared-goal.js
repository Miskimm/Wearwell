// Shared Goal Page JavaScript - 共同目标页面逻辑

// page init
document.addEventListener('DOMContentLoaded', function() {
    initializeSharedGoalPage();
});

// init shared goal page
function initializeSharedGoalPage() {
    // load buddy data
    loadBuddyData();
    
    // init activity stats
    initializeActivityStats();
    
    // init goal progress
    initializeGoalProgress();
    
    // init challenge
    initializeChallenge();
    
    // init action buttons
    initializeActionButtons();
}

// load buddy data
function loadBuddyData() {
    const matchedRunnerId = sessionStorage.getItem('matchedRunner');
    
    if (!matchedRunnerId) {
        console.log('No matched runner found, using default data');
        return;
    }
    
    const runners = getRunnersData();
    const runner = runners.find(r => r.id === matchedRunnerId);
    
    if (runner) {
        updateBuddyInfo(runner);
    }
}

// update buddy info
function updateBuddyInfo(runner) {
    // update buddy avatar
    const buddyAvatar = document.getElementById('buddy-avatar');
    if (buddyAvatar) {
        buddyAvatar.src = runner.avatar;
        buddyAvatar.alt = runner.name;
    }
    
    // update buddy name
    const buddyName = document.getElementById('buddy-name');
    if (buddyName) {
        buddyName.textContent = runner.name;
    }
    
    // update buddy pace
    const buddyPace = document.getElementById('buddy-pace');
    if (buddyPace) {
        buddyPace.textContent = runner.pace;
    }
    
    // update buddy frequency
    const buddyFrequency = document.getElementById('buddy-frequency');
    if (buddyFrequency) {
        buddyFrequency.textContent = `${runner.runsPerWeek} runs/week`;
    }
}

// initialize activity stats
function initializeActivityStats() {
    // generate random activity data
    const calories = Math.floor(Math.random() * 2000) + 1000; // 1000-3000 kcal
    const distance = Math.floor(Math.random() * 5000) + 2000; // 2000-7000m
    
    // update calories burned
    const caloriesElement = document.getElementById('calories-burned');
    if (caloriesElement) {
        caloriesElement.textContent = calories;
    }
    
    // update distance covered
    const distanceElement = document.getElementById('distance-covered');
    if (distanceElement) {
        distanceElement.textContent = distance;
    }
}

// initialize goal progress
function initializeGoalProgress() {
    // generate random progress data
    const progress = Math.floor(Math.random() * 40) + 40; // 40-80%
    const target = Math.floor(Math.random() * 1000) + 2000; // 2000-3000 kcal
    
    // update progress bar
    const progressFill = document.querySelector('.progress-fill');
    if (progressFill) {
        progressFill.style.width = `${progress}%`;
    }
    
    // update progress percentage
    const progressPercentage = document.querySelector('.progress-percentage');
    if (progressPercentage) {
        progressPercentage.textContent = `${progress}%`;
    }
    
    // update goal target
    const goalTarget = document.getElementById('goal-target');
    if (goalTarget) {
        goalTarget.textContent = target;
    }
}

// initialize challenge
function initializeChallenge() {
    // challenge is already set in HTML, no dynamic changes needed
    console.log('Challenge initialized');
}

// initialize action buttons
function initializeActionButtons() {
    // buttons are already set up in HTML with onclick handlers
    console.log('Action buttons initialized');
}

// end run function
function endRun() {
    console.log('End Run clicked');
    
    // show confirmation modal
    const modal = document.createElement('div');
    modal.className = 'modal-overlay';
    modal.innerHTML = `
        <div class="modal-content">
            <div class="modal-header">
                <div class="warning-icon">
                    <i data-lucide="alert-triangle" class="icon"></i>
                </div>
                <h3>End Run?</h3>
                <p>Are you sure you want to end this run session?</p>
            </div>
            <div class="modal-footer">
                <button class="btn btn-primary" onclick="confirmEndRun()">
                    <i data-lucide="check" class="icon"></i>
                    Yes, End Run
                </button>
                <button class="btn btn-outline" onclick="closeModal(this)">
                    <i data-lucide="x" class="icon"></i>
                    Cancel
                </button>
            </div>
        </div>
    `;
    
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

// confirm end run
function confirmEndRun() {
    console.log('Run ended');
    
    // close modal
    const modal = document.querySelector('.modal-overlay');
    if (modal) {
        modal.classList.remove('show');
        setTimeout(() => {
            modal.remove();
        }, 300);
    }
    
    // show completion notification
    showNotification('Run session ended! Great job!', 'success');
    
    // redirect to map after delay
    setTimeout(() => {
        navigateToPage('index');
    }, 2000);
}

// go to map
function goToMap() {
    console.log('Go to Map clicked');
    navigateToPage('index');
}

// go back
function goBack() {
    if (window.history.length > 1) {
        window.history.back();
    } else {
        navigateToPage('match');
    }
}

// close modal
function closeModal(button) {
    const modal = button.closest('.modal-overlay');
    if (modal) {
        modal.classList.remove('show');
        setTimeout(() => {
            modal.remove();
        }, 300);
    }
}

// export global functions
window.endRun = endRun;
window.confirmEndRun = confirmEndRun;
window.goToMap = goToMap;
window.goBack = goBack;
window.closeModal = closeModal;
