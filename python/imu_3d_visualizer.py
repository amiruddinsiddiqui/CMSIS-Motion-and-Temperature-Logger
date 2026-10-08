import serial
import time
import math
import re
from vpython import box, cylinder, compound, vector, rate, scene, color, label

# ================= CONFIGURATION =================
SERIAL_PORT = 'COM4'   # Update to match your STM32 COM port
BAUD_RATE   = 115200   # Match your UART baud rate
# =================================================

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
    print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud.")
except Exception as e:
    print(f"Error opening serial port: {e}")
    exit()

# ----------------- 3D SCENE SETUP -----------------
scene.title = "STM32 MPU6500 Real-Time 3D Visualizer"
scene.width = 1000
scene.height = 700
scene.forward = vector(-1, -0.5, -1)
scene.up = vector(0, 1, 0)
scene.background = color.gray(0.1)

PLANE_LENGTH = 4.0
fuselage  = cylinder(pos=vector(-2, 0, 0), axis=vector(PLANE_LENGTH, 0, 0), radius=0.35, color=color.gray(0.8))
main_wing = box(pos=vector(0, 0, 0), length=1.0, height=0.08, width=5.0, color=color.blue)
tail_fin  = box(pos=vector(-1.7, 0.6, 0), length=0.8, height=0.9, width=0.08, color=color.red)
elevator  = box(pos=vector(-1.7, 0.1, 0), length=0.6, height=0.06, width=1.8, color=color.red)

airplane = compound([fuselage, main_wing, tail_fin, elevator])
telemetry_label = label(pos=vector(0, 2.5, 0), text="Waiting for telemetry...", height=16, box=False)

# Smooth Angle State Variables
curr_roll, curr_pitch, curr_yaw = 0.0, 0.0, 0.0
target_roll, target_pitch, target_yaw = 0.0, 0.0, 0.0
integrated_yaw = 0.0
temp_val = 0.0

last_time = time.time()
ser.reset_input_buffer()

while True:
    rate(60) # Render frame loop at 60 FPS

    # 1. Parse incoming UART line
    if ser.in_waiting:
        try:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            if "Ax:" in line:
                vals = re.findall(r"[-+]?\d*\.\d+|\d+", line)
                if len(vals) >= 7:
                    ax, ay, az = float(vals[0]), float(vals[1]), float(vals[2])
                    gx, gy, gz = float(vals[3]), float(vals[4]), float(vals[5])
                    temp_val   = float(vals[6])

                    current_time = time.time()
                    dt = current_time - last_time
                    last_time = current_time

                    # Calculate target orientation angles (radians)
                    # Roll: rotation around X | Pitch: rotation around Z
                    target_roll  = math.atan2(ay, az)
                    target_pitch = math.atan2(ax, math.sqrt(ay**2 + az**2))
                    
                    # Inverted sign on gz integration to fix rudder/yaw direction
                    integrated_yaw -= math.radians(gz) * dt
                    target_yaw   = integrated_yaw

                    # print(f"[RX] Roll: {math.degrees(target_roll):.1f}° | Pitch: {math.degrees(target_pitch):.1f}°")
                    print(f"[RX] Roll: {math.degrees(target_roll):.1f}° | Pitch: {math.degrees(target_pitch):.1f}° | Temp: {temp_val:6.1f}°")
        except Exception:
            pass

    # 2. Smoothly glide current orientation toward target (Easing factor = 0.08)
    curr_roll  += (target_roll  - curr_roll)  * 0.08
    curr_pitch += (target_pitch - curr_pitch) * 0.08
    curr_yaw   += (target_yaw   - curr_yaw)   * 0.08

    # 3. Apply 3D Transformation using Base Vector Rotations
    # Base orientation (Plane nose facing +X, roof facing +Y)
    fwd_vec = vector(PLANE_LENGTH, 0, 0)
    up_vec  = vector(0, 1, 0)

    # Step A: Apply Roll (around X-axis)
    up_vec = up_vec.rotate(angle=curr_roll, axis=vector(1, 0, 0))

    # Step B: Apply Pitch (around Z-axis -> Nose Up/Down)
    fwd_vec = fwd_vec.rotate(angle=curr_pitch, axis=vector(0, 0, 1))
    up_vec  = up_vec.rotate(angle=curr_pitch, axis=vector(0, 0, 1))

    # Step C: Apply Yaw (around Y-axis)
    fwd_vec = fwd_vec.rotate(angle=-curr_yaw, axis=vector(0, 1, 0))
    up_vec  = up_vec.rotate(angle=-curr_yaw, axis=vector(0, 1, 0))

    # Assign rotated vectors to airplane model
    airplane.axis = fwd_vec
    airplane.up   = up_vec

    # 4. Telemetry Text Update
    telemetry_label.text = (
        f"Roll:  {math.degrees(curr_roll):6.1f}°\n"
        f"Pitch: {math.degrees(curr_pitch):6.1f}°\n"
        f"Yaw:   {math.degrees(curr_yaw):6.1f}°\n"
        f"Temp:  {temp_val:6.1f} °C"
    )