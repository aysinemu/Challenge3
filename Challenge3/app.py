from fastapi import FastAPI, Request, Form, HTTPException, WebSocket, WebSocketDisconnect
from fastapi.templating import Jinja2Templates
from fastapi.staticfiles import StaticFiles
from fastapi.responses import HTMLResponse
from fastapi.middleware.cors import CORSMiddleware
from pathlib import Path
from pydantic import BaseModel
import time
import serial
import asyncio
import threading
import queue

SERIAL_PORT = 'COM3'
BAUD_RATE = 9600
WORK_DIR = Path("V:\Company\Challenge3")
STATIC_DIR = WORK_DIR / "static"
CSS_DIR = WORK_DIR / "css"
JS_DIR = WORK_DIR / "js"
TEMP_DIR = STATIC_DIR / "temp"

app = FastAPI(docs_url=None, redoc_url=None)

app.mount("/static", StaticFiles(directory=STATIC_DIR), name="static")
app.mount("/css", StaticFiles(directory=CSS_DIR), name="css")
app.mount("/js", StaticFiles(directory=JS_DIR), name="js")
templates = Jinja2Templates(directory=WORK_DIR / "templates")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

fake_users = {
    "admin": {"password": "1", "position": "admin"},
    "guest": {"password": "1", "position": "lecture"},
}

class ConnectionManager:
    def __init__(self):
        self.active_connections = []
        self.message_queue = queue.Queue()

    async def connect(self, websocket: WebSocket):
        await websocket.accept()
        self.active_connections.append(websocket)

    def disconnect(self, websocket: WebSocket):
        self.active_connections.remove(websocket)

    async def broadcast(self, message: str):
        for connection in self.active_connections:
            try:
                await connection.send_text(message)
            except:
                self.disconnect(connection)

manager = ConnectionManager()

ser = None

def init_serial():
    global ser
    if ser is None or not ser.is_open:
        try:
            ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2)
            time.sleep(2)
            print("Serial port initialized")
            return True
        except Exception as e:
            print(f"Failed to open serial port: {e}")
            return False
    return True

def arduino_reader():
    while True:
        try:
            if ser and ser.is_open and ser.in_waiting:
                line = ser.readline().decode('utf-8').strip()
                print(f"Received from Arduino: {line}")
                
                if "done" in line.lower():
                    manager.message_queue.put("DONE")
                
        except Exception as e:
            print(f"Error reading from serial: {e}")
            time.sleep(1)

async def message_broadcaster():
    while True:
        try:
            if not manager.message_queue.empty():
                message = manager.message_queue.get()
                await manager.broadcast(message)
            await asyncio.sleep(0.1)
        except Exception as e:
            print(f"Broadcaster error: {e}")

@app.on_event("startup")
def startup_event():
    if init_serial():
        print("Serial connection established")
    else:
        print("Failed to establish serial connection")
    
    threading.Thread(target=arduino_reader, daemon=True).start()
    
    asyncio.create_task(message_broadcaster())

@app.on_event("shutdown")
def shutdown_event():
    if ser and ser.is_open:
        ser.close()
        print("Serial port closed")

@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    await manager.connect(websocket)
    try:
        while True:
            await websocket.receive_text()
    except WebSocketDisconnect:
        manager.disconnect(websocket)

@app.get("/")
async def root(request: Request):
    return templates.TemplateResponse("index.html", {"request": request})

@app.get("/run", response_class=HTMLResponse)
async def run(request: Request):
    return templates.TemplateResponse("run.html", {"request": request})

@app.get("/circular", response_class=HTMLResponse)
async def run(request: Request):
    return templates.TemplateResponse("circular.html", {"request": request})

@app.post("/login")
async def login(username: str = Form(...), password: str = Form(...)):
    if username in fake_users and fake_users[username]["password"] == password:
        return {
            "success": True,
            "position": fake_users[username]["position"]
        }
    return {
        "success": False,
        "message": "Wrong user name or pass"
    }

class Command(BaseModel):
    cmd: str

def send_cmd(cmd: str) -> None:
    print(f"Sending to Arduino: {cmd}")
    try:
        if not ser or not ser.is_open:
            if not init_serial():
                raise Exception("Serial port not available")
        
        while ser.in_waiting:
            ser.readline()
        
        ser.write((cmd + '\n').encode('utf-8'))
        print(f"Command sent: {cmd}")
        
    except Exception as e:
        print(f"Error communicating with Arduino: {e}")
        raise

@app.post('/run', status_code=204)
async def send_route(command: Command):
    cmd = command.cmd.strip()
    if not cmd:
        raise HTTPException(status_code=400, detail='Missing `cmd` in JSON payload')
    try:
        send_cmd(cmd)
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))
    return None

if __name__ == "__main__":
    import uvicorn
    uvicorn.run("app:app", host="0.0.0.0", port=8000, reload=True)