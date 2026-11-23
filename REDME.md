# Smart Home Firmware

A modular smart-home firmware project written in C, designed to explore embedded software architecture, interrupt-driven event capture, state machines, and hardware abstraction.

The project currently runs as a host-based simulation on Windows. Its architecture is being developed so that the application logic can later run on an STM32 microcontroller with minimal changes to the higher-level software.

## Current Status

**Work in progress**

The current version implements a simulated smart-home room containing sensors, actuators, and controllers.

The host simulation includes:

- Simulated GPIO registers
- GPIO input/output configuration
- Interrupt enable and pending status
- ISR-like callback registration with per-device context
- Motion-based lighting control
- Smoke detection and sprinkler control
- Temperature-based climate control
- Room-level device composition
- Basic house-level room management
- Static allocation with no dynamic memory

The next major step is migrating the platform-dependent layer from the Windows simulation to STM32.

---

## Architecture

The project separates hardware access, device semantics, control policy, and system composition.

```text
House
└── Room
    ├── Devices
    │   ├── Light
    │   ├── AC
    │   ├── Sprinkler
    │   ├── Motion Detector
    │   ├── Smoke Detector
    │   └── Temperature Sensor
    │
    └── Controllers
        ├── Motion Controller
        ├── Smoke Controller
        └── Climate Controller
                │
                ▼
        Platform / GPIO Layer
```

### Platform Layer

The current GPIO implementation simulates a hardware peripheral using a register-like structure containing:

- Direction register
- Input register
- Output register
- Interrupt enable register
- Interrupt status register

Inputs can be changed using the host simulation, generating an interrupt-like callback when the input changes.

The interrupt handler records a software event, while the main application later processes the event through the relevant controller.

This follows a common embedded pattern:

```text
Hardware event
      ↓
Interrupt / ISR
      ↓
Record event
      ↓
Main execution context
      ↓
Controller state machine
```

---

## Devices

### Light

A GPIO-controlled actuator that can be turned on or off.

### AC

A GPIO-controlled actuator used by the climate controller.

### Sprinkler

A GPIO-controlled actuator controlled by the smoke system.

### Motion Detector

A simulated digital input device.

Changes in its GPIO input trigger an interrupt callback that records a pending motion event.

### Smoke Detector

A simulated digital input device using the same interrupt-driven event capture approach.

### Temperature Sensor

Stores and exposes the current simulated temperature.

A future STM32 version will replace the simulated sensor with a hardware peripheral or external sensor interface.

---

## Controllers

### Motion Controller

The motion controller controls linked lights using a state machine.

States:

```text
MOTION_CONTROL_OFF
MOTION_CONTROL_ON
MOTION_WAITING_OFF
```

Behavior:

```text
Motion detected
      ↓
Light ON

Motion cleared
      ↓
WAITING_OFF
      ↓
Timeout
      ↓
Light OFF
```

If motion returns during the waiting period, the controller returns to the active state.

The current light timeout is:

```text
4000 ms
```

---

### Smoke Controller

The smoke controller uses confirmation and delayed shutdown periods to avoid reacting immediately to short-lived signals.

States:

```text
SMOKE_CONTROL_OFF
SMOKE_CONFIRMING
SMOKE_CONTROL_ON
SMOKE_DELAYED_OFF
```

Behavior:

```text
Smoke detected
      ↓
Confirmation period
      ↓
Confirmed smoke
      ↓
Sprinkler ON

Smoke cleared
      ↓
Delayed shutdown period
      ↓
Sprinkler OFF
```

Current timing:

```text
Smoke confirmation: 2000 ms
Delayed sprinkler shutdown: 5000 ms
```

---

### Climate Controller

The climate controller manages one or more AC devices based on temperature.

It uses hysteresis around a target temperature to prevent rapid switching.

For example, with:

```text
Target temperature = 22°C
Margin = 1°C
```

the controller behaves approximately as:

```text
Temperature >= 23°C → AC ON
Temperature <= 21°C → AC OFF
21°C < Temperature < 23°C → preserve previous state
```

The climate controller can also optionally use room occupancy information from a motion controller.

---

## Room

`Room` owns the devices and controllers associated with a room.

It is responsible for:

- Creating devices
- Creating controllers
- Linking sensors to controllers
- Linking controllers to actuators
- Updating controller state machines
- Reporting room occupancy
- Reporting confirmed smoke alerts
- Calculating average temperature
- Turning all room lights off

Devices are stored using fixed-size arrays.

No dynamic memory allocation is currently used.

---

## House

`House` provides a higher-level container for multiple rooms.

The current implementation supports adding and accessing rooms and updating the complete house.

More house-level policies are planned for later development.

---

## Example Demo

The current `main.c` demonstrates the motion and smoke control paths.

### Motion

```text
Motion detected
      ↓
Interrupt callback runs
      ↓
Software event recorded
      ↓
Room_Update()
      ↓
MotionController processes event
      ↓
Light ON
```

When motion clears:

```text
Motion cleared
      ↓
Controller enters WAITING_OFF
      ↓
4 second timeout
      ↓
Light OFF
```

### Smoke

```text
Smoke detected
      ↓
Interrupt callback
      ↓
Controller enters CONFIRMING
      ↓
2 second confirmation
      ↓
Sprinkler ON
```

After smoke clears:

```text
Smoke cleared
      ↓
Controller enters DELAYED_OFF
      ↓
5 second timeout
      ↓
Sprinkler OFF
```

---

## Project Structure

```text
smart_home/
│
├── controllers/
│   ├── climate_controller.c
│   ├── climate_controller.h
│   ├── motion_controller.c
│   ├── motion_controller.h
│   ├── smoke_controller.c
│   └── smoke_controller.h
│
├── core/
│   ├── event.h
│   ├── event_queue.c
│   └── event_queue.h
│
├── devices/
│   ├── ac.c
│   ├── light.c
│   ├── motion_detector.c
│   ├── smoke_detector.c
│   ├── sprinkler.c
│   └── temperature_sensor.c
│
├── drivers/
│   ├── gpio.c
│   └── gpio.h
│
├── house/
│   ├── house.c
│   ├── house.h
│   ├── room.c
│   └── room.h
│
├── system/
│   ├── system_time.c
│   └── system_time.h
│
├── tests/
│
└── main.c
```

> The event queue implementation is currently experimental and is not part of the active runtime architecture.

---

## Design Decisions

### Static Allocation

The project currently avoids `malloc` and dynamically allocated objects.

Rooms own their devices and controllers directly through fixed-size arrays.

This keeps memory ownership explicit and predictable.

### Interrupt Capture, Cooperative Processing

Sensor input changes are captured immediately through interrupt-style callbacks.

The callback itself performs minimal work and records an event.

The controller logic runs later from the normal execution context.

This avoids placing application policy inside interrupt handlers.

### State-Machine Based Controllers

Timing-dependent behavior is implemented using explicit controller states instead of long blocking operations inside the controller logic.

### Separation of Hardware and Policy

Devices represent hardware semantics, while controllers implement behavior and policy.

For example:

```text
MotionDetector
→ reports whether motion exists

MotionController
→ decides how long a room should still be treated as occupied
```

---

## Building the Current Host Version

The current implementation is designed for Windows and GCC.

Example:

```powershell
gcc main.c (Get-ChildItem controllers,devices,drivers,house,system -Filter *.c -Recurse).FullName -Wall -Wextra -Wpedantic -Werror -o smart_home.exe
```

Run:

```powershell
.\smart_home.exe
```

---

## Roadmap

### 1. Stabilize the Host Version

- Update the existing tests to match the current architecture
- Remove obsolete APIs and unused code
- Restore a fully passing component and integration test suite
- Keep the host implementation as a deterministic test environment

### 2. Portable Platform Layer

Separate platform-dependent code from the application logic.

Target structure:

```text
Core application
      │
      ├── Host platform
      │   ├── simulated GPIO
      │   └── host time
      │
      └── STM32 platform
          ├── GPIO
          ├── EXTI
          └── timers
```

### 3. STM32 Target

Run the same high-level application logic on STM32.

Planned topics:

- GPIO
- EXTI
- NVIC
- Interrupt service routines
- Non-blocking timing
- SysTick / hardware timers

### 4. Wokwi Hardware Demo

Create a virtual hardware setup containing inputs and actuators representing:

- Motion sensor
- Smoke sensor
- Temperature sensor
- Light
- AC
- Sprinkler

### 5. Additional Peripherals

Planned additions include:

- Temperature sensor over I2C or another real peripheral interface
- UART command/status interface

### 6. Testing and Documentation

- Host-based unit tests
- Integration tests
- STM32/Wokwi integration demo
- Architecture diagram
- Controller state-machine diagrams
- Design tradeoff documentation

---

## Goals

The project is intended to demonstrate and develop practical embedded firmware skills including:

- C firmware architecture
- GPIO
- Interrupt handling
- ISR design
- State machines
- Non-blocking control logic
- Hardware abstraction
- Static memory management
- Peripheral integration
- Host-based firmware testing
- STM32 development

---

## Future Direction

The final goal is to keep the high-level smart-home logic independent of the underlying platform.

The same controller and device architecture should be testable on a PC while running against real STM32 peripherals on the embedded target.

```text
                   Shared Application Logic
                           │
               ┌───────────┴───────────┐
               │                       │
          Host Simulation           STM32
               │                       │
        Simulated GPIO           GPIO / EXTI
        Simulated Time           Timers / SysTick
               │                       │
          Automated Tests        Hardware Demo
```