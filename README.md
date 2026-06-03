# WRO_FE_CETYS_SEAL_AV

World Robot Olympiad - Future Engineers 2026 - CETYS SEAL AV

Systems Engineering Autonomous Labs     S.E.A.L

We are a team made up of 3 first-year engineering students from Mexicali, Baja California, México, excited to participate in Future Engineers for the first time! Our robot, Seal, aims to complete the 2026 future engineers challenges, we look forward to upgrading our prototypes while sharing our journey along the way.

Read our Build-Blog !! We document every day of our journey, including our challenges and how we overcame them.      

~ Build start date: Monday April 20th, 2026

## Meet the team !!

<img width="580" height="400" alt="Team" src="https://github.com/user-attachments/assets/24d6e1cf-8024-42e7-bde4-daf8793d4bc1" />


| |  |  |
|-----------|-----------|-----------|
| Alejandro Pineda   | Jorge Ibarra    | Yumián Rodríguez    |
| Studying: Electronic Cybernetics Engineering    | Studying:  Computer Science Engineering    | Studying: Electronic Cybernetics Engineering    |
| Excited to obtain a good placement in the 2026 Future Engineers season.  | Wants to travel to an international championship.  | Ready to work on the prototype day and night. |

We also want to thank our team mentor who has been with us from the start, Luis C. Básaca.

## Robot Overview
S.E.A.L.   Dimensions:  Width 10.5cm x  Lenght 17cm x  Height 12.5cm     Weight: 0.4kg

| Top View | Front View | Side View | 
|-----------|-----------|-----------|
| <img width="200" height="200" alt="TopView" src="https://github.com/user-attachments/assets/6b4d195e-cb35-432c-ba67-22a52ca0f535" />|<img width="200" height="200" alt="FrontView" src="https://github.com/user-attachments/assets/2aec13e2-a650-4625-961f-59a297f0c053" />|<img width="200" height="200" alt="SideView" src="https://github.com/user-attachments/assets/1dbee507-dae4-423d-84da-fa039ccfaacb" />|



## Mechanical Systems

- Diferential Rear Wheel Drive
  
| <img width="330" height="330" alt="Diferential" src="https://github.com/user-attachments/assets/5d29e850-e543-474e-83a2-6ab523eda8e3" />| Driven by 1 reduction motor with an approximate 60-1 reduction with lego gears. Currently using lego spike wheels for traction and large diameter. |
|-----------|-----------|

- Servo Motor Whole Axle Steering

| <img width="430" height="430" alt="ServoMotor" src="https://github.com/user-attachments/assets/3c14c4bf-f9b5-4e4c-91f6-41956faba97d" />| Driven by 1 9g servo motor that controls the angle on both our front wheels. We have a conventional whole axle steering mounted directly underneath our servo with a lego coupling shaft.| 
|-----------|-----------|

- Modular Double Decker Chasis

| <img width="780" height="780" alt="DoubleDecker" src="https://github.com/user-attachments/assets/5f28c95c-71c9-457d-838c-210cd75169b5" />| We built our initial chasis out of lego, this allowed us to focus on the electronic and coding aspects of our build for the first few days. Lego actually ended up being a good plattform to build up from, since we had to make quick adjustments on the fly, all of this while helping us creat a modular and organized casing for our electronics. | 
|-----------|-----------|

## Electronic Systems

- Components used

| Component | Model | Quantity | Usage |
|-----------|-----------|-----------|-----------|
| <img width="200" height="200" alt="LipoRiderPlus" src="https://github.com/user-attachments/assets/d6076567-2f07-48d6-8f42-599e3acb56a0" />| Lipo Rider Plus    | 1 | Power Distribution |
|<img width="200" height="200" alt="LipoBattery" src="https://github.com/user-attachments/assets/f3547633-941e-4afc-8189-2a754e9a692d" />| Single Cell 3.7V 1000mAh lipo battery    | 1 | Power Supply for all systems |
| <img width="200" height="200" alt="ESP32" src="https://github.com/user-attachments/assets/4749645a-bd57-4d26-9eb0-98450272df4a" />| ESP32    | 1 | Motor |
|<img width="200" height="200" alt="ESP32shield" src="https://github.com/user-attachments/assets/85fe544b-16e0-4709-b146-7d9299dd8e7b" />| ESP32 Shield    | 1 | Power Distribution |
| <img width="200" height="200" alt="IMUmpu6050" src="https://github.com/user-attachments/assets/87af0059-332a-42a2-8881-e50abf32aacd" />| IMU mpu6050    | 1 | Orientation and Acceleration |
|<img width="200" height="200" alt="Time of Flight Sensor" src="https://github.com/user-attachments/assets/5396ae09-c9c0-4d89-a187-79dc98e2b50c" />| Time of Flight Sensor VL53L0X    | 2 | Wall Sensing |
| <img width="200" height="200" alt="H Bridge DR8833" src="https://github.com/user-attachments/assets/964bc4ab-3846-4f19-8cf4-fb0aab78738e" />| H Bridge DR8833    | 1 | DC Motor Control |
|<img width="200" height="200" alt="DC Reduction Motor" src="https://github.com/user-attachments/assets/d8146747-21d1-4595-b164-c3306654f4f3" />| DC Reduction Motor    | 1 | Movement |
| <img width="200" height="200" alt="Steering Servo" src="https://github.com/user-attachments/assets/c66fa4a6-ed05-48e5-9949-b85584c79852" />| Steering Servo    | 1 | Steering Direction Control |

- IMU

| <img width="570" height="570" alt="Imu" src="https://github.com/user-attachments/assets/810dfbe1-ee96-49cc-806e-220edd4d8505" />| Our IMU(Inertial Measurement Unit) MPU6050, with the use of a sensor, meassures the aceleration of the component. Depending on the I2C protocol, we get the values of 6 acelerations, x y z and angular x y z. With these values and various other algorithms we can calculate the yaw that helps our robot stay centered while turning. |
|-----------|-----------|

- H bridge

| <img width="470" height="470" alt="H_Bridge" src="https://github.com/user-attachments/assets/f5be8ccf-68fd-40e6-a937-e94ae3d800ee" />| We take 5V power directly form our lipo rider, and use to directly power our DC motor. The ESP32 sends a PWM value that stays constantly high for the straight sections of the route, when we need to turn, we lower the speed to obtain a smoother and tighter turn around the edges. | 
|-----------|-----------|

- ESP Shield

| <img width="500" height="500" alt="ESP32andSHIELD" src="https://github.com/user-attachments/assets/b6c032ef-5b4d-4c1f-bfec-00e8c613832a" />| Our ESP32 shield allows us to connect all of our components for the ESP32 to control them, thanks to this, we are also able to send 5V power to our IMU directly for the ESP32. We may change our microcrontroller in the future, but as long as we use the ESP32, the shield is helping us a lot. | 
|-----------|-----------|

- Lipo Rider and Power Supply

| <img width="600" height="600" alt="Lipo" src="https://github.com/user-attachments/assets/af706a99-06d5-4245-9cbd-090e397aaefa" /> | We connect our 3.7V lipo to the Lipo Rider Plus, and use it to power our ESP Shield, through 5V USB A to USB C, and our H Bridge, though 5V and GND pins. We can use the battery for around 2 hours before it need to charge, which takes around half an hour from empty to full. We are looking into getting a double cell 7.4V battery which also has more than 1500 mAh. | 
|-----------|-----------|

- Time of Flight Sensors

| <img width="430" height="430" alt="TimeofFlightSensors" src="https://github.com/user-attachments/assets/901e0d3f-d5c6-48ce-b904-c7872f85e7d7" />| The sensors emit infrared light beams, which helps us read the distance from the robot to the left and right wall. **When initializing more than 1 sensor, we use the XSHUT pin to asign an id to each one of them, add a delay of 100ms beetween each initialization.** | 
|-----------|-----------|

## Code and Sensing Logic

(**TRADUCTION PENDING**)

Al iniciar nuestro código empieza un ciclo de calibración donde hace 2000 lecturas de nuestra imu con el algoritmo de SensorFusion de xioTecnologies, de esta manera guardamos la media de diferencia a los 180 grados de las lecturas y lo colocamos como un offset en los 6 ejes, la aceleración en el eje x y y z y la aceleración angular en el eje x y y z.

Después empezamos el movimiento del motor. Este nos causo algunos problemas durante el desarrollo de este robot porque no se pueden generar 2 señales pwm por el mismo canal interno de la esp32, es por eso que cuando queremos mover el motor lo hacemos con "analogWrite" y cuando queremos mover el servo lo hacemos con "ledcWrite" Esta fue nuestra solución y se las recomendamos.  

El control es la parte más importante de este proyecto; estamos usando un sistema con doble control, cada uno independiente del otro.

El control del servo. Este está dividido en dos, el 50% del error depende de la diferencia del ángulo actual al ángulo target y el otro 50% de la diferencia de la distancia de los sensores de time to flight a la pared, el target es 0 y se mide de la diferencia del doble de una lectura menos la otra, esto hace que el robot se sitúe al 33% de la pared. Esto se hace en la segunda vuelta cuando ya se sabe hacia dónde va a avanzar. Al inicio, se acomoda en la mitad.

Flujo del PID

<img width="420" height="500" alt="image" src="https://github.com/user-attachments/assets/dc7ae5a2-db3e-4662-b6e9-9c0b4839a278" />


Durante el control del robot tenemos una condición que es girando, entonces si la lectura del sensor de distancia a los lados lee más de 50 centímetros de distancia detecta como una esquina y la condición se convierte en true, y gira hacia el lado de detecte el sensor. El giro se realiza hasta que el ángulo de la lectura de la imu sea igual a +- 90. y se agrega un 1 al contador de giros.


La rutina termina cuando el contador de giros es igual a 12.






