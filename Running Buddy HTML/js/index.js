// Index.js - home page logic

// page init
document.addEventListener('DOMContentLoaded', function() {
    initializeIndexPage();
});

// init home page
function initializeIndexPage() {
    // init view toggle
    initializeViewToggle();
    
    // init runner pins
    initializeRunnerPins();
    
    // init list view
    initializeListView();
    
    // init map controls
    initializeMapControls();
    
    // 检查是否需要自动切换到列表视图
    const returnToListView = sessionStorage.getItem('returnToListView');
    if (returnToListView === 'true') {
        // 清除标记
        sessionStorage.removeItem('returnToListView');
        // 切换到列表视图
        switchToListView();
    }
}

// init view toggle
function initializeViewToggle() {
    const toggleButtons = document.querySelectorAll('.toggle-btn');
    const mapView = document.getElementById('map-view');
    const listView = document.getElementById('list-view');
    
    toggleButtons.forEach(button => {
        button.addEventListener('click', function() {
            const view = this.getAttribute('data-view');
            
            // 更新按钮状态
            toggleButtons.forEach(btn => btn.classList.remove('active'));
            this.classList.add('active');
            
            // 切换视图
            if (view === 'map') {
                mapView.classList.remove('hidden');
                listView.classList.add('hidden');
            } else {
                mapView.classList.add('hidden');
                listView.classList.remove('hidden');
            }
        });
    });
}

// 初始化跑步者标记
function initializeRunnerPins() {
    const runnerPins = document.querySelectorAll('.runner-pin');
    
    runnerPins.forEach(pin => {
        pin.addEventListener('click', function() {
            const runnerId = this.getAttribute('data-runner');
            viewRunnerProfile(runnerId);
        });
        
        // 添加悬停效果
        pin.addEventListener('mouseenter', function() {
            this.style.transform = 'translate(-50%, -50%) scale(1.1)';
        });
        
        pin.addEventListener('mouseleave', function() {
            this.style.transform = 'translate(-50%, -50%) scale(1)';
        });
    });
}

// 初始化列表视图
function initializeListView() {
    const runnerCards = document.querySelectorAll('.runner-card');
    
    runnerCards.forEach(card => {
        card.addEventListener('click', function() {
            const runnerId = this.getAttribute('data-runner');
            viewRunnerProfile(runnerId);
        });
        
        // 添加悬停效果
        card.addEventListener('mouseenter', function() {
            this.style.transform = 'translateY(-2px)';
            this.style.boxShadow = 'var(--shadow-md)';
        });
        
        card.addEventListener('mouseleave', function() {
            this.style.transform = 'translateY(0)';
            this.style.boxShadow = 'var(--shadow-sm)';
        });
    });
}

// 初始化地图控制
function initializeMapControls() {
    const switchToListBtn = document.getElementById('switch-to-list');
    
    if (switchToListBtn) {
        switchToListBtn.addEventListener('click', function() {
            // 切换到列表视图
            const listButton = document.querySelector('[data-view="list"]');
            if (listButton) {
                listButton.click();
            }
        });
    }
}

// 查看跑步者资料
function viewRunnerProfile(runnerId) {
    // 保存当前跑步者ID到sessionStorage
    sessionStorage.setItem('selectedRunner', runnerId);
    
    // 导航到资料页面
    navigateToPage('profile');
}

// 切换到列表视图
function switchToListView() {
    const listButton = document.querySelector('[data-view="list"]');
    if (listButton) {
        listButton.click();
    }
}

// 切换到地图视图
function switchToMapView() {
    const mapButton = document.querySelector('[data-view="map"]');
    if (mapButton) {
        mapButton.click();
    }
}

// 搜索跑步者
function searchRunners(query) {
    const runnerCards = document.querySelectorAll('.runner-card');
    const searchTerm = query.toLowerCase();
    
    runnerCards.forEach(card => {
        const name = card.querySelector('.runner-name').textContent.toLowerCase();
        const level = card.querySelector('.runner-badge').textContent.toLowerCase();
        
        if (name.includes(searchTerm) || level.includes(searchTerm)) {
            card.style.display = 'flex';
        } else {
            card.style.display = 'none';
        }
    });
}

// 筛选跑步者
function filterRunners(filter) {
    const runnerCards = document.querySelectorAll('.runner-card');
    
    runnerCards.forEach(card => {
        const level = card.querySelector('.runner-badge').textContent;
        const levelClass = card.querySelector('.runner-badge').className;
        
        if (filter === 'all' || levelClass.includes(filter)) {
            card.style.display = 'flex';
        } else {
            card.style.display = 'none';
        }
    });
}

// 获取跑步者统计
function getRunnerStats() {
    const runnerCards = document.querySelectorAll('.runner-card');
    const stats = {
        total: runnerCards.length,
        beginner: 0,
        intermediate: 0,
        advanced: 0
    };
    
    runnerCards.forEach(card => {
        const levelClass = card.querySelector('.runner-badge').className;
        if (levelClass.includes('badge-primary')) {
            stats.beginner++;
        } else if (levelClass.includes('badge-success')) {
            stats.intermediate++;
        } else if (levelClass.includes('badge-warning')) {
            stats.advanced++;
        }
    });
    
    return stats;
}

// 更新跑步者计数
function updateRunnerCount() {
    const countElement = document.querySelector('.list-count');
    if (countElement) {
        const visibleCards = document.querySelectorAll('.runner-card:not([style*="display: none"])');
        countElement.textContent = `${visibleCards.length} 人`;
    }
}

// 显示跑步者详情
function showRunnerDetails(runnerId) {
    const runners = getRunnersData();
    const runner = runners.find(r => r.id === runnerId);
    
    if (runner) {
        // 创建详情模态框
        const modal = createRunnerModal(runner);
        document.body.appendChild(modal);
        
        // 显示模态框
        setTimeout(() => {
            modal.classList.add('show');
        }, 100);
    }
}

// 创建跑步者模态框
function createRunnerModal(runner) {
    const modal = document.createElement('div');
    modal.className = 'modal-overlay';
    modal.innerHTML = `
        <div class="modal-content">
            <div class="modal-header">
                <h3 class="modal-title">${runner.name}</h3>
                <button class="modal-close" onclick="closeModal(this)">
                    <i data-lucide="x" class="icon"></i>
                </button>
            </div>
            <div class="modal-body">
                <div class="runner-details">
                    <div class="runner-avatar">
                        <img src="${runner.avatar}" alt="${runner.name}" class="avatar-img">
                    </div>
                    <div class="runner-info">
                        <div class="info-item">
                            <span class="label">等级:</span>
                            <span class="value">${runner.level}</span>
                        </div>
                        <div class="info-item">
                            <span class="label">配速:</span>
                            <span class="value">${runner.pace}</span>
                        </div>
                        <div class="info-item">
                            <span class="label">频率:</span>
                            <span class="value">${runner.runsPerWeek}次/周</span>
                        </div>
                        <div class="info-item">
                            <span class="label">距离:</span>
                            <span class="value">${runner.distance}</span>
                        </div>
                    </div>
                </div>
            </div>
            <div class="modal-footer">
                <button class="btn btn-primary" onclick="viewRunnerProfile('${runner.id}')">
                    查看资料
                </button>
                <button class="btn btn-outline" onclick="closeModal(this)">
                    关闭
                </button>
            </div>
        </div>
    `;
    
    // 初始化图标
    if (typeof lucide !== 'undefined') {
        lucide.createIcons();
    }
    
    return modal;
}

// 关闭模态框
function closeModal(button) {
    const modal = button.closest('.modal-overlay');
    if (modal) {
        modal.classList.remove('show');
        setTimeout(() => {
            document.body.removeChild(modal);
        }, 300);
    }
}

// 导出全局函数
window.viewRunnerProfile = viewRunnerProfile;
window.switchToListView = switchToListView;
window.switchToMapView = switchToMapView;
window.searchRunners = searchRunners;
window.filterRunners = filterRunners;
window.showRunnerDetails = showRunnerDetails;
window.closeModal = closeModal;
