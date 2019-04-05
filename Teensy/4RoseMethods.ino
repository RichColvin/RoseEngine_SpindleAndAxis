/* *****************************************************************
* Rose Engine utility methods
* Author: Edward French
******************************************************************/
// ================================================================================= 
// Utility Methods
// ================================================================================= 
/// <summary>
/// Determines which Serial input to use when getting a float from input
/// </summary>
/// <comment>
/// </comment>
/// <param name="serialId">Id of serial input</param>
/// <returns>Float from serial input</returns>
double GetSerialFloat(int serialId)
{
	double retVal =-1;

	switch (serialId)
	{
		case 0:
		{
			retVal = GetSerialFloatA(Serial);
			break;
		}
		case 1:
		{
			retVal = GetSerialFloatA(Serial3);
			break;
		}
		case 3:
		{

			retVal = GetSerialFloatA(Serial3);
			break;
		}
	}

#ifdef DEBUG
	Serial.print("FloatA: ");
	Serial.println(retVal);
#endif // DEBUG


	return retVal;
}

/// <summary>
/// Get a float from USB serial input
/// </summary>
/// <returns>Float from USB serial input</returns>
double GetSerial0Float()
{
	const int bufferLength = 10;
	char in[bufferLength];
	int decimalFound = -1;
	int endFound = -1;
	int i;
	for (i = 0; i <= bufferLength; i++) 
	{
		in[i] = Serial.read();

		// Use 255 as end chars
		if (in[i] == 255)
		{
			endFound = i;
			break;
		}
		if (in[i] == 46)
		{
			decimalFound = i;
		}
	}

	return GetFloatFromCharArray(in, decimalFound, endFound);
}

/// <summary>
/// Get a float from Bluetooth serial input (Serial3)
/// </summary>
/// <returns>Float from Bluetooth serial input</returns>
double GetSerial1Float()
{
	const int bufferLength = 10;
	char in[bufferLength];
	int decimalFound = -1;
	int endFound = -1;
	int i;
	for (i = 0; i <= bufferLength; i++)
	{
		in[i] = Serial3.read();

		// Use 255 as end chars
		if (in[i] == 255)
		{
			endFound = i;
			while (Serial3.available() > 0)
			{
				Serial3.read();
			}
			break;
		}
		if (in[i] == 46)
		{
			decimalFound = i;
		}
	}

	return GetFloatFromCharArray(in, decimalFound,endFound);
}

//////////////////////////////////
/// <summary>
/// Get a float from Bluetooth serial input (Serial3)
/// </summary>
/// <returns>Float from Bluetooth serial input</returns>
double GetSerialFloatA(Stream &port )
{
	const int bufferLength = 10;
	char in[bufferLength];
	int decimalFound = -1;
	int endFound = -1;
	int i;
	for (i = 0; i <= bufferLength; i++)
	{
		in[i] = port.read();

		// Use 255 as end chars
		if (in[i] == 255)
		{
			endFound = i;
			while (port.available() > 0)
			{
				port.read();
			}
			break;
		}
		if (in[i] == 46)
		{
			decimalFound = i;
		}
	}

	return GetFloatFromCharArray(in, decimalFound, endFound);
}

/////////////////////////////////////////
/// <summary>
/// Parse serial input for new integer value
/// </summary>
/// <comment>
/// </comment>
/// <returns></returns>
int GetSerialInteger()
{
	char inputHex;

	// Value will be 0 to 100
	inputHex = Serial3.read();

	int newInteger = (int)inputHex ;
	while (Serial3.available() > 0)
	{
		// Discard remaining serial chars
		Serial3.read();
	}

	Serial.print("Int:");
	Serial.println(newInteger);
	return newInteger;
}

/// <summary>
/// Parse serial input for new integer value
/// </summary>
/// <comment>
/// </comment>
/// <returns></returns>
int GetSerialIntegerOnly()
{
	char inputHex;

	// Value will be 0 to 100
	inputHex = Serial3.read();

	int newInteger = (int)inputHex;

	return newInteger;
}

/// <summary>
/// Convert Char array to float (double)
/// </summary>
/// <comment>
/// </comment>
/// <param name="in">Char array</param>
/// <param name="decimalFound">Decimal place</param>
/// <param name="endFound">Length of array</param>
/// <returns>Double(float)</returns>
double GetFloatFromCharArray(char in[], int decimalFound, int endFound)
{
	int j;
	double inputNumber = 0;
	int integerPart = 0;
	for (j = 0; j < endFound; j++)
	{
		// Build the number
		if (j != decimalFound)
		{
			integerPart = in[j] - '0';
			inputNumber = (inputNumber * 10) + integerPart;
		}
	}

	// Set the decimal place
	int decimalPlace = 0;
	if (decimalFound > -1)
	{
		decimalPlace = endFound - 1 - decimalFound;
		int k;
		for (k = 0; k < decimalPlace; k++)
		{
			inputNumber = (inputNumber / 10);
		}
	}

#ifdef DEBUG
	Serial.print("Float:");
	Serial.println(inputNumber);
#endif //DEBUG
	return inputNumber;
}

float StepsToDegrees_Spindle(float steps)
{

	// (Steps / fullsteps*microsteps*gearRatio)*360
	float retVal = (steps / (configMain.steps360_Spindle * configMain.microsteps_Spindle* configMain.gearRatio_Spindle)) * 360;
	return retVal;
}

float StepsToDistance_Axis(float steps, int axisId)
{
	float retVal = 0;
	switch (axisId)
	{
		case ID_AXIS_Z:
		{
			retVal = (steps * configMain.distancePerRev_AxisZ) / (configMain.steps360_Axis_Z * configMain.microsteps_Axis_Z);
			break;
		}
		case ID_AXIS_X:
		{
			retVal = (steps * configMain.distancePerRev_AxisX) / (configMain.steps360_Axis_X * configMain.microsteps_Axis_X);
			break;
		}
	}
	return retVal;
}

/// <summary>
/// Index spindle with TeensyStep
/// </summary>
/// <comment>
/// </comment>
/// <param name="directionSpindle">Counterclockwise: -1, Clockwise: 1</param>
/// <returns></returns>
void MoveAxis(int axisId, int directionAxis)
{
	StepControl stepController;
	Stepper stepper_X(PIN_AXIS_X_STEP, PIN_AXIS_X_DIR);
	Stepper stepper_Z(PIN_AXIS_Z_STEP, PIN_AXIS_Z_DIR);
	double stepsToMove = 0;
	float nextSpeed = 0;
//	long moveTo = 0;

	switch (axisId)
	{
		case ID_AXIS_Z:
		{
			stepper_Z.setPosition(0);
			stepsToMove = (configMain.distance_MoveZ / (configMain.distancePerRev_AxisZ) * (configMain.steps360_Axis_Z * configMain.microsteps_Axis_Z));

			// Set speed and acceleration
			nextSpeed = configMain.speedPercent_MoveZ * configMoveZ.maxSpd * .01;
			stepper_Z
				.setMaxSpeed(nextSpeed)
				.setAcceleration(configMoveZ.accel)
				.setTargetRel(stepsToMove * directionAxis);

#ifdef DEBUG
			Serial.print("distancePerRev_AxisZ:");
			Serial.println(configMain.distancePerRev_AxisZ);
			Serial.print("steps360_Axis_Z:");
			Serial.println(configMain.steps360_Axis_Z);
			Serial.print("microsteps_Axis_Z:");
			Serial.println(configMain.microsteps_Axis_Z);
			Serial.print("distance_MoveZ:");
			Serial.println(configMain.distance_MoveZ);

			Serial.print("axisSteps:");
			Serial.println(stepsToMove);

			Serial.print("AxisMaxSpd:");
			Serial.println(configMoveZ.maxSpd);
			Serial.print("speedPercent_MoveZ:");
			Serial.println(configMain.speedPercent_MoveZ);
			Serial.print("nextSpeed:");
			Serial.println(nextSpeed);
			Serial.print("AxisAccel:");
			Serial.println(configMoveZ.accel);
#endif // DEBUG

			digitalWrite(PIN_AXIS_Z_ENABLE, LOW);
			stepController.moveAsync(stepper_Z);
			break;
		}
#ifdef FOUR_AXES
		case ID_AXIS_X:
		{
			//teensyStep_Axis_X.setPosition(0);
			stepsToMove = (configMain.distance_MoveX / (configMain.distancePerRev_AxisX) * (configMain.steps360_Axis_X * configMain.microsteps_Axis_X));
			stepper_X.setPosition(0);


			// Set speed and acceleration
			nextSpeed = configMain.speedPercent_MoveX * configMoveX.maxSpd * .01;
			//teensyStep_Axis_X
			stepper_X
				.setMaxSpeed(nextSpeed)
				.setAcceleration(configMoveX.accel)
				.setTargetRel(stepsToMove * directionAxis);

#ifdef DEBUG
			Serial.print("distancePerRev_AxisX:");
			Serial.println(configMain.distancePerRev_AxisX);
			Serial.print("steps360_Axis_X:");
			Serial.println(configMain.steps360_Axis_X);
			Serial.print("microsteps_Axis_X:");
			Serial.println(configMain.microsteps_Axis_X);
			Serial.print("distance_MoveX:");
			Serial.println(configMain.distance_MoveX);

			Serial.print("axisSteps:");
			Serial.println(stepsToMove);

			Serial.print("AxisMaxSpd:");
			Serial.println(configMoveX.maxSpd);
			Serial.print("speedPercent_MoveX:");
			Serial.println(configMain.speedPercent_MoveX);
			Serial.print("nextSpeed:");
			Serial.println(nextSpeed);
			Serial.print("AxisAccel:");
			Serial.println(configMoveX.accel);
#endif // DEBUG
			digitalWrite(PIN_AXIS_X_ENABLE, LOW);
			stepController.moveAsync(stepper_X);
			break;
		}
#endif // FOUR_AXES
	}

#ifdef DEBUG
	int iCounter = 0;
#endif // DEBUG

	while (stepController.isRunning())
	{
#ifdef SHOW_POSITION //Controls must be added to Nextion pages
		if (configMain.trackPositions == 1)
		{
			switch (axisId)
			{
				case ID_AXIS_Z:
				{

					endPosition_Axis = teensyStep_Axis_Z.getPosition();
					distance_Axis = StepsToDistance_Axis(endPosition_Axis, ID_AXIS_Z);

					Serial3.print("pageMoveZ.t2.txt=");
					Serial3.write(0x22);
					Serial3.print(distance_Axis);
					Serial3.write(0x22);
					Serial3.write(0xff);
					Serial3.write(0xff);
					Serial3.write(0xff);
					delay(5);
					Serial3.print("pageMoveZ.t2.txt=");
					Serial3.write(0x22);
					Serial3.print(distance_Axis);
					Serial3.write(0x22);
					Serial3.write(0xff);
					Serial3.write(0xff);
					Serial3.write(0xff);
					break;
				}
				case ID_AXIS_X:
				{
					endPosition_Axis = teensyStep_Axis_X.getPosition();
					distance_Axis = StepsToDistance_Axis(endPosition_Axis, ID_AXIS_Z);
					Serial3.print("pageMoveX.t2.txt=");
					Serial3.write(0x22);
					Serial3.print(distance_Axis);
					Serial3.write(0x22);
					Serial3.write(0xff);
					Serial3.write(0xff);
					Serial3.write(0xff);
					delay(5);
					Serial3.print("pageMoveX.t2.txt=");
					Serial3.write(0x22);
					Serial3.print(distance_Axis);
					Serial3.write(0x22);
					Serial3.write(0xff);
					Serial3.write(0xff);
					Serial3.write(0xff);
					break;
				}
			}
		}
#endif // Show Position


		// Check for Cancel code  
		if (SerialAvailable() >= 0)
		{
			incomingByte = SerialRead(serialId);
			switch (incomingByte)
			{
				case 93: // - ] 
				case 99: // - c
				case 109: // - m
				{
					stepController.stop();
					break;
				}

			}

		}
		delay(10);
#ifdef DEBUG



		if (iCounter % 50 == 0)
		{
			switch (axisId)
			{
				case ID_AXIS_Z:
				{
					endPosition_Axis = stepper_Z.getPosition();
					Serial.print("Z.position:");
					break;
				}
				case ID_AXIS_X:
				{
					endPosition_Axis = stepper_X.getPosition();
					Serial.print("X.position:");
					break;
				}
			}

			Serial.println(endPosition_Axis);
		}

		iCounter++;
#endif // DEBUG
	}

	switch (axisId)
	{

		case ID_AXIS_Z:
		{
			// Update Index flag on Nextion. 
			Serial3.print("pageMoveZ.bt2.pco=59391");// The first one may be ignored by Nextion
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.print("pageMoveZ.va0.val=0");
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.print("pageMoveZ.bt1.val=0");
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.print("pageMoveZ.bt2.val=0");
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.print("pageMoveZ.bt3.val=0");
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.print("pageMoveZ.t6.txt=");
			Serial3.write(0x22);
			Serial3.print(distanceTotal_MoveZ);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);

			endPosition_Axis = stepper_Z.getPosition();
#ifdef DEBUG
			Serial.print("Z.position:");
			Serial.println(endPosition_Axis);
#endif // DEBUG


#ifdef SHOW_POSITION //Controls must be added to Nextion pages
			distance_Axis = StepsToDistance_Axis(endPosition_Axis, ID_AXIS_Z);
			Serial3.print("pageMoveZ.t2.txt=");
			Serial3.write(0x22);
			Serial3.print(distance_Axis);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(5);
			Serial3.print("pageMoveZ.t2.txt=");
			Serial3.write(0x22);
			Serial3.print(distance_Axis);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(5);
#endif // Show Position
			endPosition_Spindle = 0;

			digitalWrite(PIN_AXIS_Z_ENABLE, HIGH);  // Disable 
			break;
		}
#ifdef FOUR_AXES
		case ID_AXIS_X:
		{
			// Update Index flag on Nextion. 
			Serial3.print("pageMoveX.bt2.pco=59391");// The first one may be ignored by Nextion
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.print("pageMoveX.va0.val=0");
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.print("pageMoveX.bt1.val=0");
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.print("pageMoveX.bt2.val=0");
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.print("pageMoveX.bt3.val=0");
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.print("pageMoveX.t6.txt=");
			Serial3.write(0x22);
			Serial3.print(distanceTotal_MoveZ);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);

			endPosition_Axis = stepper_X.getPosition();
			Serial.print("X.position:");
			Serial.println(endPosition_Axis);

#ifdef SHOW_POSITION //Controls must be added to Nextion pages
			distance_Axis = StepsToDistance_Axis(endPosition_Axis, ID_AXIS_Z);
			Serial3.print("pageMoveX.t2.txt=");
			Serial3.write(0x22);
			Serial3.print(distance_Axis);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(5);
			Serial3.print("pageMoveX.t2.txt=");
			Serial3.write(0x22);
			Serial3.print(distance_Axis);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(5);
#endif // Show Position
			endPosition_Spindle = 0;

			digitalWrite(PIN_AXIS_X_ENABLE, HIGH);  // Disable 
			break;
		}
#endif // FOUR_AXES
	}

	returnSteps_Axis = endPosition_Axis;
	endPosition_Axis = 0;
}

void RunOneStepper(int direction) // pageSp
{
	RotateControl rotateController;

	int stepPin = PIN_AXIS_Z_STEP;
	int dirPin = PIN_AXIS_Z_DIR;
	int enablePin = PIN_AXIS_Z_ENABLE;
	int limitPin_Max = configMain.limit_Max_Z;
	int limitPin_Min = configMain.limit_Min_Z;

	int polarity = LOW;
	int maxSpd = 0;
	int accel = 0;
	//float currentSpeed = 0;
	float speedPercent = 0;
	float currentSpeedPercent = 0;
	int32_t targetPosition = 2000000000 * direction;

	switch (configSp.activeAxis)
	{
		case ID_AXIS_Z:
		{
			stepPin = PIN_AXIS_Z_STEP;
			dirPin = PIN_AXIS_Z_DIR;
			enablePin = PIN_AXIS_Z_ENABLE;
			polarity = configMain.polarity_Axis_Z ? (LOW) : (HIGH);
			maxSpd = configSp.maxSpd_Axis_Z;
			accel = configSp.accel_Axis_Z;
			speedPercent = configSp.speedPercent_Axis_Z;
			currentSpeedPercent = speedPercent * .01;// *direction;
			limitPin_Max = configMain.limit_Max_Z;
			limitPin_Min = configMain.limit_Min_Z;
			break;
		}
		case ID_AXIS_X:
		{
			stepPin = PIN_AXIS_X_STEP;
			dirPin = PIN_AXIS_X_DIR;
			enablePin = PIN_AXIS_X_ENABLE;
			polarity = configMain.polarity_Axis_X ? (LOW) : (HIGH);
			maxSpd = configSp.maxSpd_Axis_X;
			accel = configSp.accel_Axis_X;
			speedPercent = configSp.speedPercent_Axis_X;
			currentSpeedPercent = speedPercent * .01 *direction;
			limitPin_Max = configMain.limit_Max_X;
			limitPin_Min = configMain.limit_Min_X;
			break;
		}
		case ID_AXIS_B:
		{
			stepPin = PIN_AXIS_B_STEP;
			dirPin = PIN_AXIS_B_DIR;
			enablePin = PIN_AXIS_B_ENABLE;
			polarity = configMain.polarity_Axis_B ? (LOW) : (HIGH);
			maxSpd = configSp.maxSpd_Axis_B;
			accel = configSp.accel_Axis_B;
			speedPercent = configSp.speedPercent_Axis_B;
			currentSpeedPercent = speedPercent * .01 * direction;
			limitPin_Max = configMain.limit_Max_B;
			limitPin_Min = configMain.limit_Min_B;
			break;
		}
		case ID_SPINDLE:
		{
			stepPin = PIN_SPINDLE_STEP;
			dirPin = PIN_SPINDLE_DIR;
			enablePin = PIN_SPINDLE_ENABLE;
			polarity = configMain.polarity_Spindle ? (LOW) : (HIGH);
			maxSpd = configSp.maxSpd_Spindle;
			accel = configSp.accel_Spindle;
			speedPercent = configSp.speedPercent_Spindle;
			currentSpeedPercent = speedPercent * .01 * direction;
			break;
		}
	}


	Stepper stepper_One(stepPin, dirPin);
	stepper_One.setPosition(0);
	delay(5);
	stepper_One
		.setStepPinPolarity(polarity)
		.setMaxSpeed(maxSpd)
		.setAcceleration(accel)
		.setTargetAbs(targetPosition);

#ifdef DEBUG
	Serial.print("configSp.activeAxis");
	Serial.println(configSp.activeAxis);
	Serial.print("direction:");
	Serial.println(direction);
	Serial.print("polarity");
	Serial.println(polarity);
	Serial.print("maxSpd:");
	Serial.println(maxSpd);
	Serial.print("accel:");
	Serial.println(accel);
	Serial.print("SpeedPercent:");
	Serial.println(speedPercent);
	Serial.print("currentSpeedPercent:");
	Serial.println(currentSpeedPercent);
	Serial.print("targetPosition:");
	Serial.println(targetPosition);
	Serial.println("+++++++++++++++++++++++++++++++");
	Serial.print("getPosition:");
	Serial.println(stepper_One.getPosition());
#endif // DEBUG

	digitalWrite(enablePin, LOW); // Enable
	rotateController.rotateAsync(stepper_One);
	rotateController.overrideSpeed(currentSpeedPercent);
	while (rotateController.isRunning())
	{
#ifdef DEBUG

		//endPosition_Spindle = stepper_One.getPosition();
		//Serial.print("Spindle:");
		//Serial.println(endPosition_Spindle);
#endif // DEBUG

		// Check for Cancel code  
		if (SerialAvailable() > 0)
		{
			incomingByte = SerialRead(serialId);
			switch (incomingByte)
			{
				case 67: // - C
				{
#ifdef DEBUG
					Serial.print("stop:");

#endif // DEBUG
					rotateController.overrideSpeed(0);
					delay(5);
					rotateController.stop();
#ifdef DEBUG
					Serial.print("goto endLoop:");

#endif // DEBUG
					goto endLoop;
					break;
				}
				case 102: // Speed
				{

					int newSpeedPercent = (int)GetSerialFloat(serialId);
					switch (configSp.activeAxis)
					{
						case ID_AXIS_Z:
						{
							configSp.speedPercent_Axis_Z = newSpeedPercent;
							//speedPercent = (float)(newSpeedPercent);// *direction);
							currentSpeedPercent = float(newSpeedPercent * .01);// *direction);
							break;
						}
						case ID_AXIS_X:
						{
							configSp.speedPercent_Axis_X = newSpeedPercent;
							currentSpeedPercent = (float)(newSpeedPercent * .01 * direction);
							break;
						}
						case ID_AXIS_B:
						{
							configSp.speedPercent_Axis_B = newSpeedPercent;
							currentSpeedPercent = (float)(newSpeedPercent * .01 * direction);
							break;
						}
						case ID_SPINDLE:
						{
							configSp.speedPercent_Spindle = newSpeedPercent;
							currentSpeedPercent = (float)(newSpeedPercent * .01 * direction);
							break;
						}
					}




					EEPROM.put(eePromAddress_Setup, configSp);
	#ifdef DEBUG
					//Serial.print("currentSpeedPercent:");
					//Serial.println(currentSpeedPercent);

	#endif // DEBUG
					rotateController.overrideSpeed(currentSpeedPercent); // set new speed
					break;
				}
				default:
				{
					break;
				}
			}
		}

		if (digitalRead(limitPin_Max) == LOW)
		{
			rotateController.overrideSpeed(0);
			rotateController.stop();
			goto endLoop;
		}
		else if (digitalRead(limitPin_Min) == LOW)
		{
			rotateController.overrideSpeed(0);
			rotateController.stop();
			goto endLoop;
		}
		delay(10);
	}

	endLoop:
	// Reset enable/disable status on Nextion
	digitalWrite(enablePin, HIGH); // Disable 
	Serial3.print("pageSpindle2.bt10.bco1=55715");// The first one may be ignored by Nextion
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageSpZ.vaSpEnabled.val=1");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageSpindle2.bt10.val=1");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageSpindle2.bt10.txt=");
	SerialWrite(0x22);
	SerialPrint("Disabled");
	SerialWrite(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
}


 ///<summary>
/// RunTwoSteppers_Sp_Axis
/// </summary>
/// <comment>
/// </comment>
/// <param name="direction_Axis">Axis rotation</param>
/// <param name="direction_Spindle">Spindle rotation</param>
/// <param name="initialCaller">Spindle or Axis</param>
/// <param name="axisId">Z, X, or B Axis</param>
void RunTwoSteppers_Sp_Axis(
	int direction_Axis,
	int direction_Spindle,
	int initialCaller,
	int axisId
)
{
	RotateControl rotateControllerAxis;
	RotateControl rotateControllerSpindle;
	Stepper stepper_Spindle(PIN_SPINDLE_STEP, PIN_SPINDLE_DIR);
	int stepPin_Axis = PIN_AXIS_Z_STEP;
	int dirPin_Axis = PIN_AXIS_Z_DIR;
	int enablePin_Axis = PIN_AXIS_Z_ENABLE;
	int limitPin_Max = configMain.limit_Max_Z;
	int limitPin_Min = configMain.limit_Min_Z;

	int polarity_Axis = LOW;
	int maxSpd_Axis = 0;
	int accel_Axis = 0;

	int polarity_Spindle = configMain.polarity_Spindle ? (LOW) : (HIGH);
	int maxSpd_Spindle = 0;
	int accel_Spindle = 0;

	float speedPercentAxis = 0;
	float currentSpeedPercentAxis = 0;
	float speedPercentSpindle = 0;
	float currentSpeedPercentSpindle = 0;
	int32_t targetPosition_Axis = 2000000000 * direction_Axis;
	int32_t targetPosition_Spindle = 2000000000 * direction_Spindle;

	// initialCaller: Spindle or axis
	bool stepper_Axis_Go = false;
	bool stepper_Spindle_Go = false;

	if (initialCaller < ID_SPINDLE)
	{
		stepper_Axis_Go = true;
	}
	else
	{
		stepper_Spindle_Go = true;
	}

	switch (axisId)
	{
		case ID_AXIS_Z:
		{
			stepPin_Axis = PIN_AXIS_Z_STEP;
			dirPin_Axis = PIN_AXIS_Z_DIR;
			enablePin_Axis = PIN_AXIS_Z_ENABLE;
			polarity_Axis = configMain.polarity_Axis_Z ? (LOW) : (HIGH);
			maxSpd_Axis = configSpZ.maxSpd_Axis;// *direction_Axis;
			accel_Axis = configSpZ.accel_Axis;
			speedPercentAxis = configMain.speedPercent_Axis_Z;
			maxSpd_Spindle = configSpZ.maxSpd_Spindle;
			accel_Spindle = configSpZ.accel_Spindle;
			speedPercentSpindle = configMain.speedPercentSpindle_SpZ;
		
			limitPin_Max = configMain.limit_Max_Z;
			limitPin_Min = configMain.limit_Min_Z;
			break;
		}
		case ID_AXIS_X:
		{
			stepPin_Axis = PIN_AXIS_X_STEP;
			dirPin_Axis = PIN_AXIS_X_DIR;
			enablePin_Axis = PIN_AXIS_X_ENABLE;
			polarity_Axis = configMain.polarity_Axis_X ? (LOW) : (HIGH);
			maxSpd_Axis = configSpX.maxSpd_Axis;
			accel_Axis = configSpX.accel_Axis;

			maxSpd_Spindle = configSpX.maxSpd_Spindle;
			accel_Spindle = configSpX.accel_Spindle;

			speedPercentAxis = configMain.speedPercent_Axis_X;
			speedPercentSpindle = configMain.speedPercentSpindle_SpX;

			limitPin_Max = configMain.limit_Max_X;
			limitPin_Min = configMain.limit_Min_X;
			break;
		}
		case ID_AXIS_B:
		{
			stepPin_Axis = PIN_AXIS_B_STEP;
			dirPin_Axis = PIN_AXIS_B_DIR;
			enablePin_Axis = PIN_AXIS_B_ENABLE;
			polarity_Axis = configMain.polarity_Axis_B ? (LOW) : (HIGH);
			maxSpd_Axis = configSpB.maxSpd_Axis;
			accel_Axis = configSpB.accel_Axis;

			maxSpd_Spindle = configSpB.maxSpd_Spindle;
			accel_Spindle = configSpB.accel_Spindle;

			speedPercentAxis = configMain.speedPercent_Axis_B;
			speedPercentSpindle = (float)(configMain.speedPercentSpindle_SpB *.01 * direction_Spindle);

			limitPin_Max = configMain.limit_Max_B;
			limitPin_Min = configMain.limit_Min_B;
			break;
		}
	}

	currentSpeedPercentAxis = speedPercentAxis * .01 * direction_Axis;
	currentSpeedPercentSpindle = speedPercentSpindle * .01 * direction_Spindle;

	Stepper stepper_Axis(stepPin_Axis, dirPin_Axis);
	stepper_Axis.setPosition(0);
	stepper_Spindle.setPosition(0);

	delay(5);

	stepper_Axis
		.setStepPinPolarity(polarity_Axis)
		.setMaxSpeed(maxSpd_Axis)
		.setAcceleration(accel_Axis)
		.setTargetAbs(targetPosition_Axis);

	stepper_Spindle
		.setStepPinPolarity(polarity_Spindle)
		.setMaxSpeed(maxSpd_Spindle)
		.setAcceleration(accel_Spindle)
		.setTargetAbs(targetPosition_Spindle);


#ifdef DEBUG
	Serial.print("initialCaller:");
	Serial.println(initialCaller);

	//Serial.print("direction_Spindle:");
	//Serial.println(direction_Spindle);
	//Serial.print("maxSpd_Spindle:");
	//Serial.println(maxSpd_Spindle);
	//Serial.print("speedPercentSpindle:");
	//Serial.println(speedPercentSpindle);
	//Serial.print("targetPosition_Axis:");
	//Serial.println(targetPosition_Axis);
	//Serial.println("------------------------------");
	//Serial.print("SpindleAccel:");
	//Serial.println(accel_Spindle);

	//Serial.print("microsteps_Spindle:");
	//Serial.println(configMain.microsteps_Spindle);
	//Serial.print("steps360_Spindle:");
	//Serial.println(configMain.steps360_Spindle);
	//Serial.print("gearRatio_Spindle:");
	//Serial.println(configMain.gearRatio_Spindle);
	Serial.println("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
	Serial.print("direction_Axis:");
	Serial.println(direction_Axis);
	Serial.print("polarity_Axis");
	Serial.println(polarity_Axis);
	Serial.print("maxSpd_Axis:");
	Serial.println(maxSpd_Axis);
	Serial.print("accel_Axis:");
	Serial.println(accel_Axis);
	Serial.print("speedPercentAxis:");
	Serial.println(speedPercentAxis);
	Serial.print("currentSpeedPercentAxis:");
	Serial.println(currentSpeedPercentAxis);
	Serial.print("targetPosition_Axis:");
	Serial.println(targetPosition_Axis);
	Serial.println("+++++++++++++++++++++++++++++++");
	Serial.print("getPosition:");
	Serial.println(stepper_Axis.getPosition());
	//Serial.print("AxisAccel:");
	//Serial.println(accel_Axis);

	//Serial.print("distancePerRev_AxisZ:");
	//Serial.println(configMain.distancePerRev_AxisZ);
	Serial.print("steps360_Axis_Z:");
	Serial.println(configMain.steps360_Axis_Z);
	Serial.print("microsteps_Axis_Z:");
	Serial.println(configMain.microsteps_Axis_Z);
	Serial.println("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");

#endif // DEBUG

	if (stepper_Axis_Go)
	{
		digitalWrite(enablePin_Axis, LOW);  // Enable
		rotateControllerAxis.rotateAsync(stepper_Axis);
		rotateControllerAxis.overrideSpeed(0);
		delay(100);
		rotateControllerAxis.overrideSpeed(currentSpeedPercentAxis);
	}
	if (stepper_Spindle_Go)
	{
		digitalWrite(PIN_SPINDLE_ENABLE, LOW); // Enable 
		delay(5);
		rotateControllerSpindle.rotateAsync(stepper_Spindle);
		rotateControllerSpindle.overrideSpeed(0);
		delay(100);
		rotateControllerSpindle.overrideSpeed(currentSpeedPercentSpindle);
	}

	while (stepper_Axis_Go || stepper_Spindle_Go)
	{

		delay(5);
		if (SerialAvailable() >= 0)
		{
			incomingByte = SerialRead(serialId);
			Serial.print("incomingByte:");
			Serial.println(incomingByte);
			switch (incomingByte)
			{
			case 90: // Z - Sp-Z Axis CW
			case 166: // � - Sp-X Axis CW
			case 172: // � - Sp-B Axis CW
			{
				if (!rotateControllerAxis.isRunning())
				{
					Serial.println("Axis is not running.");

					direction_Axis = DIR_CW;
					targetPosition_Axis = 2000000000 * direction_Axis;
					switch (axisId)
					{
					case ID_AXIS_Z:
					{
						speedPercentAxis = (float)(configMain.speedPercent_Axis_Z * .01);// *direction_Axis);
						break;
					}
					case ID_AXIS_X:
					{
						speedPercentAxis = (float)(configMain.speedPercent_Axis_X * .01 * direction_Axis);
						break;
					}
					case ID_AXIS_B:
					{
						speedPercentAxis = (float)(configMain.speedPercent_Axis_B * .01 * direction_Axis);
						break;
					}
					}
					stepper_Axis
						.setMaxSpeed(maxSpd_Axis)
						.setTargetAbs(targetPosition_Axis);
					stepper_Axis_Go = true;
					digitalWrite(enablePin_Axis, LOW);  // Enable
					rotateControllerAxis.rotateAsync(stepper_Axis);
					rotateControllerAxis.overrideSpeed(0);
					delay(5);
					rotateControllerAxis.overrideSpeed(speedPercentAxis);
				}
				else
				{
					Serial.println("Axis is running.");
				}
				break;
			}
			case 91: // [ - Sp-Z Axis CCW
			case 165: // � - Sp-X Axis CCW
			case 171: // � - Sp-B Axis CCW
			{
				if (!rotateControllerAxis.isRunning())
				{
					direction_Axis = DIR_CCW;
					targetPosition_Axis = 2000000000 * direction_Axis;


					switch (axisId)
					{
					case ID_AXIS_Z:
					{
						speedPercentAxis = (float)(configMain.speedPercent_Axis_Z * .01);// *direction_Axis);
						break;
					}
					case ID_AXIS_X:
					{
						speedPercentAxis = (float)(configMain.speedPercent_Axis_X * .01 * direction_Axis);
						break;
					}
					case ID_AXIS_B:
					{
						speedPercentAxis = (float)(configMain.speedPercent_Axis_B * .01 * direction_Axis);
						break;
					}
					}

					stepper_Axis
						.setMaxSpeed(maxSpd_Axis)
						.setTargetAbs(targetPosition_Axis);
					stepper_Axis_Go = true;
					digitalWrite(enablePin_Axis, LOW);  // Enable 
					rotateControllerAxis.rotateAsync(stepper_Axis);
					rotateControllerAxis.overrideSpeed(0);
					delay(5);
					rotateControllerAxis.overrideSpeed(speedPercentAxis);
				}
				break;
			}
			case 67: // C - Spindle Stop
			{
				Serial.print("Stop Spindle:");
				rotateControllerSpindle.overrideSpeed(0);
				delay(5);
				rotateControllerSpindle.stop();
				digitalWrite(PIN_SPINDLE_ENABLE, HIGH); // Disable

				stepper_Spindle_Go = false;
				break;
			}
			case 97: // a - Sp-Z axis speed
			case 162: // � - Sp-X axis speed
			case 168: // � - Sp-B axis speed
			{
				int newSpeedPercent = (int)GetSerialFloat(serialId);
#ifdef DEBUG
				Serial.print("NewSpeedPercent: ");
				Serial.println(newSpeedPercent);
#endif // DEBUG

				switch (axisId)
				{
				case ID_AXIS_Z:
				{
					configMain.speedPercent_Axis_Z = newSpeedPercent;
					speedPercentAxis = (float)(newSpeedPercent * .01);// *direction_Axis);
					break;
				}
				case ID_AXIS_X:
				{
					configMain.speedPercent_Axis_X = newSpeedPercent;
					speedPercentAxis = (float)(newSpeedPercent * .01 * direction_Axis);
					break;
				}
				case ID_AXIS_B:
				{
					configMain.speedPercent_Axis_B = newSpeedPercent;
					speedPercentAxis = (float)(newSpeedPercent * .01 * direction_Axis);
					break;
				}
				}
				EEPROM.put(eePromAddress_Setup, configMain);

				rotateControllerAxis.overrideSpeed(speedPercentAxis);
				break;
			}
			case 99: // c - Axis stop
			{
				rotateControllerAxis.overrideSpeed(0);
				delay(5);
				rotateControllerAxis.stop();
				digitalWrite(enablePin_Axis, HIGH);  // Disable 
				//stepper_Z.setPosition(0);
				stepper_Axis_Go = false;
				break;
			}
			case 107: // k - Sp-Z spindle CW
			case 164: // � - Sp-X spindle CW
			case 170: // � - Sp-B spindle CW
			{
				if (!rotateControllerSpindle.isRunning())
				{
					direction_Spindle = DIR_CW;
					targetPosition_Spindle = 2000000000 * direction_Spindle;

					switch (axisId)
					{
					case ID_AXIS_Z:
					{
						speedPercentSpindle = (float)(configMain.speedPercentSpindle_SpZ *.01 * direction_Spindle);
						break;
					}
					case ID_AXIS_X:
					{
						speedPercentSpindle = (float)(configMain.speedPercentSpindle_SpX *.01 * direction_Spindle);
						break;
					}
					case ID_AXIS_B:
					{
						speedPercentSpindle = (float)(configMain.speedPercentSpindle_SpB *.01 * direction_Spindle);
						break;
					}
					}


					stepper_Spindle
						.setMaxSpeed(maxSpd_Spindle)
						.setTargetAbs(targetPosition_Spindle);
					stepper_Spindle_Go = true;
					digitalWrite(PIN_SPINDLE_ENABLE, LOW); // Enable
					rotateControllerSpindle.rotateAsync(stepper_Spindle);
					rotateControllerSpindle.overrideSpeed(0);
					delay(5);
					rotateControllerSpindle.overrideSpeed(speedPercentSpindle);
				}
				break;
			}
			case 108: // l - Sp-Z spindle CCW
			case 163: // � - Sp-X spindle CCW
			case 169: // � - Sp-B spindle CCW
			{
				if (!rotateControllerSpindle.isRunning())
				{
					direction_Spindle = DIR_CCW;
					targetPosition_Spindle = 2000000000 * direction_Spindle;
					switch (axisId)
					{
					case ID_AXIS_Z:
					{
						speedPercentSpindle = (float)(configMain.speedPercentSpindle_SpZ *.01 * direction_Spindle);
						break;
					}
					case ID_AXIS_X:
					{
						speedPercentSpindle = (float)(configMain.speedPercentSpindle_SpX *.01 * direction_Spindle);
						break;
					}
					case ID_AXIS_B:
					{
						speedPercentSpindle = (float)(configMain.speedPercentSpindle_SpB *.01 * direction_Spindle);
						break;
					}
					}
					stepper_Spindle
						.setMaxSpeed(maxSpd_Spindle)
						.setTargetAbs(targetPosition_Spindle);
					stepper_Spindle_Go = true;
					digitalWrite(PIN_SPINDLE_ENABLE, LOW); // Enable
					rotateControllerSpindle.rotateAsync(stepper_Spindle);
					rotateControllerSpindle.overrideSpeed(0);
					delay(5);
					rotateControllerSpindle.overrideSpeed(speedPercentSpindle);
				}
				break;
			}
			case 110: // n - Sp-Z spindle speed
			case 161: // � - Sp-X spindle speed
			case 167: // � - Sp-B spindle speed
			{
				switch (axisId)
				{
				case ID_AXIS_Z:
				{
					configMain.speedPercentSpindle_SpZ = (int)GetSerialFloat(serialId);
					speedPercentSpindle = (float)(configMain.speedPercentSpindle_SpZ * .01 * direction_Spindle);
					break;
				}
				case ID_AXIS_X:
				{
					configMain.speedPercentSpindle_SpX = (int)GetSerialFloat(serialId);
					speedPercentSpindle = (float)(configMain.speedPercentSpindle_SpX* .01 * direction_Spindle);
					break;
				}
				case ID_AXIS_B:
				{
					configMain.speedPercentSpindle_SpB = (int)GetSerialFloat(serialId);
					speedPercentSpindle = (float)(configMain.speedPercentSpindle_SpB * .01 * direction_Spindle);
					break;
				}
				}
				EEPROM.put(eePromAddress_Setup, configMain);
				rotateControllerSpindle.overrideSpeed(speedPercentSpindle);
				break;
			}
			}
		}

		if (stepper_Axis_Go)
		{
			if (digitalRead(limitPin_Max) == LOW)
			{
				rotateControllerAxis.overrideSpeed(0);
				rotateControllerAxis.stop();
				stepper_Axis_Go = false;
			}
		}
		else
		{
			if (digitalRead(limitPin_Min) == LOW)
			{
				rotateControllerAxis.overrideSpeed(0);
				rotateControllerAxis.stop();
				stepper_Axis_Go = false;
			}
		}

		//#ifdef DEBUG
		//
		//		endPosition_Spindle = stepper_Spindle.getPosition();
		//		Serial.print("Spindle:");
		//		Serial.println(endPosition_Spindle);
		//#endif // DEBUG
		delay(5);
	}

	stepper_Axis_Go = false;
	stepper_Spindle_Go = false;

	returnSteps_Axis  = stepper_Axis.getPosition();
	returnSteps_Spindle = stepper_Spindle.getPosition();
	stepper_Axis.setPosition(0);
	stepper_Spindle.setPosition(0);
	digitalWrite(PIN_SPINDLE_ENABLE, HIGH); // Disable 
	digitalWrite(PIN_AXIS_Z_ENABLE, HIGH);  // Disable 
	digitalWrite(PIN_AXIS_X_ENABLE, HIGH);  // Disable 
	digitalWrite(PIN_AXIS_B_ENABLE, HIGH);  // Disable 
}

/// <summary>
/// Run steppers
/// </summary>
/// <comment>
/// </comment>
/// <param name="direction_Axis">Counterclockwise: 0, Clockwise: 1</param>
/// <returns>Success: 0, Cancel/Stop: -1</returns>
int RunTwoSteppers_SpZ(
	int direction_Axis,
	int direction_Spindle,
	int initialCaller
)
{


	// Axis Stop flag (Used to reset Nextion when limit switch is hit.)
	bool stopFlag_Axis = false;

	// initialCaller: Spindle or axis
	bool stepper_Axis_Z_Go = false;
	bool stepper_Spindle_Go = false;

	if (initialCaller < 2)
	{
		stepper_Axis_Z_Go = true;

	}
	else //if (initialCaller == 3)
	{
		stepper_Spindle_Go = true;
	}

	// Configure Axis
	float nextSpeed_Axis = configMain.speedPercent_Axis_Z * configSpZ.maxSpd_Axis * .01;
	accelStep_Axis_Z.setMaxSpeed(nextSpeed_Axis);
	accelStep_Axis_Z.setAcceleration(configSpZ.accel_Axis);
	accelStep_Axis_Z.setCurrentPosition(0);
	long moveTo_Axis = 1000000000;
	if (direction_Axis == DIR_CW)
	{
		moveTo_Axis = -moveTo_Axis;
	}
	accelStep_Axis_Z.moveTo(moveTo_Axis);


	// Configure Spindle
	int spindleSpeedPercentage = configMain.speedPercentSpindle_SpZ;
	int spindleMaxSpeed = configSpZ.maxSpd_Spindle;

	float nextSpeed_Spindle = spindleSpeedPercentage * spindleMaxSpeed * .01;
	accelStep_Spindle.setMaxSpeed(nextSpeed_Spindle);
	accelStep_Spindle.setAcceleration(configSpZ.accel_Spindle);
	accelStep_Spindle.setCurrentPosition(0);
	long moveTo_Spindle = 1000000000;

	if (direction_Spindle == DIR_CCW)
	{
		moveTo_Spindle = -moveTo_Spindle;
	}
	accelStep_Spindle.moveTo(moveTo_Spindle);
	
	while ((accelStep_Spindle.distanceToGo() != 0) || (accelStep_Axis_Z.distanceToGo() != 0))
	{
		stopFlag_Axis = false;
		// Check for Cancel code  
		if (Serial3.available() >= 0)
		{
			incomingByte = SerialRead(serialId);
			switch (incomingByte)
			{
			case 90: // Z - Axis Clockwise
			{
				configMain.speedPercent_Axis_Z = GetSerialInteger();
				EEPROM.put(eePromAddress_Setup, configMain);
				nextSpeed_Axis = configMain.speedPercent_Axis_Z  * configSpZ.maxSpd_Axis * .01;
				accelStep_Axis_Z.setMaxSpeed(nextSpeed_Axis);
				accelStep_Axis_Z.setCurrentPosition(0);
				accelStep_Axis_Z.moveTo(-2000000000);
				stepper_Axis_Z_Go = true;
				stopFlag_Axis = false;
				break;
			}
			case 91: // [ - Axis counter clockwise
			{
				nextSpeed_Axis = configMain.speedPercent_Axis_Z  * configSpZ.maxSpd_Axis * .01;
				accelStep_Axis_Z.setMaxSpeed(nextSpeed_Axis);
				accelStep_Axis_Z.setCurrentPosition(0);
				accelStep_Axis_Z.moveTo(2000000000);
				stepper_Axis_Z_Go = true;
				stopFlag_Axis = false;
				break;
			}
			case 67: // C - Spindle Stop
			{
				accelStep_Spindle.stop();
				accelStep_Spindle.setCurrentPosition(0);
				stepper_Spindle_Go = false;
				// Disable the spindle stepper motor
				accelStep_Spindle.disableOutputs();
				digitalWrite(PIN_SPINDLE_ENABLE, HIGH); // Disable
				if (!stepper_Axis_Z_Go)
				{
					accelStep_Axis_Z.setCurrentPosition(0);
				}

#ifdef DEBUG
				Serial.println("Stop Spindle ");
#endif // DEBUG

				break;
			}
			case 97: // a - Axis speed
			{
				delay(10);
				configMain.speedPercent_Axis_Z = (int)GetSerialFloat(serialId);
				EEPROM.put(eePromAddress_Setup, configMain);
				nextSpeed_Axis = configMain.speedPercent_Axis_Z * configSpZ.maxSpd_Axis * .01;
				accelStep_Axis_Z.setMaxSpeed(nextSpeed_Axis);
#ifdef DEBUG
				Serial.print("speedPercent_Axis_Z: ");
				Serial.println(configMain.speedPercent_Axis_Z);
				Serial.print("maxSpd_Axis: ");
				Serial.println(configSpZ.maxSpd_Axis);
				Serial.print("nextSpeed_Axis: ");
				Serial.println(nextSpeed_Axis);
#endif // DEBUG

				break;
			}
			case 99: // c - Axis stop
			{
				stepper_Axis_Z_Go = false;
				stopFlag_Axis = true;
				accelStep_Axis_Z.stop();
				accelStep_Axis_Z.setCurrentPosition(0);
				// Disable the Z stepper motor
				accelStep_Axis_Z.disableOutputs();

				digitalWrite(PIN_AXIS_Z_ENABLE, HIGH);  // Disable 

#ifdef DEBUG
				Serial.println("Stop Axis");
#endif // DEBUG

				if (!stepper_Spindle_Go)
				{
					accelStep_Spindle.setCurrentPosition(0);
				}
				break;
			}
			case 107: // k - Spindle Clockwise
			{
				nextSpeed_Spindle = configMain.speedPercentSpindle_SpZ * configSpZ.maxSpd_Spindle * .01;
				accelStep_Spindle.setMaxSpeed(nextSpeed_Spindle);
				accelStep_Spindle.setCurrentPosition(0);
				accelStep_Spindle.moveTo(2000000000);
				stepper_Spindle_Go = true;
				break;
			}
			case 108: // l - Spindle Counterclockwise
			{
				nextSpeed_Spindle = configMain.speedPercentSpindle_SpZ * configSpZ.maxSpd_Spindle * .01;
				accelStep_Spindle.setMaxSpeed(nextSpeed_Spindle);
				accelStep_Spindle.setCurrentPosition(0);
				accelStep_Spindle.moveTo(-2000000000);
				stepper_Spindle_Go = true;
				break;
			}
			case 110: // n - Set Spindle Speed
			{
				delay(10);
				configMain.speedPercentSpindle_SpZ = (int)GetSerialFloat(serialId);
#ifdef DEBUG
				Serial.print("speedPercentSpindle_SpZ: ");
				Serial.println(configMain.speedPercentSpindle_SpZ);
#endif // DEBUG
				float nextSpeed = configMain.speedPercentSpindle_SpZ * configSpZ.maxSpd_Spindle * .01;
				accelStep_Spindle.setMaxSpeed(nextSpeed);
				EEPROM.put(eePromAddress_Setup, configMain);
#ifdef DEBUG
				Serial.print("Speed: ");
				Serial.println(nextSpeed);
#endif // DEBUG
				break;
			}
			}
		}

		// Limit switches ?
		if (accelStep_Axis_Z.distanceToGo() > 0)
		{
			if (digitalRead(configMain.limit_Max_Z) == LOW)
			{
				accelStep_Axis_Z.stop();

				// Disable the Z stepper motor
				accelStep_Axis_Z.disableOutputs();
				digitalWrite(PIN_AXIS_Z_ENABLE, HIGH);  // Disable 
				stopFlag_Axis = true;
				stepper_Axis_Z_Go = false;
				if (!stepper_Spindle_Go)
				{
					accelStep_Spindle.setCurrentPosition(0);
				}
			}
		}
		else
		{
			if (digitalRead(configMain.limit_Min_Z) == LOW)
			{
				accelStep_Axis_Z.stop();

				// Disable the Z stepper motor
				accelStep_Axis_Z.disableOutputs();
				digitalWrite(PIN_AXIS_Z_ENABLE, HIGH);  // Disable 
				stopFlag_Axis = true;
				stepper_Axis_Z_Go = false;
				if (!stepper_Spindle_Go)
				{
					accelStep_Spindle.setCurrentPosition(0);
				}
			}
		}

		if (stepper_Spindle_Go)
		{
			accelStep_Spindle.enableOutputs();
			accelStep_Spindle.run();
		}
		else
		{
			// Disable the spindle stepper motor
			accelStep_Spindle.disableOutputs();
			digitalWrite(PIN_SPINDLE_ENABLE, HIGH); // Disable
		}

		if (stepper_Axis_Z_Go)
		{
			// Check limit switches
			if (accelStep_Axis_Z.distanceToGo() > 0)
			{
				if (digitalRead(configMain.limit_Max_Z) == LOW)
				{
					accelStep_Axis_Z.stop();

					// Disable the Z stepper motor
					accelStep_Axis_Z.disableOutputs();
					digitalWrite(PIN_AXIS_Z_ENABLE, HIGH);  // Disable 
					stopFlag_Axis = true;
					stepper_Axis_Z_Go = false;
				}
			}
			else
			{
				if (digitalRead(configMain.limit_Min_Z) == LOW)
				{
					accelStep_Axis_Z.stop();

					// Disable the Z stepper motor
					accelStep_Axis_Z.disableOutputs();
					digitalWrite(PIN_AXIS_Z_ENABLE, HIGH);  // Disable 
					stopFlag_Axis = true;
					stepper_Axis_Z_Go = false;
				}
			}

			if (stopFlag_Axis)
			{
				accelStep_Axis_Z.disableOutputs();
			}
			else
			{
				accelStep_Axis_Z.enableOutputs();
				accelStep_Axis_Z.run();
			}

		}
		else
		{
			// Disable the Z stepper motor
			accelStep_Axis_Z.disableOutputs();
			digitalWrite(PIN_AXIS_Z_ENABLE, HIGH);  // Disable 
		}
	}

	stepper_Axis_Z_Go = false;
	stepper_Spindle_Go = false;
	returnSteps_Axis = accelStep_Axis_Z.currentPosition();
	returnSteps_Spindle = accelStep_Spindle.currentPosition();
	accelStep_Axis_Z.setCurrentPosition(0);
	accelStep_Spindle.setCurrentPosition(0);
	// Disable the spindle stepper motor
	accelStep_Spindle.disableOutputs();
	digitalWrite(PIN_SPINDLE_ENABLE, HIGH); // Disable
	// Disable the Z stepper motor
	accelStep_Axis_Z.disableOutputs();
	digitalWrite(PIN_AXIS_Z_ENABLE, HIGH);  // Disable 

	return 0;
}

///////////////////////////////////////
/// <summary>
/// RunTwoSteppers_SpZ --- BugBug: Speed and direction failure
/// </summary>
/// <comment>
/// </comment>
/// <param name="direction_Axis">Axis rotation</param>
/// <param name="direction_Spindle">Spindle rotation</param>
/// <param name="initialCaller">Spindle or Axis</param>
void RunTwoSteppers_SpZzz(
	int direction_Axis,
	int direction_Spindle,
	int initialCaller
)
{
	RotateControl rotateControllerAxis;
	RotateControl rotateControllerSpindle;
	Stepper stepper_Spindle(PIN_SPINDLE_STEP, PIN_SPINDLE_DIR);
	Stepper stepper_Z(PIN_AXIS_Z_STEP, PIN_AXIS_Z_DIR);


	stepper_Z.setPosition(0);
	stepper_Spindle.setPosition(0);
	
	// initialCaller: Spindle or axis
	bool stepper_Axis_Z_Go = false;
	bool stepper_Spindle_Go = false;

	if (initialCaller < 2)
	{
		stepper_Axis_Z_Go = true;
	}
	else 
	{
		stepper_Spindle_Go = true;
	}

	int32_t targetPosition_Axis = 2000000000 * direction_Axis;
	int32_t targetPosition_Spindle = 2000000000;// *direction_Spindle;
	float speedPercentAxis = (float)(configMain.speedPercent_Axis_Z * .01);// *direction_Axis);
	float speedPercentSpindle = (float)(configMain.speedPercentSpindle_SpZ *.01);// *direction_Spindle);
	if (direction_Spindle == DIR_CW)
	{
		//targetPosition_Spindle = -targetPosition_Spindle;
		speedPercentSpindle = -speedPercentSpindle;
	}
	stepper_Z
		.setStepPinPolarity(configMain.polarity_Axis_Z ? (LOW) : (HIGH))
		.setMaxSpeed(configSpZ.maxSpd_Axis)
		.setAcceleration(configSpZ.accel_Axis)
		.setTargetAbs(targetPosition_Axis);

	stepper_Spindle
		.setStepPinPolarity(configMain.polarity_Spindle ? (LOW) : (HIGH))
		.setMaxSpeed(configSpZ.maxSpd_Spindle)
		.setAcceleration(configSpZ.accel_Spindle)
		.setTargetAbs(targetPosition_Spindle);


#ifdef DEBUG
	Serial.print("initialCaller:");
	Serial.println(initialCaller);

	Serial.print("direction_Spindle:");
	Serial.println(direction_Spindle);
	Serial.print("maxSpd_Spindle:");
	Serial.println(configSpZ.maxSpd_Spindle);
	Serial.print("speedPercentSpindle:");
	Serial.println(speedPercentSpindle);
	Serial.print("targetPosition_Axis:");
	Serial.println(targetPosition_Axis);
	Serial.println("------------------------------");
	//Serial.print("SpindleAccel:");
	//Serial.println(configSpZ.accel_Spindle);

	//Serial.print("microsteps_Spindle:");
	//Serial.println(configMain.microsteps_Spindle);
	//Serial.print("steps360_Spindle:");
	//Serial.println(configMain.steps360_Spindle);
	//Serial.print("gearRatio_Spindle:");
	//Serial.println(configMain.gearRatio_Spindle);
	Serial.println("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
	Serial.print("direction_Axis:");
	Serial.println(direction_Axis);
	Serial.print("maxSpd_Axis:");
	Serial.println(configSpZ.maxSpd_Axis);
	Serial.print("speedPercentAxis:");
	Serial.println(speedPercentAxis);
	Serial.print("targetPosition_Axis:");
	Serial.println(targetPosition_Axis);
	Serial.println("+++++++++++++++++++++++++++++++");
	//Serial.print("AxisAccel:");
	//Serial.println(configSpZ.accel_Axis);

	//Serial.print("distancePerRev_AxisZ:");
	//Serial.println(configMain.distancePerRev_AxisZ);
	Serial.print("steps360_Axis_Z:");
	Serial.println(configMain.steps360_Axis_Z);
	Serial.print("microsteps_Axis_Z:");
	Serial.println(configMain.microsteps_Axis_Z);
	Serial.println("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");

#endif // DEBUG

	if (stepper_Axis_Z_Go)
	{
		digitalWrite(PIN_AXIS_Z_ENABLE, LOW);  // Enable
		rotateControllerAxis.rotateAsync(stepper_Z);
		rotateControllerAxis.overrideSpeed(speedPercentAxis);
	}
	if (stepper_Spindle_Go)
	{
		digitalWrite(PIN_SPINDLE_ENABLE, LOW); // Enable 
		delay(5);
		rotateControllerSpindle.rotateAsync(stepper_Spindle);
		rotateControllerSpindle.overrideSpeed(speedPercentSpindle);
	}

	while (stepper_Axis_Z_Go || stepper_Spindle_Go)
	{

		delay(5);
		if (SerialAvailable() >= 0)
		{
			incomingByte = SerialRead(serialId);
			Serial.print("incomingByte:");
			Serial.println(incomingByte);
			switch (incomingByte)
			{
				case 90: // Z - Sp-Z Axis CW
				{
					if (!rotateControllerAxis.isRunning())
					{
						Serial.println("Axis is not running.");

						direction_Axis = DIR_CW;
						targetPosition_Axis = 2000000000;// *direction_Axis;
						//speedPercentAxis = (float)(configMain.speedPercent_Axis_Z * .01);// *direction_Axis);
						stepper_Z
							.setMaxSpeed(configSpZ.maxSpd_Axis)
							.setTargetAbs(targetPosition_Axis);
						stepper_Axis_Z_Go = true;
						digitalWrite(PIN_AXIS_Z_ENABLE, LOW);  // Enable
						rotateControllerAxis.rotateAsync(stepper_Z);
						rotateControllerAxis.overrideSpeed(speedPercentAxis);
					}
					else
					{
						Serial.println("Axis is running.");
					}
					break;
				}
				case 91: // [ - Sp-Z Axis CCW
				{
					if (!rotateControllerAxis.isRunning())
					{
						direction_Axis = DIR_CCW;
						targetPosition_Axis = -2000000000;// *direction_Axis;
						//speedPercentAxis = (float)(configMain.speedPercent_Axis_Z * .01);// *direction_Axis);
						stepper_Z
							.setMaxSpeed(configSpZ.maxSpd_Axis)
							.setTargetAbs(targetPosition_Axis);
						stepper_Axis_Z_Go = true;
						digitalWrite(PIN_AXIS_Z_ENABLE, LOW);  // Enable 
						rotateControllerAxis.rotateAsync(stepper_Z);
						rotateControllerAxis.overrideSpeed(speedPercentAxis);
					}
					break;
				}
				case 67: // C - Spindle Stop
				{
					Serial.print("Stop Spindle:");
					rotateControllerSpindle.overrideSpeed(0);
					delay(5);
					rotateControllerSpindle.stopAsync();
					digitalWrite(PIN_SPINDLE_ENABLE, HIGH); // Disable

					stepper_Spindle_Go = false;
					break;
				}
				case 97: // a - Sp-Z axis speed
				{
					configMain.speedPercent_Axis_Z = (int)GetSerialFloat(serialId);
					EEPROM.put(eePromAddress_Setup, configMain);
					speedPercentAxis = (float)(configMain.speedPercent_Axis_Z * .01);// *direction_Axis);
					rotateControllerAxis.overrideSpeed(speedPercentAxis);
#ifdef DEBUG
					Serial.print("SpeedPercent: ");
					Serial.println(speedPercentAxis);
#endif // DEBUG

					break;
				}
				case 99: // c - Axis stop
				{
					rotateControllerAxis.overrideSpeed(0);
					delay(5);
					rotateControllerAxis.stop();
					digitalWrite(PIN_AXIS_Z_ENABLE, HIGH);  // Disable 
					//stepper_Z.setPosition(0);
					stepper_Axis_Z_Go = false;
					break;
				}
				case 107: // k - Sp-Z spindle CW
				{
					if (!rotateControllerSpindle.isRunning())
					{
						direction_Spindle = DIR_CW;
						targetPosition_Spindle = -2000000000;// *direction_Spindle;
						speedPercentSpindle = (float)(configMain.speedPercentSpindle_SpZ * .01 *(-1));// *direction_Spindle);
						stepper_Spindle
							.setMaxSpeed(configSpZ.maxSpd_Spindle)
							.setTargetAbs(targetPosition_Spindle);
						stepper_Spindle_Go = true;
						digitalWrite(PIN_SPINDLE_ENABLE, LOW); // Enable
						rotateControllerSpindle.rotateAsync(stepper_Spindle);
						rotateControllerSpindle.overrideSpeed(speedPercentSpindle);
					}
					break;
				}
				case 108: // l - Sp-Z spindle CCW
				{
					if (!rotateControllerSpindle.isRunning())
					{
						direction_Spindle = DIR_CCW;
						targetPosition_Spindle = 2000000000;// *direction_Spindle;
						speedPercentSpindle = (float)(configMain.speedPercentSpindle_SpZ * .01);// *direction_Spindle);
						stepper_Spindle
							.setMaxSpeed(configSpZ.maxSpd_Spindle)
							.setTargetAbs(targetPosition_Spindle);
						stepper_Spindle_Go = true;
						digitalWrite(PIN_SPINDLE_ENABLE, LOW); // Enable
						rotateControllerSpindle.rotateAsync(stepper_Spindle);
						rotateControllerSpindle.overrideSpeed(speedPercentSpindle);
					}
					break;
				}
				case 110: // n - Sp-Z spindle speed
				{
					configMain.speedPercentSpindle_SpZ = (int)GetSerialFloat(serialId);
					EEPROM.put(eePromAddress_Setup, configMain);
					speedPercentSpindle = (float)(configMain.speedPercentSpindle_SpZ * .01 * direction_Spindle);
					rotateControllerSpindle.overrideSpeed(speedPercentSpindle);
					break;
				}
			}
		}

		if (stepper_Axis_Z_Go)
		{
			if (digitalRead(configMain.limit_Max_Z) == LOW)
			{
				rotateControllerAxis.overrideSpeed(0);
				rotateControllerAxis.stop();
				stepper_Axis_Z_Go = false;
			}
		}
		else
		{
			if (digitalRead(configMain.limit_Min_Z) == LOW)
			{
				rotateControllerAxis.overrideSpeed(0);
				rotateControllerAxis.stop();
				stepper_Axis_Z_Go = false;
			}
		}

#ifdef DEBUG

		//endPosition_Spindle = stepper_Spindle.getPosition();
		//Serial.print("Spindle:");
		//Serial.println(endPosition_Spindle);
#endif // DEBUG
		delay(5);
	}

	stepper_Axis_Z_Go = false;
	stepper_Spindle_Go = false;

	returnSteps_Axis = stepper_Z.getPosition();
	returnSteps_Spindle = stepper_Spindle.getPosition();
	stepper_Z.setPosition(0);
	stepper_Spindle.setPosition(0);
	digitalWrite(PIN_SPINDLE_ENABLE, HIGH); // Disable 
	digitalWrite(PIN_AXIS_X_ENABLE, HIGH);  // Disable 

}


/// <summary>
/// Index spindle with TeensyStep
/// </summary>
/// <comment>
/// </comment>
/// <param name="directionSpindle">Counterclockwise: -1, Clockwise: 1</param>
/// <returns></returns>
void IndexSpindle(int indexId, int directionSpindle)
{
	StepControl stepController;
	Stepper stepper_Spindle(PIN_SPINDLE_STEP, PIN_SPINDLE_DIR);

	stepper_Spindle.setPosition(0);
	double stepsToMove = configMain.size_Index1;

	switch (indexId)
	{
		case 1:
		{
			// Set speed and acceleration
			stepper_Spindle
				.setMaxSpeed(configIndex1.maxSpd)
				.setAcceleration(configIndex1.accel)
				.setTargetRel(stepsToMove * directionSpindle);
			break;
		}
		case 2:
		{
			stepsToMove = configMain.size_Index2;

			// Set speed and acceleration
			stepper_Spindle
				.setMaxSpeed(configIndex2.maxSpd)
				.setAcceleration(configIndex2.accel)
				.setTargetRel(stepsToMove * directionSpindle);
			break;
		}
	}

	digitalWrite(PIN_SPINDLE_ENABLE, LOW);
	stepController.moveAsync(stepper_Spindle);

	while (stepController.isRunning())
	{
#ifdef SHOW_POSITION //Controls must be added to Nextion pages
		//if (configMain.trackPositions == 1)
		//{
		//	endPosition_Spindle = teensyStep_Spindle.getPosition();
		//	degrees_Spindle = StepsToDegrees_Spindle(endPosition_Spindle);
		//	switch (indexId)
		//		{
		//		case 1:
		//		{
		//			Serial3.print("pageIndex1.t1.txt=");
		//			break;
		//		}
		//		case 2:
		//		{
		//			Serial3.print("pageIndex2.t1.txt=");
		//			break;
		//		}
		//	}

		//	Serial3.write(0x22);
		//	Serial3.print(degrees_Spindle);
		//	Serial3.write(0x22);
		//	Serial3.write(0xff);
		//	Serial3.write(0xff);
		//	Serial3.write(0xff);
		//	delay(10);
		//}
#endif // Show Position

		// Check for Cancel code  
		if (SerialAvailable() >= 0)
		{
			incomingByte = SerialRead(serialId);
			switch (incomingByte)
			{
				case 93: // - ] 
				case 99: // - c
				case 109: // - m
				{
					stepController.stop();
					break;
				}
			}
		}
		delay(10);
	}

	if (indexId == 2)
	{
		// Update Index flag on Nextion. 
		Serial3.print("pageIndex2.bt3.pco=59391");// The first one may be ignored by Nextion, so resend.
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageIndex2.va0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageIndex2.bt3.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageIndex2.bt2.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageIndex2.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
	}
	else
	{ 
		
		// Update Index flag on Nextion. 
		Serial3.print("pageIndex1.bt3.pco=59391");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageIndex1.va0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageIndex1.bt3.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageIndex1.bt2.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageIndex1.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageSync.b6.pco=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageSync.b5.pco=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageSyncX.b6.pco=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageSyncX.b5.pco=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
	}

	returnSteps_Axis = 0;
	returnSteps_Spindle = stepper_Spindle.getPosition();
#ifdef SHOW_POSITION //Controls must be added to Nextion pages
	degrees_Spindle = StepsToDegrees_Spindle(endPosition_Spindle);
	switch (indexId)
	{
	case 1:
	{
		Serial3.print("pageIndex1.t1.txt=");
		break;
	}
	case 2:
	{
		Serial3.print("pageIndex2.t1.txt=");
		break;
	}
	}

	Serial3.write(0x22);
	Serial3.print(degrees_Spindle);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(10);
#endif // Show Position

	digitalWrite(PIN_SPINDLE_ENABLE, HIGH); // Disable 
}

/// <summary>
/// Sync spindle and axis with TeensyStep
/// </summary>
/// <comment>
/// </comment>
/// <param name="directionSpindle">Counterclockwise: -1, Clockwise: 1</param>
/// <param name = "directionAxis">Counterclockwise: -1, Clockwise : 1< /param>
/// <param name ="idAxis"> Id of axis: Z = 0 or X = 1< /param>
/// <returns></returns>
void Sync(int directionSpindle, int directionAxis, int idAxis)
{
	StepControl stepController;
	Stepper stepper_Spindle(PIN_SPINDLE_STEP, PIN_SPINDLE_DIR);
	Stepper stepper_X(PIN_AXIS_X_STEP, PIN_AXIS_X_DIR);
	Stepper stepper_Z(PIN_AXIS_Z_STEP, PIN_AXIS_Z_DIR);
	float targetSteps_Spindle = 0;
	// Set speed and acceleration
	switch (idAxis)
	{
		case ID_AXIS_X:
		{
			targetSteps_Spindle = configMain.revolutionsSyncX_Spindle * configMain.gearRatio_Spindle * configMain.microsteps_Spindle * configMain.steps360_Spindle * directionSpindle;
			stepper_Spindle
				.setMaxSpeed(configSyncX.maxSpd_Spindle * configMain.speedPercentSpindle_SyncX * .01)
				.setAcceleration(configSyncX.accel_Spindle)
				.setTargetRel(targetSteps_Spindle);

			stepper_Spindle.setPosition(0);
			break;
		}
		case ID_AXIS_Z:
		{
			targetSteps_Spindle = configMain.revolutionsSyncZ_Spindle * configMain.gearRatio_Spindle * configMain.microsteps_Spindle * configMain.steps360_Spindle * directionSpindle;
			stepper_Spindle
				.setMaxSpeed(configSyncZ.maxSpd_Spindle * configMain.speedPercentSpindle_SyncZ * .01)
				.setAcceleration(configSyncZ.accel_Spindle)
				.setTargetRel(targetSteps_Spindle);
			stepper_Spindle.setPosition(0);
			break;
		}
	}
	int32_t axisStepsZ = (configMain.distanceSyncZ / configMain.distancePerRev_AxisZ) * configMain.microsteps_Axis_Z * configMain.steps360_Axis_Z * directionAxis;
	stepper_Z
		.setMaxSpeed(configSyncZ.maxSpd_Axis * configMain.speedPercentAxis_SyncZ * .01)
		.setAcceleration(configSyncZ.accel_Axis)
		.setTargetRel(axisStepsZ);
	stepper_Z.setPosition(0);

	int32_t axisStepsX = (configMain.distanceSyncX / configMain.distancePerRev_AxisX) * configMain.microsteps_Axis_X * configMain.steps360_Axis_X * directionAxis;
	stepper_X
		.setMaxSpeed(configSyncX.maxSpd_Axis * configMain.speedPercentAxis_SyncX * .01)
		.setAcceleration(configSyncX.accel_Axis)
		.setTargetRel(axisStepsX);
	stepper_X.setPosition(0);

#ifdef DEBUG
	Serial.print("idAxis:");
	Serial.println(idAxis);

	Serial.print("targetSteps_Spindle:");
	Serial.println(targetSteps_Spindle);
	Serial.print("microsteps_Spindle:");
	Serial.println(configMain.microsteps_Spindle);
	Serial.print("steps360_Spindle:");
	Serial.println(configMain.steps360_Spindle);
	Serial.print("gearRatio_Spindle:");
	Serial.println(configMain.gearRatio_Spindle);
	Serial.print("revolutionsSyncX_Spindle:");
	Serial.println(configMain.revolutionsSyncX_Spindle);
	switch (idAxis)
	{
		case ID_AXIS_X:
		{

			Serial.print("SpindleMaxSpd:");
			Serial.println(configSyncX.maxSpd_Spindle);
			Serial.print("SpindleSpeedPercent:");
			Serial.println(configMain.speedPercentSpindle_SyncX);
			Serial.print("SpindleAccel:");
			Serial.println(configSyncX.accel_Spindle);
			Serial.print("revolutionsSyncX_Spindle:");
			Serial.println(configMain.revolutionsSyncX_Spindle);
			/////////////////////////////
			Serial.print("distanceSyncX:");
			Serial.println(configMain.distanceSyncX);
			Serial.print("distancePerRev_AxisX:");
			Serial.println(configMain.distancePerRev_AxisX);
			Serial.print("steps360_Axis_X:");
			Serial.println(configMain.steps360_Axis_X);
			Serial.print("microsteps_Axis_X:");
			Serial.println(configMain.microsteps_Axis_X);
			Serial.print("distanceSyncX:");
			Serial.println(configMain.distanceSyncX);
			Serial.print("axisStepsX:");
			Serial.println(axisStepsX);

			Serial.print("AxisMaxSpd:");
			Serial.println(configSyncX.maxSpd_Axis);
			Serial.print("speedPercentAxis_SyncX:");
			Serial.println(configMain.speedPercentAxis_SyncX);
			Serial.print("AxisAccel:");
			Serial.println(configSyncX.accel_Axis);
			break;
		}
		case ID_AXIS_Z:
		{
			Serial.print("SpindleMaxSpd:");
			Serial.println(configSyncZ.maxSpd_Spindle);
			Serial.print("SpindleSpeedPercent:");
			Serial.println(configMain.speedPercentSpindle_SyncZ);
			Serial.print("SpindleAccel:");
			Serial.println(configSyncZ.accel_Spindle);
			Serial.print("revolutionsSyncZ_Spindle:");
			Serial.println(configMain.revolutionsSyncZ_Spindle);
			/////////////////////////////
			Serial.print("distanceSyncZ:");
			Serial.println(configMain.distanceSyncZ);
			Serial.print("distancePerRev_AxisZ:");
			Serial.println(configMain.distancePerRev_AxisZ);
			Serial.print("steps360_Axis_Z:");
			Serial.println(configMain.steps360_Axis_Z);
			Serial.print("microsteps_Axis_Z:");
			Serial.println(configMain.microsteps_Axis_Z);
			Serial.print("distanceSyncZ:");
			Serial.println(configMain.distanceSyncZ);
			Serial.print("axisStepsZ:");
			Serial.println(axisStepsZ);

			Serial.print("AxisMaxSpd:");
			Serial.println(configSyncZ.maxSpd_Axis);
			Serial.print("speedPercentAxis_SyncZ:");
			Serial.println(configMain.speedPercentAxis_SyncZ);
			Serial.print("AxisAccel:");
			Serial.println(configSyncZ.accel_Axis);
			break;
		}
	}
#endif // DEBUG

	switch (idAxis)
	{
		case ID_AXIS_X:
		{

			digitalWrite(PIN_SPINDLE_ENABLE, LOW); // Enable (Uses TeensyStep library)
			digitalWrite(PIN_AXIS_X_ENABLE, LOW);  // Enable (Uses TeensyStep library)
			stepController.moveAsync(stepper_Spindle, stepper_X);
			break;
		}
		case ID_AXIS_Z:
		{

			digitalWrite(PIN_SPINDLE_ENABLE, LOW); // Enable (Uses TeensyStep library)
			digitalWrite(PIN_AXIS_Z_ENABLE, LOW);  // Enable (Uses TeensyStep library)
			stepController.moveAsync(stepper_Spindle, stepper_Z);
			break;
		}
	}
	

	while (stepController.isRunning())
	{
#ifdef DEBUG

		endPosition_Spindle = stepper_Spindle.getPosition();
		switch (idAxis)
		{
			case ID_AXIS_X:
			{
				endPosition_Axis = stepper_X.getPosition();
#ifdef SHOW_POSITION //Controls must be added to Nextion pages
				if (configMain.trackPositions == 1)
				{
					degrees_Spindle = StepsToDegrees_Spindle(endPosition_Spindle);
					distance_Axis = StepsToDistance_Axis(endPosition_Axis, ID_AXIS_X);
					Serial3.print("pageSyncX.t1.txt=");
					Serial3.write(0x22);
					Serial3.print(degrees_Spindle);
					Serial3.write(0x22);
					Serial3.write(0xff);
					Serial3.write(0xff);
					Serial3.write(0xff);
					delay(10);
					Serial3.print("pageSyncX.t2.txt=");
					Serial3.write(0x22);
					Serial3.print(distance_Axis);
					Serial3.write(0x22);
					Serial3.write(0xff);
					Serial3.write(0xff);
					Serial3.write(0xff);
				}
#endif // Show Position
				break;
			}
			case ID_AXIS_Z:
			{
				endPosition_Axis = stepper_Z.getPosition();
#ifdef SHOW_POSITION //Controls must be added to Nextion pages
				if (configMain.trackPositions == 1)
				{
					degrees_Spindle = StepsToDegrees_Spindle(endPosition_Spindle);
					distance_Axis = StepsToDistance_Axis(endPosition_Axis, ID_AXIS_Z);
					Serial3.print("pageSync.t1.txt=");
					Serial3.write(0x22);
					Serial3.print(degrees_Spindle);
					Serial3.write(0x22);
					Serial3.write(0xff);
					Serial3.write(0xff);
					Serial3.write(0xff);
					delay(10);
					Serial3.print("pageSync.t2.txt=");
					Serial3.write(0x22);
					Serial3.print(distance_Axis);
					Serial3.write(0x22);
					Serial3.write(0xff);
					Serial3.write(0xff);
					Serial3.write(0xff);
				}
#endif // Show Position
				break;
			}
		}

		//Serial.print("Spindle:");
		//Serial.println(endPosition_Spindle);
		//Serial.print("Axis:");
		//Serial.println(endPosition_Axis);
#endif // DEBUG

		// Check for Cancel code  
		if (SerialAvailable() >= 0)
		{
			incomingByte = SerialRead(serialId);
			switch (incomingByte)
			{
				case 93: // - ] 
				case 99: // - c
				case 109: // - m
				{
					stepController.stopAsync();
					break;
				}
			}
		}

		delay(10);
	}

	returnSteps_Spindle = stepper_Spindle.getPosition();

	switch (idAxis)
	{
		case ID_AXIS_X:
		{
			endPosition_Axis = stepper_X.getPosition();

	#ifdef SHOW_POSITION //Controls must be added to Nextion pages
			degrees_Spindle = StepsToDegrees_Spindle(endPosition_Spindle);
			distance_Axis = StepsToDistance_Axis(endPosition_Axis, ID_AXIS_X);

			// Update Nextion
			Serial3.print("pageSyncX.t1.txt=");
			Serial3.write(0x22);
			Serial3.print(degrees_Spindle);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(10);
			Serial3.print("pageSyncX.t2.txt=");
			Serial3.write(0x22);
			Serial3.print(distance_Axis);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
	#endif // Show Position
			Serial3.print("pageSyncX.bt6.pco=0");
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.print("pageSyncX.va0.val=0");
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.print("pageSyncX.bt6.val=0");// The first one may be ignored by Nextion, so resend.
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.print("pageSyncX.bt2.val=0");
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.print("pageSyncX.bt1.val=0");
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);

			digitalWrite(PIN_AXIS_X_ENABLE, HIGH);  // Disable 
			break;
		}
		case ID_AXIS_Z:
		{
			endPosition_Axis = stepper_Z.getPosition();
	#ifdef SHOW_POSITION //Controls must be added to Nextion pages
			degrees_Spindle = StepsToDegrees_Spindle(endPosition_Spindle);
			distance_Axis = StepsToDistance_Axis(endPosition_Axis, ID_AXIS_Z);

			// Update Nextion
			Serial3.print("pageSync.t1.txt=");
			Serial3.write(0x22);
			Serial3.print(degrees_Spindle);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(10);
			Serial3.print("pageSync.t2.txt=");
			Serial3.write(0x22);
			Serial3.print(distance_Axis);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
	#endif // Show Position
			Serial3.print("pageSync.bt6.pco=0");
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.print("pageSync.va0.val=0");
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.print("pageSync.bt6.val=0");// The first one may be ignored by Nextion, so resend.
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.print("pageSync.bt2.val=0");
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.print("pageSync.bt1.val=0");
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);

			digitalWrite(PIN_AXIS_Z_ENABLE, HIGH);  // Disable 
			break;
		}
	}

	returnSteps_Axis = endPosition_Axis;

	stepper_Spindle.setPosition(0);
	stepper_X.setPosition(0);
	stepper_Z.setPosition(0);

	digitalWrite(PIN_SPINDLE_ENABLE, HIGH); // Disable 
#ifdef DEBUG
	Serial.print("Spindle:");
	Serial.println(endPosition_Spindle);
	Serial.print("Axis:");
	Serial.println(endPosition_Axis);

	Serial.println("End Sync()");
#endif // DEBUG
}

/// <summary>
/// DoRec1_RadialZ
/// </summary>
/// <comment>
/// </comment>
/// <param name="wavDir">Counterclockwise: -1, Clockwise: 1</param>
/// <returns></returns>
void Reciprocate_RadialZ(int wavDir)
{
	StepControl stepController;
	Stepper stepper_Spindle(PIN_SPINDLE_STEP, PIN_SPINDLE_DIR);
	Stepper stepper_Z(PIN_AXIS_Z_STEP, PIN_AXIS_Z_DIR);

	// Set speed and acceleration
	stepper_Spindle
		.setMaxSpeed(configReci.maxSpd_Spindle * configReci.speedPercent_Spindle * .01)
		.setAcceleration(configReci.accel_Spindle);
	stepper_Spindle.setPosition(0);

	float spindleDegrees = configReci.degrees_RadialZ/ (configReci.waves_RadialZ * 2);
	int spindleSteps = round((configMain.microsteps_Spindle * configMain.steps360_Spindle * configMain.gearRatio_Spindle) * (spindleDegrees / 360) * wavDir);
	returnSteps_Spindle = spindleSteps* configReci.waves_RadialZ * 2;

	stepper_Z
		.setMaxSpeed(configReci.maxSpd_Axis_Z * configReci.speedPercent_Axis_Z * .01)
		.setAcceleration(configReci.accel_Axis_Z);
	stepper_Z.setPosition(0);
	float axisDistance = configReci.amplitude_RadialZ;
	int axisSteps = round((axisDistance / configMain.distancePerRev_AxisZ) * configMain.microsteps_Axis_Z * configMain.steps360_Axis_Z * wavDir);

#ifdef DEBUG
	Serial.print("SpindleMaxSpd:");
	Serial.println(configReci.maxSpd_Spindle);
	Serial.print("SpindleSpeedPercent:");
	Serial.println(configReci.speedPercent_Spindle);
	Serial.print("SpindleAccel:");
	Serial.println(configReci.accel_Spindle);

	Serial.print("microsteps_Spindle:");
	Serial.println(configMain.microsteps_Spindle);
	Serial.print("steps360_Spindle:");
	Serial.println(configMain.steps360_Spindle);
	Serial.print("gearRatio_Spindle:");
	Serial.println(configMain.gearRatio_Spindle);
	Serial.print("amplitude_AxialZ:");
	Serial.println(configReci.amplitude_AxialZ);

	Serial.print("distancePerRev_AxisZ:");
	Serial.println(configMain.distancePerRev_AxisZ);
	Serial.print("steps360_Axis_Z:");
	Serial.println(configMain.steps360_Axis_Z);
	Serial.print("microsteps_Axis_Z:");
	Serial.println(configMain.microsteps_Axis_Z);
	Serial.print("amplitude_RadialZ:");
	Serial.println(configReci.amplitude_RadialZ);
	Serial.print("waves_RadialZ:");
	Serial.println(configReci.waves_RadialZ);

	Serial.print("spindleSteps:");
	Serial.println(spindleSteps);
	Serial.print("axisSteps:");
	Serial.println(axisSteps);

	Serial.print("AxisMaxSpd:");
	Serial.println(configReci.maxSpd_Axis_Z);
	Serial.print("AxisSpeedPercent:");
	Serial.println(configReci.speedPercent_Axis_Z);
	Serial.print("AxisAccel:");
	Serial.println(configReci.accel_Axis_Z);
#endif // DEBUG

	digitalWrite(PIN_SPINDLE_ENABLE, LOW); // Enable
	digitalWrite(PIN_AXIS_Z_ENABLE, LOW);  // Enable
	for (int i = 0; i < (configReci.waves_RadialZ * 2); i++)
	{
		axisSteps *= -1;

		stepper_Spindle.setTargetRel(spindleSteps);
		stepper_Z.setTargetRel(axisSteps);

		stepController.moveAsync(stepper_Spindle, stepper_Z);

		while (stepController.isRunning())
		{
#ifdef DEBUG
			endPosition_Axis = stepper_Z.getPosition();
			endPosition_Spindle = stepper_Spindle.getPosition();
			Serial.print("Spindle:");
			Serial.println(endPosition_Spindle);
			Serial.print("Axis:");
			Serial.println(endPosition_Axis);
#endif // DEBUG
#ifdef SHOW_POSITION //Controls must be added to Nextion pages
			if (configMain.trackPositions == 1)
			{

				degrees_Spindle = StepsToDegrees_Spindle(endPosition_Spindle);
				distance_Axis = StepsToDistance_Axis(endPosition_Axis, ID_AXIS_Z);
				Serial3.print("pageRecRadialZ.t1.txt=");
				Serial3.write(0x22);
				Serial3.print(degrees_Spindle);
				Serial3.write(0x22);
				Serial3.write(0xff);
				Serial3.write(0xff);
				Serial3.write(0xff);
				delay(10);
				Serial3.print("pageRecRadialZ.t2.txt=");
				Serial3.write(0x22);
				Serial3.print(distance_Axis);
				Serial3.write(0x22);
				Serial3.write(0xff);
				Serial3.write(0xff);
				Serial3.write(0xff);
			}
#endif // Show Position
			// Check for Cancel code  
			if (SerialAvailable() > 0)
			{
				incomingByte = SerialRead(serialId);
				switch (incomingByte)
				{
					case 93: // - ] 
					case 99: // - c
					case 109: // - m
					{
						stepController.stop();
						goto endLoop;
						break;
					}
					default:
					{
						break;
					}
				}
			}

			delay(10);
		}
	}

endLoop:

	endPosition_Axis = stepper_Z.getPosition();
	endPosition_Spindle = stepper_Spindle.getPosition();
#ifdef SHOW_POSITION //Controls must be added to Nextion pages
	degrees_Spindle = StepsToDegrees_Spindle(endPosition_Spindle);
	distance_Axis = StepsToDistance_Axis(endPosition_Axis, ID_AXIS_Z);
	Serial3.print("pageRecRadialZ.t1.txt=");
	Serial3.write(0x22);
	Serial3.print(degrees_Spindle);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(10);
	Serial3.print("pageRecRadialZ.t2.txt=");
	Serial3.write(0x22);
	Serial3.print(distance_Axis);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
#endif // Show Position
	// Update Nextion
	Serial3.print("pageRecRadialZ.bt3.pco=0");// The first one may be ignored by Nextion, so resend.
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageRecRadialZ.va0.val=0");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageRecRadialZ.bt3.val=0");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageRecRadialZ.bt2.val=0");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageRecRadialZ.bt1.val=0");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);

	returnSteps_Axis = endPosition_Axis;
	returnSteps_Spindle = endPosition_Spindle;

#ifdef DEBUG

	Serial.print("Spindle:");
	Serial.println(endPosition_Spindle);
	Serial.print("Axis:");
	Serial.println(endPosition_Axis);
#endif // DEBUG
	digitalWrite(PIN_SPINDLE_ENABLE, HIGH); // Disable 
	digitalWrite(PIN_AXIS_Z_ENABLE, HIGH);  // Disable 

}

/// <summary>
/// DoRec1_RadialX
/// </summary>
/// <comment>
/// </comment>
/// <param name="wavDir">Counterclockwise: -1, Clockwise: 1</param>
/// <returns></returns>
void Reciprocate_RadialX(int wavDir)
{
	StepControl stepController;
	Stepper stepper_Spindle(PIN_SPINDLE_STEP, PIN_SPINDLE_DIR);
	Stepper stepper_X(PIN_AXIS_X_STEP, PIN_AXIS_X_DIR);

	// Set speed and acceleration
	stepper_Spindle
		.setMaxSpeed(configReci.maxSpd_Spindle * configReci.speedPercent_Spindle * .01)
		.setAcceleration(configReci.accel_Spindle);
	stepper_Spindle.setPosition(0);

	float spindleDegrees = configReci.degrees_RadialX / (configReci.waves_RadialX * 2);
	int spindleSteps = round((configMain.microsteps_Spindle * configMain.steps360_Spindle * configMain.gearRatio_Spindle) * (spindleDegrees / 360) * wavDir);
	returnSteps_Spindle = spindleSteps * configReci.waves_RadialX * 2;

	stepper_X
		.setMaxSpeed(configReci.maxSpd_Axis_X * configReci.speedPercent_Axis_X * .01)
		.setAcceleration(configReci.accel_Axis_X);
	stepper_X.setPosition(0);
	float axisDistance = configReci.amplitude_RadialX;
	int axisSteps = round((axisDistance / configMain.distancePerRev_AxisX) * configMain.microsteps_Axis_X * configMain.steps360_Axis_X * wavDir);

#ifdef DEBUG
	Serial.print("SpindleMaxSpd:");
	Serial.println(configReci.maxSpd_Spindle);
	Serial.print("SpindleSpeedPercent:");
	Serial.println(configReci.speedPercent_Spindle);
	Serial.print("SpindleAccel:");
	Serial.println(configReci.accel_Spindle);

	Serial.print("microsteps_Spindle:");
	Serial.println(configMain.microsteps_Spindle);
	Serial.print("steps360_Spindle:");
	Serial.println(configMain.steps360_Spindle);
	Serial.print("gearRatio_Spindle:");
	Serial.println(configMain.gearRatio_Spindle);
	Serial.print("amplitude_AxialX:");
	Serial.println(configReci.amplitude_AxialX);

	Serial.print("distancePerRev_AxisX:");
	Serial.println(configMain.distancePerRev_AxisX);
	Serial.print("steps360_Axis_X:");
	Serial.println(configMain.steps360_Axis_X);
	Serial.print("microsteps_Axis_X:");
	Serial.println(configMain.microsteps_Axis_X);
	Serial.print("amplitude_RadialX:");
	Serial.println(configReci.amplitude_RadialX);
	Serial.print("waves_RadialX:");
	Serial.println(configReci.waves_RadialX);

	Serial.print("spindleSteps:");
	Serial.println(spindleSteps);
	Serial.print("axisSteps:");
	Serial.println(axisSteps);

	Serial.print("maxSpd_Axis_X:");
	Serial.println(configReci.maxSpd_Axis_X);
	Serial.print("speedPercent_Axis_X:");
	Serial.println(configReci.speedPercent_Axis_X);
	Serial.print("accel_Axis_X:");
	Serial.println(configReci.accel_Axis_X);
#endif // DEBUG
	digitalWrite(PIN_SPINDLE_ENABLE, LOW); // Enable
	digitalWrite(PIN_AXIS_X_ENABLE, LOW);  // Enable
	for (int i = 0; i < (configReci.waves_RadialX * 2); i++)
	{
		axisSteps *= -1;

		stepper_Spindle.setTargetRel(spindleSteps);
		stepper_X.setTargetRel(axisSteps);

		stepController.moveAsync(stepper_Spindle, stepper_X);

		while (stepController.isRunning())
		{
#ifdef DEBUG
			endPosition_Axis = stepper_X.getPosition();
			endPosition_Spindle = stepper_Spindle.getPosition();
			Serial.print("Spindle:");
			Serial.println(endPosition_Spindle);
			Serial.print("Axis:");
			Serial.println(endPosition_Axis);
#endif // DEBUG
#ifdef SHOW_POSITION //Controls must be added to Nextion pages
			if (configMain.trackPositions == 1)
			{
				endPosition_Axis = teensyStep_Axis_X.getPosition();
				endPosition_Spindle = teensyStep_Spindle.getPosition();
				degrees_Spindle = StepsToDegrees_Spindle(endPosition_Spindle);
				distance_Axis = StepsToDistance_Axis(endPosition_Axis, ID_AXIS_X);
				Serial3.print("pageRecRadialX.t1.txt=");
				Serial3.write(0x22);
				Serial3.print(degrees_Spindle);
				Serial3.write(0x22);
				Serial3.write(0xff);
				Serial3.write(0xff);
				Serial3.write(0xff);
				delay(10);
				Serial3.print("pageRecRadialX.t2.txt=");
				Serial3.write(0x22);
				Serial3.print(distance_Axis);
				Serial3.write(0x22);
				Serial3.write(0xff);
				Serial3.write(0xff);
				Serial3.write(0xff);
			}
#endif // Show Position
			// Check for Cancel code  
			if (SerialAvailable() > 0)
			{
				incomingByte = SerialRead(serialId);
				switch (incomingByte)
				{
				case 93: // - ] 
				case 99: // - c
				case 109: // - m
				{
					stepController.stop();
					goto endLoop;
					break;
				}
				default:
				{
					break;
				}
				}
			}

			delay(10);
		}
	}

endLoop:

	endPosition_Axis = stepper_X.getPosition();
	endPosition_Spindle = stepper_Spindle.getPosition();
#ifdef SHOW_POSITION //Controls must be added to Nextion pages
	degrees_Spindle = StepsToDegrees_Spindle(endPosition_Spindle);
	distance_Axis = StepsToDistance_Axis(endPosition_Axis, ID_AXIS_X);
	Serial3.print("pageRecRadialX.t1.txt=");
	Serial3.write(0x22);
	Serial3.print(degrees_Spindle);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(10);
	Serial3.print("pageRecRadialX.t2.txt=");
	Serial3.write(0x22);
	Serial3.print(distance_Axis);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
#endif // Show Position
	// Update Nextion
	Serial3.print("pageRecRadialX.bt3.pco=0");// The first one may be ignored by Nextion, so resend.
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageRecRadialX.va0.val=0");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageRecRadialX.bt3.val=0");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageRecRadialX.bt2.val=0");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageRecRadialX.bt1.val=0");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);

	returnSteps_Axis = endPosition_Axis;
	returnSteps_Spindle = endPosition_Spindle;

#ifdef DEBUG
	Serial.print("Spindle:");
	Serial.println(endPosition_Spindle);
	Serial.print("Axis:");
	Serial.println(endPosition_Axis);
#endif // DEBUG
	digitalWrite(PIN_SPINDLE_ENABLE, HIGH); // Disable 
	digitalWrite(PIN_AXIS_X_ENABLE, HIGH);  // Disable 
}

/// <summary>
/// DoRec1_AxialZ
/// </summary>
/// <comment>
/// </comment>
/// <param name="wavDir">In: -1, Out: 1</param>
/// <returns></returns>
void Reciprocate_AxialZ(int wavDir)
{
	StepControl stepController;
	Stepper stepper_Spindle(PIN_SPINDLE_STEP, PIN_SPINDLE_DIR);
	Stepper stepper_Z(PIN_AXIS_Z_STEP, PIN_AXIS_Z_DIR);
	// Set speed and acceleration
	stepper_Spindle
		.setMaxSpeed(configReci.maxSpd_Spindle * configReci.speedPercent_Spindle * .01)
		.setAcceleration(configReci.accel_Spindle);
	stepper_Spindle.setPosition(0);

	int spindleSteps = (configMain.microsteps_Spindle * configMain.steps360_Spindle * configMain.gearRatio_Spindle) * (configReci.amplitude_AxialZ / 360) * wavDir;

	long axisSteps = (((configReci.distance_AxialZ / (configMain.distancePerRev_AxisZ)) * (configMain.steps360_Axis_Z * configMain.microsteps_Axis_Z)) / (configReci.waves_AxialZ * 2));
	returnSteps_Axis = (configReci.distance_AxialZ / configMain.distancePerRev_AxisZ) * (configMain.steps360_Axis_Z * configMain.microsteps_Axis_Z);
	//returnSteps_Axis = axisSteps* configReci.waves_AxialZ * 2;

	if (wavDir == DIR_CW)
	{
		axisSteps = -axisSteps;
	}
	stepper_Z
		.setMaxSpeed(configReci.maxSpd_Axis_Z * configReci.speedPercent_Axis_Z * .01)
		.setAcceleration(configReci.accel_Axis_Z);
	stepper_Z.setPosition(0);

#ifdef DEBUG
	Serial.print("waves_AxialZ:");
	Serial.println(configReci.waves_AxialZ);
	Serial.println("---------------------------");
	Serial.print("SpindleMaxSpd:");
	Serial.println(configReci.maxSpd_Spindle);
	Serial.print("SpindleSpeedPercent:");
	Serial.println(configReci.speedPercent_Spindle);
	Serial.print("SpindleAccel:");
	Serial.println(configReci.accel_Spindle);
	Serial.println("========================");
	Serial.print("microsteps_Spindle:");
	Serial.println(configMain.microsteps_Spindle);
	Serial.print("steps360_Spindle:");
	Serial.println(configMain.steps360_Spindle);
	Serial.print("gearRatio_Spindle:");
	Serial.println(configMain.gearRatio_Spindle);
	Serial.print("amplitude_AxialZ:");
	Serial.println(configReci.amplitude_AxialZ);
	Serial.print("spindleSteps:");
	Serial.println(spindleSteps);
	Serial.println("========================");
	Serial.print("AxisMaxSpd:");
	Serial.println(configReci.maxSpd_Axis_Z);
	Serial.print("AxisSpeedPercent:");
	Serial.println(configReci.speedPercent_Axis_Z);
	Serial.print("AxisAccel:");
	Serial.println(configReci.accel_Axis_Z);
	Serial.println("+++++++++++++++++++++++++++");
	Serial.print("distancePerRev_AxisZ:");
	Serial.println(configMain.distancePerRev_AxisZ);
	Serial.print("steps360_Axis_Z:");
	Serial.println(configMain.steps360_Axis_Z);
	Serial.print("microsteps_Axis_Z:");
	Serial.println(configMain.microsteps_Axis_Z);
	Serial.print("distance_AxialZ:");
	Serial.println(configReci.distance_AxialZ);
	Serial.print("axisSteps:");
	Serial.println(axisSteps);
	Serial.println("+++++++++++++++++++++++++++");

#endif // DEBUG

	digitalWrite(PIN_SPINDLE_ENABLE, LOW); // Enable (Uses TeensyStep library)
	digitalWrite(PIN_AXIS_Z_ENABLE, LOW);  // Enable (Uses TeensyStep library)
	for (int i = 0; i < (configReci.waves_AxialZ * 2); i++)
	{
		spindleSteps *= -1;

		stepper_Spindle.setTargetRel(spindleSteps);
		stepper_Z.setTargetRel(axisSteps);

		stepController.moveAsync(stepper_Spindle, stepper_Z);
		delay(10);

		while (stepController.isRunning())
		{
#ifdef DEBUG
			endPosition_Axis = stepper_Z.getPosition();
			endPosition_Spindle = stepper_Spindle.getPosition();
			Serial.print("Spindle:");
			Serial.println(endPosition_Spindle);
			Serial.print("Axis:");
			Serial.println(endPosition_Axis);
#endif // DEBUG
#ifdef SHOW_POSITION //Controls must be added to Nextion pages
			if (configMain.trackPositions == 1)
			{
				endPosition_Axis = teensyStep_Axis_Z.getPosition();
				endPosition_Spindle = teensyStep_Spindle.getPosition();
				degrees_Spindle = StepsToDegrees_Spindle(endPosition_Spindle);
				distance_Axis = StepsToDistance_Axis(endPosition_Axis, ID_AXIS_Z);
				Serial3.print("pageRecAxialZ.t1.txt=");
				Serial3.write(0x22);
				Serial3.print(degrees_Spindle);
				Serial3.write(0x22);
				Serial3.write(0xff);
				Serial3.write(0xff);
				Serial3.write(0xff);
				delay(10);
				Serial3.print("pageRecAxialZ.t2.txt=");
				Serial3.write(0x22);
				Serial3.print(distance_Axis);
				Serial3.write(0x22);
				Serial3.write(0xff);
				Serial3.write(0xff);
				Serial3.write(0xff);
			}
#endif //Show position

			// Check for Cancel code  
			if (SerialAvailable() > 0)
			{
				incomingByte = SerialRead(serialId);
				switch (incomingByte)
				{
					case 93: // - ] 
					case 99: // - c
					case 109: // - m
					{
						stepController.stop();
						goto endLoop;
						break;
					}
					default:
					{
						break;
					}
				}
			}
		}
	}

endLoop:

	endPosition_Axis = stepper_Z.getPosition();
	endPosition_Spindle = stepper_Spindle.getPosition();
#ifdef SHOW_POSITION //Controls must be added to Nextion pages
	degrees_Spindle = StepsToDegrees_Spindle(endPosition_Spindle);
	distance_Axis = StepsToDistance_Axis(endPosition_Axis, ID_AXIS_Z);
	Serial3.print("pageRecAxialZ.t1.txt=");
	Serial3.write(0x22);
	Serial3.print(degrees_Spindle);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(10);
	Serial3.print("pageRecAxialZ.t2.txt=");
	Serial3.write(0x22);
	Serial3.print(distance_Axis);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
#endif //Show position

	// Update Nextion
	Serial3.print("pageRecAxialZ.bt3.pco=59391");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageRecAxialZ.va0.val=0");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageRecAxialZ.bt3.val=0");// The first one may be ignored by Nextion, so resend.
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageRecAxialZ.bt2.val=0");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageRecAxialZ.bt1.val=0");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);



#ifdef DEBUG
	endPosition_Axis = stepper_Z.getPosition();
	endPosition_Spindle = stepper_Spindle.getPosition();
	Serial.print("Spindle:");
	Serial.println(endPosition_Spindle);
	Serial.print("Axis:");
	Serial.println(endPosition_Axis);
#endif // DEBUG
	returnSteps_Axis = endPosition_Axis;
	returnSteps_Spindle = endPosition_Spindle;
	digitalWrite(PIN_SPINDLE_ENABLE, HIGH); // Disable 
	digitalWrite(PIN_AXIS_Z_ENABLE, HIGH);  // Disable 
}

/// <summary>
/// DoRec1_AxialX
/// </summary>
/// <comment>
/// </comment>
/// <param name="wavDir">In: -1, Out: 1</param>
/// <returns></returns>
void Reciprocate_AxialX(int wavDir)
{
	StepControl stepController;
	Stepper stepper_Spindle(PIN_SPINDLE_STEP, PIN_SPINDLE_DIR);
	Stepper stepper_X(PIN_AXIS_X_STEP, PIN_AXIS_X_DIR);

	// Set speed and acceleration
	stepper_Spindle
		.setMaxSpeed(configReci.maxSpd_Spindle * configReci.speedPercent_Spindle * .01)
		.setAcceleration(configReci.accel_Spindle);
	stepper_Spindle.setPosition(0);

int spindleSteps = (configMain.microsteps_Spindle * configMain.steps360_Spindle * configMain.gearRatio_Spindle) * (configReci.amplitude_AxialX / 360) * wavDir;
	long axisSteps = (((configReci.distance_AxialX / (configMain.distancePerRev_AxisX)) * (configMain.steps360_Axis_X * configMain.microsteps_Axis_X)) / (configReci.waves_AxialX * 2)) * wavDir;
	returnSteps_Axis = (configReci.distance_AxialX / configMain.distancePerRev_AxisX) * (configMain.steps360_Axis_X * configMain.microsteps_Axis_X);
	//returnSteps_Axis = axisSteps * configReci.waves_AxialX * 2;

	stepper_X
		.setMaxSpeed(configReci.maxSpd_Axis_X * configReci.speedPercent_Axis_X * .01)
		.setAcceleration(configReci.accel_Axis_X);
	stepper_X.setPosition(0);

#ifdef DEBUG
	Serial.print("SpindleMaxSpd:");
	Serial.println(configReci.maxSpd_Spindle);
	Serial.print("SpindleSpeedPercent:");
	Serial.println(configReci.speedPercent_Spindle);
	Serial.print("SpindleAccel:");
	Serial.println(configReci.accel_Spindle);

	Serial.print("microsteps_Spindle:");
	Serial.println(configMain.microsteps_Spindle);
	Serial.print("steps360_Spindle:");
	Serial.println(configMain.steps360_Spindle);
	Serial.print("gearRatio_Spindle:");
	Serial.println(configMain.gearRatio_Spindle);
	Serial.print("amplitude_AxialX:");
	Serial.println(configReci.amplitude_AxialX);

	Serial.print("distancePerRev_AxisX:");
	Serial.println(configMain.distancePerRev_AxisX);
	Serial.print("steps360_Axis_X:");
	Serial.println(configMain.steps360_Axis_X);
	Serial.print("microsteps_Axis_X:");
	Serial.println(configMain.microsteps_Axis_X);
	Serial.print("distance_AxialX:");
	Serial.println(configReci.distance_AxialX);
	Serial.print("waves_AxialX:");
	Serial.println(configReci.waves_AxialX);

	Serial.print("spindleSteps:");
	Serial.println(spindleSteps);
	Serial.print("axisSteps:");
	Serial.println(axisSteps);

	Serial.print("AxisMaxSpd:");
	Serial.println(configReci.maxSpd_Axis_X);
	Serial.print("AxisSpeedPercent:");
	Serial.println(configReci.speedPercent_Axis_X);
	Serial.print("AxisAccel:");
	Serial.println(configReci.accel_Axis_X);
#endif // DEBUG
	digitalWrite(PIN_SPINDLE_ENABLE, LOW); // Enable (Uses TeensyStep library)
	digitalWrite(PIN_AXIS_X_ENABLE, LOW);  // Enable (Uses TeensyStep library)
	for (int i = 0; i < (configReci.waves_AxialX * 2); i++)
	{
		spindleSteps *= -1;

		stepper_Spindle.setTargetRel(spindleSteps);
		stepper_X.setTargetRel(axisSteps);

		stepController.moveAsync(stepper_Spindle, stepper_X);
		delay(10);

		while (stepController.isRunning())
		{
#ifdef DEBUG
			endPosition_Axis = stepper_X.getPosition();
			endPosition_Spindle = stepper_Spindle.getPosition();
			Serial.print("Spindle:");
			Serial.println(endPosition_Spindle);
			Serial.print("Axis:");
			Serial.println(endPosition_Axis);
#endif // DEBUG
#ifdef SHOW_POSITION //Controls must be added to Nextion pages
			if (configMain.trackPositions == 1)
			{
				endPosition_Axis = teensyStep_Axis_X.getPosition();
				endPosition_Spindle = teensyStep_Spindle.getPosition();
				degrees_Spindle = StepsToDegrees_Spindle(endPosition_Spindle);
				distance_Axis = StepsToDistance_Axis(endPosition_Axis, ID_AXIS_X);
				Serial3.print("pageRecAxialX.t1.txt=");
				Serial3.write(0x22);
				Serial3.print(degrees_Spindle);
				Serial3.write(0x22);
				Serial3.write(0xff);
				Serial3.write(0xff);
				Serial3.write(0xff);
				delay(10);
				Serial3.print("pageRecAxialX.t2.txt=");
				Serial3.write(0x22);
				Serial3.print(distance_Axis);
				Serial3.write(0x22);
				Serial3.write(0xff);
				Serial3.write(0xff);
				Serial3.write(0xff);
			}
#endif //SHOW_POSITION
			////digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN)); // toggle LED
			// Check for Cancel code  
			if (SerialAvailable() > 0)
			{
				incomingByte = SerialRead(serialId);
				switch (incomingByte)
				{
				case 93: // - ] 
				case 99: // - c
				case 109: // - m
				{
					stepController.stop();
					goto endLoop;
					break;
				}
				default:
				{
					break;
				}
				}
			}
		}
	}

endLoop:

	endPosition_Axis = stepper_X.getPosition();
	endPosition_Spindle = stepper_Spindle.getPosition();
#ifdef SHOW_POSITION //Controls must be added to Nextion pages
	degrees_Spindle = StepsToDegrees_Spindle(endPosition_Spindle);
	distance_Axis = StepsToDistance_Axis(endPosition_Axis, ID_AXIS_X);
	Serial3.print("pageRecAxialX.t1.txt=");
	Serial3.write(0x22);
	Serial3.print(degrees_Spindle);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(10);
	Serial3.print("pageRecAxialX.t2.txt=");
	Serial3.write(0x22);
	Serial3.print(distance_Axis);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
#endif //SHOW_POSITION
	// Update Nextion
	Serial3.print("pageRecAxialX.bt3.pco=59391");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageRecAxialX.va0.val=0");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageRecAxialX.bt3.val=0");// The first one may be ignored by Nextion, so resend.
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageRecAxialX.bt2.val=0");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageRecAxialX.bt1.val=0");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);

#ifdef DEBUG
	endPosition_Axis = stepper_X.getPosition();
	endPosition_Spindle = stepper_Spindle.getPosition();
	Serial.print("Spindle:");
	Serial.println(endPosition_Spindle);
	Serial.print("Axis:");
	Serial.println(endPosition_Axis);
#endif // DEBUG
	returnSteps_Axis = endPosition_Axis;
	returnSteps_Spindle = endPosition_Spindle;
	digitalWrite(PIN_SPINDLE_ENABLE, HIGH); // Disable 
	digitalWrite(PIN_AXIS_X_ENABLE, HIGH);  // Disable 
}

/// <summary>
/// Serial Print
/// </summary>
/// <comment>
/// Prints data to USB and Bluetooth serial ports.  Only one will be active.
/// </comment>
/// <param name="text">Text to send</param>
/// <returns></returns>
void SerialPrint(String text)
{
	Serial.print(text);
	////Serialxx1.print(text);
	Serial3.print(text);
}

/// <summary>
/// Serial Write
/// </summary>
/// <comment>
/// Writes data to USB and Bluetooth serial ports.  Only one will be active.
/// </comment>
/// <param name="text">Char to send</param>
/// <returns></returns>
void SerialWrite(char text)
{
	Serial.write(text);
	////Serialxx1.write(text);
	Serial3.write(text);
}

/// <summary>
/// Serial Print Line
/// </summary>
/// <comment>
/// Prints data to USB and Bluetooth serial ports.  Only one will be active.
/// </comment>
/// <param name="text">Text to send</param>
/// <returns></returns>
void SerialPrintLn(String text)
{
	Serial.println(text);
	Serial3.println(text);
}

/// <summary>
/// Serial Read
/// </summary>
/// <comment>
/// Gets data from USB or Bluetooth serial ports.  Only one will be active.
/// </comment>
/// <param name="serialId">Id of serial port</param>
/// <returns>Integer</returns>
int SerialRead(int serialId)
{
	int incomingData = 0;
	if (serialId == 0)
	{
		incomingData = Serial.read();
	}
	else
	{
		incomingData = Serial3.read();
	}

	return incomingData;
}

/// <summary>
/// Serial Read Byte
/// </summary>
/// <comment>
/// Gets data from USB or Bluetooth serial ports.  Only one will be active.
/// </comment>
/// <param name="serialId">Id of serial port</param>
/// <returns>Integer</returns>
int SerialReadByte()
{
	int incomingData = 0;
	if (Serial.available() > 0)
	{
		incomingData = Serial.read();
	}
	else
	{
		incomingData = Serial3.read();
	}

	return incomingData;
}

/// <summary>
/// Serial Available
/// </summary>
/// <comment>
/// Verifies a Serial port is active
/// Serial is USB connected to PC
/// Serial3 is connected to pins 0 and 1 (Nextion LCD)
/// </comment>
/// <returns>SerialId. -1 if no serial port is active</returns>
int SerialAvailable()
{
	int serialId = -1;
	if (Serial.available() > 0)
	{
		serialId = 0;
	}
	//else if (Serialxx1.available() > 0)
	//{
	//	serialId = 1;
	//}
	else if (Serial3.available() > 0)
	{
		serialId = 3;
	}
	return serialId;
}

/// <summary>
/// Get a char from USB serial input
/// </summary>
/// <returns>char from USB serial input</returns>
void GetIndex2FilenameFromSerial1()
{
	const int bufferLength = 20;
	char input = 0;
	int i;
	for (i = 0; i <= bufferLength; i++)
	{
		input = Serial3.read();

		// Use 255 as end chars
		if (input == 255)
		{
			EEPROM.write(eePromAddress_Filename + i, 0);
			break;
		}
		EEPROM.write(eePromAddress_Filename + i, input);
	}

	EEPROM.put(eePromAddress_Filename_Length, i);
#ifdef DEBUG
	Serial.print("Filename:");
	Serial.println(input);
#endif // DEBUG
}

/// <summary>
/// GetDataFromSD
/// </summary>
/// <comment>
/// Get a value from file on SD card
/// </comment>
/// <param name="lineNumber">Line containing data</param>
/// <returns>Double value</returns>
double GetDataFromSD(int lineNumber)
{
	String newSizeString = "";
	int currentLineNumber = 0;
	int filename_Length;

	// Get filename from EEPROM
	EEPROM.get(eePromAddress_Filename_Length, filename_Length);
	char arChar[20];
	int i;
	for (i = 0; i <= filename_Length; i++)
	{
		arChar[i] = EEPROM.read(i + eePromAddress_Filename);
	}
	arChar[i++] = 0;

	// Open file
	File sdFile = SD.open(arChar, FILE_READ);

	delay(100);
	while (sdFile.available())
	{
		newSizeString = sdFile.readStringUntil('\r');
		if (currentLineNumber == lineNumber)
		{
			break;
		}
		currentLineNumber++; // Increment the line number
	}

	// Close the file
	sdFile.close();

	// Return size string as double
	const char *newSizeChar = static_cast<const char*>(newSizeString.c_str());
	return atof(newSizeChar);
}



// ================================================================================= 
// RosePatternSpindle Methods
// ================================================================================= 
float slideFunc_Spindle_Z(float spindleAngleZ)
{
	float phi = fmodf(spindleAngleZ * kRatio, TWO_PI);
	float retVal = spindleStepsAmplitude * cosf(phi);
	return retVal;
}

//------------------------------------------------------------------------------------
// tick()
//
// This function is called periodically with period recalcPeriod. 
// It calculates 
//  1) a new target value for the slide depending on the spindle angle
//  2) the new speed for the spindle so that it will reach the target until it is called again
void tickSpindleZ()
{
	float spindleAngleZ = stepperAxis_Z.getPosition();// *(TWO_PI / spindleStepsPerRev); //convert steps to angle
	float slidePositionZ = stepperSpindle.getPosition();
	float slideTargetZ = slideFunc_Spindle_Z(spindleAngleZ);

	float newSpeedZ = (slideTargetZ - slidePositionZ) / dtRose; // speed to reach target in given delta t (neglecting acceleration)
	float speedFacZ = newSpeedZ / newMaxSpd_RoseSpindle;              // transform in relative factor (-1.0 .. 1.0)

	rotateController1.overrideSpeed(speedFacZ);             // set new speed

}

void RosePattern_Axial_Z(int direction)
{
	IntervalTimer tickTimerZ;
	RotateControl controllerRose;
	bool runPattern = true;

	kRatio = (float)configRose.n / configRose.d;
	spindleStepsPerRev = configMain.gearRatio_Spindle * configMain.microsteps_Spindle * configMain.steps360_Spindle;
	spindleStepsAmplitude = (configRose.amplitude_Spindle / 360) * spindleStepsPerRev;

	slideStepsAmplitude = ((configRose.amplitude_Axis_Z / (configMain.distancePerRev_AxisZ)) * (configMain.steps360_Axis_Z * configMain.microsteps_Axis_Z));

	newMaxSpd_RoseSpindle = configRose.maxSpd_Spindle * configRose.speedPercent_Spindle * .01;
	stepperSpindle
		.setMaxSpeed(newMaxSpd_RoseSpindle)
		.setAcceleration(configRose.accel_Spindle)
		.setPosition(0);

	newMaxSpd_RoseAxis = configRose.maxSpd_Axis_Z * configRose.speedPercent_Axis * .01 * direction;
	stepperAxis_Z
		.setMaxSpeed(newMaxSpd_RoseAxis)
		.setAcceleration(configRose.accel_Axis_Z)
		.setPosition(0);
		//.setPosition(slideFunc_Axis_Z(0)); // set start position of counter

		// Enable steppers
	digitalWrite(PIN_SPINDLE_ENABLE, LOW);
	digitalWrite(PIN_AXIS_Z_ENABLE, LOW);

	rotateController1.rotateAsync(stepperAxis_Z);
	//controllerRose_Axis.overrideSpeed(0); // start with stopped slide
	controllerRose.rotateAsync(stepperSpindle); // let the spindle run with constant speed

	tickZ();

	// use a timer to periodically calculate new targets for the slide
	tickTimerZ.priority(128); // lowest priority, potentially long caclulations need to be interruptable by TeensyStep
	tickTimerZ.begin(tickZ, recalcPeriod);

	while (runPattern)
	{
		digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

#ifdef SHOW_POSITION //Controls must be added to Nextion pages
		if (configMain.trackPositions == 1)
		{
			endPosition_Axis = teensyStep_Axis_Z.getPosition();
			endPosition_Spindle = teensyStep_Spindle.getPosition();

			// print current values of spindle angle [rad] and slide position [steps]
			//float phiZ = endPosition_Spindle * (TWO_PI / spindleStepsPerRev);
			//Serial.printf("%f\t%d\n", phiZ, endPosition_Axis);
			degrees_Spindle = StepsToDegrees_Spindle(endPosition_Spindle);
			distance_Axis = StepsToDistance_Axis(endPosition_Axis, ID_AXIS_Z);
			Serial3.print("pageRose.t12.txt=");
			Serial3.write(0x22);
			Serial3.print(degrees_Spindle);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(5);
			Serial3.print("pageRose.t12.txt=");
			Serial3.write(0x22);
			Serial3.print(degrees_Spindle);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(5);
			Serial3.print("pageRose.t13.txt=");
			Serial3.write(0x22);
			Serial3.print(distance_Axis);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
		}
#endif // Show Position

		// Check for Cancel code  
		if (SerialAvailable() >= 0)
		{
			incomingByte = SerialRead(serialId);
			switch (incomingByte)
			{
			case 109:
			{
				runPattern = false;
				controllerRose.overrideSpeed(0);
				rotateController1.overrideSpeed(0);
				controllerRose.stopAsync();
				rotateController1.stopAsync();
				break;
			}
			}
		}
	}

	endPosition_Spindle = stepperSpindle.getPosition();
	endPosition_Axis = stepperAxis_Z.getPosition();
	returnSteps_Axis = endPosition_Axis;
	returnSteps_Spindle = endPosition_Spindle;
	stepperSpindle.setPosition(0);
	stepperAxis_Z.setPosition(0);

	digitalWrite(PIN_SPINDLE_ENABLE, HIGH); // Disable 
	digitalWrite(PIN_AXIS_Z_ENABLE, HIGH);  // Disable 
}

// ================================================================================= 
// RosePatternZ Methods
// ================================================================================= 
/// <summary>
/// Return Spindle and/or axes to start position
/// </summary>
/// <comment>
/// </comment>
/// <returns></returns>
void ReturnToStartPosition(int selection)
{

	//int32_t target_Spindle = endPosition_Spindle % spindleStepsPerRev * (-1);
	int32_t target_Spindle = returnSteps_Spindle % spindleStepsPerRev * (-1);
	int32_t target_Axis = returnSteps_Axis * (-1);

	StepControl stepControllerAxis;
	Stepper stepper_Spindle(PIN_SPINDLE_STEP, PIN_SPINDLE_DIR);
	Stepper stepper_Z(PIN_AXIS_Z_STEP, PIN_AXIS_Z_DIR);
	Stepper stepper_X(PIN_AXIS_X_STEP, PIN_AXIS_X_DIR);

	stepper_Spindle.setPosition(0);
	stepper_X.setPosition(0);
	stepper_Z.setPosition(0);
	endPosition_Axis = 0;
	endPosition_Spindle = 0;
	delay(15);
	stepper_Spindle
		.setMaxSpeed(configMain.maxSpd_Return_Spindle)
		.setAcceleration(configMain.accel_Return_Spindle)
		.setTargetRel(target_Spindle);
	stepper_Z
		.setMaxSpeed(configMain.maxSpd_Return_Axis)
		.setAcceleration(configMain.accel_Return_Axis)
		.setTargetAbs(target_Axis);

	stepper_X
		.setMaxSpeed(configMain.maxSpd_Return_Axis)
		.setAcceleration(configMain.accel_Return_Axis)
		.setTargetAbs(target_Axis);
#ifdef DEBUG
	Serial.println("Begin ReturnToStartPosition()");
	Serial.print("target_Spindle: ");
	Serial.println(target_Spindle);
	Serial.print("target_Axis: ");
	Serial.println(target_Axis);	
#endif // DEBUG
	digitalWrite(PIN_SPINDLE_ENABLE, LOW); // Enable 
	delay(20);
	switch (selection)
	{
		case 0:
		case 2:
		{
			digitalWrite(PIN_AXIS_Z_ENABLE, LOW);  // Enable 
			stepControllerAxis.moveAsync(stepper_Z, stepper_Spindle);
			break;
		}
		case 1:
		case 3:
		{
			digitalWrite(PIN_AXIS_X_ENABLE, LOW);  // Enable 
			stepControllerAxis.moveAsync(stepper_X, stepper_Spindle);
			break;
		}
	}

	while (stepControllerAxis.isRunning())
	{

//#ifdef DEBUG
//		switch (selection)
//		{
//			case 0:
//			case 2:
//			{
//				endPosition_Axis = stepper_Z.getPosition();
//				break;
//			}
//			case 1:
//			case 3:
//			{
//				endPosition_Axis = stepper_X.getPosition();
//				break;
//			}
//		}
//		Serial.print("Selection: ");
//		Serial.println(selection);
//		endPosition_Spindle = stepper_Spindle.getPosition();
//		Serial.print("Spindle: ");
//		Serial.println(endPosition_Spindle);
//
//		Serial.print("Axis: ");
//		Serial.println(endPosition_Axis);
//#endif // DEBUG
		// Check for Cancel code  
		if (SerialAvailable() >= 0)
		{
			incomingByte = SerialRead(serialId);
			switch (incomingByte)
			{
			case 93: // - ] 
			case 99: // - c
			case 109: // - m
			{
				stepControllerAxis.stop();
				digitalWrite(PIN_SPINDLE_ENABLE, HIGH); // Disable 
				digitalWrite(PIN_AXIS_Z_ENABLE, HIGH);  // Disable 
				digitalWrite(PIN_AXIS_X_ENABLE, HIGH);  // Disable 
				stepper_X.setPosition(0);
				stepper_Z.setPosition(0);
				//goto endLoop;
				break;
			}
			}
		}

		delay(10);
	}

//endLoop:
	endPosition_Spindle = stepper_Spindle.getPosition();
	switch (selection)
	{
	case 0:
	case 2:
	{
		endPosition_Axis = stepper_Z.getPosition();
		break;
	}
	case 1:
	case 3:
	{
		endPosition_Axis = stepper_X.getPosition();
		break;
	}
	}

	switch (pageCallerId)
	{
	case PAGE_SYNCZ:
	{
		Serial3.print("pageSync.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageSync.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageSync.va0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageSync.bt0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageSync.t1.txt=");
		Serial3.write(0x22);
		Serial3.print(endPosition_Spindle);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		delay(10);
		Serial3.print("pageSync.t2.txt=");
		Serial3.write(0x22);
		Serial3.print(endPosition_Axis);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		break;
	}
	case PAGE_SYNCX:
	{
		Serial3.print("pageSyncX.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageSyncX.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageSyncX.va0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageSyncX.bt0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageSyncX.t1.txt=");
		Serial3.write(0x22);
		Serial3.print(endPosition_Spindle);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		delay(10);
		Serial3.print("pageSyncX.t2.txt=");
		Serial3.write(0x22);
		Serial3.print(endPosition_Axis);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		break;
	}
	case PAGE_RECI_AXIAL_Z:
	{
		Serial3.print("pageRecAxialZ.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageRecAxialZ.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageRecAxialZ.va0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageRecAxialZ.bt0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageRecAxialZ.t1.txt=");
		Serial3.write(0x22);
		Serial3.print(endPosition_Spindle);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		delay(10);
		Serial3.print("pageRecAxialZ.t2.txt=");
		Serial3.write(0x22);
		Serial3.print(endPosition_Axis);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		break;
	}
	case PAGE_RECI_RADIAL_Z:
	{
		Serial3.print("pageRecRadialZ.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageRecRadialZ.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageRecRadialZ.va0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageRecRadialZ.bt0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageRecRadialZ.t1.txt=");
		Serial3.write(0x22);
		Serial3.print(endPosition_Spindle);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		delay(10);
		Serial3.print("pageRecRadialZ.t2.txt=");
		Serial3.write(0x22);
		Serial3.print(endPosition_Axis);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		break;
	}
	case PAGE_RECI_AXIAL_X:
	{
		Serial3.print("pageRecAxialX.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageRecAxialX.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageRecAxialX.va0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageRecAxialX.bt0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageRecAxialX.t1.txt=");
		Serial3.write(0x22);
		Serial3.print(endPosition_Spindle);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		delay(10);
		Serial3.print("pageRecAxialX.t2.txt=");
		Serial3.write(0x22);
		Serial3.print(endPosition_Axis);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		break;
	}
	case PAGE_RECI_RADIAL_X:
	{
		Serial3.print("pageRecRadialX.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageRecRadialX.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageRecRadialX.va0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageRecRadialX.bt0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageRecRadialX.t1.txt=");
		Serial3.write(0x22);
		Serial3.print(endPosition_Spindle);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		delay(10);
		Serial3.print("pageRecRadialX.t2.txt=");
		Serial3.write(0x22);
		Serial3.print(endPosition_Axis);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		break;
	}
	case PAGE_INDEX1:
	{
		Serial3.print("pageIndex1.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageIndex1.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageIndex1.va0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageIndex1.bt0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageIndex1.t1.txt=");
		Serial3.write(0x22);
		Serial3.print(endPosition_Spindle);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		break;
	}
	case PAGE_INDEX2:
	{
		Serial3.print("pageIndex2.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageIndex2.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageIndex2.va0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageIndex2.bt0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageIndex2.t1.txt=");
		Serial3.write(0x22);
		Serial3.print(endPosition_Spindle);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		break;
	}
	case PAGE_MOVEZ:
	{
		Serial3.print("pageMoveZ.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageMoveZ.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageMoveZ.va0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageMoveZ.bt0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageMoveZ.t2.txt=");
		Serial3.write(0x22);
		Serial3.print(endPosition_Axis);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		break;
	}
	case PAGE_MOVEX:
	{
		Serial3.print("pageMoveX.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageMoveX.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageMoveX.va0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageMoveX.bt0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.print("pageMoveX.t2.txt=");
		Serial3.write(0x22);
		Serial3.print(endPosition_Axis);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		break;
	}
	case PAGE_ROSE:
	{
		Serial3.print("pageRose.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		delay(50);
		Serial3.print("pageRose.bt1.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		delay(50);
		Serial3.print("pageRose.va0.val=0");
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);

		delay(50);
		initialPosition_Axis = 0;
		break;
	}

	}

	stepper_Spindle.setPosition(0);
	stepper_X.setPosition(0);
	stepper_Z.setPosition(0);
	endPosition_Axis = 0;
	endPosition_Spindle = 0;
	returnSteps_Axis = 0;
	returnSteps_Spindle = 0;


	digitalWrite(PIN_SPINDLE_ENABLE, HIGH); // Disable 
	digitalWrite(PIN_AXIS_Z_ENABLE, HIGH);  // Disable 
	digitalWrite(PIN_AXIS_X_ENABLE, HIGH);  // Disable 
	Serial.println("End ReturnToStartPosition()");
}

float slideFunc_Axis_Z(float spindleAngleZ)
{
	float phi = fmodf(spindleAngleZ * kRatio, TWO_PI);
	float retVal = slideStepsAmplitude * cosf(phi);
	return retVal;
}

//------------------------------------------------------------------------------------
// tick()
//
// This function is called periodically with period recalcPeriod. 
// It calculates 
//  1) a new target value for the slide depending on the spindle angle
//  2) the new speed for the spindle so that it will reach the target until it is called again
void tickZ()
{
	float spindleAngleZ = stepperSpindle.getPosition() * (TWO_PI / spindleStepsPerRev); //convert steps to angle
	float slidePositionZ = stepperAxis_Z.getPosition();

	float phi = fmodf(spindleAngleZ * kRatio, TWO_PI);
	float slideTargetZ = slideStepsAmplitude * cosf(phi);


	//float slideTargetZ = slideFunc_Axis_Z(spindleAngleZ);

	float newSpeedZ = (slideTargetZ - slidePositionZ) / dtRose; // speed to reach target in given delta t (neglecting acceleration)
	float speedFacZ = newSpeedZ / newMaxSpd_RoseAxis;              // transform in relative factor (-1.0 .. 1.0)

	rotateController1.overrideSpeed(speedFacZ);             // set new speed
}

void RoseRadial_Z(int direction)
{
	IntervalTimer tickTimerZ;
	RotateControl controllerRoseSpindle;
	bool runPattern = true;

	kRatio = (float)configRose.n / configRose.d;
	spindleStepsPerRev = configMain.gearRatio_Spindle * configMain.microsteps_Spindle * configMain.steps360_Spindle;
	slideStepsAmplitude = ((configRose.amplitude_Axis_Z / (configMain.distancePerRev_AxisZ)) * configMain.steps360_Axis_Z * configMain.microsteps_Axis_Z)/2;  // Amplitude is normally measured from the middle to the top

	newMaxSpd_RoseSpindle = configRose.maxSpd_Spindle * configRose.speedPercent_Spindle * .01 * direction;
	stepperSpindle
		.setMaxSpeed(newMaxSpd_RoseSpindle)
		.setAcceleration(configRose.accel_Spindle)
		.setPosition(0);

	initialPosition_Axis = static_cast<int>(slideFunc_Axis_Z(0));
	newMaxSpd_RoseAxis = configRose.maxSpd_Axis_Z * configRose.speedPercent_Axis * .01 * direction;
	stepperAxis_Z
		.setMaxSpeed(newMaxSpd_RoseAxis)
		.setAcceleration(configRose.accel_Axis_Z)
		.setPosition(initialPosition_Axis); // set start position of counter

	
#ifdef DEBUG
	int beginPosition = stepperAxis_Z.getPosition();
	Serial.print("BeginPosition: ");
	Serial.println(beginPosition);
	Serial.println("^^^^^^^^^^^^^^^^^");
#endif // DEBUG

#ifdef DEBUG

	Serial.println("---------------------------");
	Serial.print("SpindleMaxSpd:");
	Serial.println(configRose.maxSpd_Spindle);
	Serial.print("SpindleSpeedPercent:");
	Serial.println(configRose.speedPercent_Spindle);

	Serial.println("========================");
	Serial.print("microsteps_Spindle:");
	Serial.println(configMain.microsteps_Spindle);
	Serial.print("steps360_Spindle:");
	Serial.println(configMain.steps360_Spindle);
	Serial.print("gearRatio_Spindle:");
	Serial.println(configMain.gearRatio_Spindle);
	Serial.print("spindleStepsPerRev:");
	Serial.println(spindleStepsPerRev);
	Serial.println("========================");
	Serial.print("AxisMaxSpd:");
	Serial.println(configRose.maxSpd_Axis_Z);
	Serial.print("AxisSpeedPercent:");
	Serial.println(configRose.amplitude_Axis_Z);
	Serial.print("AxisAccel:");
	Serial.println(configRose.accel_Axis_Z);
	Serial.println("+++++++++++++++++++++++++++");
	Serial.print("newMaxSpd_RoseAxis:");
	Serial.println(newMaxSpd_RoseAxis);
	Serial.print("amplitude_Axis_Z:");
	Serial.println(configRose.amplitude_Axis_Z);
	Serial.println("+++++++++++++++++++++++++++");
	Serial.print("initialPosition_Axis:");
	Serial.println(initialPosition_Axis);
	Serial.println("+++++++++++++++++++++++++++");

#endif // DEBUG


		// Enable steppers
	digitalWrite(PIN_SPINDLE_ENABLE, LOW);
	digitalWrite(PIN_AXIS_Z_ENABLE, LOW);

	rotateController1.rotateAsync(stepperAxis_Z);
	//controllerRose_Axis.overrideSpeed(0); // start with stopped slide
	controllerRoseSpindle.rotateAsync(stepperSpindle); // let the spindle run with constant speed

	tickZ();

	// use a timer to periodically calculate new targets for the slide
	tickTimerZ.priority(128); // lowest priority, potentially long caclulations need to be interruptable by TeensyStep
	tickTimerZ.begin(tickZ, recalcPeriod);

	while (runPattern)
	{
		//digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
#ifdef SHOW_POSITION //Controls must be added to Nextion pages
		endPosition_Axis = teensyStep_Axis_Z.getPosition();
		endPosition_Spindle = teensyStep_Spindle.getPosition();

		// print current values of spindle angle [rad] and slide position [steps]
		//float phiZ = endPosition_Spindle * (TWO_PI / spindleStepsPerRev);
		//Serial.printf("%f\t%d\n", phiZ, endPosition_Axis);
		degrees_Spindle = StepsToDegrees_Spindle(endPosition_Spindle);
		distance_Axis = StepsToDistance_Axis(endPosition_Axis, ID_AXIS_Z);
		Serial3.print("pageRose.t12.txt=");
		Serial3.write(0x22);
		Serial3.print(degrees_Spindle);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		delay(10);
		Serial3.print("pageRose.t13.txt=");
		Serial3.write(0x22);
		Serial3.print(distance_Axis);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);

#endif // Show Position
		//delay(5);
		// Check for Cancel code  
		if (SerialAvailable() >= 0)
		{
			incomingByte = SerialRead(serialId);
			switch (incomingByte)
			{
			case 109:
			{
				runPattern = false;
				controllerRoseSpindle.overrideSpeed(0);
				rotateController1.overrideSpeed(0);
				rotateController1.stopAsync();
				controllerRoseSpindle.stopAsync();
				
				break;
			}
			}
		}
		delay(15);
	}
	endPosition_Spindle = stepperSpindle.getPosition();
	endPosition_Axis = stepperAxis_Z.getPosition();
	returnSteps_Axis = endPosition_Axis - initialPosition_Axis;// *direction);
	returnSteps_Spindle = endPosition_Spindle;

#ifdef DEBUG
	Serial.print("Spindle:");
	Serial.println(endPosition_Spindle);
	Serial.print("returnSteps_Spindle:");
	Serial.println(returnSteps_Spindle);
	Serial.print("Axis:");
	Serial.println(endPosition_Axis);
	Serial.print("returnSteps_Axis: ");
	Serial.println(returnSteps_Axis);
#endif // DEBUG



	stepperSpindle.setPosition(0);
	stepperAxis_Z.setPosition(0);

	digitalWrite(PIN_SPINDLE_ENABLE, HIGH); // Disable 
	digitalWrite(PIN_AXIS_Z_ENABLE, HIGH);  // Disable 
}

// ================================================================================= 
// RosePatternX Methods
// ================================================================================= 

float slideFunc_Axis_X(float spindleAngleX)
{
	float phi = fmodf(spindleAngleX * kRatio, TWO_PI);
	float retVal = slideStepsAmplitude * cosf(phi);
	return retVal;
}

////------------------------------------------------------------------------------------
//// tick()
////
//// This function is called periodically with period recalcPeriod. 
//// It calculates 
////  1) a new target value for the slide depending on the spindle angle
////  2) the new speed for the spindle so that it will reach the target until it is called again
//
void tickX()
{
	float spindleAngleX = stepperSpindle.getPosition() * (TWO_PI / spindleStepsPerRev); //convert steps to angle
	float slidePositionX = stepperAxis_X.getPosition();
	float slideTargetX = slideFunc_Axis_X(spindleAngleX);

	float newSpeedX = (slideTargetX - slidePositionX) / dtRose; // speed to reach target in given delta t (neglecting acceleration)
	float speedFacX = newSpeedX / newMaxSpd_RoseAxis;              // transform in relative factor (-1.0 .. 1.0)

	rotateController1.overrideSpeed(speedFacX);             // set new speed
}
void RoseRadial_X(int direction)
{
	IntervalTimer tickTimerX;
	RotateControl controllerRose_Spindle;
	bool runPatternX = true;

	kRatio = (float)configRose.n / configRose.d;
	spindleStepsPerRev = configMain.gearRatio_Spindle * configMain.microsteps_Spindle * configMain.steps360_Spindle;
	slideStepsAmplitude = ((configRose.amplitude_Axis_X / (configMain.distancePerRev_AxisX)) * configMain.steps360_Axis_X * configMain.microsteps_Axis_X)/2;
	stepperAxis_X.setPosition(0);

	newMaxSpd_RoseSpindle = configRose.maxSpd_Spindle * configRose.speedPercent_Spindle * .01 * direction;
	stepperSpindle
		.setMaxSpeed(newMaxSpd_RoseSpindle)
		.setAcceleration(configRose.accel_Spindle)
		.setPosition(0);

	newMaxSpd_RoseAxis = configRose.maxSpd_Axis_X * configRose.speedPercent_Axis * .01 * direction;
	stepperAxis_X
		.setMaxSpeed(newMaxSpd_RoseAxis)
		.setAcceleration(configRose.accel_Axis_X)
		.setPosition(slideFunc_Axis_X(0)); // set start position of counter

		// Enable steppers
	digitalWrite(PIN_SPINDLE_ENABLE, LOW);
	digitalWrite(PIN_AXIS_X_ENABLE, LOW);

	rotateController1.rotateAsync(stepperAxis_X);
	//controllerRose_Axis.overrideSpeed(0); // start with stopped slide
	controllerRose_Spindle.rotateAsync(stepperSpindle); // let the spindle run with constant speed

	tickX();

	// use a timer to periodically calculate new targets for the slide
	tickTimerX.priority(128); // lowest priority, potentially long caclulations need to be interruptable by TeensyStep
	tickTimerX.begin(tickX, recalcPeriod);

	while (runPatternX)
	{
		//digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
#ifdef SHOW_POSITION //Controls must be added to Nextion pages
		endPosition_Axis = teensyStep_Axis_X.getPosition();
		endPosition_Spindle = teensyStep_Spindle.getPosition();

		// print current values of spindle angle [rad] and slide position [steps]
		//float phiZ = endPosition_Spindle * (TWO_PI / spindleStepsPerRev);
		//Serial.printf("%f\t%d\n", phiZ, endPosition_Axis);
		degrees_Spindle = StepsToDegrees_Spindle(endPosition_Spindle);
		distance_Axis = StepsToDistance_Axis(endPosition_Axis, ID_AXIS_X);
		Serial3.print("pageRose.t12.txt=");
		Serial3.write(0x22);
		Serial3.print(degrees_Spindle);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
		delay(10);
		Serial3.print("pageRose.t13.txt=");
		Serial3.write(0x22);
		Serial3.print(distance_Axis);
		Serial3.write(0x22);
		Serial3.write(0xff);
		Serial3.write(0xff);
		Serial3.write(0xff);
#endif // Show Position

		// Check for Cancel code  
		if (SerialAvailable() >= 0)
		{
			incomingByte = SerialRead(serialId);
			switch (incomingByte)
			{
			case 109:
			{
				runPatternX = false;
				controllerRose_Spindle.overrideSpeed(0);
				rotateController1.overrideSpeed(0);
				controllerRose_Spindle.stopAsync();
				rotateController1.stopAsync();
				digitalWrite(PIN_AXIS_X_ENABLE, HIGH);  // Disable 
				break;
			}
			}
		}
#ifdef DEBUG
		endPosition_Spindle = stepperSpindle.getPosition();
		endPosition_Axis = stepperAxis_X.getPosition();
		Serial.print("Spindle:");
		Serial.println(endPosition_Spindle);
		Serial.print("Axis:");
		Serial.println(endPosition_Axis);
#endif // DEBUG
		delay(15);
	}

	endPosition_Spindle = stepperSpindle.getPosition();
	endPosition_Axis = stepperAxis_X.getPosition();
	returnSteps_Axis = endPosition_Axis;
	returnSteps_Spindle = endPosition_Spindle;
	stepperSpindle.setPosition(0);
	stepperAxis_X.setPosition(0);
#ifdef DEBUG
	Serial.print("Spindle:");
	Serial.println(endPosition_Spindle);
	Serial.print("Axis:");
	Serial.println(endPosition_Axis);
#endif // DEBUG

	digitalWrite(PIN_SPINDLE_ENABLE, HIGH); // Disable 
	digitalWrite(PIN_AXIS_X_ENABLE, HIGH);  // Disable 
}


// ========================================================
 //Test EEPROM
 //========================================================
/// <summary>
/// Write current Config screen eeprom values to Nextion
/// </summary>
/// <comment>
/// </comment>
/// <returns></returns>
void TestEEPROMConfig()
{


	// Write the setting to Nextion LCD
	// pageConfig
	SerialPrint("pageConfig.t19.txt=");
	SerialWrite(0x22);
	SerialPrint("Config");
	SerialWrite(0x22);
	SerialWrite(0xff);
	SerialWrite(0xff);
	SerialWrite(0xff);
	delay(2000);

	//pageCallerId = GetSerial1Float();
	pageCallerId = GetSerialIntegerOnly();
	while (Serial3.available() > 0)
	{
		// Discard remaining serial chars
		Serial3.read();
	}
	Serial3.print("pageConfig.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("Page:");
	Serial3.print(pageCallerId);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);
	switch (pageCallerId)
	{
		case PAGE_SPZ:
		{
			configSteppers eePageSpZ;
			EEPROM.get(eePromAddress_SpZ, eePageSpZ);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Main-MaxSpeed:");
			Serial3.print(eePageSpZ.maxSpd_Spindle);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Main-Accel:");
			Serial3.print(eePageSpZ.accel_Spindle);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Z Axis-MaxSpeed:");
			Serial3.print(eePageSpZ.maxSpd_Axis);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Z Axis-Accel:");
			Serial3.print(eePageSpZ.accel_Axis);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			break;
		}
		case PAGE_SPX:
		{
			configSteppers eePageSpX;
			EEPROM.get(eePromAddress_SpX, eePageSpX);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Main-MaxSpeed:");
			Serial3.print(eePageSpX.maxSpd_Spindle);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Main-Accel:");
			Serial3.print(eePageSpX.accel_Spindle);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("X Axis-MaxSpeed:");
			Serial3.print(eePageSpX.maxSpd_Axis);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("X Axis-Accel:");
			Serial3.print(eePageSpX.accel_Axis);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			break;
		}
		case PAGE_SPB:
		{
			configSteppers eePageSpB;
			EEPROM.get(eePromAddress_SpB, eePageSpB);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Main-MaxSpeed:");
			Serial3.print(eePageSpB.maxSpd_Spindle);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Main-Accel:");
			Serial3.print(eePageSpB.accel_Spindle);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("B Axis-MaxSpeed:");
			Serial3.print(eePageSpB.maxSpd_Axis);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("B Axis-Accel:");
			Serial3.print(eePageSpB.accel_Axis);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			break;
		}
		case PAGE_SYNCZ:
		{
			configSteppers eePageSyncZ;
			EEPROM.get(eePromAddress_SyncZ, eePageSyncZ);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("SyncZ-MaxSpeed:");
			Serial3.print(eePageSyncZ.maxSpd_Spindle);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("SyncZ-Accel:");
			Serial3.print(eePageSyncZ.accel_Spindle);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Z Axis-MaxSpeed:");
			Serial3.print(eePageSyncZ.maxSpd_Axis);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Z Axis-Accel:");
			Serial3.print(eePageSyncZ.accel_Axis);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			break;
		}
		case PAGE_SYNCX:
		{
			configSteppers eePageSyncX;
			EEPROM.get(eePromAddress_SyncX, eePageSyncX);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("SyncX-MaxSpeed:");
			Serial3.print(eePageSyncX.maxSpd_Spindle);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("SyncX-Accel:");
			Serial3.print(eePageSyncX.accel_Spindle);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("X Axis-MaxSpeed:");
			Serial3.print(eePageSyncX.maxSpd_Axis);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("X Axis-Accel:");
			Serial3.print(eePageSyncX.accel_Axis);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			break;
		}
		case PAGE_RECI_AXIAL_Z:
		case PAGE_RECI_RADIAL_Z:
		case PAGE_RECI_AXIAL_X:
		case PAGE_RECI_RADIAL_X:
		{
			configPageReci eePageReciRadialZ;
			EEPROM.get(eePromAddress_Reci, eePageReciRadialZ);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Rec1 Spindle-MaxSpeed:");
			Serial3.print(eePageReciRadialZ.maxSpd_Spindle);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Rec1 Spindle-Accel:");
			Serial3.print(eePageReciRadialZ.accel_Spindle);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Z Axis-MaxSpeed:");
			Serial3.print(eePageReciRadialZ.maxSpd_Axis_Z);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Z Axis-Accel:");
			Serial3.print(eePageReciRadialZ.accel_Axis_Z);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
#ifndef TWO_AXES_V2



			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("X Axis-MaxSpeed:");
			Serial3.print(eePageReciRadialZ.maxSpd_Axis_X);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("X Axis-Accel:");
			Serial3.print(eePageReciRadialZ.accel_Axis_X);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
#endif // !TWO_AXES_V2
			break;
		}
		case PAGE_SP:
		{
			configPageSp eePageSp;
			EEPROM.get(eePromAddress_Sp, eePageSp);
			switch (eePageSp.activeAxis)
			{
				case ID_SPINDLE:
				{
					Serial3.print("pageConfig.t19.txt=");
					Serial3.write(0x22);
					Serial3.print("Spindle-MaxSpeed:");
					Serial3.print(eePageSp.maxSpd_Spindle);
					Serial3.write(0x22);
					Serial3.write(0xff);
					Serial3.write(0xff);
					Serial3.write(0xff);
					delay(2000);
					Serial3.print("pageConfig.t19.txt=");
					Serial3.write(0x22);
					Serial3.print("Spindle-Accel:");
					Serial3.print(eePageSp.accel_Spindle);
					Serial3.write(0x22);
					Serial3.write(0xff);
					Serial3.write(0xff);
					Serial3.write(0xff);
					delay(2000);
					Serial3.print("pageConfig.t19.txt=");
					Serial3.write(0x22);
					Serial3.print("Spindle-Accel2:");
					Serial3.print(configSp.accel_Spindle);
					Serial3.write(0x22);
					Serial3.write(0xff);
					Serial3.write(0xff);
					Serial3.write(0xff);
					delay(2000);
					break;
				}
				case ID_AXIS_Z:
				{
					Serial3.print("pageConfig.t19.txt=");
					Serial3.write(0x22);
					Serial3.print("maxSpd_Axis_Z:");
					Serial3.print(eePageSp.maxSpd_Axis_Z);
					Serial3.write(0x22);
					Serial3.write(0xff);
					Serial3.write(0xff);
					Serial3.write(0xff);
					delay(2000);
					Serial3.print("pageConfig.t19.txt=");
					Serial3.write(0x22);
					Serial3.print("accel_Axis_Z:");
					Serial3.print(eePageSp.accel_Axis_Z);
					Serial3.write(0x22);
					Serial3.write(0xff);
					Serial3.write(0xff);
					Serial3.write(0xff);
					delay(2000);
					break;
				}
				case ID_AXIS_X:
				{
					Serial3.print("pageConfig.t19.txt=");
					Serial3.write(0x22);
					Serial3.print("maxSpd_Axis_X:");
					Serial3.print(eePageSp.maxSpd_Axis_X);
					Serial3.write(0x22);
					Serial3.write(0xff);
					Serial3.write(0xff);
					Serial3.write(0xff);
					delay(2000);
					Serial3.print("pageConfig.t19.txt=");
					Serial3.write(0x22);
					Serial3.print("accel_Axis_X:");
					Serial3.print(eePageSp.accel_Axis_X);
					Serial3.write(0x22);
					Serial3.write(0xff);
					Serial3.write(0xff);
					Serial3.write(0xff);
					delay(2000);
					break;
				}
				case ID_AXIS_B:
				{
					Serial3.print("pageConfig.t19.txt=");
					Serial3.write(0x22);
					Serial3.print("maxSpd_Axis_B:");
					Serial3.print(eePageSp.maxSpd_Axis_B);
					Serial3.write(0x22);
					Serial3.write(0xff);
					Serial3.write(0xff);
					Serial3.write(0xff);
					delay(2000);
					Serial3.print("pageConfig.t19.txt=");
					Serial3.write(0x22);
					Serial3.print("accel_Axis_B:");
					Serial3.print(eePageSp.accel_Axis_B);
					Serial3.write(0x22);
					Serial3.write(0xff);
					Serial3.write(0xff);
					Serial3.write(0xff);
					delay(2000);
					break;
				}
			}
			break;
		}
		case PAGE_INDEX1:
		{
			configStepper eePageIndex1;
			EEPROM.get(eePromAddress_Index1, eePageIndex1);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Index1-MaxSpeed:");
			Serial3.print(eePageIndex1.maxSpd);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Index1-Accel:");
			Serial3.print(eePageIndex1.accel);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			break;
		}
		case PAGE_INDEX2:
		{
			configStepper eePageIndex2;
			EEPROM.get(eePromAddress_Index2, eePageIndex2);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Index2-MaxSpeed:");
			Serial3.print(eePageIndex2.maxSpd);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Index2-Accel:");
			Serial3.print(eePageIndex2.accel);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			break;
		}
		case PAGE_MOVEZ:
		{
			configStepper eePageMoveZ;
			EEPROM.get(eePromAddress_MoveZ, eePageMoveZ);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("MoveZ-MaxSpeed:");
			Serial3.print(eePageMoveZ.maxSpd);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("MoveZ-Accel:");
			Serial3.print(eePageMoveZ.accel);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			break;
		}
		case PAGE_MOVEX:
		{
			configStepper eePageMoveX;
			EEPROM.get(eePromAddress_MoveX, eePageMoveX);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("MoveX-MaxSpeed:");
			Serial3.print(eePageMoveX.maxSpd);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("MoveX-Accel:");
			Serial3.print(eePageMoveX.accel);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			break;
		}
		case PAGE_ROSE:
		{
			configPageRose eePageRose;
			EEPROM.get(eePromAddress_Rose, eePageRose);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Rose-Spindle MaxSpeed:");
			Serial3.print(eePageRose.maxSpd_Spindle);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Rose-Spindle Accel:");
			Serial3.print(eePageRose.accel_Spindle);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Rose-ZAxis MaxSpeed:");
			Serial3.print(eePageRose.maxSpd_Axis_Z);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Rose-ZAxis Accel:");
			Serial3.print(eePageRose.accel_Axis_Z);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
#ifndef TWO_AXES_V2
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Rose-XAxis MaxSpeed:");
			Serial3.print(eePageRose.maxSpd_Axis_X);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
			Serial3.print("pageConfig.t19.txt=");
			Serial3.write(0x22);
			Serial3.print("Rose-XAxis Accel:");
			Serial3.print(eePageRose.accel_Axis_X);
			Serial3.write(0x22);
			Serial3.write(0xff);
			Serial3.write(0xff);
			Serial3.write(0xff);
			delay(2000);
#endif // !TWO_AXES_V2
			break;
		}

		
	}	

	// Update Nextion
	Serial3.print("pageConfig.bt1.bco=7158");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageConfig.bt1.val=0");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
}

/// <summary>
/// Write current Setup screen eeprom values to Nextion
/// </summary>
/// <comment>
/// </comment>
/// <returns></returns>
void TestEEPROMSetup()
{
	configs eePromConfig;
	EEPROM.get(eePromAddress_Setup, eePromConfig);
	delay(2000);

	// Write the setting to Nextion LCD
	// pageSetup
	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("Setup:");
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);

	// pageSetup
	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("Spindle-Microsteps:");
	Serial3.print(eePromConfig.microsteps_Spindle);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);

	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("Spindle-Steps360:");
	Serial3.print(eePromConfig.steps360_Spindle);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);
	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("Spindle-Gear Ratio:");
	Serial3.print(eePromConfig.gearRatio_Spindle);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);
	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("Spindle-Enable:");
	Serial3.print(eePromConfig.polarity_Spindle? "Low":"High");
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);

	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("Z Axis-Microsteps:");
	Serial3.print(eePromConfig.microsteps_Axis_Z);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);
	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("Z Axis-Steps/360:");
	Serial3.print(eePromConfig.steps360_Axis_Z);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);
	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("Z Axis-Dist/360:");
	Serial3.print(eePromConfig.distancePerRev_AxisZ);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);
	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("Z Axis-Enable:");
	Serial3.print(eePromConfig.polarity_Axis_Z ? "Low" : "High");
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);

#ifndef TWO_AXES_V2
	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("X Axis-Microsteps:");
	Serial3.print(eePromConfig.microsteps_Axis_X);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);
	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("X Axis-Steps/360:");
	Serial3.print(eePromConfig.steps360_Axis_X);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);
	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("X Axis-Dist/360:");
	Serial3.print(eePromConfig.distancePerRev_AxisX);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);
	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("X Axis-Enable:");
	Serial3.print(eePromConfig.polarity_Axis_X ? "Low" : "High");
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);
#endif // TWO_AXES_V2
#ifdef FOUR_AXES
	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("B Axis-Microsteps:");
	Serial3.print(eePromConfig.microsteps_Axis_B);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);

	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("B Axis-Steps360:");
	Serial3.print(eePromConfig.steps360_Axis_B);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);
	//Serial3.print("pageSetup.t19.txt=");
	//Serial3.write(0x22);
	//Serial3.print("B Axis-Gear Ratio:");
	//Serial3.print(eePromConfig.gearRatio_AxisB);
	//Serial3.write(0x22);
	//Serial3.write(0xff);
	//Serial3.write(0xff);
	//Serial3.write(0xff);
	//delay(2000);
	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("B Axis-Enable:");
	Serial3.print(eePromConfig.polarity_Axis_B ? "Low" : "High");
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);
#endif // FOUR_AXES
	
	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("Limit Min Z:");
	Serial3.print(configMain.limit_Min_Z);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);
	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("Limit Max Z:");
	Serial3.print(configMain.limit_Max_Z);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);
#ifndef TWO_AXES_V2
	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("Limit Min X:");
	Serial3.print(configMain.limit_Min_X);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);

	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("Limit Max X:");
	Serial3.print(configMain.limit_Max_X);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);
#endif
#ifdef FOUR_AXES
	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("Limit Min B:");
	Serial3.print(configMain.limit_Min_B);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);
	Serial3.print("pageSetup.t19.txt=");
	Serial3.write(0x22);
	Serial3.print("Limit Max B:");
	Serial3.print(configMain.limit_Max_B);
	Serial3.write(0x22);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	delay(2000);
	// Update Nextion
	Serial3.print("pageSetup.bt1.bco=23964");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.print("pageSetup.bt1.val=0");
	Serial3.write(0xff);
	Serial3.write(0xff);
	Serial3.write(0xff);
#endif


}



/// <summary>
/// Set microstepping mode
/// </summary>
/// <comment>
/// </comment>
/// <param name="microsteps">Microsteps</param>
/// <param name="pinMs0">Pin MS0</param>
/// <param name="pinMs1">Pin MS1</param>
/// <param name="pinMs2">Pin MS2</param>
/// <returns></returns>
void SetMicrosteppingMode(int microsteps, int pinMs0, int pinMs1, int pinMs2)
{
	// Settings for DRV8825 and MKS-LV8729
	switch (microsteps)
	{
		case 1:
		{
			digitalWrite(pinMs0, LOW);
			digitalWrite(pinMs1, LOW);
			digitalWrite(pinMs2, LOW);
			break;
		}
		case 2:
		{
			digitalWrite(pinMs0, HIGH);
			digitalWrite(pinMs1, LOW);
			digitalWrite(pinMs2, LOW);
			break;
		}
		case 4:
		{
			digitalWrite(pinMs0, LOW);
			digitalWrite(pinMs1, HIGH);
			digitalWrite(pinMs2, LOW);
			break;
		}
		case 8:
		{
			digitalWrite(pinMs0, HIGH);
			digitalWrite(pinMs1, HIGH);
			digitalWrite(pinMs2, LOW);
			break;
		}
		case 16:
		{
			digitalWrite(pinMs0, LOW);
			digitalWrite(pinMs1, LOW);
			digitalWrite(pinMs2, HIGH);
			break;
		}
		case 32:
		{
			digitalWrite(pinMs0, HIGH);
			digitalWrite(pinMs1, LOW);
			digitalWrite(pinMs2, HIGH);
			break;
		}
		case 64:
		{
			digitalWrite(pinMs0, LOW);
			digitalWrite(pinMs1, HIGH);
			digitalWrite(pinMs2, HIGH);
			break;
		}
		case 128:
		{
			digitalWrite(pinMs0, HIGH);
			digitalWrite(pinMs1, HIGH);
			digitalWrite(pinMs2, HIGH);
			break;
		}
	}
}