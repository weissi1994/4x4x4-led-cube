#define WAIT 50
#define WORM_SIZE 4

struct xyz{
   int x;
   int y;
   int z;
};

//initializing and declaring led rows
int column[16]={13,12,11,10,9,8,7,6,5,4,3,2,1,0,A5,A4};
//initializing and declaring led layers
int layer[4]={A3,A2,A1,A0};
// worm setup
xyz directions[] = {{-1,0,0},{0,-1,0},{1,0,0},{0,1,0},{0,0,1},{0,0,-1}};
xyz randomDirection;
xyz previousDirection={0,0,0};

// #### MAP ####
//
//  13 | 9 | 5 |  1
// ----|---|---|----
//  12 | 8 | 4 |  0
// ----|---|---|---- 
//  11 | 7 | 3 | A5 
// ----|---|---|----
//  10 | 6 | 2 | A4

void setup() {
  //setting rows to ouput
  for(int i = 0; i<16; i++)
  {
    pinMode(column[i], OUTPUT);
  }
  //setting layers to output
  for(int i = 0; i<4; i++)
  {
    pinMode(layer[i], OUTPUT);
  }
  //seeding random for random pattern
  randomSeed(1337);
}

// PORTB == 5  4  3  2 1 0
//         13 12 11 10 9 8
// PORTC == 5  4  3  2  1  0
//         A5 A4 A3 A2 A1 A0 
// PORTD == 7 6 5 4 3 2 1 0
//          7 6 5 4 3 2 1 0

void loop() {
  randomSeed(1337);
  flickerOn();
  eachBottomUp();
  eachSideBySide();
  randomFlicker();
  worm(100);
  randomRain();
  flickerOff();
}

// Worm Initialisation and basic logic
void worm(int turns){
  allOff();
  xyz worm[WORM_SIZE] = {{0, 0, 0}, {0, 0, 1}, {0, 0, 2}, {0, 0, 3}};
  for(int t = 50*turns; t >= 0; t--){
    if (t % 10 == 0){
      logic(worm);
    }
    flash_leds(worm);
  }
}

// Worm movement
void logic(xyz worm[WORM_SIZE]){
  randomDirection = directions[random(0, 6)];
  // if its a valid direction and not going backwards, do it!
  if(valid(worm[0], randomDirection) && !(inverse(randomDirection,previousDirection))){
    previousDirection = randomDirection;
    make_move(worm, randomDirection);
  }
  else{
    randomDirection = directions[random(3, 6)];
    // if its a valid direction and not going backwards, do it!
    if(valid(worm[0], randomDirection) && !(inverse(randomDirection,previousDirection))){
      previousDirection = randomDirection;
      make_move(worm, randomDirection);
    }
    else{
      for (int i = 0; i < 6; i++){
        xyz nextDir = directions[i];
        if(valid(worm[0], nextDir) && !(inverse(randomDirection,previousDirection))){
          previousDirection = nextDir;
          make_move(worm, nextDir);
          return;
        }
      }
    }
  }
}

// Worm, actually moving
void make_move(xyz worm[WORM_SIZE], xyz direction){
  for(int i = 4; i>=0; i--){
    if (i == 0) {
      worm[i] = move(worm[i], direction);
    } else {
      worm[i] = worm[i-1];
    }
  }
}

// Worm helper
xyz move(xyz now, xyz direction)
{
  xyz next = {now.x + direction.x, now.y + direction.y, now.z + direction.z};
  
  return next;
}

// Worm turn validation
bool valid(xyz now, xyz direction){
    xyz next = move(now, direction);
    if((next.y > 3) || (next.y < 0) || (next.x > 3) || (next.x < 0) || (next.z > 3) || (next.z <0) ){
      return false; // would take us off the board
    }
    //if(next.xz %4 == 0 && direction.xz == 1){
    // return false;
    //} 
    //if(now.xz %4 == 0 && direction.xz == -1){
    //  return false;
    //}
    return true;
}

// Worm inverse check
bool inverse(xyz a,xyz b){
  if((a.x + b.x == 0) && (a.y + b.y == 0) && (a.z + b.z == 0)){
    return true;
  }
  else{
    return false;
  }
}

// Worm flash array of leds
void flash_leds(xyz worm[WORM_SIZE]){
  for(int i = 0; i<WORM_SIZE; i++)
  {
    flashLed(worm[i].x, worm[i].y, worm[i].z, 1); 
  }
}
// End of Worm 

void flickerOff(){
  int i = 1;
  while(i < 150)
  {
    allOn();
    #ifdef WAIT
    delay(i);
    #endif
    allOff();
    #ifdef WAIT
    delay(i);
    #endif
    i+= 5;
  }
}

void randomFlicker(){
  allOff();
  for(int i = 0; i !=250; i+=1)
  {
    int randX = random(0,4);
    int randY = random(0,4);
    int randZ = random(0,4);
    #ifdef WAIT
    flashLed(randX, randY, randZ, WAIT*1.25); 
    #else
    flashLed(randX, randY, randZ); 
    #endif
  }
}

void randomRain(){
  allOff();
  for(int i = 0; i !=150; i+=1)
  {
    int randX = random(0,4);
    int randY = random(0,4);
    for(int z = 3; z >= 0; z--){
      #ifdef WAIT
      flashLed(randX, randY, z, WAIT*1.25); 
      #else
      flashLed(randX, randY, z); 
      #endif
    }
  }
}

void flickerOn()
{
  int i = 151;
  while(i > 0)
  {
    allOn();
    #ifdef WAIT
    delay(i);
    #endif
    allOff();
    #ifdef WAIT
    delay(i);
    #endif
    i-= 5;
  }
}

void eachSideBySide(){
  for(int y = 3; y >= 0; y--) {
    for(int x = 0; x < 4; x++) {
      for(int z = 0; z < 4; z++) {
        #ifdef WAIT
        flashLed(x, y, z, WAIT*1.25); 
        #else
        flashLed(x, y, z); 
        #endif
      } 
    } 
  }
  for(int x = 0; x < 4; x++) {
    for(int y = 3; y >= 0; y--) {
      for(int z = 3; z >= 0; z--) {
        #ifdef WAIT
        flashLed(x, y, z, WAIT*1.25); 
        #else
        flashLed(x, y, z); 
        #endif
      } 
    } 
  }
}

void eachBottomUp(){
  for(int z = 0; z < 4; z++) {
    for(int y = 0; y < 4; y++) {
      for(int x = 0; x < 4; x++) {
        #ifdef WAIT
        flashLed(x, y, z, WAIT*1.25); 
        #else
        flashLed(x, y, z); 
        #endif
      } 
    } 
  }
}

void flashLed(int x, int y, int z, int wait){
  switch(z){
    case 0:
      PORTC |= B00000001;
      flashColumnLed(x, y, wait);
      PORTC &= B11111110;
      break;
    case 1:
      PORTC |= B00000010;
      flashColumnLed(x, y, wait);
      PORTC &= B11111101;
      break;
    case 2:
      PORTC |= B00000100;
      flashColumnLed(x, y, wait);
      PORTC &= B11111011;
      break;
    case 3:
    default:
      PORTC |= B00001000;
      flashColumnLed(x, y, wait);
      PORTC &= B11110111;
  }
}

void flashLed(int x, int y, int z){
  switch(z){
    case 0:
      PORTC |= B00000001;
      #ifdef WAIT
      flashColumnLed(x, y, WAIT);
      #else
      flashColumnLed(x, y);
      #endif
      PORTC &= B11111110;
      break;
    case 1:
      PORTC |= B00000010;
      #ifdef WAIT
      flashColumnLed(x, y, WAIT);
      #else
      flashColumnLed(x, y);
      #endif
      PORTC &= B11111101;
      break;
    case 2:
      PORTC |= B00000100;
      #ifdef WAIT
      flashColumnLed(x, y, WAIT);
      #else
      flashColumnLed(x, y);
      #endif
      PORTC &= B11111011;
      break;
    case 3:
    default:
      PORTC |= B00001000;
      #ifdef WAIT
      flashColumnLed(x, y, WAIT);
      #else
      flashColumnLed(x, y);
      #endif
      PORTC &= B11110111;
  }
}

void flashColumnLed(int x, int y, int wait){
  switch(x){
    case 0:
      switch(y){
        case 0:
          PORTB &= B11011111;
          delay(wait);
          PORTB |= B00100000;
          break;
        case 1:
          PORTB &= B11101111;
          delay(wait);
          PORTB |= B00010000;
          break;
        case 2:
          PORTB &= B11110111;
          delay(wait);
          PORTB |= B00001000;
          break;
        case 3:
        default:
          PORTB &= B11111011;
          delay(wait);
          PORTB |= B00000100;
      }
      break;
    case 1:
      switch(y){
        case 0:
          PORTB &= B11111101;
          delay(wait);
          PORTB |= B00000010;
          break;
        case 1:
          PORTB &= B11111110;
          delay(wait);
          PORTB |= B00000001;
          break;
        case 2:
          PORTD &= B01111111;
          delay(wait);
          PORTD |= B10000000;
          break;
        case 3:
        default:
          PORTD &= B10111111;
          delay(wait);
          PORTD |= B01000000;
      }
      break;
    case 2:
      switch(y){
        case 0:
          PORTD &= B11011111;
          delay(wait);
          PORTD |= B00100000;
          break;
        case 1:
          PORTD &= B11101111;
          delay(wait);
          PORTD |= B00010000;
          break;
        case 2:
          PORTD &= B11110111;
          delay(wait);
          PORTD |= B00001000;
          break;
        case 3:
        default:
          PORTD &= B11111011;
          delay(wait);
          PORTD |= B00000100;
      }
      break;
    case 3:
    default:
      switch(y){
        case 0:
          PORTD &= B11111101;
          delay(wait);
          PORTD |= B00000010;
          break;
        case 1:
          PORTD &= B11111110;
          delay(wait);
          PORTD |= B00000001;
          break;
        case 2:
          PORTC &= B11011111;
          delay(wait);
          PORTC |= B00100000;
          break;
        case 3:
        default:
          PORTC &= B11101111;
          delay(wait);
          PORTC |= B00010000;
    }
  }
}

void flashColumnLed(int x, int y){
  switch(x){
    case 0:
      switch(y){
        case 0:
          PORTB &= B11011111;
          PORTB |= B00100000;
          break;
        case 1:
          PORTB &= B11101111;
          PORTB |= B00010000;
          break;
        case 2:
          PORTB &= B11110111;
          PORTB |= B00001000;
          break;
        case 3:
        default:
          PORTB &= B11111011;
          PORTB |= B00000100;
      }
      break;
    case 1:
      switch(y){
        case 0:
          PORTB &= B11111101;
          PORTB |= B00000010;
          break;
        case 1:
          PORTB &= B11111110;
          PORTB |= B00000001;
          break;
        case 2:
          PORTD &= B01111111;
          PORTD |= B10000000;
          break;
        case 3:
        default:
          PORTD &= B10111111;
          PORTD |= B01000000;
      }
      break;
    case 2:
      switch(y){
        case 0:
          PORTD &= B11011111;
          PORTD |= B00100000;
          break;
        case 1:
          PORTD &= B11101111;
          PORTD |= B00010000;
          break;
        case 2:
          PORTD &= B11110111;
          PORTD |= B00001000;
          break;
        case 3:
        default:
          PORTD &= B11111011;
          PORTD |= B00000100;
      }
      break;
    case 3:
    default:
      switch(y){
        case 0:
          PORTD &= B11111101;
          PORTD |= B00000010;
          break;
        case 1:
          PORTD &= B11111110;
          PORTD |= B00000001;
          break;
        case 2:
          PORTC &= B11011111;
          PORTC |= B00100000;
          break;
        case 3:
        default:
          PORTC &= B11101111;
          PORTC |= B00010000;
    }
  }
}

void allOn(){
  // Turn on Layers with 5V
  PORTC |= B00001111;
  // Put GND on All Collumns
  PORTD &= B00000000;
  PORTB &= B11000000;
  PORTC &= B11001111;
}

void allOff(){
  // Turn on Layers with 5V
  PORTC &= B11110000;
  // Put GND on All Collumns
  PORTD |= B11111111;
  PORTB |= B00111111;
  PORTC |= B00110000;
}
