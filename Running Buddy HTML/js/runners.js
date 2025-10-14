// Runners.js - runners list page logic

// page init
document.addEventListener('DOMContentLoaded', function() {
    initializeRunnersPage();
});

// init runners page
function initializeRunnersPage() {
    // init runner cards
    initializeRunnerCards();
    
    // init search
    initializeSearch();
    
    // init filters
    initializeFilters();
    
    // init sorting
    initializeSorting();
    
    // load runners data
    loadRunnersData();
}

// init runner cards
function initializeRunnerCards() {
    const runnerCards = document.querySelectorAll('.runner-card');
    
    runnerCards.forEach(card => {
        // click event
        card.addEventListener('click', function() {
            const runnerId = this.getAttribute('data-runner');
            viewRunnerProfile(runnerId);
        });
        
        // hover effects
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

// init search
function initializeSearch() {
    // if search input exists, add search function
    const searchInput = document.querySelector('.search-input');
    if (searchInput) {
        searchInput.addEventListener('input', debounce(function() {
            const query = this.value;
            searchRunners(query);
        }, 300));
    }
}

// init filters
function initializeFilters() {
    const filterChips = document.querySelectorAll('.filter-chip');
    
    filterChips.forEach(chip => {
        chip.addEventListener('click', function() {
            // update selected state
            filterChips.forEach(c => c.classList.remove('active'));
            this.classList.add('active');
            
            // apply filter
            const filter = this.getAttribute('data-filter');
            filterRunners(filter);
        });
    });
}

// init sorting
function initializeSorting() {
    // if sort buttons exist, add sorting function
    const sortButtons = document.querySelectorAll('.sort-btn');
    
    sortButtons.forEach(button => {
        button.addEventListener('click', function() {
            const sortBy = this.getAttribute('data-sort');
            sortRunners(sortBy);
        });
    });
}

// load runners data
function loadRunnersData() {
    const runners = getRunnersData();
    const runnersList = document.querySelector('.runners-list');
    
    if (runnersList) {
        // clear existing content
        runnersList.innerHTML = '';
        
        // create runner cards
        runners.forEach(runner => {
            const card = createRunnerCard(runner);
            runnersList.appendChild(card);
        });
        
        // re-init card events
        initializeRunnerCards();
    }
}

// create runner card
function createRunnerCard(runner) {
    const card = document.createElement('div');
    card.className = 'runner-card';
    card.setAttribute('data-runner', runner.id);
    
    const levelClass = getLevelClass(runner.level);
    const levelText = getLevelText(runner.level);
    
    card.innerHTML = `
        <div class="runner-avatar">
            <img src="${runner.avatar}" alt="${runner.name}" class="avatar-img">
        </div>
        <div class="runner-info">
            <div class="runner-header">
                <h3 class="runner-name">${runner.name}</h3>
                <div class="runner-badge badge ${levelClass}">${levelText}</div>
            </div>
            <div class="runner-stats">
                <div class="stat-item">
                    <i data-lucide="target" class="icon"></i>
                    <span>${runner.pace}</span>
                </div>
                <div class="stat-item">
                    <i data-lucide="play" class="icon"></i>
                    <span>${runner.runsPerWeek} runs/week</span>
                </div>
                <div class="stat-item">
                    <i data-lucide="map-pin" class="icon"></i>
                    <span>${runner.distance}</span>
                </div>
                <div class="stat-item">
                    <span class="last-activity">${runner.lastActivity}</span>
                </div>
            </div>
            <button class="action-btn btn btn-primary" onclick="viewRunnerProfile('${runner.id}')">
                <i data-lucide="user-plus" class="icon"></i>
                Request to Run Together
            </button>
        </div>
    `;
    
    return card;
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

// get level text
function getLevelText(level) {
    switch (level) {
        case 'Beginner': return 'Beginner';
        case 'Intermediate': return 'Intermediate';
        case 'Advanced': return 'Advanced';
        default: return level;
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
    
    // update count
    updateRunnerCount();
}

// filter runners
function filterRunners(filter) {
    const runnerCards = document.querySelectorAll('.runner-card');
    
    runnerCards.forEach(card => {
        const levelClass = card.querySelector('.runner-badge').className;
        
        if (filter === 'all' || levelClass.includes(filter)) {
            card.style.display = 'flex';
        } else {
            card.style.display = 'none';
        }
    });
    
    // update count
    updateRunnerCount();
}

// sort runners
function sortRunners(sortBy) {
    const runnersList = document.querySelector('.runners-list');
    const cards = Array.from(runnersList.querySelectorAll('.runner-card'));
    
    cards.sort((a, b) => {
        switch (sortBy) {
            case 'name':
                const nameA = a.querySelector('.runner-name').textContent;
                const nameB = b.querySelector('.runner-name').textContent;
                return nameA.localeCompare(nameB);
            
            case 'distance':
                const distanceA = getDistanceValue(a.querySelector('.stat-item:last-child span').textContent);
                const distanceB = getDistanceValue(b.querySelector('.stat-item:last-child span').textContent);
                return distanceA - distanceB;
            
            case 'level':
                const levelA = getLevelValue(a.querySelector('.runner-badge').textContent);
                const levelB = getLevelValue(b.querySelector('.runner-badge').textContent);
                return levelA - levelB;
            
            default:
                return 0;
        }
    });
    
    // re-arrange cards
    cards.forEach(card => {
        runnersList.appendChild(card);
    });
}

// get distance value
function getDistanceValue(distanceText) {
    const match = distanceText.match(/(\d+(?:\.\d+)?)/);
    return match ? parseFloat(match[1]) : 0;
}

// get level value
function getLevelValue(levelText) {
    switch (levelText) {
        case 'Beginner': return 1;
        case 'Intermediate': return 2;
        case 'Advanced': return 3;
        default: return 0;
    }
}

// update runner count
function updateRunnerCount() {
    const countElement = document.querySelector('.list-count');
    if (countElement) {
        const visibleCards = document.querySelectorAll('.runner-card:not([style*="display: none"])');
        countElement.textContent = `${visibleCards.length} runners`;
    }
}

// view runner profile
function viewRunnerProfile(runnerId) {
    // save current runner ID
    sessionStorage.setItem('selectedRunner', runnerId);
    
    // navigate to profile page
    navigateToPage('profile');
}

// request match
function requestMatch(runnerId) {
    // show loading state
    showNotification('Sending request...', 'info');
    
    // simulate API call
    setTimeout(() => {
        // save matched runner ID
        sessionStorage.setItem('matchedRunner', runnerId);
        
        // show success message
        showNotification('Request sent!', 'success');
        
        // navigate to match page
        setTimeout(() => {
            navigateToPage('match');
        }, 1000);
    }, 1500);
}


// refresh data
function refreshData() {
    // show loading state
    const runnersList = document.querySelector('.runners-list');
    if (runnersList) {
        showLoading(runnersList);
    }
    
    // simulate data loading
    setTimeout(() => {
        loadRunnersData();
        showNotification('Data refreshed', 'success');
    }, 1000);
}

// export global functions
window.viewRunnerProfile = viewRunnerProfile;
window.requestMatch = requestMatch;
window.sendMessage = sendMessage;
window.searchRunners = searchRunners;
window.filterRunners = filterRunners;
window.sortRunners = sortRunners;
window.refreshData = refreshData;
