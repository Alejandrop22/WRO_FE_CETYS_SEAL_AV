# WRO_FE_CETYS_SEAL_AV

World Robot Olympiad - Future Engineers 2026 - CETYS SEAL AV

Systems Engineering Autonomous Labs     S.E.A.L

We are a team made up of 3 first-year engineering students from Mexicali, Baja California, México, excited to participate in Future Engineers for the first time! Our robot, Seal, aims to complete the 2026 future engineers challenges, we look forward to upgrading our prototypes while sharing our journey along the way.

Read our Build-Blog !! We document every day of our journey, including our challenges and how we overcame them.      

~ Build start date: Monday April 20th, 2026

## Meet the team !!

![Placeholder](images/Team_Placeholder.png)

|![Placeholder](images/ale_holder.png)  |![Placeholder](images/jorge_holder.png)  |![Placeholder](images/yumi_holder.png)  |
|-----------|-----------|-----------|
| Alejandro Pineda   | Jorge Ibarra    | Yumián Rodríguez    |
| Electronic Cybernetics Engineering    | Computer Science Engineering    | Electronic Cybernetics Engineering    |
| Descript    | Descript    | Descript    |


### **📁 Documentation Evaluation Framework**

<div align="center">

## 📊 **WRO 2025 Engineering Documentation Scoring (30 points total)**

| Scoring Area | Maximum Points | Our Documentation Coverage |
|--------------|----------------|---------------------------|
| **1. Mobility Management** | 4 points | Complete mechanical design, motor selection, steering system, assembly instructions |
| **2. Power & Sense Management** | 4 points | Power systems, sensor integration, wiring diagrams, component specifications |
| **3. Obstacle Management** | 4 points | Navigation algorithms, parking strategies, source code with detailed comments |
| **4. Pictures – Team and Vehicle** | 4 points | Multi-angle vehicle photos, team photos, component labeling |
| **5. Performance Videos** | 4 points | Complete challenge demonstrations with commentary and analysis |
| **6. GitHub Utilization** | 4 points | Version control, structured documentation, regular commits |
| **7. Engineering Factor** | 4 points | Custom design and manufacturing throughout the vehicle |
| **8. Overall Judge Impression** | 2 points | Clear communication enabling easy replication |
| **Total Documentation Score** | **30 points** | **(≈25% of total competition score)** |
</div>

## Robot Overview
S.E.A.L.   Dimensions:  cm x  cm x  cm     Weight:

| Top View | Front View | Side View | 
|-----------|-----------|-----------|
| photo    | photo    | photo
 |

## Mechanical Systems

- Rear Wheel Drive

|(images/motor_sideview.jpg) 
<img width="800" height="1200" alt="motor_underview" src="https://github.com/user-attachments/assets/17e30f8e-fe47-426e-9e7d-5320d4492e8b" />

| Driven by 1 reduction motor with an approximate 60-1 reduction with lego gears. Currently using lego spike wheels for traction and large diameter. | 
|-----------|-----------|

- Servo Motor Ackerman Steering

| <img width="1200" height="1600" alt="image" src="https://github.com/user-attachments/assets/195ab54d-7af2-42ae-b4ed-fc2bcb3fd079" />
 | Driven by 1 9g servo motor that controls the angle on both our front wheels. Using an ackermnan steer lets us take advantage of the servo for tighter turns around the corners. | 
|-----------|-----------|

- Modular Double Decker Chasis

| <img width="1200" height="1600" alt="image" src="https://github.com/user-attachments/assets/f076f8ea-0520-443b-917b-dd9a76d0880f" />
 | We built our initial chasis out of lego, this allowed us to focus on the electronic and coding aspects of our build for the first few days. Lego actually ended up being a good plattform to build up from, since we had to make quick adjustments on the fly, all of this while helping us creat a modular and organized casing for our electronics. | 
|-----------|-----------|

## Electronic Systems

- Components used

| Component | Model | Quantity | Usage |
|-----------|-----------|-----------|-----------|
| photo    | Lipo Rider Plus    | 1 | Power Distribution |
| photo    | Single Cell 3.7V 1000mAh lipo battery    | 1 | Power Supply for all systems |
| photo    | ESP32    | 1 | Motor |
| photo    | ESP32 Shield    | 1 | Power Distribution |

- IMU

| Photo | asdasdasd | 
|-----------|-----------|

- H bridge

| Photo | asdasdasd | 
|-----------|-----------|

- ESP Shield

| Photo | asdasdasd | 
|-----------|-----------|

- Lipo Rider and Power Supply

| Photo | asdasdasd | 
|-----------|-----------|

- Time of Flight Sensors

| Photo | asdasdasd | 
|-----------|-----------|

## Coding and Sensors

Al iniciar nuestro codigo empieza un ciclo de calibracion donde hace 2000 lecturas de nuestra imu con el algoritmo de SensorFusion de xioTecnologies, de esta manera guardamos la media de diferencia a los 180 grados de las lecturas y lo colocamos como un offset en los 6 ejes, la aceleracion en el eje x y y z y la aceleracion angular en el eje x y y z.

despues empezamos el movimiento de el motor y el control de el servo.

<img width="1440" height="1720" alt="image" src="https://github.com/user-attachments/assets/dc7ae5a2-db3e-4662-b6e9-9c0b4839a278" />


El control es la parte mas importante de este proyecto, estamos usando un sistema con doble control cada uno independiente de el otro. El primero es un pid que  



