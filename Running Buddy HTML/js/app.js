// App.js - global app logic and utils

// init app
document.addEventListener('DOMContentLoaded', function() {
    // init lucide icons
    if (typeof lucide !== 'undefined') {
        lucide.createIcons();
    }
    
    // init app
    initializeApp();
});

// app init
function initializeApp() {
    // set page title
    updatePageTitle();
    
    // init navigation
    initializeNavigation();
    
    // init theme
    initializeTheme();
    
    // init data
    initializeData();
    
    // init current user display
    initializeCurrentUserDisplay();
}

// update page title
function updatePageTitle() {
    const currentPage = getCurrentPage();
    const titles = {
        'index': 'Running Buddy - Find Your Perfect Running Partner',
        'runners': 'Nearby Runners - Running Buddy',
        'profile': 'Runner Profile - Running Buddy',
        'match': 'Match Confirmation - Running Buddy',
        'schedule': 'Schedule Run - Running Buddy',
        'shared-goal': 'Shared Goal - Running Buddy'
    };
    
    document.title = titles[currentPage] || 'Running Buddy';
}

// get current page
function getCurrentPage() {
    const path = window.location.pathname;
    const filename = path.split('/').pop().split('.')[0];
    return filename || 'index';
}

// initialize navigation
function initializeNavigation() {
    // handle back buttons
    const backButtons = document.querySelectorAll('.back-btn');
    backButtons.forEach(button => {
        button.addEventListener('click', handleBackNavigation);
    });
    
    // handle link clicks
    const links = document.querySelectorAll('a[href]');
    links.forEach(link => {
        link.addEventListener('click', handleLinkClick);
    });
}

// handle back navigation
function handleBackNavigation(event) {
    event.preventDefault();
    goBack();
}

// handle link click
function handleLinkClick(event) {
    const href = event.target.getAttribute('href');
    if (href && href.startsWith('#')) {
        event.preventDefault();
        const targetId = href.substring(1);
        navigateToPage(targetId);
    }
}

// go back to previous page
function goBack() {
    const currentPage = getCurrentPage();
    
    // smart navigation based on current page and source
    if (currentPage === 'profile') {
        // if profile page, check how it was entered
        const selectedRunner = sessionStorage.getItem('selectedRunner');
        const currentUser = sessionStorage.getItem('currentUser');
        const previousView = sessionStorage.getItem('previousView');
        
        if (currentUser === 'lanbell' || !selectedRunner) {
            // if viewing current user profile or no runner selected, return to map
            sessionStorage.removeItem('previousView');
            navigateToPage('index');
        } else {
            // return based on previous view
            if (previousView === 'list') {
                // if entered from list view, return to list view
                sessionStorage.setItem('returnToListView', 'true');
            } else {
                // if entered from map view, return to map view (default)
                sessionStorage.removeItem('returnToListView');
            }
            // clear previous view record
            sessionStorage.removeItem('previousView');
            navigateToPage('index');
        }
    } else if (currentPage === 'shared-goal') {
        // from shared goal page, return to map
        navigateToPage('index');
    } else if (currentPage === 'match') {
        // from match page, return to list view
        sessionStorage.setItem('returnToListView', 'true');
        navigateToPage('index');
    } else if (currentPage === 'schedule') {
        // from schedule page, return to match page
        navigateToPage('match');
    } else {
        // use browser history for other cases
        if (window.history.length > 1) {
            window.history.back();
        } else {
            // if no history, return to home
            navigateToPage('index');
        }
    }
}

// navigate to page
function navigateToPage(page) {
    const pages = {
        'index': 'index.html',
        'profile': 'profile.html',
        'match': 'match.html',
        'schedule': 'schedule.html',
        'shared-goal': 'shared-goal.html'
    };
    
    const targetPage = pages[page];
    if (targetPage) {
        window.location.href = targetPage;
    }
}

// initialize theme
function initializeTheme() {
    // check user theme preference
    const savedTheme = localStorage.getItem('theme');
    const prefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
    
    if (savedTheme) {
        document.documentElement.setAttribute('data-theme', savedTheme);
    } else if (prefersDark) {
        document.documentElement.setAttribute('data-theme', 'dark');
    }
}

// initialize data
function initializeData() {
    // get data from URL parameters
    const urlParams = new URLSearchParams(window.location.search);
    const runnerId = urlParams.get('runner');
    
    if (runnerId) {
        // if runner ID exists, load runner data
        loadRunnerData(runnerId);
    }
}

// load runner data
function loadRunnerData(runnerId) {
    const runners = getRunnersData();
    const runner = runners.find(r => r.id === runnerId);
    
    if (runner) {
        // update page data
        updateRunnerInfo(runner);
    }
}

// get runners data
function getRunnersData() {
    return [
        {
            id: '1',
            name: 'Sarah Chen',
            avatar: 'assets/sarah.png',
            level: 'Intermediate',
            pace: '5:30 min/km',
            runsPerWeek: 4,
            distance: '200m away',
            lastActivity: 'Ran yesterday'
        },
        {
            id: '2',
            name: 'Mike Rodriguez',
            avatar: 'https://images.unsplash.com/photo-1507003211169-0a1dd7228f2d?w=150&h=150&fit=crop&crop=face',
            level: 'Advanced',
            pace: '4:45 min/km',
            runsPerWeek: 6,
            distance: '350m away',
            lastActivity: 'Ran 2 hours ago'
        },
        {
            id: '3',
            name: 'Emma Wilson',
            avatar: 'https://images.unsplash.com/photo-1438761681033-6461ffad8d80?w=150&h=150&fit=crop&crop=face',
            level: 'Beginner',
            pace: '6:45 min/km',
            runsPerWeek: 3,
            distance: '500m away',
            lastActivity: 'Ran yesterday'
        },
        {
            id: '4',
            name: 'James Park',
            avatar: 'https://images.unsplash.com/photo-1472099645785-5658abf4ff4e?w=150&h=150&fit=crop&crop=face',
            level: 'Intermediate',
            pace: '5:15 min/km',
            runsPerWeek: 5,
            distance: '750m away',
            lastActivity: 'Ran this morning'
        },
        {
            id: '5',
            name: 'Lisa Zhang',
            avatar: 'https://images.unsplash.com/photo-1544005313-94ddf0286df2?w=150&h=150&fit=crop&crop=face',
            level: 'Advanced',
            pace: '4:30 min/km',
            runsPerWeek: 7,
            distance: '900m away',
            lastActivity: 'Ran this morning'
        },
        {
            id: '6',
            name: 'David Kim',
            avatar: 'https://images.unsplash.com/photo-1500648767791-00dcc994a43e?w=150&h=150&fit=crop&crop=face',
            level: 'Beginner',
            pace: '7:00 min/km',
            runsPerWeek: 2,
            distance: '1.2km away',
            lastActivity: 'Ran 2 days ago'
        },
        {
            id: '7',
            name: 'Anna Petrov',
            avatar: 'https://images.unsplash.com/photo-1489424731084-a5d8b219a5bb?w=150&h=150&fit=crop&crop=face',
            level: 'Intermediate',
            pace: '5:45 min/km',
            runsPerWeek: 4,
            distance: '1.5km away',
            lastActivity: 'Ran yesterday'
        }
    ];
}

// update runner info
function updateRunnerInfo(runner) {
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
    
    // update other info
    const levelElement = document.getElementById('profile-level');
    if (levelElement) {
        const levelText = runner.level === 'Beginner' ? 'Beginner' : 
                         runner.level === 'Intermediate' ? 'Intermediate' : 'Advanced';
        levelElement.textContent = levelText;
    }
    
    const paceElement = document.getElementById('profile-pace');
    if (paceElement) {
        paceElement.textContent = runner.pace.split(' ')[0];
    }
    
    const runsElement = document.getElementById('profile-runs');
    if (runsElement) {
        runsElement.textContent = runner.runsPerWeek;
    }
    
    const distanceElement = document.getElementById('profile-distance');
    if (distanceElement) {
        distanceElement.textContent = runner.distance;
    }
}

// show loading state
function showLoading(element) {
    if (element) {
        element.innerHTML = `
            <div class="loading">
                <i data-lucide="loader-2" class="icon"></i>
                <span>Loading...</span>
            </div>
        `;
        if (typeof lucide !== 'undefined') {
            lucide.createIcons();
        }
    }
}

// show error state
function showError(element, message) {
    if (element) {
        element.innerHTML = `
            <div class="error-state">
                <i data-lucide="alert-circle" class="icon"></i>
                <h3>Error</h3>
                <p>${message}</p>
            </div>
        `;
        if (typeof lucide !== 'undefined') {
            lucide.createIcons();
        }
    }
}

// show empty state
function showEmpty(element, message) {
    if (element) {
        element.innerHTML = `
            <div class="empty-state">
                <i data-lucide="users" class="icon"></i>
                <h3>No Data</h3>
                <p>${message}</p>
            </div>
        `;
        if (typeof lucide !== 'undefined') {
            lucide.createIcons();
        }
    }
}

// show notification
function showNotification(message, type = 'info') {
    // create notification element
    const notification = document.createElement('div');
    notification.className = `notification notification-${type}`;
    notification.innerHTML = `
        <div class="notification-content">
            <i data-lucide="${type === 'success' ? 'check-circle' : type === 'error' ? 'x-circle' : 'info'}" class="icon"></i>
            <span>${message}</span>
        </div>
    `;
    
    // add to page
    document.body.appendChild(notification);
    
    // initialize icons
    if (typeof lucide !== 'undefined') {
        lucide.createIcons();
    }
    
    // show animation
    setTimeout(() => {
        notification.classList.add('show');
    }, 100);
    
    // auto hide
    setTimeout(() => {
        notification.classList.remove('show');
        setTimeout(() => {
            document.body.removeChild(notification);
        }, 300);
    }, 3000);
}

// format time
function formatTime(date) {
    const now = new Date();
    const diff = now - date;
    const minutes = Math.floor(diff / 60000);
    const hours = Math.floor(diff / 3600000);
    const days = Math.floor(diff / 86400000);
    
    if (minutes < 60) {
        return `${minutes} minutes ago`;
    } else if (hours < 24) {
        return `${hours} hours ago`;
    } else {
        return `${days} days ago`;
    }
}

// format distance
function formatDistance(meters) {
    if (meters < 1000) {
        return `${meters}m`;
    } else {
        return `${(meters / 1000).toFixed(1)}km`;
    }
}

// debounce function
function debounce(func, wait) {
    let timeout;
    return function executedFunction(...args) {
        const later = () => {
            clearTimeout(timeout);
            func(...args);
        };
        clearTimeout(timeout);
        timeout = setTimeout(later, wait);
    };
}

// throttle function
function throttle(func, limit) {
    let inThrottle;
    return function() {
        const args = arguments;
        const context = this;
        if (!inThrottle) {
            func.apply(context, args);
            inThrottle = true;
            setTimeout(() => inThrottle = false, limit);
        }
    };
}

// initialize current user display
function initializeCurrentUserDisplay() {
    // get current user info (LAN BELL)
    const currentUser = {
        id: 'lanbell',
        name: 'LAN BELL',
        avatar: 'assets/profile.png',
        level: 'Intermediate'
    };
    
    // update user avatar in header across all pages
    updateHeaderUserAvatar(currentUser);
}

// update header user avatar
function updateHeaderUserAvatar(user) {
    const headerAvatarImg = document.getElementById('header-avatar-img');
    if (headerAvatarImg) {
        headerAvatarImg.src = user.avatar;
        headerAvatarImg.alt = user.name;
        console.log('Updated header avatar for:', user.name);
    }
    
    // if page has user avatar placeholder, update it too
    const headerUserAvatar = document.getElementById('header-user-avatar');
    if (headerUserAvatar && !headerAvatarImg) {
        headerUserAvatar.innerHTML = `
            <img src="${user.avatar}" alt="${user.name}" class="avatar-img" id="header-avatar-img">
        `;
        console.log('Created header avatar for:', user.name);
    }
}

// view current user profile
function viewCurrentUserProfile() {
    // clear other runner selection, set to current user
    sessionStorage.removeItem('selectedRunner');
    sessionStorage.setItem('currentUser', 'lanbell');
    
    // navigate to profile page
    window.location.href = 'profile.html';
}

// export global functions
window.goBack = goBack;
window.navigateToPage = navigateToPage;
window.showNotification = showNotification;
window.formatTime = formatTime;
window.formatDistance = formatDistance;
window.initializeCurrentUserDisplay = initializeCurrentUserDisplay;
window.viewCurrentUserProfile = viewCurrentUserProfile;
