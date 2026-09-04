<img width="1000" height="163" alt="ocean-ezgif com-crop" src="https://github.com/user-attachments/assets/6f247592-862b-4f81-bef1-7ff0c6155132" />

# WRO_FE_CETYS_SEAL_AV 🪛🦭

World Robot Olympiad - Future Engineers 2026 - CETYS SEAL AV

Systems Engineering Autonomous Labs     S.E.A.L

We are a team made up of 3 second-year engineering students from Mexicali, Baja California, México, excited to participate in Future Engineers for the first time! Our robot, Seal, aims to complete the 2026 future engineers challenges, we look forward to upgrading our prototypes while sharing our journey along the way.

Read our Build-Blog !! Where we document our journey, including our challenges and how we overcame them.      

~ Build start date: Monday April 20th, 2026

## Meet the team !! 🫂
<div align="center">
<img width="800" height="600" alt="image" src="https://github.com/user-attachments/assets/0e736cc1-87b3-47f1-9623-8f7026765156" />

</div>

| |  |  |
|-----------|-----------|-----------|
| Yumián Rodríguez  | Alejandro Pineda  | Jorge Ibarra   |
| Studying: Electronic Cybernetics Engineering    | Studying: Electronic Cybernetics Engineering    | Studying:  Computer Science Engineering |
| Excited to obtain a good placement in the 2026 Future Engineers season.  | Wants to travel to an international championship.  | Ready to work on the prototype day and night. |

We also want to thank our team mentor who has been with us from the start, Luis C. Básaca.

<img width="1050" height="8" alt="image" src="https://github.com/user-attachments/assets/520cc265-1a65-4e8e-a80e-7458a93457be" />



## Robot Overview 🤖 🏎️

Click the preview image below to rotate, zoom, and explore the 3D model for our robot directly in your browser:

[![Interact with 3D Model](https://github.com/user-attachments/assets/73407719-86bf-43be-ad00-6f52b11017eb)](https://sketchfab.com/3d-models/girthy-e7a50c23a9f7431e98fa3d997f9cdc22) 

S.E.A.L.   Dimensions:  Width 10.5cm x  Lenght 17cm x  Height 12.5cm     Weight: 0.4kg

| Top View | Front View | Side View | 
|-----------|-----------|-----------|
| <img width="300" height="300" alt="image" src="https://github.com/user-attachments/assets/70552900-3139-42ac-9373-90f4a26d7d7d" />|<img width="300" height="300" alt="image" src="https://github.com/user-attachments/assets/54733bae-d69e-49d2-b38a-4dc7c6958a88" />|<img width="300" height="300" alt="image" src="https://github.com/user-attachments/assets/f0cd3352-f1b6-4f10-aa2f-ffc62dc452ec" />|

You can find more pictures in our [v-photos
](https://github.com/Alejandrop22/WRO_FE_CETYS_SEAL_AV/tree/main/v-photos) 📸📸

### Components Used ⚡
Find a detailed diagram in [schemes](https://github.com/Alejandrop22/WRO_FE_CETYS_SEAL_AV/tree/main/schemes)

| Component | Model | Quantity | Usage |
|-----------|-----------|-----------|-----------|
| <img width="200" height="200" alt="LipoRiderPlus" src="https://github.com/user-attachments/assets/d6076567-2f07-48d6-8f42-599e3acb56a0" />| Lipo Rider Plus    | 1 | Power Distribution |
|<img width="200" height="200" alt="LipoBattery" src="https://github.com/user-attachments/assets/f3547633-941e-4afc-8189-2a754e9a692d" />| Single Cell 3.7V 1000mAh lipo battery    | 1 | Power Supply for all systems |
| <img width="200" height="200" alt="ESP32" src="https://github.com/user-attachments/assets/4749645a-bd57-4d26-9eb0-98450272df4a" />| ESP32    | 1 | Motor |
|<img width="200" height="200" alt="ESP32shield" src="https://github.com/user-attachments/assets/85fe544b-16e0-4709-b146-7d9299dd8e7b" />| ESP32 Shield    | 1 | Power Distribution |
| <img width="200" height="200" alt="IMUmpu6050" src="https://github.com/user-attachments/assets/87af0059-332a-42a2-8881-e50abf32aacd" />| IMU mpu6050    | 1 | Orientation and Acceleration |
|<img width="200" height="200" alt="image" src="https://github.com/user-attachments/assets/2a38df9b-1fc6-4cae-b788-cc98c7928bd1" />| Time of Flight Sensor VL53L0X    | 1 | Front Sensing |
|<img width="200" height="200" alt="image" src="https://github.com/user-attachments/assets/009884c1-34d4-44d9-a8ec-a5e001456b03" />| Time of Flight Sensor VL53L1X  | 2 | Wall Sensing |
| <img width="200" height="200" alt="H Bridge DR8833" src="https://github.com/user-attachments/assets/964bc4ab-3846-4f19-8cf4-fb0aab78738e" />| H Bridge DR8833    | 1 | DC Motor Control |
|<img width="200" height="200" alt="DC Reduction Motor" src="https://github.com/user-attachments/assets/d8146747-21d1-4595-b164-c3306654f4f3" />| DC Reduction Motor    | 1 | Movement |
| <img width="200" height="200" alt="Steering Servo" src="https://github.com/user-attachments/assets/c66fa4a6-ed05-48e5-9949-b85584c79852" />| Steering Servo    | 1 | Steering Direction Control |
| <img width="200" height="200" alt="image" src="https://github.com/user-attachments/assets/33122929-928d-4fd0-982b-b70a2e55db85" />| ESP32 S3    | 1 | Camera Module  |
| <img width="200" height="200" alt="image" src="https://github.com/user-attachments/assets/01845506-fd29-4965-abcb-02d0c9a7cb5c" />| OV3660 3MP 24 Pin     | 1 | Camera |
| <img width="200" height="200" alt="image" src="https://github.com/user-attachments/assets/21ec1d07-71d3-4683-ad10-65a71d7a351d" />| BAOTER 3296 Buck Converter    | 1 | Voltage Control |

Electrical budget

### Electrical Budget ⚡🔋

| Component | Operating Voltage | Typical Current | Peak/Stall Current | Approx. Power (Typ.) |
|---|:---:|:---:|:---:|:---:|
| ESP32 (Motor Control) | 5V (via shield) | 140 mA | ~500 mA (TX) | 0.70 W |
| ESP32-S3 (Camera Module) | 5V (via USB) | 300 mA | ~500 mA (cam) | 1.50 W |
| OV3660 Camera | 3.3V (on-module reg.) | 120 mA | 140 mA | 0.40 W |
| IMU MPU6050 | 3.3–5V | 4 mA | 4 mA | 0.02 W |
| VL53L0X (Front ToF) | 3.3–5V (on-module reg.) | 20 mA | 40 mA (ranging) | 0.10 W |
| VL53L1X ×2 (Wall ToF) | 3.3–5V (on-module reg.) | 20 mA each (40 mA total) | 40 mA each (80 mA total) | 0.20 W |
| DRV8833 (H-Bridge logic) | 5V logic | 2 mA | 2 mA | 0.01 W |
| N20 DC Motor + Gearbox | 5V | 200 mA | ~900 mA (stall) | 1.00 W |
| 9g Steering Servo | 5V | 150 mA | ~700 mA (stall) | 0.75 W |
| **Total (typical driving)** | **5V bus** | **~976 mA** | **~2.9 A (worst-case)** | **~4.9 W** |
| Lipo Battery | 3.7V, 1000 mAh | — | — | 3.7 Wh capacity |
| Lipo Rider Plus (boost) | 3.7V → 5V | ~85–90% eff. | — | — |

**Notes:**
- Values are typical datasheet/estimated figures, not measured — recommend validating with an inline multimeter/USB power meter for your final report.
- "Peak/Stall" column assumes worst case (motor stall, servo stall, WiFi TX burst) — unlikely to occur simultaneously, but useful for battery/regulator sizing.
- At ~1A typical draw and a 3.7V→5V boost with ~85% efficiency, battery-side current is closer to ~1.3A, which lines up with your observed ~2h runtime on the 1000mAh cell.
- Consider this a soft argument for the double-cell 7.4V/1500mAh upgrade you mentioned — it would roughly double runtime margin against peak draws.

### Connections Diagram

////////////////////////////////// Esquematico ///////////////////////////////

### Usage Descriptions 🔍🔍
|   |    |   | 
| :--- | :--- | :--- |
| <img width="470" height="470" alt="H_Bridge" src="https://github.com/user-attachments/assets/f5be8ccf-68fd-40e6-a937-e94ae3d800ee" /> | **H-Bridge** | We take 5V power form our lipo rider, and use to directly power our DC motor. The ESP32 sends a PWM value that stays high for the straight sections of the route. While when we need to turn, we lower the speed to obtain a smoother and tighter turn around the edges. |
| <img width="500" height="500" alt="ESP32andSHIELD" src="https://github.com/user-attachments/assets/b6c032ef-5b4d-4c1f-bfec-00e8c613832a" /> | **ESP Shield** | We connect all of our components to our ESP32 shield so it can directly control them, thanks to this, we are also able to send 5V power to our IMU straight from the ESP32. We may change our microcrontroller in the future, but as long as we use the ESP32, the shield is of great importance. |
| <img width="600" height="600" alt="Lipo" src="https://github.com/user-attachments/assets/af706a99-06d5-4245-9cbd-090e397aaefa" /> | **Lipo Rider and Power Supply** | We connect our 3.7V lipo to the Lipo Rider Plus. With this we power many comments, like the ESP Shield, which connects from a 5V USB A to USB C and our H Bridge connects through 5V and GND pins. The battery life lasts an average of 2 hours per use meanwhile recharging takes around half an hour. We are looking into getting a double cell 7.4V battery which also has more than 1500 mAh. |
| <img width="430" height="430" alt="TimeofFlightSensors" src="https://github.com/user-attachments/assets/901e0d3f-d5c6-48ce-b904-c7872f85e7d7" /> | **Time of Flight Sensors** | The sensors emit infrared light beams, which helps us read the distance from the robot to the side walls. **When initializing more than 1 sensor, we use the XSHUT pin to asign an id to each one of them, then we add a delay of 100ms between each initialization.** |

<img width="1050" height="8" alt="image" src="https://github.com/user-attachments/assets/520cc265-1a65-4e8e-a80e-7458a93457be" />

## Mechanical Systems ⚙️
Find our material list and step by step instructions on how to build our model in [models](https://github.com/Alejandrop22/WRO_FE_CETYS_SEAL_AV/blob/main/models/README.md)

- Diferential Rear Wheel Drive

| Isometric View | System Description |
|:---|:---|
|<img width="933" height="518" alt="image" src="https://github.com/user-attachments/assets/4823358c-8f17-4d39-a4f9-9832dac28742" />| The drivetrain employs a two-stage gear reduction architecture to optimize torque delivery from the N20 motor to the differential. The motor, rated at 1500 RPM, drives a 12-tooth pinion gear that meshes with a 20-tooth primary gear, yielding an initial reduction ratio of 1.67:1. This primary gear is rigidly mounted on an intermediate shaft alongside a secondary 12-tooth pinion, which in turn drives a 36-tooth final gear attached to the differential input shaft. This second stage provides an additional 3:1 reduction, resulting in a combined gear reduction of 5:1 and bringing the differential input speed down to approximately 300 RPM — an ideal balance between wheel torque and maneuverability for the robot's intended operating conditions. |


| Side View | Top View |
|:---:|:---:|
|<img width="702" height="418" alt="image" src="https://github.com/user-attachments/assets/1d79958a-c2e3-45f8-9c50-c2fb5e2d5b9a" />|<img width="581" height="418" alt="image" src="https://github.com/user-attachments/assets/11a22165-6610-4fa6-8fc0-08b5325c58ac" />|

| Component | Teeth | Reduction Ratio |
|:---|:---:|---:|
| Motor Pinion → Primary Gear | 12T → 20T | 1.67:1 |
| Secondary Pinion → Final Gear | 12T → 36T | 3.00:1 |
| **Total Reduction** | — | **5.00:1** |

- Servo Motor Whole Axle Steering

| Isometric View | System Description |
|:---|:---|
|<img width="510" height="593" alt="image" src="https://github.com/user-attachments/assets/ec33e47b-6745-43d5-bac4-edc6fe643576" />| Steering is handled by a single 9g servo motor that actuates both front wheels through a conventional whole-axle mechanism. Mounted directly above the axle, the servo transmits motion via a LEGO coupling shaft to a central linkage, synchronizing the angle of both wheels for precise turning. This minimalist approach keeps weight low and response times quick, while the LEGO shaft simplifies prototyping and maintenance without sacrificing reliability. |

| Side View | Front View |
|:---:|:---:|
|<img width="227" height="300" alt="image" src="https://github.com/user-attachments/assets/77b5ed1a-49d4-4341-af4b-fb1fcbc3577f" />|<img width="234" height="300" alt="image" src="https://github.com/user-attachments/assets/1065b3d7-e7e6-4540-8efa-5584a762f21a" />|

- Modular Double Decker Chasis

| Isometric View | System Description |
|:---|:---|
|<img width="792" height="660" alt="image" src="https://github.com/user-attachments/assets/24323000-3cb6-4d67-bc8c-17516cf0f2a2" />| The robot's structural foundation is built around a double-decker LEGO chassis, designed to house electronic components on an elevated upper deck while the drivetrain and steering mechanisms occupy the lower level. This two-tiered configuration was initially adopted as a rapid prototyping solution, allowing the team to prioritize electronics integration and code development during the early stages of the build. However, LEGO's modular nature proved to be more than a temporary fix — it enabled quick on-the-fly adjustments without requiring specialized tools or fabrication, while simultaneously promoting a clean, organized layout for the control electronics. The resulting architecture strikes an effective balance between structural rigidity and accessibility, with the upper deck providing convenient access to sensors, microcontrollers, and wiring, all while maintaining a compact footprint suitable for the robot's intended operational environment. |

| Open Chasis | Side View |
|:---:|:---:|
|<img width="706" height="610" alt="image" src="https://github.com/user-attachments/assets/0cafbdde-0143-4406-82ca-a563f11bba73"/>|<img width="928" height="610" alt="image" src="https://github.com/user-attachments/assets/64a199ca-4e56-4807-9f8c-4ab65864125a" />|

## Code and Sensing Logic
### Open Challenge

Our code implements an efficient autonomous navigation strategy for the Open Challenge, designed to complete the course reliably in 45–60 seconds. It uses a combination of distance sensors (VL53L1X), an IMU-free approach (no gyro, only encoder), and a state machine that alternates between wall-following and precise 90° turns. The robot first detects an opening to determine its turning direction, then hugs the inner wall, performing exactly 12 turns before stopping and blinking an LED. The state machine is optimized to minimize sensor switching and computation, activating only the necessary sensor per segment. 

#### State Machine
1. BUSCAR_PARED_INICIAL (Initial Wall Search)
2. GIRO_DERECHA / GIRO_IZQUIERDA (Right/Left Turn)
3. SEGUIR_PARED_DERECHA / SEGUIR_PARED_IZQUIERDA (Follow Right/Left Wall)
4. PARAR_BLINKEAR (Stop and Blink)

Some considerations we took while troubleshooting were adding brief stops, so that our servo could allign properly before and after each turn, as well as an added distance travel for when we finish our first and last turns, to make sure we park in the section of the field we started in.

/////////////////  VIDEO DEMO PLACEHOLDER  ///////////////////////

### Obstacle Challenge

For the obstacle challenge, we brought our second esp32 in (esp32s3 n16r8), which hold our ov3660 used to detect the color of the obstacles we encounter. Currently, we're still in the troubleshooting phase for this challenge, and are yet to tackle parking at the end of our run. When our esp in front detects a blob larger than the target size either green or red, we send a signal through two gpio pins so that the other esp can control the robot to move towards the desired path.

#### Decision Flow
1.- Start in the middle lane, and advance forward using Open Challenge logic.
2.- When obstacle encountered, if it's green, follow wall on left, if it's red, follow wall on right.
3.- If outside corner encountered (front sensor < threshold & wall following sensor < threshold), perform 90° turn.
4.- If following wall on any side, and robot detects obstacle that requieres following wall on other side, perform switch.
5.- Try to keep track of laps as best as possible.

/////////////////  VIDEO DEMO PLACEHOLDER  ///////////////////////





