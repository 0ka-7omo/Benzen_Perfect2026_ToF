#include "pico_copter.hpp"
#include "modules/tof/tof_bridge.hpp"
extern float Phi, Theta; // ToFの値を角度補正するために姿勢データを使う

//グローバル変数
uint8_t Arm_flag=0;
uint8_t Red_flag = 0;
semaphore_t sem;


int main(void)
{
  int start_wait=5;
  
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  printf("#start setup\r\n"); 

  //Initialize stdio for Pico
  stdio_init_all();
  printf("#init pico\r\n");

  //adc initial
  adc_init();
  adc_gpio_init(26);
  adc_gpio_init(27);

  //Initialize LSM9DS1
  imu_mag_init();
  printf("#init gyro\r\n");

  //Initialize Radio
  radio_init();
  printf("#init radio\r\n");

  //Initialize Variavle
  variable_init();
  printf("#init cariavle\r\n");

  //Initilize Control
  control_init();
  // Kalman_init();
  // initialize_Altitude();
  // printf("#init control\r\n");

  // //RGB LED off
  rgbled_off();
  
  //Initialize PWM
  //Start 400Hz Interval
  ESC_calib= 0;
  pwm_init();
  printf("#init pwm\r\n");

  while(start_wait)
  {
    start_wait--;
    printf("#Please wait %d[s]\r",start_wait); 
    sleep_ms(1000);
  }
  printf("\n");
 
  //マルチコア関連の設定
  sem_init(&sem, 0, 1);
  multicore_launch_core1(angle_control);  

  //ToFセンサの初期化
  tof_setup();

  Arm_flag=1;
  
  while(1) 
  {
    // ToFセンサから値を取得 
    tof_poll(); 
    uint16_t z_mm = 0; 
    bool z_ok = tof_read_valid(&z_mm); 
    
    // 1秒に1回だけシリアルモニタに出力する（洪水防止） 
    static uint32_t last_print_us = 0; 
    uint32_t now = time_us_32(); 
    if ((now - last_print_us) > 1000000) { 
      last_print_us = now; 
      if (z_ok) { 
        float corrected_z = (float)z_mm * cosf(Phi) * cosf(Theta); 
        printf("TOF Raw: %4u mm | Corrected: %4.1f mm\r\n", z_mm, corrected_z); 
      } else { 
        printf("TOF Raw: NA\r\n"); 
      } 
    }
    
    tight_loop_contents(); 
    while (Logoutputflag==1){ 
      log_output(); 
    } 
  }  
  return 0;
}
