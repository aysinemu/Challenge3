const API_URL = 'http://localhost:8000/run';
const WS_URL = 'ws://localhost:8000/ws';

const activateToggle = document.getElementById('activateToggle');
const testToggle     = document.getElementById('testToggle');
const manualBtn      = document.getElementById('manualBtn');
const coordForm      = document.getElementById('coordForm');
const testFieldset   = document.getElementById('testFieldset');
const manualFieldset = document.getElementById('manualFieldset');
const coordFieldset  = document.getElementById('coordFieldset');
const inputX         = document.getElementById('inputX');
const rangeError     = document.getElementById('rangeError');
const manualControls = document.getElementById('manualControls');
const leftBtn        = document.getElementById('leftBtn');
const rightBtn       = document.getElementById('rightBtn');
const resetBtn       = document.getElementById('resetBtn');
const currentXEl     = document.getElementById('currentX');
const currentYEl     = document.getElementById('currentY');
const cycleStatus    = document.getElementById('cycleStatus');
const testProgress   = document.getElementById('testProgress');
const historyContainer = document.getElementById('historyContainer');
const emptyHistory   = document.getElementById('emptyHistory');
const clearHistory   = document.getElementById('clearHistory');
const exportHistory  = document.getElementById('exportHistory');
const connectionDot  = document.querySelector('.connection-dot');
const autohomeBtn    = document.getElementById('autohomeBtn');
const autohomeStatus = document.getElementById('autohomeStatus');

let manualActive = false;
let manualX = 0;
const manualY = 0;  
let isMoving = false;
const manualStep = 1;
let testRunning = false;
let history = [];
let previousTestX = -1;
let homingInProgress = false;

let socket = null;
let coordResolve = null;

function connectWebSocket() {
    socket = new WebSocket(WS_URL);
    
    socket.onopen = () => {
        console.log('WebSocket connected');
        connectionDot.classList.remove('disconnected');
        showNotification('WebSocket connected', 'success');
    };
    
    socket.onmessage = (event) => {
        const message = event.data;
        console.log('Received WebSocket message:', message);
        
        if (message.toLowerCase().includes("done")) {
            console.log("Received DONE confirmation");
            
            if (coordResolve) {
                coordResolve();
                coordResolve = null;
            }
            
            if (homingInProgress) {
                homingInProgress = false;
                autohomeStatus.style.display = 'none';
                autohomeBtn.innerHTML = '<i class="fas fa-home"></i> Auto Home';
                autohomeBtn.disabled = false;
                showNotification('AutoHome completed successfully', 'autohome');
                updateControls();
            }
            
            const waitingElements = document.querySelectorAll('.waiting');
            waitingElements.forEach(el => {
                el.classList.remove('waiting', 'manual-waiting', 'coord-waiting', 'test-waiting', 'autohome-waiting');
            });
        }
    };
    
    socket.onclose = () => {
        console.log('WebSocket disconnected');
        connectionDot.classList.add('disconnected');
        showNotification('WebSocket disconnected. Reconnecting...', 'warning');
        setTimeout(connectWebSocket, 5000);
    };
    
    socket.onerror = (error) => {
        console.error('WebSocket error:', error);
        showNotification('WebSocket error: ' + error.message, 'error');
    };
}

connectWebSocket();

async function send(cmd) {
    try {
        console.log(`Sending command to server: ${cmd}`);
        const response = await fetch(API_URL, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ cmd })
        });
        
        if (!response.ok) {
            const errorText = await response.text();
            throw new Error(`Server responded with ${response.status}: ${errorText}`);
        }
        
        console.log(`Command sent successfully: ${cmd}`);
    } catch (error) {
        console.error('Error sending command to server:', error);
        showNotification(`Failed to send command: ${error.message}`, 'error');
        throw error;
    }
    
    if (cmd.startsWith("COORD:")) {
        return new Promise((resolve) => {
            coordResolve = resolve;
        });
    }
    
    return Promise.resolve();
}

function updateControls() {
    const activated = activateToggle.checked;
    
    const indicators = document.querySelectorAll('.status-indicator');
    indicators.forEach(indicator => {
        if (activated) {
            indicator.parentElement.parentElement.classList.add('active-controls');
        } else {
            indicator.parentElement.parentElement.classList.remove('active-controls');
        }
    });
    
    const disableDuringHoming = homingInProgress;
    
    testFieldset.classList.toggle('disabled', !activated || disableDuringHoming);
    manualFieldset.classList.toggle('disabled', !activated || testToggle.checked || disableDuringHoming);
    coordFieldset.classList.toggle('disabled', !activated || testToggle.checked || manualActive || disableDuringHoming);
    document.getElementById('clearHistory').disabled = disableDuringHoming;
    document.getElementById('exportHistory').disabled = disableDuringHoming;
    
    manualBtn.textContent = manualActive ? 'Exit Manual Mode' : 'Enter Manual Mode';
    manualBtn.classList.toggle('btn-danger', manualActive);
    manualBtn.classList.toggle('btn', !manualActive);
}

function initControls() {
    updateControls();
    
    activateToggle.addEventListener('change', handleActivation);
    testToggle.addEventListener('change', handleTestToggle);
    manualBtn.addEventListener('click', handleManualToggle);
    coordForm.addEventListener('submit', handleCoordinateSubmit);
    
    leftBtn.addEventListener('click', () => moveManual(-manualStep));
    rightBtn.addEventListener('click', () => moveManual(manualStep));
    
    resetBtn.addEventListener('click', () => {
        manualX = 0;
        updateManualDisplay();
        send(`COORD:0.00,0.00`)
            .then(() => addHistoryEntry(0, 0, 'Manual Reset'))
            .catch(error => console.error('Reset error:', error));
    });
    
    clearHistory.addEventListener('click', clearHistoryLog);
    exportHistory.addEventListener('click', exportHistoryToCSV);
    
    autohomeBtn.addEventListener('click', handleAutoHome);
}

async function handleAutoHome() {
    if (!activateToggle.checked) {
        showNotification('Please activate the system first', 'warning');
        return;
    }
    
    if (homingInProgress) return;
    
    homingInProgress = true;
    autohomeBtn.innerHTML = '<i class="fas fa-spinner fa-spin"></i> Homing...';
    autohomeBtn.disabled = true;
    autohomeStatus.style.display = 'flex';
    
    const systemCard = document.querySelector('.card.active-controls');
    systemCard.classList.add('waiting', 'autohome-waiting');
    
    updateControls();
    
    try {
        await send('AUTOHOME');
    } catch (error) {
        console.error('AutoHome error:', error);
        homingInProgress = false;
        autohomeBtn.innerHTML = '<i class="fas fa-home"></i> Auto Home';
        autohomeBtn.disabled = false;
        autohomeStatus.style.display = 'none';
        systemCard.classList.remove('waiting', 'autohome-waiting');
        showNotification('AutoHome failed: ' + error.message, 'error');
        updateControls();
    }
}

async function handleActivation() {
    try {
        await send(activateToggle.checked ? 'ACTIVATE_ON' : 'ACTIVATE_OFF');
        if (!activateToggle.checked) {
            manualActive = false;
            testToggle.checked = false;
            manualControls.style.display = 'none';
            cycleStatus.textContent = '';
            testProgress.style.width = '0%';
            homingInProgress = false;
            autohomeStatus.style.display = 'none';
            autohomeBtn.innerHTML = '<i class="fas fa-home"></i> Auto Home';
            autohomeBtn.disabled = false;
        }
        updateControls();
    } catch (error) {
        console.error('Activation error:', error);
        showNotification('Activation failed: ' + error.message, 'error');
    }
}

async function handleTestToggle() {
    if (testToggle.checked) {
        if (manualActive) {
            manualActive = false;
            await send('MANUAL_OFF');
            manualControls.style.display = 'none';
        }
        await send('TEST_ON');
        updateControls();
        runTestSequence();
    } else {
        await send('TEST_OFF');
        testRunning = false;
        updateControls();
    }
}

async function handleManualToggle() {
    if (testToggle.checked) return;
    try {
        manualActive = !manualActive;
        await send(manualActive ? 'MANUAL_ON' : 'MANUAL_OFF');
        manualControls.style.display = manualActive ? 'block' : 'none';
        updateManualDisplay();
        updateControls();
    } catch (error) {
        console.error('Manual toggle error:', error);
        showNotification('Failed to toggle manual mode', 'error');
    }
}

function updateManualDisplay() {
    currentXEl.textContent = manualX.toFixed(2);
    currentYEl.textContent = manualY.toFixed(2); 
}

async function sendManualCoord() {
    updateManualDisplay();
    await send(`COORD:${manualX.toFixed(2)},0.00`);
    addHistoryEntry(manualX, 0, 'Manual');
}

async function moveManual(dx) {
    if (!manualActive || isMoving) return;
    
    try {
        isMoving = true;
        manualFieldset.classList.add('waiting', 'manual-waiting');
        
        [leftBtn, rightBtn].forEach(btn => {
            btn.style.opacity = '0.6';
            btn.style.cursor = 'not-allowed';
        });

        manualX += dx;
        manualX = Math.max(0, Math.min(610, manualX));
        
        await sendManualCoord();
    } catch (error) {
        console.error('Move error:', error);
        showNotification('Movement failed: ' + error.message, 'error');
    } finally {
        isMoving = false;
        manualFieldset.classList.remove('waiting', 'manual-waiting');
        
        [leftBtn, rightBtn].forEach(btn => {
            btn.style.opacity = '1';
            btn.style.cursor = 'pointer';
        });
    }
}

async function handleCoordinateSubmit(e) {
    e.preventDefault();
    if (!activateToggle.checked || testToggle.checked || manualActive) return;

    const x = parseFloat(inputX.value);

    if (isNaN(x)) {
        rangeError.textContent = 'Please enter a valid number for X.';
        return;
    }

    if (x < 0 || x > 610) {
        rangeError.textContent = 'X must be between 0 and 610.';
        return;
    }

    try {
        coordFieldset.classList.add('waiting', 'coord-waiting');
        const submitBtn = coordForm.querySelector('button');
        submitBtn.disabled = true;
        
        manualX = x;
        updateManualDisplay();
        rangeError.textContent = '';
        
        await send(`COORD:${x.toFixed(2)},0.00`);
        addHistoryEntry(x, 0, 'Form');
        
        submitBtn.innerHTML = '<i class="fas fa-check"></i> Sent!';
        submitBtn.style.backgroundColor = '#27ae60';
    } catch (error) {
        console.error('Coordinate error:', error);
        const submitBtn = coordForm.querySelector('button');
        submitBtn.innerHTML = '<i class="fas fa-times"></i> Error!';
        submitBtn.style.backgroundColor = '#e74c3c';
        showNotification('Coordinate send failed: ' + error.message, 'error');
    } finally {
        setTimeout(() => {
            const submitBtn = coordForm.querySelector('button');
            submitBtn.innerHTML = '<i class="fas fa-paper-plane"></i> Send Coordinates';
            submitBtn.style.backgroundColor = '';
            submitBtn.disabled = false;
            coordFieldset.classList.remove('waiting', 'coord-waiting');
        }, 2000);
    }
}

function generateTestXCoords(count = 20, min = 0, max = 610, minDistance = 400) {
    const coords = [];
    let lastX = -minDistance - 1; 

    while (coords.length < count) {
        let newX;
        let attempts = 0;
        const MAX_ATTEMPTS = 100;
        
        do {
            newX = Math.random() * (max - min) + min;
            attempts++;
        } while (Math.abs(newX - lastX) < minDistance && attempts < MAX_ATTEMPTS);
        
        if (attempts >= MAX_ATTEMPTS) {
            console.error("Could not find valid position after 100 attempts");
            break;
        }
        
        coords.push({ x: newX, y: 0 });
        lastX = newX;
    }

    return coords;
}

async function runTestSequence() {
    testRunning = true;
    const coords = generateTestXCoords(20);
    disableDuringTest(true);
    
    testFieldset.classList.add('waiting', 'test-waiting');

    let completedCycles = 0;
    
    for (let i = 0; i < coords.length; i++) {
        if (!testRunning) break;
        
        const { x } = coords[i];
        manualX = x;
        updateManualDisplay();
        
        try {
            await send(`COORD:${x.toFixed(2)},0.00`);
            addHistoryEntry(x, 0, 'Test');
            completedCycles++;
            
            const progress = (completedCycles / coords.length) * 100;
            testProgress.style.width = `${progress}%`;
            cycleStatus.textContent = `Test in progress: ${completedCycles}/${coords.length}`;
        } catch (error) {
            console.error(`Test cycle ${i+1} error:`, error);
            showNotification(`Test cycle ${i+1} failed`, 'error');
            break;
        }
    }

    if (testRunning) {
        cycleStatus.textContent = 'Test completed!';
        testToggle.checked = false;
        setTimeout(() => {
            cycleStatus.textContent = '';
            testProgress.style.width = '0%';
        }, 10000);
    }
    
    testFieldset.classList.remove('waiting', 'test-waiting');
    disableDuringTest(false);
    testRunning = false;
}

function disableDuringTest(isRunning) {
    manualBtn.disabled = isRunning;
    inputX.disabled = isRunning;
    coordForm.querySelector('button[type="submit"]').disabled = isRunning;
    testToggle.disabled = isRunning;

    if (!isRunning) {
        testToggle.checked = false;
        updateControls();
    }
}

function addHistoryEntry(x, y, source) {
    const now = new Date();
    const timeString = now.toLocaleTimeString();
    
    if (history.length === 0) {
        emptyHistory.style.display = 'none';
    }
    
    const entry = document.createElement('div');
    entry.className = 'history-entry';
    entry.innerHTML = `
        <div>
            <div class="history-coord">(${x.toFixed(2)}, ${y.toFixed(2)})</div>
            <div class="history-time">${timeString}</div>
        </div>
        <div class="history-source">${source}</div>
    `;
    
    historyContainer.insertBefore(entry, historyContainer.firstChild);
    
    history.unshift({x, y, source, time: now});
    
    if (history.length > 50) {
        history.pop();
        if (historyContainer.children.length > 50) {
            historyContainer.removeChild(historyContainer.lastChild);
        }
    }
}

function clearHistoryLog() {
    history = [];
    historyContainer.innerHTML = '';
    emptyHistory.style.display = 'block';
    showNotification('History cleared', 'success');
}

function exportHistoryToCSV() {
    if (history.length === 0) {
        showNotification('No history to export', 'warning');
        return;
    }
    
    let csvContent = "Time,X Coordinate,Y Coordinate,Source\n";
    
    history.forEach(entry => {
        csvContent += `"${entry.time.toLocaleString()}","${entry.x.toFixed(2)}","${entry.y.toFixed(2)}","${entry.source}"\n`;
    });
    
    const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8;' });
    const url = URL.createObjectURL(blob);
    const link = document.createElement("a");
    link.setAttribute("href", url);
    link.setAttribute("download", "coordinate_history.csv");
    link.style.visibility = 'hidden';
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
    
    showNotification('History exported successfully', 'success');
}

function showNotification(message, type) {
    const existing = document.querySelector('.notification');
    if (existing) existing.remove();
    
    const notification = document.createElement('div');
    notification.className = `notification ${type}`;
    notification.textContent = message;
    
    document.body.appendChild(notification);
    
    setTimeout(() => {
        notification.remove();
    }, 3000);
}

window.addEventListener('DOMContentLoaded', initControls);