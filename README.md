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
  
| <img width="250" height="350" alt="motor_frontview" src="https://github.com/user-attachments/assets/417764c1-f9d4-4c95-8530-00304cb9c319" /> | Driven by 1 reduction motor with an approximate 60-1 reduction with lego gears. Currently using lego spike wheels for traction and large diameter. |
|-----------|-----------|

- Servo Motor Ackerman Steering

|<img width="300" height="400" alt="servo" src="https://github.com/user-attachments/assets/4aa42ded-26b2-4716-ae23-06af04561040" />| Driven by 1 9g servo motor that controls the angle on both our front wheels. Using an ackermnan steer lets us take advantage of the servo for tighter turns around the corners. | 
|-----------|-----------|

- Modular Double Decker Chasis

|<img width="570" height="800" alt="doubledecker" src="https://github.com/user-attachments/assets/7dd58118-e45a-4dce-a07c-2d63caec1027" />| We built our initial chasis out of lego, this allowed us to focus on the electronic and coding aspects of our build for the first few days. Lego actually ended up being a good plattform to build up from, since we had to make quick adjustments on the fly, all of this while helping us creat a modular and organized casing for our electronics. | 
|-----------|-----------|

## Electronic Systems

- Components used

| Component | Model | Quantity | Usage |
|-----------|-----------|-----------|-----------|
| <img width="200" height="200" alt="LipoRiderPlus" src="https://github.com/user-attachments/assets/d6076567-2f07-48d6-8f42-599e3acb56a0" />| Lipo Rider Plus    | 1 | Power Distribution |
|<img width="200" height="200" alt="LipoBattery" src="https://github.com/user-attachments/assets/f3547633-941e-4afc-8189-2a754e9a692d" />| Single Cell 3.7V 1000mAh lipo battery    | 1 | Power Supply for all systems |
| <img width="200" height="200" alt="ESP32" src="https://github.com/user-attachments/assets/4749645a-bd57-4d26-9eb0-98450272df4a" />| ESP32    | 1 | Motor |
|<img width="200" height="200" alt="ESP32shield" src="https://github.com/user-attachments/assets/85fe544b-16e0-4709-b146-7d9299dd8e7b" />| ESP32 Shield    | 1 | Power Distribution |

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

## Coding and electronica

Electronics

Estamos utilizando una esp 32 base y esta tiene un problema muy grande que no conociamos, al intentar generar la señal pwm del motor y del servo a la vez. La esp32 hacía Brownout entonces creímos que era fallo de nuestro puente H o de nuestra alimentación. Pero este problema resultó estar en código.

Actualmente contamos con 2 sensores distintos, tenemos el mpu 5060 y el vl53l0xv2 conectados en el mismo canal de I2C, el único problema es que al contar con vl53l0xv2 estos tienen la misma dirección, entonces lo que se tiene que hacer es utilizar el pin XSHUT de cada sensor para asignarle una dirección distinta y poder leer ambos durante el código. **Cuidado al iniciar los sensores, tiene que estar el otro XSHUT en low al iniciar y tiene que haber un delay de aproximadamente 100ms entre la inicialización de ambos para que funcione**


C0ding


Al iniciar nuestro código empieza un ciclo de calibración donde hace 2000 lecturas de nuestra imu con el algoritmo de SensorFusion de xioTecnologies, de esta manera guardamos la media de diferencia a los 180 grados de las lecturas y lo colocamos como un offset en los 6 ejes, la aceleración en el eje x y y z y la aceleración angular en el eje x y y z.

Después empezamos el movimiento del motor. Este nos causo algunos problemas durante el desarrollo de este robot porque no se pueden generar 2 señales pwm por el mismo canal interno de la esp32, es por eso que cuando queremos mover el motor lo hacemos con "analogWrite" y cuando queremos mover el servo lo hacemos con "ledcWrite" Esta fue nuestra solución y se las recomendamos.  

El control es la parte más importante de este proyecto; estamos usando un sistema con doble control, cada uno independiente del otro.

El control del servo. Este está dividido en dos, el 50% del error depende de la diferencia del ángulo actual al ángulo target y el otro 50% de la diferencia de la distancia de los sensores de time to flight a la pared, el target es 0 y se mide de la diferencia del doble de una lectura menos la otra, esto hace que el robot se sitúe al 33% de la pared. Esto se hace en la segunda vuelta cuando ya se sabe hacia dónde va a avanzar. Al inicio, se acomoda en la mitad.

Flujo del PID

<img width="420" height="500" alt="image" src="https://github.com/user-attachments/assets/dc7ae5a2-db3e-4662-b6e9-9c0b4839a278" />


Durante el control del robot tenemos una condición que es girando, entonces si la lectura del sensor de distancia a los lados lee más de 50 centímetros de distancia detecta como una esquina y la condición se convierte en true, y gira hacia el lado de detecte el sensor. El giro se realiza hasta que el ángulo de la lectura de la imu sea igual a +- 90. y se agrega un 1 al contador de giros.


El código termina cuando el contador de giros es igual a 16.






