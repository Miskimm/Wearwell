// Shared Goal Page JavaScript - 共同目标页面逻辑

// page init
document.addEventListener('DOMContentLoaded', function() {
    console.log('DOM Content Loaded - Starting initialization');
    initializeSharedGoalPage();
    initializeInlineGoalSetting();
    
    // 添加滑杆事件监听器的备用初始化
    setTimeout(() => {
        setupSliderEventListeners();
    }, 100);
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

// 当前活动数据（用于和下方进度同步）
let currentActivityData = { calories: 0, distance: 0, steps: 0 };

// initialize activity stats
function initializeActivityStats() {
    // generate random activity data
    const calories = Math.floor(Math.random() * 2000) + 1000; // 1000-3000 kcal
    const distance = Math.floor(Math.random() * 5000) + 2000; // 2000-7000m
    const steps = Math.floor(Math.random() * 8000) + 5000;   // 5000-13000 steps

    // 保存到全局，供下方进度计算使用
    currentActivityData = { calories, distance, steps };
    
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

    // update steps count
    const stepsElement = document.getElementById('steps-count');
    if (stepsElement) {
        stepsElement.textContent = steps;
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
    // 跑步进行中禁止返回上一页，只允许“End Run”或“Go to Map”
    navigateToPage('index');
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

// initialize inline goal setting
function initializeInlineGoalSetting() {
    console.log('initializeInlineGoalSetting called');
    const goalTypeSelect = document.getElementById('goal-type-select');
    const inlineSlider = document.getElementById('inline-goal-slider');
    
    console.log('Elements found:', {
        goalTypeSelect: !!goalTypeSelect,
        inlineSlider: !!inlineSlider
    });
    
    if (!goalTypeSelect || !inlineSlider) {
        console.log('Missing elements - cannot initialize');
        return;
    }
    
    // load saved goal settings
    const savedGoals = loadGoalSettings();
    const goalType = savedGoals.type || 'calories';
    
    // set initial goal type
    goalTypeSelect.value = goalType;
    updateSliderForGoalType(goalType, savedGoals);
    
    // add event listeners
    goalTypeSelect.addEventListener('change', function() {
        const selectedType = this.value;
        updateSliderForGoalType(selectedType, savedGoals);
        autoSaveGoalSettings();
    });
    
    // initialize display
    updateInlineGoalDisplay(inlineSlider.value);
    updateSliderFill(inlineSlider.value);
}

// update slider for goal type
function updateSliderForGoalType(goalType, savedGoals) {
    const slider = document.getElementById('inline-goal-slider');
    const sliderMin = document.getElementById('slider-min');
    const sliderMax = document.getElementById('slider-max');
    const goalUnit = document.getElementById('current-goal-unit');
    
    if (!slider || !sliderMin || !sliderMax || !goalUnit) return;
    
    const config = getGoalTypeConfig(goalType);
    const currentValue = savedGoals.value || config.defaultValue;
    
    // update slider attributes
    slider.min = config.min;
    slider.max = config.max;
    slider.step = config.step;
    slider.value = currentValue;
    
    // update display
    sliderMin.textContent = config.min;
    sliderMax.textContent = config.max;
    goalUnit.textContent = config.unit;
    
    // update goal display
    updateInlineGoalDisplay(currentValue);
}

// update inline goal display
function updateInlineGoalDisplay(value) {
    console.log('updateInlineGoalDisplay called with value:', value);
    
    const goalValue = document.getElementById('current-goal-value');
    const goalUnit = document.getElementById('current-goal-unit');
    const progressPercentage = document.getElementById('current-progress');
    
    console.log('Elements found:', {
        goalValue: !!goalValue,
        goalUnit: !!goalUnit,
        progressPercentage: !!progressPercentage
    });
    
    if (goalValue) {
        goalValue.textContent = value;
        console.log('Updated goal value to:', value);
    }
    
    // get current goal type and unit
    const goalType = document.getElementById('goal-type-select')?.value || 'calories';
    const config = getGoalTypeConfig(goalType);
    
    if (goalUnit) {
        goalUnit.textContent = config.unit;
        console.log('Updated goal unit to:', config.unit);
    }
    
    // calculate progress (simulate current activity vs goal)
    const currentActivity = getCurrentActivity();
    const progress = Math.min(Math.floor((currentActivity / parseInt(value)) * 100), 100);
    
    if (progressPercentage) {
        progressPercentage.textContent = `${progress}%`;
        console.log('Updated progress to:', progress + '%');
    }
    
    // update slider fill
    updateSliderFill(value);
    
    console.log(`Updated display: ${value} ${config.unit}, progress: ${progress}%`);
}

// update slider fill
function updateSliderFill(value) {
    const slider = document.getElementById('inline-goal-slider');
    const sliderFill = document.getElementById('slider-fill');
    
    if (!slider || !sliderFill) return;
    
    const min = parseInt(slider.min);
    const max = parseInt(slider.max);
    const currentValue = parseInt(value);
    const percentage = Math.max(0, Math.min(100, ((currentValue - min) / (max - min)) * 100));
    
    sliderFill.style.width = `${percentage}%`;
    console.log(`Updating slider fill: ${currentValue} (${percentage}%)`);
}

// update progress display
function updateProgressDisplay(goalValue) {
    const progressFill = document.getElementById('progress-fill');
    const progressPercentage = document.getElementById('progress-percentage');
    const goalTarget = document.getElementById('goal-target');
    const goalUnit = document.getElementById('current-goal-unit');
    
    if (!progressFill || !progressPercentage || !goalTarget) return;
    
    // calculate progress
    const currentActivity = getCurrentActivity();
    const progress = Math.min(Math.floor((currentActivity / parseInt(goalValue)) * 100), 100);
    
    // update progress bar
    progressFill.style.width = `${progress}%`;
    progressPercentage.textContent = `${progress}%`;
    goalTarget.textContent = goalValue;
    
    // update unit if available
    if (goalUnit) {
        const unit = goalUnit.textContent;
        goalTarget.nextSibling.textContent = ` ${unit}`;
    }
}

// get current activity (simulate)
function getCurrentActivity() {
    // 使用与顶部展示相同的数据，确保协同
    const goalType = document.getElementById('goal-type-select')?.value || 'calories';
    const activityData = {
        calories: currentActivityData.calories || 1800,
        steps: currentActivityData.steps || 8500,
        distance: currentActivityData.distance || 3500,
        duration: 45
    };
    return activityData[goalType] || 1800;
}

// get goal type configuration
function getGoalTypeConfig(goalType) {
    const configs = {
        calories: { min: 1000, max: 10000, step: 50, defaultValue: 2250, unit: 'kcal' },
        steps: { min: 1000, max: 50000, step: 100, defaultValue: 10000, unit: 'steps' },
        distance: { min: 1000, max: 20000, step: 100, defaultValue: 5000, unit: 'm' },
        duration: { min: 15, max: 150, step: 5, defaultValue: 60, unit: 'min' }
    };
    return configs[goalType] || configs.calories;
}

// load goal settings from localStorage
function loadGoalSettings() {
    try {
        const saved = localStorage.getItem('sharedGoalSettings');
        if (saved) {
            const settings = JSON.parse(saved);
            console.log('Loaded goal settings:', settings);
            return settings;
        }
    } catch (error) {
        console.error('Error loading goal settings:', error);
    }
    
    // 返回默认设置
    return {
        type: 'calories',
        value: 2250,
        lastUpdated: new Date().toISOString()
    };
}

// validate goal value
function validateGoalValue(goalType, value) {
    const config = getGoalTypeConfig(goalType);
    
    if (value < config.min || value > config.max) {
        return {
            valid: false,
            message: `Goal value must be between ${config.min} and ${config.max} ${config.unit}`
        };
    }
    
    return { valid: true };
}

// save inline goal settings
function saveInlineGoalSettings() {
    const goalType = document.getElementById('goal-type-select')?.value || 'calories';
    const goalValue = parseInt(document.getElementById('inline-goal-slider')?.value || 2250);
    
    // validate input
    const validation = validateGoalValue(goalType, goalValue);
    if (!validation.valid) {
        showNotification(validation.message, 'error');
        return;
    }
    
    // save goal settings
    const goalSettings = {
        type: goalType,
        value: goalValue,
        lastUpdated: new Date().toISOString()
    };
    
    localStorage.setItem('sharedGoalSettings', JSON.stringify(goalSettings));
    
    // show success notification
    showNotification('Goal settings saved successfully!', 'success');
}

// auto-save on change
function autoSaveGoalSettings() {
    const goalType = document.getElementById('goal-type-select')?.value || 'calories';
    const goalValue = parseInt(document.getElementById('inline-goal-slider')?.value || 2250);
    
    // save goal settings
    const goalSettings = {
        type: goalType,
        value: goalValue,
        lastUpdated: new Date().toISOString()
    };
    
    localStorage.setItem('sharedGoalSettings', JSON.stringify(goalSettings));
}

// 设置滑杆事件监听器
function setupSliderEventListeners() {
    console.log('Setting up slider event listeners');
    
    const inlineSlider = document.getElementById('inline-goal-slider');
    const goalTypeSelect = document.getElementById('goal-type-select');
    
    console.log('Elements found for event listeners:', {
        inlineSlider: !!inlineSlider,
        goalTypeSelect: !!goalTypeSelect
    });
    
    if (inlineSlider) {
        // 移除之前的事件监听器（如果有的话）
        inlineSlider.removeEventListener('input', handleSliderInput);
        inlineSlider.removeEventListener('change', autoSaveGoalSettings);
        
        // 添加新的事件监听器
        inlineSlider.addEventListener('input', handleSliderInput);
        inlineSlider.addEventListener('change', autoSaveGoalSettings);
        
        console.log('Slider event listeners added');
    }
    
    if (goalTypeSelect) {
        goalTypeSelect.addEventListener('change', autoSaveGoalSettings);
        console.log('Goal type select event listener added');
    }
}

// 处理滑杆输入事件
function handleSliderInput(event) {
    console.log('Slider input event triggered:', event.target.value);
    const value = event.target.value;
    updateInlineGoalDisplay(value);
    updateProgressDisplay(value);
    updateSliderFill(value);
}

// export global functions
window.endRun = endRun;
window.confirmEndRun = confirmEndRun;
window.goToMap = goToMap;
window.goBack = goBack;
window.closeModal = closeModal;
window.saveInlineGoalSettings = saveInlineGoalSettings;
