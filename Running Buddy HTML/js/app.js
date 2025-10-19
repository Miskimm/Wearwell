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

// 初始化导航
function initializeNavigation() {
    // 处理返回按钮
    const backButtons = document.querySelectorAll('.back-btn');
    backButtons.forEach(button => {
        button.addEventListener('click', handleBackNavigation);
    });
    
    // 处理链接点击
    const links = document.querySelectorAll('a[href]');
    links.forEach(link => {
        link.addEventListener('click', handleLinkClick);
    });
}

// 处理返回导航
function handleBackNavigation(event) {
    event.preventDefault();
    goBack();
}

// 处理链接点击
function handleLinkClick(event) {
    const href = event.target.getAttribute('href');
    if (href && href.startsWith('#')) {
        event.preventDefault();
        const targetId = href.substring(1);
        navigateToPage(targetId);
    }
}

// 返回上一页
function goBack() {
    const currentPage = getCurrentPage();
    
    // 根据当前页面和来源智能导航
    if (currentPage === 'profile') {
        // 如果是个人资料页面，检查是如何进入的
        const selectedRunner = sessionStorage.getItem('selectedRunner');
        const currentUser = sessionStorage.getItem('currentUser');
        
        if (currentUser === 'lanbell' || !selectedRunner) {
            // 如果是查看当前用户资料或没有选择特定跑步者，返回地图页面
            navigateToPage('index');
        } else {
            // 如果是从推荐进入，则返回地图视图；否则返回列表视图
            const entrySource = sessionStorage.getItem('entrySource');
            if (entrySource === 'recommendation') {
                sessionStorage.removeItem('entrySource');
                sessionStorage.removeItem('returnToListView');
                navigateToPage('index');
            } else {
                sessionStorage.setItem('returnToListView', 'true');
                navigateToPage('index');
            }
        }
    } else if (currentPage === 'shared-goal') {
        // 从共同目标页面返回地图页面
        navigateToPage('index');
    } else if (currentPage === 'match') {
        // 从匹配页面返回首页列表视图
        sessionStorage.setItem('returnToListView', 'true');
        navigateToPage('index');
    } else if (currentPage === 'schedule') {
        // 从日程页面返回匹配页面
        navigateToPage('match');
    } else {
        // 其他情况使用浏览器历史记录
        if (window.history.length > 1) {
            window.history.back();
        } else {
            // 如果没有历史记录，返回首页
            navigateToPage('index');
        }
    }
}

// 页面导航
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

// 初始化主题
function initializeTheme() {
    // 检查用户主题偏好
    const savedTheme = localStorage.getItem('theme');
    const prefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
    
    if (savedTheme) {
        document.documentElement.setAttribute('data-theme', savedTheme);
    } else if (prefersDark) {
        document.documentElement.setAttribute('data-theme', 'dark');
    }
}

// 初始化数据
function initializeData() {
    // 从URL参数获取数据
    const urlParams = new URLSearchParams(window.location.search);
    const runnerId = urlParams.get('runner');
    
    if (runnerId) {
        // 如果有跑步者ID，加载跑步者数据
        loadRunnerData(runnerId);
    }
}

// 加载跑步者数据
function loadRunnerData(runnerId) {
    const runners = getRunnersData();
    const runner = runners.find(r => r.id === runnerId);
    
    if (runner) {
        // 更新页面数据
        updateRunnerInfo(runner);
    }
}

// 获取跑步者数据
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

// 更新跑步者信息
function updateRunnerInfo(runner) {
    // 更新头像
    const avatarImg = document.getElementById('profile-avatar');
    if (avatarImg) {
        avatarImg.src = runner.avatar;
        avatarImg.alt = runner.name;
    }
    
    // 更新姓名
    const nameElement = document.getElementById('profile-name');
    if (nameElement) {
        nameElement.textContent = runner.name;
    }
    
    // 更新其他信息
    const levelElement = document.getElementById('profile-level');
    if (levelElement) {
        const levelText = runner.level === 'Beginner' ? '初级' : 
                         runner.level === 'Intermediate' ? '中级' : '高级';
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

// 显示加载状态
function showLoading(element) {
    if (element) {
        element.innerHTML = `
            <div class="loading">
                <i data-lucide="loader-2" class="icon"></i>
                <span>加载中...</span>
            </div>
        `;
        if (typeof lucide !== 'undefined') {
            lucide.createIcons();
        }
    }
}

// 显示错误状态
function showError(element, message) {
    if (element) {
        element.innerHTML = `
            <div class="error-state">
                <i data-lucide="alert-circle" class="icon"></i>
                <h3>出错了</h3>
                <p>${message}</p>
            </div>
        `;
        if (typeof lucide !== 'undefined') {
            lucide.createIcons();
        }
    }
}

// 显示空状态
function showEmpty(element, message) {
    if (element) {
        element.innerHTML = `
            <div class="empty-state">
                <i data-lucide="users" class="icon"></i>
                <h3>暂无数据</h3>
                <p>${message}</p>
            </div>
        `;
        if (typeof lucide !== 'undefined') {
            lucide.createIcons();
        }
    }
}

// 显示通知
function showNotification(message, type = 'info') {
    // 创建通知元素
    const notification = document.createElement('div');
    notification.className = `notification notification-${type}`;
    notification.innerHTML = `
        <div class="notification-content">
            <i data-lucide="${type === 'success' ? 'check-circle' : type === 'error' ? 'x-circle' : 'info'}" class="icon"></i>
            <span>${message}</span>
        </div>
    `;
    
    // 添加到页面
    document.body.appendChild(notification);
    
    // 初始化图标
    if (typeof lucide !== 'undefined') {
        lucide.createIcons();
    }
    
    // 显示动画
    setTimeout(() => {
        notification.classList.add('show');
    }, 100);
    
    // 自动隐藏
    setTimeout(() => {
        notification.classList.remove('show');
        setTimeout(() => {
            document.body.removeChild(notification);
        }, 300);
    }, 3000);
}

// 格式化时间
function formatTime(date) {
    const now = new Date();
    const diff = now - date;
    const minutes = Math.floor(diff / 60000);
    const hours = Math.floor(diff / 3600000);
    const days = Math.floor(diff / 86400000);
    
    if (minutes < 60) {
        return `${minutes}分钟前`;
    } else if (hours < 24) {
        return `${hours}小时前`;
    } else {
        return `${days}天前`;
    }
}

// 格式化距离
function formatDistance(meters) {
    if (meters < 1000) {
        return `${meters}m`;
    } else {
        return `${(meters / 1000).toFixed(1)}km`;
    }
}

// 防抖函数
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

// 节流函数
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
    // 获取当前用户信息（LAN BELL）
    const currentUser = {
        id: 'lanbell',
        name: 'LAN BELL',
        avatar: 'assets/profile.png',
        level: 'Intermediate'
    };
    
    // 更新所有页面的用户头像
    updateHeaderUserAvatar(currentUser);
}

// 更新头部用户头像
function updateHeaderUserAvatar(user) {
    const headerAvatarImg = document.getElementById('header-avatar-img');
    if (headerAvatarImg) {
        headerAvatarImg.src = user.avatar;
        headerAvatarImg.alt = user.name;
        console.log('Updated header avatar for:', user.name);
    }
    
    // 如果页面有用户头像占位符，也更新它
    const headerUserAvatar = document.getElementById('header-user-avatar');
    if (headerUserAvatar && !headerAvatarImg) {
        headerUserAvatar.innerHTML = `
            <img src="${user.avatar}" alt="${user.name}" class="avatar-img" id="header-avatar-img">
        `;
        console.log('Created header avatar for:', user.name);
    }
}

// 查看当前用户资料
function viewCurrentUserProfile() {
    // 清除其他跑步者选择，设置为当前用户
    sessionStorage.removeItem('selectedRunner');
    sessionStorage.setItem('currentUser', 'lanbell');
    
    // 跳转到个人资料页面
    window.location.href = 'profile.html';
}

// 导出全局函数
window.goBack = goBack;
window.navigateToPage = navigateToPage;
window.showNotification = showNotification;
window.formatTime = formatTime;
window.formatDistance = formatDistance;
window.initializeCurrentUserDisplay = initializeCurrentUserDisplay;
window.viewCurrentUserProfile = viewCurrentUserProfile;
