
#include <string>
#include <vector>
#include <list>

#include <iostream>
#include <fstream>
#include <sstream>

#include <thread>
#include <chrono>

/*
Input File Displaying
	int viewLength = 20
	int padding = std::min(5, (viewSize - note.size()) / 2);
	int startLine = std::max(0, currentLine - padding);
	int endline = startLine + ((noteLength > viewLength) ? noteLength : viewLength );
	endline = std::min( lines.size(), currentLine + noteLength + padding );
	std::cout << (endline - startLine) << "\n";

Headers
	headerPosition, headerLength
	getNextNoteLine
	h - Goto next Header or skip Header

Auto Sorting - Term Frequency - Inverse Document Frequency
	Filter out common words, count instances of term and sort them sort them
	find term occurance by dividing by total length
	For documents, find average for all other documents and divide the average by the document's
	total - / total
	weight = tf / length
	normalized = (weight - minWeight) / (maxWeight - minWeight)
	tf / averageTF
*/

typedef std::string String;

#define ESK "\033["
#define CON_MOVETO(r,c) ESK #r ";" #c "H"

#ifndef NEWLINE_AFTER_NOTE
	#define NEWLINE_AFTER_NOTE 1
#endif

void mainLoop();
void getNextNoteLine();
void getNoteLength();
void display();
void removeCurrentNote();
void parseLine(String line);
bool parseSetCommand(String line);
bool loadInputFile();
bool loadState();
bool saveState();
void consoleTest();

String inputFilename;
int currentLine = 0;
int noteLength = 1;
std::vector<String> lines;
String configFilename;
std::fstream configfile;
const int indexes = 10;
String outputFilenames[indexes];
std::ofstream outputFiles[indexes];
bool doQuit = false;
std::list<String> log;
int logLength = 3;
int inputViewLength = 15;

int endCol = 0;

int getIndentLevel(String line) {
	int indent = 0;
	for (char c: line) {
		if (std::isspace(c)) { ++indent; }
		else { break; }
	}
	return indent;
}

bool isBlankLine(String str) {
	if ( str.empty() ) { return true; }
	for(char ch : str) {
		if ( !std::isspace(ch) ) { return false; }
	}
	return true;
}

bool isSymbolLine(int line) {
	if ( lines[line].empty() ) { return false; }
	for(char ch : lines[line]) {
		switch(ch) {
		case'<':case'>':case'(':case')':case'[':case']':case'{':case'}':
		case';':case':':case'\'':case'"':case'-':case'_':case'=':case'+':
		case'!':case'@':case'#':case'$':case'%':case'^':case'&':case'*':
		case'`':case'~':case'/':case'?':case',':case'.':case'\\':case'|':
			break; 
		default: return false;
		}
	
	}
	return true;
}

void getNextNoteLine() {
	currentLine += noteLength;
	while (currentLine < (int)lines.size()) {
		if (isBlankLine(lines[currentLine])) {
			++currentLine;
		} else { break; }
	}
}

bool enterSubNote() {
	if ( noteLength <= 1 ) { return false; }
	++currentLine;
	getNoteLength();
	return true;
	/*
	int indent = getIndentLevel(lines[currentLine]);
	int line = currentLine + 1;
	while (line < currentLine + noteLength) {
		if ( indent < getIndentLevel(lines[line]) ) { ++line; }
		else if ( isSymbolLine(line) ) { ++line; }
		else { break; }
	}
	*/
}

void getPrevNoteLine() {
	//int indent = getIndentLevel( lines[currentLine] );
	while (currentLine >= 0) {
		--currentLine;
	}
}

void getNoteLength() {
	if (currentLine >= (int)lines.size()) { noteLength = 0; return; }
	//if (isHeaderLine(currentLine)) { //Read until 2 blank lines }
	int indent = getIndentLevel(lines[currentLine]);
	int line = currentLine + 1;
	while (line < (int)lines.size()) {
		if ( indent < getIndentLevel(lines[line]) ) { ++line; }
		else if ( isSymbolLine(line)
				&& (indent == getIndentLevel(lines[line])))
		{ ++line; }
		else { break; }
	}
	if (NEWLINE_AFTER_NOTE && lines[line].empty()) { ++line; }
	noteLength = line - currentLine;
}

void display() {
	std::cout << "Commands:\n";
	std::cout << "    set 1-" << (indexes-1) << " destination_file\n";
	std::cout << "    0-" << (indexes-1) << " - Send Note to Destination File\n";
	//std::cout << "[space]- Send note to previous destination\n";
	//std::cout << "    a - Auto Sort Note\n";
	//std::cout << "    u - Undo previous operation\n";
	std::cout << "    s - Skip Note\n";
	std::cout << "    p - Previous Note\n";
	std::cout << "    i - Insert custom Note\n";
	//std::cout << "  TAB - Enter into inner sub-note"
	//std::cout << "    n - Insert Newline into previous Destination output file\n";
	std::cout << "    d - Delete Note\n";
	std::cout << " save - Save output files and configuration\n";
	std::cout << "    q - Quit the Program\n";
	
	// Display Output file destinations
	std::cout << "\n";
	for ( int i = 0; i < indexes; ++i ) {
		std::cout << i << " " << outputFilenames[i] << "  ";
	}
	std::cout << "\n";
	
	// Display Log
	/*if ( !log.empty() ) {
		std::cout << "----------\n";
		for (auto elem : log) { std::cout << elem << "\n"; }
		if ((int)log.size() >= logLength) { log.pop_front(); }
		//std::cout << "----------\n";
	}
	*/
	
	// Display Input file section
	std::cout << ESK "36;1m" "##############################\n" ESK "0m";
	int startLine = std::max(0, currentLine);
	//int endline = startLine + ((noteLength > inputViewLength) ? noteLength : intputViewLength );
	//endline = std::min( endline, (int)lines.size()-1 );
	int endline = std::min( startLine  + inputViewLength, (int)lines.size()-1 );
	//std::cout << (endline - startLine) << "\n";
	
	for ( int i = startLine; i < endline; ++i ) {
		if (i >= (int)lines.size()) { break; }
		if ((i >= currentLine) && (i < noteLength + currentLine)) {
			std::cout << ESK "7m " << lines[i];
			std::cout << ESK "0m" "\n";
		} else {
			std::cout << " " << lines[i] << "\n";
		}
	}
	for ( int i = 0; i < inputViewLength-(endline-startLine); ++i ) { std::cout << "\n"; }
	std::cout << ESK "36;1m" "##############################\n" ESK "0m";
}

void removeCurrentNote() {
	lines.erase( lines.begin() + currentLine, lines.begin() + currentLine + noteLength );
}

void parseLine(String str) {
	if (str.empty()) { return; }
	if ( parseSetCommand( str ) ) { return; }
	
	std::stringstream ss;
	ss.str(str);
	ss >> str;
	
	if ( (str[0] >= '0') && (str[0] <= '9') ) {	
		int dest = std::stoi(str);
		if ( (dest < 0) || (dest >= indexes) ) { return; }
		for ( int i = currentLine; i < currentLine + noteLength; ++i ) {
			outputFiles[dest] << lines[i] << "\n";
		}
		removeCurrentNote();
		return;
	}
	
	if ( str == "save" ) {
		saveState();
	} else if ( str == "s" ) {
		if (currentLine != (int)lines.size()) {
			log.push_back( String("Skipped ") + std::to_string(currentLine) );
		}
		int before = currentLine;
		getNextNoteLine();
		if ((before != currentLine) && (currentLine == (int)lines.size())) {
			log.push_back("Reached End of Input");
		}
		return;
	}
	if ( str == "i" ) {
		std::cout << "Insert Custom Note > ";
		String line;
		getline(std::cin, line);
		if ( !isBlankLine(line) ) {
			lines.insert( lines.begin() + currentLine, line );
		}
	}
	if ( str == "d" ) {
		log.push_back( String("Deleted ") + std::to_string(currentLine) );
		removeCurrentNote();
	}
	if ( str == "p" ) {
		log.push_back("Previous");
		--currentLine;
		currentLine = std::max(0, currentLine);
	}
	if ( str == "q" ) {
		doQuit = true;
		return;
	}
	if ( str == "\t" ) {
		enterSubNote();
	}
	
	ss.str("");
	ss.clear();
}

bool parseSetCommand(String str) {
	std::stringstream ss;
	ss.str(str);
	ss >> str;
	if (str.empty()) { return false; }
	if ( str == "set") {
		int n = 0;
		ss >> n;
		if ( (n < 0) || (n >= indexes) ) {
			std::cout << n << "\n";
			std::cout << "Number must be within range [0-" << (indexes-1) << "]\n";
			return false;
		}
		
		String filename;
		str = ss.str();
		filename = str.substr( (int)ss.tellg() + 1, str.size() - ss.tellg() - 1 );
		if ( outputFiles[n].is_open() ) {
			std::cout << "Closing " << outputFilenames[n] << "\n";
			outputFiles[n].close();
		}
		if ( !filename.empty() ) {
			std::cout << "Opening " << filename << "\n";
			outputFiles[n].open( filename, std::ofstream::out | std::ofstream::app );
			outputFilenames[n] = filename;
		}
		log.push_back( String("set ") + std::to_string(n)
						+ String(" to ") + filename );
		return true;
	}
	return false;
}

void mainLoop() {
	std::cout << "\033[?47h";
	loadState();
	loadInputFile();
	while (!doQuit) {
		std::cout << "\033[2J";
		std::cout << CON_MOVETO(1,1);
		if (currentLine >= (int)lines.size() - 1) { break; }
		getNoteLength();
		display();
		std::cout << "> ";
		String line;
		getline(std::cin, line);
		parseLine( line );
	}
	std::cout << "\033[?47l";
	if (currentLine >= (int)lines.size() - 1) {
		std::cout << "Input note file completely empty.\n";
	}
	saveState();
	for ( int i = 0; i < indexes; ++i ) { outputFiles[i].close(); }
}


bool saveState() {
	if (configfile.is_open()) { configfile.close(); }
	configfile.open( configFilename, std::fstream::out | std::fstream::trunc );
	for ( int i = 0; i < indexes; ++i ) {
		if ( !outputFilenames[i].empty() ) {
			configfile << "set " << i << " " << outputFilenames[i] << "\n";
		}
	}
	// Save Input File
	configfile.close();
	std::fstream inputFile(inputFilename, std::fstream::out | std::fstream::trunc);
	for (uint32_t i = 0; i+1 < lines.size(); ++i) {
		inputFile << lines[i] << "\n";
	}
	if (!lines.empty()) { inputFile << lines.back(); }
	
	std::cout << "Saved configuration to '" << configFilename << "'\n";
	std::cout << "Saved '" << inputFilename << "' modifications\n";
	
	// Save Output Files
	for ( int i = 0; i < indexes; ++i ) {
		outputFiles[i].close();
		outputFiles[i].open( outputFilenames[i], std::ofstream::out | std::ofstream::app );
	}
	
	std::this_thread::sleep_for( std::chrono::milliseconds(1000) );
	return true;
}

bool loadState() {
	if (configfile.is_open()) { configfile.close(); }
	configfile.open(configFilename, std::ifstream::in);
	String line;
	while ( !configfile.eof() ) {
		getline( configfile, line );
		parseSetCommand( line );
	}
	return true;
}

bool loadInputFile() {
	std::ifstream ifile(inputFilename);
	if (!ifile.is_open()) { return false; }
	String line;
	while (ifile.good()) {
		getline(ifile, line);
		lines.push_back(line);
	}
	ifile.close();
	return true;
}

int main(int argc, char* argv[] ) {
	if (argc <= 1) {
		std::cout << "USAGE: " << String(argv[0]) << " input_file [config_file].\n";
		return 1;
	}
	
	inputFilename = argv[1];
	if (argc > 2) { configFilename = argv[2]; }
	
	mainLoop();
	
	return 0;
}

