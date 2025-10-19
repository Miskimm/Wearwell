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
    
    // check if need to auto switch to list view
    const returnToListView = sessionStorage.getItem('returnToListView');
    if (returnToListView === 'true') {
        // clear flag
        sessionStorage.removeItem('returnToListView');
        // switch to list view
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
            
            // update button state
            toggleButtons.forEach(btn => btn.classList.remove('active'));
            this.classList.add('active');
            
            // switch view
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

// initialize runner pins
function initializeRunnerPins() {
    const runnerPins = document.querySelectorAll('.runner-pin');
    
    runnerPins.forEach(pin => {
        pin.addEventListener('click', function() {
            const runnerId = this.getAttribute('data-runner');
            viewRunnerProfile(runnerId);
        });
        
        // add hover effect
        pin.addEventListener('mouseenter', function() {
            this.style.transform = 'translate(-50%, -50%) scale(1.1)';
        });
        
        pin.addEventListener('mouseleave', function() {
            this.style.transform = 'translate(-50%, -50%) scale(1)';
        });
    });
}

// initialize list view
function initializeListView() {
    const runnerCards = document.querySelectorAll('.runner-card');
    
    runnerCards.forEach(card => {
        card.addEventListener('click', function() {
            const runnerId = this.getAttribute('data-runner');
            viewRunnerProfile(runnerId);
        });
        
        // add hover effect
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

// initialize map controls
function initializeMapControls() {
    const switchToListBtn = document.getElementById('switch-to-list');
    
    if (switchToListBtn) {
        switchToListBtn.addEventListener('click', function() {
            // switch to list view
            const listButton = document.querySelector('[data-view="list"]');
            if (listButton) {
                listButton.click();
            }
        });
    }
}

// view runner profile
function viewRunnerProfile(runnerId) {
    // save current runner ID to sessionStorage
    sessionStorage.setItem('selectedRunner', runnerId);
    
    // record current view (map or list)
    const mapView = document.getElementById('map-view');
    const listView = document.getElementById('list-view');
    
    if (mapView && !mapView.classList.contains('hidden')) {
        sessionStorage.setItem('previousView', 'map');
    } else if (listView && !listView.classList.contains('hidden')) {
        sessionStorage.setItem('previousView', 'list');
    }
    
    // navigate to profile page
    navigateToPage('profile');
}

// switch to list view
function switchToListView() {
    const listButton = document.querySelector('[data-view="list"]');
    if (listButton) {
        listButton.click();
    }
}

// switch to map view
function switchToMapView() {
    const mapButton = document.querySelector('[data-view="map"]');
    if (mapButton) {
        mapButton.click();
    }
}

// search runners
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

// filter runners
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

// get runner stats
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

// update runner count
function updateRunnerCount() {
    const countElement = document.querySelector('.list-count');
    if (countElement) {
        const visibleCards = document.querySelectorAll('.runner-card:not([style*="display: none"])');
        countElement.textContent = `${visibleCards.length} runners`;
    }
}

// show runner details
function showRunnerDetails(runnerId) {
    const runners = getRunnersData();
    const runner = runners.find(r => r.id === runnerId);
    
    if (runner) {
        // create details modal
        const modal = createRunnerModal(runner);
        document.body.appendChild(modal);
        
        // show modal
        setTimeout(() => {
            modal.classList.add('show');
        }, 100);
    }
}

// create runner modal
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
                            <span class="label">Level:</span>
                            <span class="value">${runner.level}</span>
                        </div>
                        <div class="info-item">
                            <span class="label">Pace:</span>
                            <span class="value">${runner.pace}</span>
                        </div>
                        <div class="info-item">
                            <span class="label">Frequency:</span>
                            <span class="value">${runner.runsPerWeek} runs/week</span>
                        </div>
                        <div class="info-item">
                            <span class="label">Distance:</span>
                            <span class="value">${runner.distance}</span>
                        </div>
                    </div>
                </div>
            </div>
            <div class="modal-footer">
                <button class="btn btn-primary" onclick="viewRunnerProfile('${runner.id}')">
                    View Profile
                </button>
                <button class="btn btn-outline" onclick="closeModal(this)">
                    Close
                </button>
            </div>
        </div>
    `;
    
    // initialize icons
    if (typeof lucide !== 'undefined') {
        lucide.createIcons();
    }
    
    return modal;
}

// close modal
function closeModal(button) {
    const modal = button.closest('.modal-overlay');
    if (modal) {
        modal.classList.remove('show');
        setTimeout(() => {
            document.body.removeChild(modal);
        }, 300);
    }
}

// export global functions
window.viewRunnerProfile = viewRunnerProfile;
window.switchToListView = switchToListView;
window.switchToMapView = switchToMapView;
window.searchRunners = searchRunners;
window.filterRunners = filterRunners;
window.showRunnerDetails = showRunnerDetails;
window.closeModal = closeModal;
