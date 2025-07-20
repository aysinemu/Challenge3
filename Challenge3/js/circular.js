const API_URL = 'http://localhost:8000/run';

const activateToggle = document.getElementById('activateToggle');
const testToggle     = document.getElementById('testToggle');
const manualBtn      = document.getElementById('manualBtn');
const coordForm      = document.getElementById('coordForm');
const testFieldset   = document.getElementById('testFieldset');
const manualFieldset = document.getElementById('manualFieldset');
const coordFieldset  = document.getElementById('coordFieldset');
const inputX         = document.getElementById('inputX');
const inputY         = document.getElementById('inputY');
const rangeError     = document.getElementById('rangeError');
const manualControls = document.getElementById('manualControls');
const leftBtn        = document.getElementById('leftBtn');
const rightBtn       = document.getElementById('rightBtn');
const upBtn          = document.getElementById('upBtn');
const downBtn        = document.getElementById('downBtn');
const resetBtn       = document.getElementById('resetBtn');
const currentXEl     = document.getElementById('currentX');
const currentYEl     = document.getElementById('currentY');
const cycleStatus    = document.getElementById('cycleStatus');
const testProgress   = document.getElementById('testProgress');
const historyContainer = document.getElementById('historyContainer');
const emptyHistory   = document.getElementById('emptyHistory');
const clearHistory   = document.getElementById('clearHistory');
const exportHistory  = document.getElementById('exportHistory');

let manualActive = false;
let manualX = 0;
let manualY = 0;
let isMoving = false;
const manualStep = 1;
let testRunning = false;
let history = [];

async function send(cmd) {
try {
    const response = await fetch(API_URL, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ cmd })
    });

    console.log('Response status:', response.status);
    console.log('Response headers:', response.headers.get('content-type'));

    if (!response.ok) {
    const text = await response.text();
    console.error(`HTTP error! Status: ${response.status}, Response: ${text.slice(0, 200)}...`);
    return { status: 'error', message: `Server error: ${response.status}` };
    }

    const contentType = response.headers.get('content-type');
    if (!contentType || !contentType.includes('application/json')) {
    const text = await response.text();
    console.error(`Expected JSON, got ${contentType}: ${text.slice(0, 200)}...`);
    return { status: 'error', message: 'Invalid server response format' };
    }

    return await response.json();
} catch (error) {
    console.error('Error sending command:', error);
    return { status: 'error', message: 'Failed to process server response' };
}
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
    
    testFieldset.classList.toggle('disabled', !activated);
    manualFieldset.classList.toggle('disabled', !activated || testToggle.checked);
    coordFieldset.classList.toggle('disabled', !activated || testToggle.checked || manualActive);
    
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
    
    leftBtn.addEventListener('click', () => moveManual(-manualStep, 0));
    rightBtn.addEventListener('click', () => moveManual(manualStep, 0));
    upBtn.addEventListener('click', () => moveManual(0, manualStep));
    downBtn.addEventListener('click', () => moveManual(0, -manualStep));
    
    resetBtn.addEventListener('click', () => {
    manualX = 0;
    manualY = 0;
    updateManualDisplay();
    send(`COORD:0.00,0.00`);
    addHistoryEntry(0, 0, 'Manual Reset');
    });
    
    clearHistory.addEventListener('click', clearHistoryLog);
    exportHistory.addEventListener('click', exportHistoryToCSV);
}

function handleActivation() {
    send(activateToggle.checked ? 'ACTIVATE_ON' : 'ACTIVATE_OFF');
    if (!activateToggle.checked) {
    manualActive = false;
    testToggle.checked = false;
    manualControls.style.display = 'none';
    cycleStatus.textContent = '';
    testProgress.style.width = '0%';
    }
    updateControls();
}

function handleTestToggle() {
    if (testToggle.checked) {
    if (manualActive) {
        manualActive = false;
        send('MANUAL_OFF');
        manualControls.style.display = 'none';
    }
    send('TEST_ON');
    updateControls();
    runTestSequence();
    } else {
    send('TEST_OFF');
    testRunning = false;
    updateControls();
    }
}

function handleManualToggle() {
    if (testToggle.checked) return;
    manualActive = !manualActive;
    send(manualActive ? 'MANUAL_ON' : 'MANUAL_OFF');
    manualControls.style.display = manualActive ? 'block' : 'none';
    updateManualDisplay();
    updateControls();
}

function updateManualDisplay() {
    currentXEl.textContent = manualX.toFixed(2);
    currentYEl.textContent = manualY.toFixed(2);
}

function sendManualCoord(onDone) {
    updateManualDisplay();
    send(`COORD:${manualX.toFixed(2)},${manualY.toFixed(2)}`).then(() => {
    addHistoryEntry(manualX, manualY, 'Manual');
    if (typeof onDone === 'function') onDone();
    });
}

function moveManual(dx, dy) {
    if (!manualActive || isMoving) return;
    isMoving = true;

    [leftBtn, rightBtn, upBtn, downBtn].forEach(btn => {
    btn.style.opacity = '0.6';
    btn.style.cursor = 'not-allowed';
    });

    manualX += dx;
    manualY += dy;

    sendManualCoord(() => {
    setTimeout(() => {
        isMoving = false;
        [leftBtn, rightBtn, upBtn, downBtn].forEach(btn => {
        btn.style.opacity = '1';
        btn.style.cursor = 'pointer';
        });
    }, 300);
    });
}

function handleCoordinateSubmit(e) {
    e.preventDefault();
    if (!activateToggle.checked || testToggle.checked || manualActive) return;

    const x = parseFloat(inputX.value);
    const y = parseFloat(inputY.value);

    if (isNaN(x) || isNaN(y)) {
    rangeError.textContent = 'Please enter valid numbers for both X and Y.';
    return;
    }

    manualX = x;
    manualY = y;
    updateManualDisplay();
    rangeError.textContent = '';
    
    const submitBtn = coordForm.querySelector('button');
    const originalText = submitBtn.innerHTML;
    
    send(`COORD:${x.toFixed(2)},${y.toFixed(2)}`)
    .then(() => {
        addHistoryEntry(x, y, 'Form');
        
        submitBtn.innerHTML = '<i class="fas fa-check"></i> Sent!';
        submitBtn.style.backgroundColor = '#27ae60';
        setTimeout(() => {
        submitBtn.innerHTML = originalText;
        submitBtn.style.backgroundColor = '';
        }, 2000);
    })
    .catch(error => {
        submitBtn.innerHTML = '<i class="fas fa-times"></i> Error!';
        submitBtn.style.backgroundColor = '#e74c3c';
        setTimeout(() => {
        submitBtn.innerHTML = originalText;
        submitBtn.style.backgroundColor = '';
        }, 2000);
    });
}

function degToRad(deg) {
    return deg * Math.PI / 180;
}

function generateTestCoords(count = 20, radius = 40) {
    const angles = [];
    let lastAngle = Math.floor(Math.random() * 360);

    while (angles.length < count) {
    let newAngle = (lastAngle + 90 + Math.floor(Math.random() * 90)) % 360;
    angles.push(newAngle);
    lastAngle = newAngle;
    }

    return angles.map(angle => {
    const rad = degToRad(angle);
    return {
        x: Math.cos(rad) * radius,
        y: Math.sin(rad) * radius
    };
    });
}

async function runTestSequence() {
    testRunning = true;
    const coords = generateTestCoords(20);
    disableDuringTest(true);

    for (let i = 0; i < coords.length; i++) {
    if (!testRunning) break;
    
    const { x, y } = coords[i];
    manualX = x;
    manualY = y;
    updateManualDisplay();
    
    const progress = ((i + 1) / coords.length) * 100;
    testProgress.style.width = `${progress}%`;
    
    await send(`COORD:${x.toFixed(2)},${y.toFixed(2)}`);
    addHistoryEntry(x, y, 'Test');
    
    cycleStatus.textContent = `Test in progress: ${i + 1}/${coords.length}`;
    await new Promise(resolve => setTimeout(resolve, 1500));
    }

    if (testRunning) {
    cycleStatus.textContent = 'Test completed!';
    testToggle.checked = false;
    setTimeout(() => {
        cycleStatus.textContent = '';
        testProgress.style.width = '0%';
    }, 10000);
    }
    
    disableDuringTest(false);
    testRunning = false;
}

function disableDuringTest(isRunning) {
    manualBtn.disabled = isRunning;
    inputX.disabled = isRunning;
    inputY.disabled = isRunning;
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
    notification.style.cssText = `
    position: fixed;
    top: 20px;
    right: 20px;
    padding: 15px 25px;
    border-radius: 6px;
    background: ${type === 'error' ? 'var(--danger)' : type === 'warning' ? 'var(--warning)' : 'var(--success)'};
    color: white;
    box-shadow: 0 5px 15px rgba(0,0,0,0.3);
    z-index: 1000;
    animation: slideIn 0.3s, fadeOut 0.5s 2.5s;
    `;
    
    document.body.appendChild(notification);
    
    setTimeout(() => {
    notification.remove();
    }, 3000);
}

const style = document.createElement('style');
style.textContent = `
    @keyframes slideIn {
    from { transform: translateX(100px); opacity: 0; }
    to { transform: translateX(0); opacity: 1; }
    }
    @keyframes fadeOut {
    from { opacity: 1; }
    to { opacity: 0; }
    }
`;
document.head.appendChild(style);

window.addEventListener('DOMContentLoaded', initControls);