// Login.js - login page logic

// page init
document.addEventListener('DOMContentLoaded', function() {
    initializeLoginPage();
});

// init login page
function initializeLoginPage() {
    // init lucide icons
    if (typeof lucide !== 'undefined') {
        lucide.createIcons();
    }
    
    // add keyboard navigation
    setupKeyboardNavigation();
}

// setup keyboard navigation
function setupKeyboardNavigation() {
    document.addEventListener('keydown', function(e) {
        if (e.key === 'Enter') {
            const focusedElement = document.activeElement;
            if (focusedElement && focusedElement.classList.contains('btn')) {
                focusedElement.click();
            }
        }
    });
}

// handle login
function handleLogin() {
    console.log('Login clicked');
    
    // directly redirect to home page
    window.location.href = 'index.html';
}

// handle sign up
function handleSignUp() {
    console.log('Sign Up clicked');
    
    // directly redirect to home page
    window.location.href = 'index.html';
}

// handle guest login
function handleGuest() {
    console.log('Continue as Guest clicked');
    
    // directly redirect to home page
    window.location.href = 'index.html';
}

// handle back navigation
function goBack() {
    if (window.history.length > 1) {
        window.history.back();
    } else {
        window.location.href = 'index.html';
    }
}

// export global functions
window.handleLogin = handleLogin;
window.handleSignUp = handleSignUp;
window.handleGuest = handleGuest;
window.goBack = goBack;
