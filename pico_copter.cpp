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

  // //adc initial
  // adc_init();
  // adc_gpio_init(26);
  // adc_gpio_init(27);

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
  Kalman_init();
  // initialize_Altitude();
  printf("#init control\r\n");

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
  // tof_setup();
  initialize_Altitude();
  printf("#init tof\r\n");
  
  Arm_flag=1;
  
  while(1) 
  {
    // ToFセンサから値を取得 
    // tof_poll(); 
    // static uint32_t sw_print_time = 0;
    // if (time_us_32() - sw_print_time > 500000) { 
    //     sw_print_time = time_us_32();
    //     extern uint16_t Chdata[]; 
        
    //     // Chdata[8] と Chdata[9] の値を表示
    //     // printf("CH8: %4d | CH9: %4d\r\n", Chdata[8], Chdata[9]);
    // }
    
    tight_loop_contents(); 
    while (Logoutputflag==1){ 
      log_output(); 
    } 
  }  
  return 0;
}
