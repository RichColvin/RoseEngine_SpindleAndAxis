/* *****************************************************************
* Rose Engine Spindle definitions
* Author: Edward French
******************************************************************/
#pragma once
#define FOUR_AXES  // Teensy 3.5
//#define THREE_AXES // Teensy 3.2
//#define TWO_AXES_V2 // Teensy 3.2
//#define DEBUG

//==================================================================
// Pin assignments
//==================================================================

#ifdef FOUR_AXES
// Spindle
#define ID_SPINDLE 3
#define PIN_SPINDLE_DIR 2  //Stepper direction
#define PIN_SPINDLE_STEP 3  //Stepper step
#define PIN_SPINDLE_ENABLE 4 //Enable
//  User runtime settable microstepping: Uncomment all microstepping #defines. 
//  Uncomment calls to SetMicrosteppingMode().
//#define PIN_SPINDLE_MS0 24
//#define PIN_SPINDLE_MS1 25 
//#define PIN_SPINDLE_MS2 26

// Axes
//#define PIN_LIMIT_MIN 18 // Limit switch: Moving towards headstock
//#define PIN_LIMIT_MAX 19 // Limit switch: Moving away from headstock

// Z axis
#define ID_Z_AXIS 0
#define PIN_AXIS_Z_DIR 5 // Stepper direction
#define PIN_AXIS_Z_STEP 6 // Stepper step
#define PIN_AXIS_Z_ENABLE 14 // Enable 
//#define PIN_AXIS_Z_MS0 27
//#define PIN_AXIS_Z_MS1 28 
//#define PIN_AXIS_Z_MS2 29

// X axis
#define ID_AXIS_X 1
#define PIN_AXIS_X_DIR 20  // Stepper direction
#define PIN_AXIS_X_STEP 21  // Stepper step
#define PIN_AXIS_X_ENABLE 22 // Enable 
//#define PIN_AXIS_X_MS0 30
//#define PIN_AXIS_X_MS1 31 
//#define PIN_AXIS_X_MS2 32

// B axis
#define ID_AXIS_B 2
#define PIN_AXIS_B_DIR 23  // Stepper direction
#define PIN_AXIS_B_STEP 16  // Stepper step
#define PIN_AXIS_B_ENABLE 17 // Enable 
//#define PIN_AXIS_B_MS0 27
//#define PIN_AXIS_B_MS1 28 
//#define PIN_AXIS_B_MS2 29

#elif defined(TWO_AXES_V2)
// Spindle
#define ID_SPINDLE 3
#define PIN_SPINDLE_DIR 2 // Stepper direction
#define PIN_SPINDLE_STEP 3  // Stepper step
#define PIN_SPINDLE_ENABLE 4 // Enable
// User runtime settable microstepping requires inner pins on Teensy 3.2
//#define PIN_SPINDLE_MS0 24
//#define PIN_SPINDLE_MS1 25 
//#define PIN_SPINDLE_MS2 26

// Axes
#define PIN_LIMIT_MIN 16 // Limit switch: Moving towards headstock
#define PIN_LIMIT_MAX 17 // Limit switch: Moving away from headstock

// Z axis
#define ID_Z_AXIS 0
#define PIN_AXIS_Z_DIR 5  // Stepper direction
#define PIN_AXIS_Z_STEP 6  // Stepper step
#define PIN_AXIS_Z_ENABLE 14 // Enable 
// User runtime settable microstepping requires inner pins on Teensy 3.2
//#define PIN_AXIS_Z_MS0 27
//#define PIN_AXIS_Z_MS1 28 
//#define PIN_AXIS_Z_MS2 29

// X axis
#define ID_AXIS_X 1
#define PIN_AXIS_X_DIR 24  // Stepper direction
#define PIN_AXIS_X_STEP 25  // Stepper step
#define PIN_AXIS_X_ENABLE 26 // Enable 
//#define PIN_AXIS_X_MS0 30
//#define PIN_AXIS_X_MS1 31 
//#define PIN_AXIS_X_MS2 32

// B axis
#define ID_AXIS_B 2
#define PIN_AXIS_B_DIR 24  // Stepper direction
#define PIN_AXIS_B_STEP 25  // Stepper step
#define PIN_AXIS_B_ENABLE 26 // Enable 
//#define PIN_AXIS_B_MS0 27
//#define PIN_AXIS_B_MS1 28 
//#define PIN_AXIS_B_MS2 29

#elif defined(THREE_AXES) // Three axes board
// Spindle
#define ID_SPINDLE 3
#define PIN_SPINDLE_DIR 3 // Stepper direction
#define PIN_SPINDLE_STEP 2  // Stepper step
#define PIN_SPINDLE_ENABLE 6 // Enable
// User runtime settable microstepping requires inner pins on Teensy 3.2
//#define PIN_SPINDLE_MS0 24
//#define PIN_SPINDLE_MS1 25 
//#define PIN_SPINDLE_MS2 26

// Limit Switches
//#define PIN_LIMIT_MIN_Z 16 // Limit switch: Moving towards headstock
//#define PIN_LIMIT_MAX_Z 17 // Limit switch: Moving away from headstock
//#define PIN_LIMIT_MIN_X 16 // Limit switch: Moving towards center.  Change to 18 if separate limit switches.
//#define PIN_LIMIT_MAX_X 17 // Limit switch: Moving away from center. Change to 19 if separate limit switches.

// Z axis
#define ID_Z_AXIS 0
#define PIN_AXIS_Z_DIR 4  // Stepper direction
#define PIN_AXIS_Z_STEP 5  // Stepper step
#define PIN_AXIS_Z_ENABLE 20 //PCB jumper:33>>20 // Enable  

// User runtime settable microstepping requires inner pins on Teensy 3.2
//#define PIN_AXIS_Z_MS0 27
//#define PIN_AXIS_Z_MS1 28 
//#define PIN_AXIS_Z_MS2 29

// X axis
#define ID_AXIS_X 1
#define PIN_AXIS_X_DIR 23  //PCB jumper:24>>23 // Stepper direction
#define PIN_AXIS_X_STEP 22  //PCB jumper:30>>22// Stepper step
#define PIN_AXIS_X_ENABLE 21 //PCB jumper:29>>21 // Enable 
// User runtime settable microstepping requires inner pins on Teensy 3.2
//#define PIN_AXIS_X_MS0 30
//#define PIN_AXIS_X_MS1 31 
//#define PIN_AXIS_X_MS2 32

// B axis
#define ID_AXIS_B 2
#define PIN_AXIS_B_DIR 24  // Stepper direction
#define PIN_AXIS_B_STEP 25  // Stepper step
#define PIN_AXIS_B_ENABLE 26 // Enable 
//#define PIN_AXIS_B_MS0 27
//#define PIN_AXIS_B_MS1 28 
//#define PIN_AXIS_B_MS2 29

#endif //FOUR_AXES/Two Axes/3 Axes

// SPI
#define PIN_SPI_CS 10

// Index page ID
#define ID_INDEX_1 1
#define ID_INDEX_2 2

#define DIR_CCW -1
#define DIR_CW 1

#define BY_DEGREES 0
#define BY_DIVISIONS 2

#define FIXED 0
#define FILE_SD 1

#define INCHES 0
#define MILLIMETERS 1

#define WAVE_IN -1
#define WAVE_OUT 1

struct configs
{
	int speedPercentSpindle_SpZ;
	int speedPercentSpindle_SpX;
	int speedPercentSpindle_SpB;
	int speedPercentSpindle_SyncZ;
	int speedPercentSpindle_SyncX;
	int speedPercentSpindle_Rec1S;
	int speedPercentSpindle_Rec1Z;
	int speedPercentAxis_SyncZ;
	int speedPercentAxis_SyncX;
	int speedPercentAxis_Rec1S;
	int speedPercentAxis_Rec1Z;
	int speedPercent_Index1;
	int speedPercent_Index2;
	int speedPercent_Sp2;
	int speedPercent_Axis_Z;
	int speedPercent_Axis_X;
	int speedPercent_Axis_B;
	int speedPercent_MoveZ;
	int speedPercent_MoveX;
	int microsteps_Spindle;
	int microsteps_Axis_Z;
	int microsteps_Axis_X;
	int microsteps_Axis_B;
	int steps360_Spindle;
	int steps360_Axis_Z;
	int steps360_Axis_X;
	int steps360_Axis_B;
	int spindleDir_SyncZ;
	int spindleDir_SyncX;
	int axisDir_SyncZ;
	int axisDir_SyncX;
	int helixType_SyncZ;
	int helixType_SyncX;
	int type_Index1;
	int type_Index2;
	int source_Index1;
	int source_Index2;
	int waves_Rec1_Z;
	int waves_Rec1_S;
	double size_Index1;
	double size_Index2;
	float distance_MoveZ;
	float distance_MoveX;
	float distance_Rec1_Z;
	float amplitude_Rec1_Z;
	float amplitude_Rec1_S;
	float degrees_Rec1_S;
	float gearRatio_Spindle;
	float gearRatio_AxisB;
	float distancePerRev_AxisZ;
	float distancePerRev_AxisX;
	float distanceSyncZ;
	float distanceSyncX;
	float revolutionsSyncZ_Spindle;
	float revolutionsSyncX_Spindle;
	bool enable_Spindle;
	bool polarity_Axis_Z;
	bool polarity_Axis_X;
	bool polarity_Axis_B;
	int limit_Min_Z;
	int limit_Max_Z;
	int limit_Min_X;
	int limit_Max_X;
	int limit_Min_B;
	int limit_Max_B;
};

// Config Structs
struct configPageSpZ // page 0
{
	int maxSpd_Spindle;
	int accel_Spindle;
	int maxSpd_Axis_Z;
	int accel_Axis_Z;
};
struct configPageSpX // page 1
{
	int maxSpd_Spindle;
	int accel_Spindle;
	int maxSpd_Axis_X;
	int accel_Axis_X;
};
struct configPageSpB // page 2
{
	int maxSpd_Spindle;
	int accel_Spindle;
	int maxSpd_Axis_B;
	int accel_Axis_B;
};
struct configPageSyncZ // page 3
{
	int maxSpd_Spindle;
	int accel_Spindle;
	int maxSpd_Axis_Z;
	int accel_Axis_Z;
};
struct configPageSyncX // page 4
{
	int maxSpd_Spindle;
	int accel_Spindle;
	int maxSpd_Axis_X;
	int accel_Axis_X;
};

struct configPageRec1_ZAxis // page 5
{
	int maxSpd_Spindle;
	int accel_Spindle;
	int maxSpd_Axis_Z;
	int accel_Axis_Z;
};
struct configPageRec1_Spindle // page 6
{
	int maxSpd_Spindle;
	int accel_Spindle;
	int maxSpd_Axis_Z;
	int accel_Axis_Z;
};

struct configPageSp // page 7
{
	int maxSpd_Spindle;
	int accel_Spindle;
};

struct configPageIndex1 // page 8
{
	int maxSpd_Spindle;
	int accel_Spindle;
};
struct configPageIndex2 // page 9
{
	int maxSpd_Spindle;
	int accel_Spindle;
};

struct configPageMoveZ // page 10
{
	int maxSpd_Axis_Z;
	int accel_Axis_Z;
};
struct configPageMoveX // page 11
{
	int maxSpd_Axis_X;
	int accel_Axis_X;
};
struct configPageRose // page 12
{
	int maxSpd_Spindle;
	int accel_Spindle;
	int maxSpd_Axis_Z;
	int accel_Axis_Z;
	int maxSpd_Axis_X;
	int accel_Axis_X;
	int speedPercent_Spindle;
	int n;
	int d;
	int amplitude_Axis_Z;
	int speedPercent_Axis;
};

//==================================================================
// Global Variables
//==================================================================

//// Initialize AccelStepper object
//AccelStepper accelStep_Spindle(AccelStepper::DRIVER, PIN_SPINDLE_STEP, PIN_SPINDLE_DIR);
//AccelStepper accelStep_Axis_Z(AccelStepper::DRIVER, PIN_AXIS_Z_STEP, PIN_AXIS_Z_DIR);
//
//// TeensyStep initialization
//Stepper teensyStep_Spindle(PIN_SPINDLE_STEP, PIN_SPINDLE_DIR);
//Stepper teensyStep_Axis_Z(PIN_AXIS_Z_STEP, PIN_AXIS_Z_DIR);
//
//
////#ifndef TWO_AXES_V2 // Three and four axes boards
//Stepper teensyStep_Axis_X(PIN_AXIS_X_STEP, PIN_AXIS_X_DIR);
//AccelStepper accelStep_Axis_X(AccelStepper::DRIVER, PIN_AXIS_X_STEP, PIN_AXIS_X_DIR);
////#endif // TWO_AXES_V2
//
////#ifdef FOUR_AXES
//AccelStepper accelStep_Axis_B(AccelStepper::DRIVER, PIN_AXIS_B_STEP, PIN_AXIS_B_DIR);
//Stepper teensyStep_Axis_B(PIN_AXIS_B_STEP, PIN_AXIS_B_DIR);
////#endif // B_AXIS_ENABLED
//
//// TeensyStep controllers
//StepControl controller_Spindle;
//StepControl controller_Axis;
//RotateControl controllerRose_Spindle;
//RotateControl controllerRose_Axis;

int serialId = 9; // Initialize with unused serial id.  Serial-0, Serial1-1
byte incomingByte = 0;	// store incoming Serial data
unsigned int eePromAddress_Setup = 0;  // EEProm address for spindleConfig values
unsigned int eePromAddress_Filename = 900; // EEProm address for Index2 filename
unsigned int eePromAddress_Filename_Length = 996; // EEProm address for length of Index2 filename

unsigned int eePromAddress_SpZ = 1000;  // EEProm address for SpZ 200-207
unsigned int eePromAddress_SpX = 1016;  // EEProm address for SpX  208-215
unsigned int eePromAddress_SpB = 1032;  // EEProm address for SpB 216-223
unsigned int eePromAddress_SyncZ = 1048;  // EEProm address for SyncZ 224-231
unsigned int eePromAddress_SyncX = 1064;  // EEProm address for SyncX 232-239
unsigned int eePromAddress_Rec1_Z = 1080;  // EEProm address for Rec1_ZAxis 240-247
unsigned int eePromAddress_Rec1_S = 1096;  // EEProm address for Rec1_Spindle 248-255
unsigned int eePromAddress_Sp = 1112;  // EEProm address for Sp 256-259
unsigned int eePromAddress_Index1 = 1128;  // EEProm address for Index1 260-263
unsigned int eePromAddress_Index2 = 1144;  // EEProm address for Index2 264-267
unsigned int eePromAddress_MoveZ = 1160;  // EEProm address for MoveZ 268-271
unsigned int eePromAddress_MoveX = 1176;  // EEProm address for MoveX 272-275
unsigned int eePromAddress_Rose = 1200; // EEProm address for Rose

float distanceTotal_MoveZ = 0;
float distanceTotal_MoveX = 0;

String filename_Index1;
int filenameLength = 0;
String filename_Index2a;
char * filename_Index2;

double returnSteps_Rec1_Z = 0;
double returnSteps_Rec1_S = 0;

// Rose Functions variables
//float spindleSpeed_Rose = 1000;
volatile int32_t newMaxSpd_RoseSpindle;
volatile int32_t newMaxSpd_RoseAxis;
volatile int spindleStepsPerRev;// = 6400;
volatile float kRatio;
volatile float stepDistance_RoseAxis;
// TeensyStep 
//RotateControl slideController;
//RotateControl spindleController;
//Stepper teensyStep_Spindle(PIN_SPINDLE_STEP, PIN_SPINDLE_DIR);
//Stepper teensyStep_Axis_Z(PIN_AXIS_Z_STEP, PIN_AXIS_Z_DIR);

//accuracy 
//IntervalTimer tickTimer;
constexpr unsigned recalcPeriod = 25'000; //?s  period for calculation of new target points. Straight lines between those points. 
//constexpr float dt = recalcPeriod / 1E6;  // seconds 
constexpr float dt = .075;  // seconds 
volatile int iCounterX = 0;

/////////////////////////////////////////////////////////////////////////



//spindle settings 
constexpr unsigned rpmX = 10;
constexpr unsigned spindleSPRX = 6400;                    // stp per rev 
constexpr unsigned spindleSpeedX = rpmX * spindleSPRX / 60; // stp/sec 
////constexpr unsigned spindleStepPinX = 3;
////constexpr unsigned spindleDirPinX = 2;


//slide settings 
constexpr unsigned slideAmplitudeX = 10000; // stp 
constexpr unsigned slideSpeedX = 15000;     //stp/sec 
//constexpr unsigned slideStepPinX = 6;
//constexpr unsigned slideDirPinX = 5;

//accuracy 
//IntervalTimer tickTimerX;
constexpr unsigned recalcPeriodX = 25'000; //?s  period for calculation of new target points. Straight lines between those points. 
constexpr float dtX = recalcPeriodX / 1E6;  // seconds 


// rose fuction 
constexpr int nX = 5;
constexpr int dX = 8;
constexpr float kX = (float)nX / dX;




/////////////////////////////////////////////////////////////////////////////
// Page defines
int pageCallerId = 20;
#define PAGE_SPZ 0
#define PAGE_SPX 1
#define PAGE_SPB 2
#define PAGE_SYNCZ 3
#define PAGE_SYNCX 4
#define PAGE_REC1_Z 5
#define PAGE_REC1_S 6
#define PAGE_SP 7
#define PAGE_INDEX1 8
#define PAGE_INDEX2 9
#define PAGE_MOVEZ 10
#define PAGE_MOVEX 11
#define PAGE_ROSE 12

// Config and Setup variables
configs configMain;
configPageSpZ configSpZ;
configPageSpX configSpX;
configPageSpB configSpB;
configPageSyncZ configSyncZ;
configPageSyncX configSyncX;
configPageRec1_ZAxis configRec1_Z;
configPageRec1_Spindle configRec1_S;
configPageSp configSp;
configPageIndex1 configIndex1;
configPageIndex2 configIndex2;
configPageMoveZ configMoveZ;
configPageMoveX configMoveX;
configPageRose configRose;

// End Global Variables
//==================================================================

