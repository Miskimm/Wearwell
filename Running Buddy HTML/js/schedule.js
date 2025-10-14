// Schedule.js - schedule run page logic

// page init
document.addEventListener('DOMContentLoaded', function() {
    initializeSchedulePage();
});

// init schedule page
function initializeSchedulePage() {
    // load runner data
    loadRunnerData();
    
    // init form
    initializeForm();
    
    // init action buttons
    initializeActionButtons();
}

// load runner data
function loadRunnerData() {
    const runnerId = sessionStorage.getItem('selectedRunner') || sessionStorage.getItem('matchedRunner');
    
    if (!runnerId) {
        showError(document.querySelector('.main-content'), 'Runner data not found');
        return;
    }
    
    const runners = getRunnersData();
    const runner = runners.find(r => r.id === runnerId);
    
    if (runner) {
        updateRunnerInfo(runner);
    } else {
        showError(document.querySelector('.main-content'), 'Runner data not found');
    }
}

// update runner info
function updateRunnerInfo(runner) {
    // update runner avatar
    const avatarImg = document.querySelector('.partner-avatar .avatar-img');
    if (avatarImg) {
        avatarImg.src = runner.avatar;
        avatarImg.alt = runner.name;
    }
    
    // update runner name
    const nameElement = document.querySelector('.partner-name');
    if (nameElement) {
        nameElement.textContent = runner.name;
    }
    
    // update runner level
    const levelElement = document.querySelector('.partner-level');
    if (levelElement) {
        const levelText = getLevelText(runner.level);
        levelElement.textContent = levelText;
    }
    
    // update runner stats
    const statElement = document.querySelector('.stat');
    if (statElement) {
        statElement.textContent = `Pace: ${runner.pace}`;
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

// init form
function initializeForm() {
    // init date selection
    initializeDateSelection();
    
    // init time selection
    initializeTimeSelection();
    
    // init distance selection
    initializeDistanceSelection();
    
    // init route selection
    initializeRouteSelection();
}

// init date selection
function initializeDateSelection() {
    const dateOptions = document.querySelectorAll('.date-option');
    
    dateOptions.forEach(option => {
        option.addEventListener('click', function() {
            // update selected state
            dateOptions.forEach(opt => opt.classList.remove('active'));
            this.classList.add('active');
            
            // update form data
            updateFormData('date', this.getAttribute('data-date'));
        });
    });
}

// init time selection
function initializeTimeSelection() {
    const timeOptions = document.querySelectorAll('.time-option');
    
    timeOptions.forEach(option => {
        option.addEventListener('click', function() {
            // update selected state
            timeOptions.forEach(opt => opt.classList.remove('active'));
            this.classList.add('active');
            
            // update form data
            updateFormData('time', this.getAttribute('data-time'));
        });
    });
}

// init distance selection
function initializeDistanceSelection() {
    const distanceOptions = document.querySelectorAll('.distance-option');
    
    distanceOptions.forEach(option => {
        option.addEventListener('click', function() {
            // update selected state
            distanceOptions.forEach(opt => opt.classList.remove('active'));
            this.classList.add('active');
            
            // update form data
            updateFormData('distance', this.getAttribute('data-distance'));
        });
    });
}

// init route selection
function initializeRouteSelection() {
    const routeOptions = document.querySelectorAll('.route-option');
    
    routeOptions.forEach(option => {
        option.addEventListener('click', function() {
            // update selected state
            routeOptions.forEach(opt => opt.classList.remove('active'));
            this.classList.add('active');
            
            // update form data
            updateFormData('route', this.getAttribute('data-route'));
        });
    });
}

// init action buttons
function initializeActionButtons() {
    // submit button
    const submitBtn = document.querySelector('.action-btn[onclick*="submitSchedule"]');
    if (submitBtn) {
        submitBtn.addEventListener('click', function() {
            submitSchedule();
        });
    }
    
    // cancel button
    const cancelBtn = document.querySelector('.action-btn[onclick*="goBack"]');
    if (cancelBtn) {
        cancelBtn.addEventListener('click', function() {
            goBack();
        });
    }
}

// update form data
function updateFormData(field, value) {
    // save to sessionStorage
    const formData = JSON.parse(sessionStorage.getItem('scheduleForm') || '{}');
    formData[field] = value;
    sessionStorage.setItem('scheduleForm', JSON.stringify(formData));
}

// get form data
function getFormData() {
    return JSON.parse(sessionStorage.getItem('scheduleForm') || '{}');
}

// get current form data from UI
function getCurrentFormData() {
    const formData = {
        date: 'Today', // default date
        time: 'Evening', // default time
        distance: '5km', // default distance
        route: 'Central Park' // default route
    };
    
    // get selected time
    const selectedTime = document.querySelector('.time-tab.active');
    if (selectedTime) {
        formData.time = selectedTime.textContent.trim();
    }
    
    // get selected distance
    const selectedDistance = document.querySelector('.distance-option.selected');
    if (selectedDistance) {
        formData.distance = selectedDistance.querySelector('.option-label').textContent.trim();
    }
    
    // get selected route
    const selectedRoute = document.querySelector('.route-option.selected');
    if (selectedRoute) {
        formData.route = selectedRoute.querySelector('.option-label').textContent.trim();
    }
    
    return formData;
}

// submit schedule
function submitSchedule() {
    console.log('submitSchedule called');
    
    // get current form data from UI
    const formData = getCurrentFormData();
    const notes = document.querySelector('.form-textarea')?.value || '';
    
    console.log('Form data:', formData);
    
    // validate form
    if (!validateForm(formData)) {
        console.log('Form validation failed');
        return;
    }
    
    // show loading state
    showNotification('Sending invitation...', 'info');
    
    // simulate API call
    setTimeout(() => {
        // save schedule data
        const scheduleData = {
            ...formData,
            notes: notes,
            timestamp: new Date().toISOString(),
            status: 'pending'
        };
        
        sessionStorage.setItem('scheduleData', JSON.stringify(scheduleData));
        
        // show success message
        showNotification('Invitation sent!', 'success');
        
        // show confirmation modal with Go to Map option
        setTimeout(() => {
            showScheduleConfirmation();
        }, 1000);
    }, 1500);
}

// show schedule confirmation modal
function showScheduleConfirmation() {
    const modal = document.createElement('div');
    modal.className = 'modal-overlay';
    modal.innerHTML = `
        <div class="modal-content">
            <div class="modal-header">
                <div class="success-icon">
                    <i data-lucide="check-circle" class="icon"></i>
                </div>
                <h3>Schedule Confirmed!</h3>
                <p>Your running invitation has been sent successfully.</p>
            </div>
            <div class="modal-body">
                <div class="confirmation-details">
                    <div class="detail-item">
                        <i data-lucide="calendar" class="icon"></i>
                        <span>Schedule saved to your calendar</span>
                    </div>
                    <div class="detail-item">
                        <i data-lucide="bell" class="icon"></i>
                        <span>You'll be notified when they respond</span>
                    </div>
                </div>
            </div>
            <div class="modal-footer">
                <button class="btn btn-primary" onclick="goToMapFromSchedule()">
                    <i data-lucide="map" class="icon"></i>
                    Go to Map
                </button>
                <button class="btn btn-outline" onclick="closeModal(this)">
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

// go to map from schedule
function goToMapFromSchedule() {
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

// validate form
function validateForm(formData) {
    console.log('Validating form data:', formData);
    
    // since we have defaults, always return true for now
    // in a real app, you might want to validate specific fields
    return true;
}

// get field label
function getFieldLabel(field) {
    const labels = {
        'date': 'date',
        'time': 'time',
        'distance': 'distance',
        'route': 'route'
    };
    
    return labels[field] || field;
}

// preview schedule
function previewSchedule() {
    const formData = getFormData();
    const notes = document.querySelector('.form-textarea')?.value || '';
    
    if (!validateForm(formData)) {
        return;
    }
    
    // show preview modal
    showPreviewModal(formData, notes);
}

// show preview modal
function showPreviewModal(formData, notes) {
    const modal = document.createElement('div');
    modal.className = 'modal-overlay';
    modal.innerHTML = `
        <div class="modal-content">
            <div class="modal-header">
                <h3 class="modal-title">Run Schedule Preview</h3>
                <button class="modal-close" onclick="closeModal(this)">
                    <i data-lucide="x" class="icon"></i>
                </button>
            </div>
            <div class="modal-body">
                <div class="schedule-preview">
                    <div class="preview-item">
                        <span class="label">Date:</span>
                        <span class="value">${getDateText(formData.date)}</span>
                    </div>
                    <div class="preview-item">
                        <span class="label">Time:</span>
                        <span class="value">${formData.time}</span>
                    </div>
                    <div class="preview-item">
                        <span class="label">Distance:</span>
                        <span class="value">${formData.distance}km</span>
                    </div>
                    <div class="preview-item">
                        <span class="label">Route:</span>
                        <span class="value">${getRouteText(formData.route)}</span>
                    </div>
                    ${notes ? `
                    <div class="preview-item">
                        <span class="label">Notes:</span>
                        <span class="value">${notes}</span>
                    </div>
                    ` : ''}
                </div>
            </div>
            <div class="modal-footer">
                <button class="btn btn-primary" onclick="submitSchedule(); closeModal(this)">
                    Confirm Send
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

// get date text
function getDateText(date) {
    const dateTexts = {
        'today': 'Today',
        'tomorrow': 'Tomorrow',
        'day-after': 'Day After Tomorrow'
    };
    
    return dateTexts[date] || date;
}

// get route text
function getRouteText(route) {
    const routeTexts = {
        'park': 'Central Park',
        'track': 'Stadium Track',
        'street': 'Street Running'
    };
    
    return routeTexts[route] || route;
}

// clear form
function clearForm() {
    // clear all selections
    document.querySelectorAll('.date-option, .time-option, .distance-option, .route-option').forEach(option => {
        option.classList.remove('active');
    });
    
    // clear textarea
    const textarea = document.querySelector('.form-textarea');
    if (textarea) {
        textarea.value = '';
    }
    
    // clear sessionStorage
    sessionStorage.removeItem('scheduleForm');
    
    showNotification('Form cleared', 'info');
}

// export global functions
window.submitSchedule = submitSchedule;
window.previewSchedule = previewSchedule;
window.clearForm = clearForm;
window.closeModal = closeModal;
window.goToMapFromSchedule = goToMapFromSchedule;
