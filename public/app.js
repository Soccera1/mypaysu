let currentUser = '';

const loginScreen = document.getElementById('login-screen');
const dashboardScreen = document.getElementById('dashboard-screen');
const loginStatus = document.getElementById('login-status');
const transferStatus = document.getElementById('transfer-status');
const balanceDisplay = document.getElementById('balance-display');
const currentUserDisplay = document.getElementById('current-user-display');

const loginUsername = document.getElementById('login-username');
const loginPassword = document.getElementById('login-password');
const transferRecipient = document.getElementById('transfer-recipient');
const transferAmount = document.getElementById('transfer-amount');

const btnLogin = document.getElementById('btn-login');
const btnRegister = document.getElementById('btn-register');
const btnTransfer = document.getElementById('btn-transfer');
const btnRefresh = document.getElementById('btn-refresh');
const btnLogout = document.getElementById('btn-logout');

function showMessage(element, message, isError = false) {
    element.textContent = message;
    element.className = 'status-message ' + (isError ? 'error' : 'success');
    setTimeout(() => {
        element.className = 'status-message';
    }, 5000);
}

function switchToScreen(screen) {
    loginScreen.classList.remove('active');
    dashboardScreen.classList.remove('active');
    screen.classList.add('active');
}

async function makeRequest(endpoint, data) {
    try {
        const formData = new URLSearchParams(data);
        const response = await fetch(endpoint, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded',
            },
            body: formData
        });
        return await response.json();
    } catch (error) {
        return { success: false, message: 'Network error. Is the server running?' };
    }
}

async function login() {
    const username = loginUsername.value.trim();
    const password = loginPassword.value.trim();

    if (!username || !password) {
        showMessage(loginStatus, 'Please enter both username and password', true);
        return;
    }

    const result = await makeRequest('/api/login', { username, password });

    if (result.success) {
        currentUser = username;
        currentUserDisplay.textContent = username;
        switchToScreen(dashboardScreen);
        await refreshBalance();
        loginPassword.value = '';
    } else {
        showMessage(loginStatus, result.message, true);
    }
}

async function register() {
    const username = loginUsername.value.trim();
    const password = loginPassword.value.trim();

    if (!username || !password) {
        showMessage(loginStatus, 'Please enter both username and password', true);
        return;
    }

    const result = await makeRequest('/api/register', { username, password });
    showMessage(loginStatus, result.message, !result.success);
}

async function refreshBalance() {
    if (!currentUser) return;

    const result = await makeRequest('/api/balance', { username: currentUser });

    if (result.success) {
        balanceDisplay.textContent = result.balance.toFixed(2) + ' SXMR';
    }
}

async function transfer() {
    const recipient = transferRecipient.value.trim();
    const amount = parseFloat(transferAmount.value);

    if (!recipient || !amount || amount <= 0) {
        showMessage(transferStatus, 'Please enter valid recipient and amount', true);
        return;
    }

    const result = await makeRequest('/api/transfer', {
        username: currentUser,
        target_user: recipient,
        amount: amount.toString()
    });

    showMessage(transferStatus, result.message, !result.success);

    if (result.success) {
        transferRecipient.value = '';
        transferAmount.value = '';
        balanceDisplay.textContent = result.balance.toFixed(2) + ' SXMR';
    }
}

function logout() {
    currentUser = '';
    loginUsername.value = '';
    loginPassword.value = '';
    switchToScreen(loginScreen);
    loginStatus.textContent = '';
    transferStatus.textContent = '';
}

btnLogin.addEventListener('click', login);
btnRegister.addEventListener('click', register);
btnTransfer.addEventListener('click', transfer);
btnRefresh.addEventListener('click', refreshBalance);
btnLogout.addEventListener('click', logout);

loginUsername.addEventListener('keypress', (e) => {
    if (e.key === 'Enter') login();
});

loginPassword.addEventListener('keypress', (e) => {
    if (e.key === 'Enter') login();
});

transferRecipient.addEventListener('keypress', (e) => {
    if (e.key === 'Enter') document.getElementById('transfer-amount').focus();
});

transferAmount.addEventListener('keypress', (e) => {
    if (e.key === 'Enter') transfer();
});
