#include <iostream>
#include <map>
//#include <algorithm>
#include <wiringPi.h>
#include <softTone.h>
//#include <stdio.h>
#include <time.h>
#include <string>

/*
	MorseCodec Program by David Hudson and Ken Injerd.
	This program can take button inputs and translate them to morse code. 
	This program also can a string of characters (All uppercase and numbers 0-9) and beep out the morse code.
*/

using namespace std;

const long DOT_TIME = 1000;	// Noise Duration
const int SPEAKER_PIN = 0;	// Speaker
const int BUTTON_PIN = 1;	// Morse Button
const int STOP_PIN = 5;	// Button to escape morse decoding

// string arrays for encoding characters into morse
const string CHAR_TO_MORSE[] = {"SL", "LSSS", "LSLS", "LSS", "S", "SSLS",	//ABCDEF
                                "LLS", "SSSS", "SS", "SLLL", "LSL", "SLSS",	//GHIJKL
                                "LL", "LS", "LLL", "SLLS", "LLSL", "SLS",	//MNOPQR
                                "SSS", "L", "SSL", "SSSL", "SLL", "LSSL",	//STUVWX
                                "LSLL", "LLSS"};				//YZ
const string NUM_TO_MORSE[] = {"LLLLL", "SLLLL", "SSLLL", "SSSLL", "SSSSL",	//01234
			       "SSSSS", "LSSSS", "LLSSS", "LLLSS", "LLLLS"};	//56789
const int TONE_DELAY=80;

map<string,char> morse_to_char;

void init_Map() {	// A map for the decoding of a morse sequence.
    morse_to_char["SL"] = 'A';
    morse_to_char["LSSS"] = 'B';
    morse_to_char["LSLS"] = 'C';
    morse_to_char["LSS"] = 'D';
    morse_to_char["S"] = 'E';
    morse_to_char["SSLS"] = 'F';
    morse_to_char["LLS"] = 'G';
    morse_to_char["SSSS"] = 'H';
    morse_to_char["SS"] = 'I';
    morse_to_char["SLLL"] = 'J';
    morse_to_char["LSL"] = 'K';
    morse_to_char["SLSS"] = 'L';
    morse_to_char["LL"] = 'M';
    morse_to_char["LS"] = 'N';
    morse_to_char["LLL"] = 'O';
    morse_to_char["SLLS"] = 'P';
    morse_to_char["LLSL"] = 'Q';
    morse_to_char["SLS"] = 'R';
    morse_to_char["SSS"] = 'S';
    morse_to_char["L"] = 'T';
    morse_to_char["SSL"] = 'U';
    morse_to_char["SSSL"] = 'V';
    morse_to_char["SLL"] = 'W';
    morse_to_char["LSSL"] = 'X';
    morse_to_char["LSLL"] = 'Y';
    morse_to_char["LLSS"] = 'Z';
    morse_to_char["LLLLL"] = '0';
    morse_to_char["SLLLL"] = '1';
    morse_to_char["SSLLL"] = '2';
    morse_to_char["SSSLL"] = '3';
    morse_to_char["SSSSL"] = '4';
    morse_to_char["SSSSS"] = '5';
    morse_to_char["LSSSS"] = '6';
    morse_to_char["LLSSS"] = '7';
    morse_to_char["LLLSS"] = '8';
    morse_to_char["LLLLS"] = '9';
}

bool scanButton(int); // 
void decodeMorse();	// decode a Morse_Button_Input
void toMorse(string decode);	// 
string encode_morse(string input);

int main() {
	init_Map();
	bool quit = false;
	string input = "";
	wiringPiSetup();
	pinMode (BUTTON_PIN, INPUT);
	pinMode (STOP_PIN, INPUT);
	pullUpDnControl(BUTTON_PIN,PUD_UP);
	pullUpDnControl(STOP_PIN,PUD_UP);
	softToneCreate(SPEAKER_PIN);
	while (!quit) {
		cout << "Make your selection:\n";
		cout << "\t1: Morse >> Text\n";
		cout << "\t2: Text >> Morse\n";
		cout << "\tq: Quit\n";
		cin >> input;
		if (input == "1")
			decodeMorse();
		if (input == "2") {
			cout << "Allowed characters are UPPER CASE, numeric, period, and comma.\n";
			cout << "Enter string to convert: ";
			string toDecode = "SOS";
			getline(cin, toDecode);
			getline(cin, toDecode);
			//cout << int(toDecode.at(0))<<endl;
			toMorse(encode_morse(toDecode));
		}
		if (input == "q")
			quit = true;
	}
	cout << "Program end\n";
	return 0;
}

bool scanButton(int button) {
        if (digitalRead (button) == LOW) {
                delay(10);
		return true;
        }
        if (digitalRead (button) == HIGH) {
                delay(10);
                return false;
        }
}

void decodeMorse() {	// 1) 
	long switchedTime = 0;
	long time_diff = 0;
	int lineLength = 0;
	bool pressed = false;
	string code = "";
	string morseWord = "";
	cout << "Reading Morse . . ." << endl;

	while(digitalRead(STOP_PIN) == HIGH) {
        	// Button down event
		if (scanButton(BUTTON_PIN) && (!pressed)) {
			// Short pause check
			if (((clock() - switchedTime) > (DOT_TIME*4)) && ((clock() - switchedTime) < (DOT_TIME*10))) {
				if ((int(morse_to_char[code]) == 0))
					morseWord += "-";
				else {
					morseWord += morse_to_char[code];
					//cout << morse_to_char[code] << endl;
				}
				code = "";
			}
			// Long pause check
			else if ((clock() - switchedTime) >= (DOT_TIME*10)) {
            			if ((int(morse_to_char[code]) == 0))
					morseWord += "-";
				else {
					morseWord += morse_to_char[code];
					//cout << morse_to_char[code] << endl;
				}
				cout << morseWord << endl;
				morseWord = "";
				code = "";
			}
			softToneWrite(SPEAKER_PIN, 800);
			pressed = true;
			switchedTime = clock();
		}
		// Button down duration check
		if ((!scanButton(BUTTON_PIN)) && (pressed)) {
			time_diff = (clock() - switchedTime);
			if (time_diff <= (DOT_TIME*3))
				code += "S";
			else
				code += "L";
			pressed = false;
			softToneWrite(SPEAKER_PIN, 0);
			switchedTime = clock();
		}

    }
}

// Decode Function by David
void toMorse(string decode) {	// 2)
	softToneCreate(0);
	for (int i=0; i<decode.length(); i++) {
		if (decode[i] =='S') {
			softToneWrite(0,800);
			delay(TONE_DELAY);
		}
		else if (decode[i] == 'L') {
			softToneWrite(0,800);
			delay(TONE_DELAY*3);

		}
		else if (decode[i] == 'B') {
			delay(TONE_DELAY*2);
		}
		else if (decode[i] == 'W') {
			delay(TONE_DELAY*6);

		}
		softToneWrite(0,0);
		delay(TONE_DELAY);
	}
	softToneWrite(0,0);
}

// By Ken Injerd
string encode_morse(string input) { // 2)
    string toRet = "";
    for (int i = 0; i < input.length(); i++)
    {		
	// Check for whitespace.
        if (input.at(i) == 32)
            toRet = toRet + "W";
	// Check for punctuation.
	else if (input.at(i) == '.')
	    toRet = toRet + "SLSLSLB";
	else if (input.at(i) == ',')
	    toRet = toRet + "LLSSLLB";
	// Check for numeric input.
        else if (input.at(i) < 58)
            toRet = toRet + (NUM_TO_MORSE[(int(input.at(i))-65)] + "B");
	// Otherwise translate alphabet.
	else
            toRet = toRet + (CHAR_TO_MORSE[(int(input.at(i))-65)] + "B");
    }
    return toRet;
}
